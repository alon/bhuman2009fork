/**
* \file Platform/linux/NaoCamera.cpp
* Interface to the Nao camera using linux-uvc.
* \author Colin Graf
* \author Thomas Röfer
*/

#if defined(TARGET_SIM) && !defined(NO_NAO_EXTENSIONS)
#define NO_NAO_EXTENSIONS
#endif

#include <cstring>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "NaoCamera.h"
#include "Platform/GTAssert.h"
#include "Representations/Infrastructure/CameraInfo.h"
#include "Tools/Debugging/Debugging.h"

#undef __STRICT_ANSI__
#include <linux/videodev.h>
#include <linux/version.h>
#include <linux/i2c-dev.h>
#define __STRICT_ANSI__

#ifndef V4L2_CID_AUTOEXPOSURE
#  define V4L2_CID_AUTOEXPOSURE     (V4L2_CID_BASE+32)
#endif

#ifndef V4L2_CID_CAM_INIT
#  define V4L2_CID_CAM_INIT         (V4L2_CID_BASE+33)
#endif

#ifndef V4L2_CID_AUDIO_MUTE
#  define V4L2_CID_AUDIO_MUTE       (V4L2_CID_BASE+9)
#endif

#ifndef V4L2_CID_POWER_LINE_FREQUENCY
#  define V4L2_CID_POWER_LINE_FREQUENCY  (V4L2_CID_BASE+24)
enum v4l2_power_line_frequency {
  V4L2_CID_POWER_LINE_FREQUENCY_DISABLED  = 0,
  V4L2_CID_POWER_LINE_FREQUENCY_50HZ  = 1,
  V4L2_CID_POWER_LINE_FREQUENCY_60HZ  = 2,
};

#define V4L2_CID_HUE_AUTO      (V4L2_CID_BASE+25)
#define V4L2_CID_WHITE_BALANCE_TEMPERATURE  (V4L2_CID_BASE+26)
#define V4L2_CID_SHARPNESS      (V4L2_CID_BASE+27)
#define V4L2_CID_BACKLIGHT_COMPENSATION   (V4L2_CID_BASE+28)

#define V4L2_CID_CAMERA_CLASS_BASE     (V4L2_CTRL_CLASS_CAMERA | 0x900)
#define V4L2_CID_CAMERA_CLASS       (V4L2_CTRL_CLASS_CAMERA | 1)

#define V4L2_CID_EXPOSURE_AUTO      (V4L2_CID_CAMERA_CLASS_BASE+1)
enum  v4l2_exposure_auto_type {
  V4L2_EXPOSURE_MANUAL = 0,
  V4L2_EXPOSURE_AUTO = 1,
  V4L2_EXPOSURE_SHUTTER_PRIORITY = 2,
  V4L2_EXPOSURE_APERTURE_PRIORITY = 3
};
#define V4L2_CID_EXPOSURE_ABSOLUTE    (V4L2_CID_CAMERA_CLASS_BASE+2)
#define V4L2_CID_EXPOSURE_AUTO_PRIORITY    (V4L2_CID_CAMERA_CLASS_BASE+3)

#define V4L2_CID_PAN_RELATIVE      (V4L2_CID_CAMERA_CLASS_BASE+4)
#define V4L2_CID_TILT_RELATIVE      (V4L2_CID_CAMERA_CLASS_BASE+5)
#define V4L2_CID_PAN_RESET      (V4L2_CID_CAMERA_CLASS_BASE+6)
#define V4L2_CID_TILT_RESET      (V4L2_CID_CAMERA_CLASS_BASE+7)

#define V4L2_CID_PAN_ABSOLUTE      (V4L2_CID_CAMERA_CLASS_BASE+8)
#define V4L2_CID_TILT_ABSOLUTE      (V4L2_CID_CAMERA_CLASS_BASE+9)

#define V4L2_CID_FOCUS_ABSOLUTE      (V4L2_CID_CAMERA_CLASS_BASE+10)
#define V4L2_CID_FOCUS_RELATIVE      (V4L2_CID_CAMERA_CLASS_BASE+11)
#define V4L2_CID_FOCUS_AUTO      (V4L2_CID_CAMERA_CLASS_BASE+12)
 
#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26) */

NaoCamera* NaoCamera::theInstance = 0;

