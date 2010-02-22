/** 
* @file WhiteCorrection.cpp
* This file implements a module that can provide white corrected cameraSettings
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
* @author <a href="mailto:ingsie@informatik.uni-bremen.de">Ingo Sieverdingbeck</a>
*/

#include "WhiteCorrection.h"
#include "Tools/Settings.h"
#include "Tools/Streams/InStreams.h"
#include "Tools/Streams/OutStreams.h"
#include <limits>

WhiteCorrection::WhiteCorrection() :
theCameraSettings(0),
optimizationTerminated(false),
optimizationStarted(false)
{
  readCameraSettings();
}

WhiteCorrection::~WhiteCorrection()
{
  if(theCameraSettings)
  {
    delete theCameraSettings;
  }
}

void WhiteCorrection::update(CameraSettings& cameraSettings)
{
  readIfRequired();
  ASSERT(loadedCameraSettings);
  DEBUG_RESPONSE("module:WhiteCorrection:printColorValues", 
  {
    if (theImage.timeStamp != 0)
    {
      printColorValues();
    }
  });
  DEBUG_RESPONSE("module:WhiteCorrection:reset", 
  {
    if (theImage.timeStamp != 0)
    {
      resetWhiteCorrection();
    }
  });
  DEBUG_RESPONSE("module:WhiteCorrection:start", 
  {
    if (theImage.timeStamp != 0)
    {
      if (!optimizationStarted)
      {
        optimizationStarted = true;
        redBlue[0] = cameraSettings.red;
        redBlue[1] = cameraSettings.blue;
      }
      findWhiteCorrectionParameters();
      cameraSettings.red = redBlue[0];
      cameraSettings.blue = redBlue[1];
    }
  });
  if(theCameraSettings)
  {
    cameraSettings = *theCameraSettings;
    delete theCameraSettings;
    theCameraSettings = 0;
  }
}

/** Computes the difference between the three color channels for white correction
 *  @return The average difference
 */
double WhiteCorrection::averageYCbCrDifferenceOfCurrentImage() const
{
  // We take 19*19 pixels, thus we divide the image in 20x20 areas
  int colSkip(theImage.cameraInfo.resolutionWidth * 2 / 20);
  int lineSkip(colSkip*10*theImage.cameraInfo.resolutionHeight/20);
  int ySum(0), cbSum(0), crSum(0);
  Image::Pixel* address = &(theImage.image[0][0]);
  address += lineSkip + colSkip;
  for(int y=0; y<19; ++y)
  {
    for(int x=0; x<19; ++x)
    {
      ySum += address->y;
      cbSum += address->cb;
      crSum += address->cr;
      address += colSkip;
    }
    address += lineSkip + colSkip;
  }
  double yDiff = fabs(255.0 - (static_cast<double>(ySum) / (19.0*19.0)));
  double cbDiff = fabs(128.0 - (static_cast<double>(cbSum) / (19.0*19.0)));
  double crDiff = fabs(128.0 - (static_cast<double>(crSum) / (19.0*19.0)));
  return yDiff*cbDiff + yDiff*crDiff + cbDiff*crDiff;
}
          
void WhiteCorrection::printColorValues() const
{
  // We take 19*19 pixels, thus we divide the image in 20x20 areas
  int colSkip(theImage.cameraInfo.resolutionWidth * 2 / 20);
  int lineSkip(colSkip*10*theImage.cameraInfo.resolutionHeight/20);
  int rSum(0), gSum(0), bSum(0);
  int ySum(0), cbSum(0), crSum(0);
  Image::Pixel* address = &(theImage.image[0][0]);
  address += lineSkip + colSkip;
  for(int y=0; y<19; ++y)
  { 
    for(int x=0; x<19; ++x)
    { 
      unsigned char Y = address->y;
      unsigned char cb = address->cb;
      unsigned char cr = address->cr;
      int r = Y + ((1436 * (cr - 128)) >> 10),
          g = Y - ((354 * (cb - 128) + 732 * (cr - 128)) >> 10),
          b = Y + ((1814 * (cb - 128)) >> 10);
      ySum += Y;
      cbSum += cb;
      crSum += cr;
      if(r < 0) r = 0; else if(r > 255) r = 255;
      if(g < 0) g = 0; else if(g > 255) g = 255;
      if(b < 0) b = 0; else if(b > 255) b = 255;
      rSum += r;
      gSum += g;
      bSum += b;
      address += colSkip;
    }
    address += lineSkip + colSkip;
  }
  double RGDiff = fabs(static_cast<double>(rSum) - static_cast<double>(gSum)) / (19.0*19.0);
  double RBDiff = fabs(static_cast<double>(rSum) - static_cast<double>(bSum)) / (19.0*19.0);
  double GBDiff = fabs(static_cast<double>(gSum) - static_cast<double>(bSum)) / (19.0*19.0);
  double yDiff = fabs(255.0 - (static_cast<double>(ySum) / (19.0*19.0)));
  double cbDiff = fabs(128.0 - (static_cast<double>(cbSum) / (19.0*19.0)));
  double crDiff = fabs(128.0 - (static_cast<double>(crSum) / (19.0*19.0)));
  ySum /= (19*19);
  cbSum /= (19*19);
  crSum /= (19*19);
  rSum /= (19*19);
  gSum /= (19*19);
  bSum /= (19*19);
  OUTPUT(idText, text, "RGB color values: " << rSum << " " << gSum << " " << bSum);
  OUTPUT(idText, text, "RGB diff values: RG: " << RGDiff << " RB: " << RBDiff << " GB: " << GBDiff);
  OUTPUT(idText, text, "YCbCr color values: " << ySum << " " << cbSum << " " << crSum);
  OUTPUT(idText, text, "YCbCr diff values: " << yDiff << " " << cbDiff << " " << crDiff);
}


