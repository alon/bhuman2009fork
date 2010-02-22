/**
* @file ControllerQt/Representations/ColorTableHandler.cpp
*
* Implementation of class ColorTableHandler
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#include "ColorTableHandler.h"
#include "Platform/SystemCall.h"
#include "Tools/Settings.h"
#include "Tools/Math/Common.h"
#include "Tools/Streams/OutStreams.h"
#include "Tools/MessageQueue/OutMessage.h"
#include "Platform/GTAssert.h"

ColorTableHandler::ColorTableHandler()
: colorTable(*new ColorTable64),
  timeStamp(0),
  selectedColorClass(ColorClasses::none),
  maskColorClass(ColorClasses::none),
  colorSpaceRadius(2),
  imageRadius(0),
  active(false),
  replace(true),
  smart(true)
{
}

ColorTableHandler::~ColorTableHandler()
{
  for(unsigned i = 0; i < buffer.size(); ++i)
    delete [] (char*)buffer[i];
  delete &colorTable;
}

bool ColorTableHandler::handleMessage(InMessage& message)
{
  if(message.getMessageID() == idColorTable64)
  {
    message.bin >> colorTable;
    // this is a hack: avoid sending color table to robot, but send any change 
    // that may be done innediately after this function (see class RobotConsole).
    timeStamp = SystemCall::getCurrentSystemTime() - 1;
    return true;
  }
  else
    return false;
}

void ColorTableHandler::push()
{
  OutBinarySize size;
  size << colorTable;
  if(buffer.size() == 10)
  {
    delete [] (char*)buffer.front();
    buffer.pop_front();
  }
  buffer.push_back(new char[size.getSize()]);
  OutBinaryMemory stream(buffer.back());
  stream << colorTable;
  timeStamp = SystemCall::getCurrentSystemTime();
}

void ColorTableHandler::undo()
{
  if(!buffer.empty())
  {
    InBinaryMemory stream(buffer.back());
    stream >> colorTable;
    delete [] (char*)buffer.back();
    buffer.pop_back();
  }
  timeStamp = SystemCall::getCurrentSystemTime();
}

void ColorTableHandler::addPixel(const Image& image, const Vector2<int>& pos)
{
  // disable smart mode for add pixel;
  bool smart = this->smart;
  this->smart = false;

  addRectangle(image, pos - Vector2<int>(imageRadius, imageRadius), pos + Vector2<int>(imageRadius, imageRadius));

  // set smart mode back to previous state
  this->smart = smart;
}

void ColorTableHandler::addRectangle(const Image& image, const Vector2<int>& topLeft, const Vector2<int>& bottomRight)
{
  if(smart)
    addSegment(image,topLeft,bottomRight);
  else
  {
    push();
    for(int y = std::max(0, topLeft.y); y <= std::min(bottomRight.y, image.cameraInfo.resolutionHeight - 1); ++y)
      for(int x = std::max(0, topLeft.x); x <= std::min(bottomRight.x, image.cameraInfo.resolutionWidth - 1); ++x)
      {
        if(maskColorClass == ColorClasses::none)
        {
          if(replace)
            colorTable.addColorClass(ColorClasses::none, image.image[y][x].y, image.image[y][x].cb, image.image[y][x].cr, colorSpaceRadius * 2 + 1);
          colorTable.addColorClass(selectedColorClass, image.image[y][x].y, image.image[y][x].cb, image.image[y][x].cr, colorSpaceRadius * 2 + 1);
        }
        else if(replace)
          colorTable.replaceColorClass(maskColorClass, selectedColorClass, image.image[y][x].y, image.image[y][x].cb, image.image[y][x].cr, colorSpaceRadius * 2 + 1);
        else
          colorTable.replaceColorClass(maskColorClass, ColorClasses::Color(1 << (selectedColorClass - 1) | 1 << (maskColorClass - 1) | 0x80),
                                       image.image[y][x].y, image.image[y][x].cb, image.image[y][x].cr, colorSpaceRadius * 2 + 1);
      }
   }
}

void ColorTableHandler::addSegment(const Image& image, const Vector2<int>& topLeft, const Vector2<int>& bottomRight)
{
  push();
  int range = colorSpaceRadius*4;

  unsigned char imageBin[cameraResolutionHeight][cameraResolutionWidth];
  for(int y = cameraResolutionHeight-1; y >= 0 ; y--)
    for(int x = cameraResolutionWidth-1; x >= 0; x--)
      imageBin[y][x] = 0;

  unsigned int avgY = 0, avgCr = 0, avgCb = 0, count = 0;
  for(int y = std::max(0, topLeft.y); y <= std::min(bottomRight.y, image.cameraInfo.resolutionHeight - 1); ++y)
    for(int x = std::max(0, topLeft.x); x <= std::min(bottomRight.x, image.cameraInfo.resolutionWidth - 1); ++x)
    {
      imageBin[y][x] = 1;
      Image::Pixel& get = image.image[y][x];
      avgY += get.y;
      avgCr += get.cr;
      avgCb += get.cb;
      count++;
    }

  if(!count)
    return;

  Image::Pixel cur;
  cur.y = avgY/count;
  cur.cr = avgCr/count;
  cur.cb = avgCb/count;

  
  for(int i = 0; i < 75; i++)
  {
    bool hasChanged = false;

    for(int y = cameraResolutionHeight-2; y >= 1; y--)
      for(int x = cameraResolutionWidth-2; x >= 1; x--)
      {
        if(imageBin[y][x])
        {
          Image::Pixel& getB = image.image[y][x];
          if(int(Image::getColorDistance(getB,cur)) > range)
          { imageBin[y][x] = 0; hasChanged = true; avgY -= getB.y; avgCb -= getB.cb; avgCr -= getB.cr; count--; }
          else
          {
            y--;
            {Image::Pixel& get = image.image[y][x];
            if(!imageBin[y][x] && int(Image::getColorDistance(get,cur)) <= range)
            { imageBin[y][x] = 1; hasChanged = true; avgY += get.y; avgCb += get.cb; avgCr += get.cr; count++; }
            }y++;
            x--;
            {Image::Pixel& get = image.image[y][x];
            if(!imageBin[y][x] && int(Image::getColorDistance(get,cur)) <= range)
            { imageBin[y][x] = 1; hasChanged = true; avgY += get.y; avgCb += get.cb; avgCr += get.cr; count++; }
            }x += 2;
            {Image::Pixel& get = image.image[y][x];
            if(!imageBin[y][x] && int(Image::getColorDistance(get,cur)) <= range)
            { imageBin[y][x] = 1; hasChanged = true; avgY += get.y; avgCb += get.cb; avgCr += get.cr; count++; }
            }x--;
            y++; 
            {Image::Pixel& get = image.image[y][x];
            if(!imageBin[y][x] && int(Image::getColorDistance(get,cur)) <= range)
            { imageBin[y][x] = 1; hasChanged = true; avgY += get.y; avgCb += get.cb; avgCr += get.cr; count++; }
            }y--; 
          }
        }
      }

    if(!count)
      break;

    cur.y = avgY/count;
    cur.cb = avgCb/count;
    cur.cr = avgCr/count;

    if(!hasChanged)
      break;
  }

  for(int y = cameraResolutionHeight-1; y >= 0 ; y--)
    for(int x = cameraResolutionWidth-1; x >= 0; x--)
      if(imageBin[y][x])
      {
        if(maskColorClass == ColorClasses::none)
        {
          if(replace)
            colorTable.addColorClass(ColorClasses::none, image.image[y][x].y, image.image[y][x].cb, image.image[y][x].cr);
          colorTable.addColorClass(selectedColorClass, image.image[y][x].y, image.image[y][x].cb, image.image[y][x].cr);
        }
        else if(replace)
          colorTable.replaceColorClass(maskColorClass, selectedColorClass, image.image[y][x].y, image.image[y][x].cb, image.image[y][x].cr);
        else
          colorTable.replaceColorClass(maskColorClass, ColorClasses::Color(1 << (selectedColorClass - 1) | 1 << (maskColorClass - 1) | 0x80),
                                       image.image[y][x].y, image.image[y][x].cb, image.image[y][x].cr);
      }
}

void ColorTableHandler::clear()
{
  push();
  colorTable.clear();
}

void ColorTableHandler::clear(ColorClasses::Color color)
{
  push();
  colorTable.clearChannel(color);
}

void ColorTableHandler::clearPixel(const Image& image, const Vector2<int>& pos)
{
  clearRectangle(image, pos - Vector2<int>(imageRadius, imageRadius), pos + Vector2<int>(imageRadius, imageRadius));
}

void ColorTableHandler::clearRectangle(const Image& image, const Vector2<int>& topLeft, const Vector2<int>& bottomRight)
{
  push();
  for(int y = std::max(0, topLeft.y); y <= std::min(bottomRight.y, image.cameraInfo.resolutionHeight - 1); ++y)
    for(int x = std::max(0, topLeft.x); x <= std::min(bottomRight.x, image.cameraInfo.resolutionWidth - 1); ++x)
      colorTable.replaceColorClass(selectedColorClass, ColorClasses::none, image.image[y][x].y, image.image[y][x].cb, image.image[y][x].cr, colorSpaceRadius * 2 + 1);
}

void ColorTableHandler::fillRectangle(const Image& image, const Vector2<int>& topLeft, const Vector2<int>& bottomRight)
{
  push();
  for(int y = std::max(0, topLeft.y); y <= std::min(bottomRight.y, image.cameraInfo.resolutionHeight - 1); ++y)
    for(int x = std::max(0, topLeft.x); x <= std::min(bottomRight.x, image.cameraInfo.resolutionWidth - 1); ++x)
      colorTable.replaceColorClass(ColorClasses::none, selectedColorClass, image.image[y][x].y, image.image[y][x].cb, image.image[y][x].cr, colorSpaceRadius * 2 + 1);
}

void ColorTableHandler::clearPixels(const Image& image, ColorClasses::Color color)
{
  push();
  for(int y = 0; y < image.cameraInfo.resolutionHeight; ++y)
    for(int x = 0; x < image.cameraInfo.resolutionWidth; ++x)
      colorTable.replaceColorClass(selectedColorClass, ColorClasses::none, image.image[y][x].y, image.image[y][x].cb, image.image[y][x].cr, 1);
}

bool ColorTableHandler::load(const std::string& fileName)
{
  push();
  std::string name(fileName);
  if(name.find_first_of('.') == std::string::npos )
    name += ".c64";
  if(name[0] != '/' && name[0] != '\\' && (name.size() < 2 || name[1] != ':'))
    name = Global::getSettings().expandLocationFilename(name.c_str());
  InBinaryFile stream(name);
  if(stream.exists())
  {
    stream >> colorTable;
    return true;
  }
  else
    return false;
}

void ColorTableHandler::save(const std::string& fileName) const
{
  std::string name(fileName);
  if(name.find_first_of('.') == std::string::npos)
    name += ".c64";
  if(name[0] != '/' && name[0] != '\\' && (name.size() < 2 || name[1] != ':'))
    name = Global::getSettings().expandLocationFilename(name.c_str());
  OutBinaryFile stream(name);
  stream << colorTable;
}

void ColorTableHandler::send(OutMessage& message) const
{
  message.bin << colorTable;
  message.finishMessage(idColorTable64);
}

void ColorTableHandler::sendAndWrite(OutMessage& message) const
{
  message.bin << colorTable;
  message.finishMessage(idWriteColorTable64);
}

void ColorTableHandler::shrink()
{
  push();
  colorTable.shrink();
}

void ColorTableHandler::shrink(unsigned char color)
{
  push();
  colorTable.shrink(color);
}

void ColorTableHandler::grow()
{
  push();
  colorTable.grow();
}

void ColorTableHandler::grow(unsigned char color)
{
  push();
  colorTable.grow(color);
}

void ColorTableHandler::set(const ColorTable64& colorTable)
{
  push();
  this->colorTable = colorTable;
}