NaoCamera::NaoCamera() :
currentBuf(0),
timeStamp(0),
storedTimeStamp(SystemCall::getCurrentSystemTime())
{
  ASSERT(theInstance == 0);
  theInstance = this;

#ifndef NO_NAO_EXTENSIONS
  int i2cFd = open("/dev/i2c-0", O_RDWR);
  ASSERT(i2cFd != -1);
  VERIFY(ioctl(i2cFd, 0x703, 8) == 0);
  VERIFY(i2c_smbus_read_byte_data(i2cFd, 170) >= 2); // at least Nao V3
  unsigned char cmd[2] = {2, 0};
  VERIFY(i2c_smbus_write_block_data(i2cFd, 220, 1, cmd) != -1); // select lower camera
  close(i2cFd);
#endif

  // open device
  fd = open("/dev/video0", O_RDWR);
  ASSERT(fd != -1);

  // set default parameters
#ifndef NO_NAO_EXTENSIONS
  struct v4l2_control control;
  memset(&control, 0, sizeof(control));
  control.id = V4L2_CID_CAM_INIT;
  control.value = 0;
  VERIFY(ioctl(fd, VIDIOC_S_CTRL, &control) >= 0);

  v4l2_std_id esid0 = WIDTH == 320 ? 0x04000000UL : 0x08000000UL;
  VERIFY(!ioctl(fd, VIDIOC_S_STD, &esid0));
#endif
  
  // set format
  struct v4l2_format fmt;
  memset(&fmt, 0, sizeof(struct v4l2_format));
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width = WIDTH;
  fmt.fmt.pix.height = HEIGHT;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
  fmt.fmt.pix.field = V4L2_FIELD_NONE;
  VERIFY(!ioctl(fd, VIDIOC_S_FMT, &fmt));

  ASSERT(fmt.fmt.pix.sizeimage == SIZE);

  // set frame rate
  struct v4l2_streamparm fps;  
  memset(&fps, 0, sizeof(struct v4l2_streamparm));
  fps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  VERIFY(!ioctl(fd, VIDIOC_G_PARM, &fps));
  fps.parm.capture.timeperframe.numerator = 1;
  fps.parm.capture.timeperframe.denominator = 30;
  VERIFY(ioctl(fd, VIDIOC_S_PARM, &fps) != -1);

  // request buffers
  struct v4l2_requestbuffers rb;
  memset(&rb, 0, sizeof(struct v4l2_requestbuffers));
  rb.count = frameBufferCount;
  rb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  rb.memory = V4L2_MEMORY_MMAP;
  VERIFY(ioctl(fd, VIDIOC_REQBUFS, &rb) != -1);

  // map the buffers
  buf = static_cast<struct v4l2_buffer*>(calloc(1, sizeof(struct v4l2_buffer)));
  for(int i = 0; i < frameBufferCount; ++i)
  {
    buf->index = i;
    buf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf->memory = V4L2_MEMORY_MMAP;
    VERIFY(ioctl(fd, VIDIOC_QUERYBUF, buf) != -1);
    memLength[i] = buf->length;
    mem[i] = mmap(0, buf->length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf->m.offset);
    ASSERT(mem[i] != MAP_FAILED);
  }

  // queue the buffers
  for(int i = 0; i < frameBufferCount; ++i)
  {
    buf->index = i;
    buf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf->memory = V4L2_MEMORY_MMAP;
    VERIFY(ioctl(fd, VIDIOC_QBUF, buf) != -1);
  }

  // request camera's default control settings
  settings.exposure = getControlSetting(V4L2_CID_EXPOSURE);
  settings.brightness = getControlSetting(V4L2_CID_BRIGHTNESS);
  settings.red = getControlSetting(V4L2_CID_RED_BALANCE); 
  settings.blue = getControlSetting(V4L2_CID_BLUE_BALANCE); 
  settings.gain = getControlSetting(V4L2_CID_GAIN);
  // make sure automatic stuff is off
#ifndef NO_NAO_EXTENSIONS
  VERIFY(setControlSetting(V4L2_CID_AUTOEXPOSURE , 0));
  VERIFY(setControlSetting(V4L2_CID_AUTO_WHITE_BALANCE, 0));
  VERIFY(setControlSetting(V4L2_CID_AUTOGAIN, 0));
  VERIFY(setControlSetting(V4L2_CID_HFLIP, 0));
  VERIFY(setControlSetting(V4L2_CID_VFLIP, 0));
#else
  setControlSetting(V4L2_CID_AUTO_WHITE_BALANCE, 0);
  setControlSetting(V4L2_CID_AUTOGAIN, 0);
  setControlSetting(V4L2_CID_HUE_AUTO, 0);
  setControlSetting(V4L2_CID_EXPOSURE_AUTO, V4L2_EXPOSURE_MANUAL);
  setControlSetting(V4L2_CID_HFLIP, 0);
  setControlSetting(V4L2_CID_VFLIP, 0);
#endif

  // enable streaming
  int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  VERIFY(ioctl(fd, VIDIOC_STREAMON, &type) != -1);
}

