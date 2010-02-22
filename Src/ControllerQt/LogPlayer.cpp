/**
* @file ControllerQt/LogPlayer.cpp
*
* Implementation of class LogPlayer
*
* @author Martin Lötzsch
*/

#include "LogPlayer.h"
#include "Representations/Perception/JPEGImage.h"
#include "Platform/SystemCall.h"
#include "Platform/GTAssert.h"

LogPlayer::LogPlayer(MessageQueue& targetQueue) : 
targetQueue(targetQueue)
{
  init();
}

void LogPlayer::init()
{
  clear();
  stop();
  numberOfFrames = 0;
  numberOfMessagesWithinCompleteFrames = 0;
  state = initial;
  loop = true; //default: loop enabled
}

bool LogPlayer::open(const char* fileName)
{
  InBinaryFile file(fileName);
  if(file.exists())
  {
    clear();
    file >> *this;
    stop();
    countFrames();
    return true;
  }
  return false;
}

void LogPlayer::play()
{
  state = playing;
}

void LogPlayer::stop()
{
  if (state == recording)
  {
    recordStop();
    return;
  }
  currentMessageNumber = -1;
  currentFrameNumber = -1;
  state = initial;
}

void LogPlayer::pause()
{
  if(getNumberOfMessages() == 0)
    state = initial;
  else
    state = paused;
}

void LogPlayer::stepBackward()
{
  pause();
  if(state == paused && currentFrameNumber > 0)
  {
    do
      queue.setSelectedMessageForReading(--currentMessageNumber);
    while(currentMessageNumber > 0 && queue.getMessageID() != idProcessFinished);
    --currentFrameNumber;
    stepRepeat();
  }
}

void LogPlayer::stepForward()
{
  pause();
  if(state == paused && currentFrameNumber < numberOfFrames - 1)
  {
    do
      copyMessage(++currentMessageNumber, targetQueue);
    while(queue.getMessageID() != idProcessFinished);
    ++currentFrameNumber;
  }
}

void LogPlayer::stepRepeat()
{
  pause();
  if(state == paused && currentFrameNumber >= 0)
  {
    do
      queue.setSelectedMessageForReading(--currentMessageNumber);
    while(currentMessageNumber > 0 && queue.getMessageID() != idProcessFinished);
    --currentFrameNumber;
    stepForward();
  }
}

void LogPlayer::gotoFrame(int frame)
{
  pause();
  if(state == paused && frame < numberOfFrames)
  {
    currentFrameNumber = -1;
    currentMessageNumber = -1; 
    while(++currentMessageNumber < getNumberOfMessages() && frame > currentFrameNumber + 1)
    {
      queue.setSelectedMessageForReading(currentMessageNumber);
      if(queue.getMessageID() == idProcessFinished)
        ++currentFrameNumber;
    }
    stepForward();
  }
}

bool LogPlayer::save(const char* fileName)
{
  if(state == recording)
    recordStop();

  if(!getNumberOfMessages())
    return false;

  OutBinaryFile file(fileName);
  if(file.exists())
  {
    file << *this;
    return true;
  }
  return false;
}

bool LogPlayer::saveImages(const bool raw, const char* fileName)
{
  struct BmpHeader{
    unsigned long k0,k1,k2;
    unsigned long ofs1,ofs2;
    unsigned long xsiz,ysiz;
    unsigned long modbit;
    unsigned long z1;
    unsigned long len;
    unsigned long z2,z3,z4,z5;
  } bmpHeader;
  
  char name[512];
  char fname[512];
  strcpy(name,fileName);
  if ((strlen(name)>4)&&((strncmp(name+strlen(name)-4,".bmp",4)==0)||(strncmp(name+strlen(name)-4,".jpg",4)==0)))
  {
    *(name+strlen(name)-4) = 0;
  }
  if ((strlen(name)>4)&&((strncmp(name+strlen(name)-4,"_000",4)==0)||(strncmp(name+strlen(name)-4,"_001",4)==0)))
  {
    *(name+strlen(name)-4) = 0;
  }
  int i=0;
  for (currentMessageNumber=0; currentMessageNumber<getNumberOfMessages();currentMessageNumber++)
  {
    queue.setSelectedMessageForReading(currentMessageNumber);
    Image image;
    if(queue.getMessageID() == idImage)
    {
      in.bin >> image;
    }
    else if(queue.getMessageID() == idJPEGImage)
    {
      JPEGImage jpegImage;
      in.bin >> jpegImage;
      jpegImage.toImage(image);
    }
    else
      continue;

    Image rgbImage;
    if(!raw)
      rgbImage.convertFromYCbCrToRGB(image);

    sprintf(fname,"%s_%03i.bmp",name,i++);
    OutBinaryFile file(fname);
    if (file.exists())
    {
      long truelinelength=(3*rgbImage.cameraInfo.resolutionWidth+3) & 0xfffffc;
      char line[3*320+4];
      memset(line,0,truelinelength);
      bmpHeader.k0=0x4d420000;    //2 dummy bytes 4 alignment, then "BM"
      bmpHeader.k1=rgbImage.cameraInfo.resolutionHeight*truelinelength+0x36;
      bmpHeader.k2=0;
      bmpHeader.ofs1=0x36;
      bmpHeader.ofs2=0x28;
      bmpHeader.xsiz=rgbImage.cameraInfo.resolutionWidth;
      bmpHeader.ysiz=rgbImage.cameraInfo.resolutionHeight;
      bmpHeader.modbit=0x00180001;
      bmpHeader.z1=0;
      bmpHeader.len=truelinelength*rgbImage.cameraInfo.resolutionHeight;
      bmpHeader.z2=0;
      bmpHeader.z3=0;
      bmpHeader.z4=0;
      bmpHeader.z5=0;
      file.write(2+(char*)&bmpHeader,sizeof(bmpHeader)-2);
      for (int i=rgbImage.cameraInfo.resolutionHeight-1; i>=0; i--)
      {
        int ofs=0;
        if(raw)
        {
          for (int j=0;j<image.cameraInfo.resolutionWidth;j++)
          {
            line[ofs++]=image.image[i][j].cr;
            line[ofs++]=image.image[i][j].cb;
            line[ofs++]=image.image[i][j].y;
          }
        }
        else
        {
          for (int j=0;j<rgbImage.cameraInfo.resolutionWidth;j++)
          {
            line[ofs++]=rgbImage.image[i][j].b;
            line[ofs++]=rgbImage.image[i][j].g;
            line[ofs++]=rgbImage.image[i][j].r;
         }
        }
        file.write(line,truelinelength);
      }
    }
    else
    {
      stop();
      return false;
    }
  }
  stop();
  return true;
}

