/**
* \file Platform/linux/NaoCamera.h
* Interface to the Nao camera using linux-uvc.
* \author Colin Graf
*/

#ifndef _NaoCamera_H_
#define _NaoCamera_H_

#include "Thread.h"
#include "Representations/Configuration/CameraSettings.h"
#include "Tools/Optimization/HillClimbing.h"


/**
* \class NaoCamera
* Interface class to the Nao camera.
*/
class NaoCamera
{
public:

  /** Constructor. */
  NaoCamera();

  /** Destructor. */
  ~NaoCamera();

  /**
  * Sets the camera control settings to the camera.
  * \param settings The settings.
  */
  void setSettings(const CameraSettings& settings);
  
  /**
  * Requests the current camera control settings of the camera.
  * \return The settings.
  */
  const CameraSettings& getSettings() const { return settings; };

  /** 
  * The method blocks till a new image arrives.
  * \return true (except a not manageable exception occurs) 
  */
  bool capturedNew();

  /**
  * The last captured image.
  * \return The image data buffer.
  */
  const unsigned char* getImage() const;

  /** 
  * Timestamp of the last captured image. 
  * \return The timestamp.
  */
  unsigned getTimeStamp() const;

private:
  static NaoCamera* theInstance; /**< The only instance of this class. */

  CameraSettings settings; /**< The camera control settings. */

  enum 
  {
    frameBufferCount = 3, /**< Amount of available frame buffers. */
    WIDTH = 640,
    HEIGHT = 480,
    SIZE = WIDTH * HEIGHT * 2
  };
  int fd; /**< The file descriptor for the video device. */
  void* mem[frameBufferCount]; /**< Frame buffer addresses. */
  int memLength[frameBufferCount]; /**< The length of each frame buffer. */
  struct v4l2_buffer* buf; /**< Reusable parameter struct for some ioctl calls. */
  struct v4l2_buffer* currentBuf; /**< The last dequeued frame buffer. */
  unsigned timeStamp, /**< Timestamp of the last captured image. */
           storedTimeStamp; /**< Timestamp when the next image recording starts. */
  
  /**
  * Requests the value of a camera control setting from camera.
  * \param id The setting id.
  * \return The value.
  */
  int getControlSetting(unsigned int id);

  /**
  * Sets the value of a camera control setting to camera.
  * \param id The setting id.
  * \param value The value.
  * \return True on success.
  */
  bool setControlSetting(unsigned int id, int value);
};

#endif // _NaoCamera_H_