NaoCamera::~NaoCamera()
{
  // disable streaming
  int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  VERIFY(ioctl(fd, VIDIOC_STREAMOFF, &type) != -1);

  // unmap buffers
  for(int i = 0; i < frameBufferCount; ++i)
    munmap(mem[i], memLength[i]);
  
  // close the device
  close(fd);
  free(buf);

  theInstance = 0;
}

bool NaoCamera::capturedNew()
{
  // requeue the buffer of the last captured image which is obselete now
  if(currentBuf) 
    VERIFY(ioctl(fd, VIDIOC_QBUF, currentBuf) != -1);
  
  // dequeue a frame buffer (this call blocks when there is no new image available) */
  VERIFY(ioctl(fd, VIDIOC_DQBUF, buf) != -1);
  ASSERT(buf->bytesused == SIZE);
  currentBuf = buf;
  timeStamp = storedTimeStamp + 33;
  storedTimeStamp = SystemCall::getCurrentSystemTime();

  return true;
}

const unsigned char* NaoCamera::getImage() const
{
  ASSERT(currentBuf);
  return static_cast<unsigned char*>(mem[currentBuf->index]);
}

unsigned int NaoCamera::getTimeStamp() const
{
  ASSERT(currentBuf);
  return timeStamp;
}

int NaoCamera::getControlSetting(unsigned int id)
{
  struct v4l2_queryctrl queryctrl;
  queryctrl.id = id;
  if(ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl) < 0)
    return -1;
  if(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
    return -1; // not available
  if(queryctrl.type != V4L2_CTRL_TYPE_BOOLEAN && queryctrl.type != V4L2_CTRL_TYPE_INTEGER && queryctrl.type != V4L2_CTRL_TYPE_MENU)
    return -1; // not supported

  struct v4l2_control control_s;
  control_s.id = id;
  if(ioctl(fd, VIDIOC_G_CTRL, &control_s) < 0) 
    return -1;
  if(control_s.value == queryctrl.default_value)
    return -1;
  return control_s.value;
}

bool NaoCamera::setControlSetting(unsigned int id, int value)
{
  struct v4l2_queryctrl queryctrl;
  queryctrl.id = id;
  if(ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl) < 0)
    return false;
  if(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
    return false; // not available
  if(queryctrl.type != V4L2_CTRL_TYPE_BOOLEAN && queryctrl.type != V4L2_CTRL_TYPE_INTEGER && queryctrl.type != V4L2_CTRL_TYPE_MENU)
    return false; // not supported

  // clip value
  if(value < queryctrl.minimum)
    value = queryctrl.minimum;
  if(value > queryctrl.maximum)
    value = queryctrl.maximum;
  if(value < 0)
    value = queryctrl.default_value;

  struct v4l2_control control_s;
  control_s.id = id;
  control_s.value = value;
  if(ioctl(fd, VIDIOC_S_CTRL, &control_s) < 0) 
    return false;
  return true;
}

void NaoCamera::setSettings(const CameraSettings& newset)
{
  if(newset.exposure != settings.exposure)
    setControlSetting(V4L2_CID_EXPOSURE, (settings.exposure = newset.exposure));
  if(newset.brightness != settings.brightness)
    setControlSetting(V4L2_CID_BRIGHTNESS, (settings.brightness = newset.brightness));
  if(newset.gain != settings.gain)
    setControlSetting(V4L2_CID_GAIN, (settings.gain = newset.gain));
  if(newset.blue != settings.blue)
    setControlSetting(V4L2_CID_BLUE_BALANCE, (settings.blue = newset.blue));
  if(newset.red != settings.red)
    setControlSetting(V4L2_CID_RED_BALANCE, (settings.red = newset.red));
}