void LogPlayer::recordStart()
{
  state = recording;
}

void LogPlayer::recordStop()
{
  while(getNumberOfMessages() > numberOfMessagesWithinCompleteFrames)
    removeLastMessage();
  currentMessageNumber = -1;
  currentFrameNumber = -1;
  state = initial;
}


void LogPlayer::setLoop(bool loop_)
{
  loop = loop_;
}

void LogPlayer::handleMessage(InMessage& message)
{
  if(state == recording)
  {
    message >> *this;
    if(message.getMessageID() == idProcessFinished)
    {
      numberOfMessagesWithinCompleteFrames = getNumberOfMessages();
      ++numberOfFrames;
    }
  }
}

bool LogPlayer::replay()
{
  if(state == playing)
  {
    if(currentFrameNumber < numberOfFrames - 1)
    {
      do
        copyMessage(++currentMessageNumber, targetQueue);
      while(queue.getMessageID() != idProcessFinished);
      ++currentFrameNumber;
      if(currentFrameNumber == numberOfFrames - 1)
      {
        if (loop) //restart in loop mode
        {
          gotoFrame(0);
          play();
        }
        else
          stop();
      }
      return true;
    }
    else
    {
      if (loop) //restart in loop mode
      {
        gotoFrame(0);
        play();
      }
      else
        stop();
    }
  }
  return false;
}

void LogPlayer::keep(MessageID* messageIDs)
{
  LogPlayer temp((MessageQueue&) *this);
  moveAllMessages(temp);
  for(temp.currentMessageNumber = 0; temp.currentMessageNumber < temp.getNumberOfMessages(); ++temp.currentMessageNumber)
  {
    temp.queue.setSelectedMessageForReading(temp.currentMessageNumber);
    MessageID* m = messageIDs;
    while(*m)
    {
      if(temp.queue.getMessageID() == *m || 
         temp.queue.getMessageID() == idProcessBegin || 
         temp.queue.getMessageID() == idProcessFinished)
      {
        temp.copyMessage(temp.currentMessageNumber, *this);
        break;
      }
      ++m;
    }
  }
}

void LogPlayer::remove(MessageID* messageIDs)
{
  LogPlayer temp((MessageQueue&) *this);
  moveAllMessages(temp);
  for(temp.currentMessageNumber = 0; temp.currentMessageNumber < temp.getNumberOfMessages(); ++temp.currentMessageNumber)
  {
    temp.queue.setSelectedMessageForReading(temp.currentMessageNumber);
    MessageID* m = messageIDs;
    while(*m)
    {
      if(temp.queue.getMessageID() == *m || 
         temp.queue.getMessageID() == idProcessBegin || 
         temp.queue.getMessageID() == idProcessFinished)
        break;
      ++m;
    }
    if(!*m)
      temp.copyMessage(temp.currentMessageNumber, *this);
  }
}

void LogPlayer::statistics(int frequency[numOfDataMessageIDs])
{
  for(int i = 0; i < numOfDataMessageIDs; ++i)
    frequency[i] = 0;

  int current = queue.getSelectedMessageForReading();
  for(int i = 0; i < getNumberOfMessages(); ++i)
  {
    queue.setSelectedMessageForReading(i);
    ASSERT(queue.getMessageID() < numOfDataMessageIDs);
    ++frequency[queue.getMessageID()];
  }
  queue.setSelectedMessageForReading(current);
}

void LogPlayer::countFrames()
{
  numberOfFrames = 0;
  for(int i = 0; i < getNumberOfMessages(); ++i)
  {
    queue.setSelectedMessageForReading(i);
    if(queue.getMessageID() == idProcessFinished)
    {
      ++numberOfFrames;
      numberOfMessagesWithinCompleteFrames = i + 1;
    }
  }
}