void WhiteCorrection::resetWhiteCorrection()
{
  optimizer.uninitialize();
  optimizationTerminated = false;
  optimizationStarted = false;
  loadedCameraSettings = false;
  readIfRequired();
  timeStampLastWhiteCorrection = 0;
  int numOfRuns = 5;
  int maxUnchangedSteps = 64;
  int minValues[] = {0,0};
  int maxValues[] = {255,255};
  int maxStepSizes[] = {32,32};
  optimizer.init(redBlue, minValues, maxValues, maxStepSizes,
    10, std::numeric_limits<double>::max(), HillClimbing<int, double, 2>::MINIMIZE, numOfRuns, maxUnchangedSteps);
}

void WhiteCorrection::findWhiteCorrectionParameters()
{ 
  if (optimizationTerminated)
  { 
    return;
  }
  // Check for initialization:
  if(!optimizer.isInitialized())
  { 
    resetWhiteCorrection();
  }
  if(theImage.timeStamp - timeStampLastWhiteCorrection > 500) // Every 500 ms
  { 
    optimizer.setReward(averageYCbCrDifferenceOfCurrentImage());
    optimizer.getNextParameterSet(redBlue);
    timeStampLastWhiteCorrection = theImage.timeStamp;
    if(optimizer.wasNewHillDuringLastExecution())
    { 
      int bestParameters[2];
      double bestReward;
      optimizer.getBestParameterSetAndReward(bestParameters, bestReward);
      int maxStepSize[2];
      optimizer.getMaxStepSize(maxStepSize);
      OUTPUT(idText, text, "New hill  *********");
      OUTPUT(idText, text, "Red: "<<bestParameters[0]<<"   Blue: "<<bestParameters[1]);
      OUTPUT(idText, text, "Reward: "<<bestReward);
      OUTPUT(idText, text, "step size red: " << maxStepSize[0] << "    step size blue: " << maxStepSize[1]);
      printColorValues();
    }
    else
    { 
      static int oldRunCount=-1;
      int runCount = optimizer.getRunCount();
      if (runCount!=oldRunCount)
      { 
        oldRunCount=runCount;
        int maxStepSize[2];
        optimizer.getMaxStepSize(maxStepSize);
        int unchangedSteps = optimizer.getUnchangedSteps();
        OUTPUT(idText, text, "Optimizer status");
        OUTPUT(idText, text, "step size red: " << maxStepSize[0] << "    step size blue: " << maxStepSize[1]);
        OUTPUT(idText, text, "run: " << runCount << "    unchanged steps: " << unchangedSteps);
      }
    }
  }
  else
  { 
    optimizer.getCurrentParameterSet(redBlue);
  }
  // Check for termination:
  if(optimizer.hasReachedMinimum() && !optimizationTerminated)
  { 
    
    optimizationTerminated=true;
    int bestParameters[2];
    double bestReward;
    optimizer.getBestParameterSetAndReward(bestParameters, bestReward);
    OUTPUT(idText, text, "Optimization terminated. *********");
    OUTPUT(idText, text, "Red: "<<bestParameters[0]<<"   Blue: "<<bestParameters[1]);
    OUTPUT(idText, text, "Reward: "<<bestReward);
    printColorValues();
  }
}


void WhiteCorrection::readIfRequired()
{
  if(loadedCameraSettings == false || robot != Global::getSettings().robot)
  {
    robot = Global::getSettings().robot;
    readCameraSettings();
  }
}


void WhiteCorrection::readCameraSettings()
{
  loadedCameraSettings = false;
  InConfigFile* stream = new InConfigFile(Global::getSettings().expandRobotLocationFilename("camera.cfg"));
  if(!stream->exists())
  {
    delete stream;
    stream = new InConfigFile(Global::getSettings().expandLocationFilename("camera.cfg"));
  }

  if(stream->exists())
  {
    if(!theCameraSettings)
      theCameraSettings = new CameraSettings;
    *stream >> *theCameraSettings;
    loadedCameraSettings = true;
  }

  delete stream;
}

MAKE_MODULE(WhiteCorrection, Infrastructure)

