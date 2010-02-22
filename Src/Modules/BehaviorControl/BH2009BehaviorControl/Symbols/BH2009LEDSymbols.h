/** 
* @file BH2009LEDSymbols.h
*
* Declaration of class BH2009LEDSymbols.
*
* @author Judith Müller
* @author André Schreck
* @author <a href="mailto:Tim.Laue@dfki.de">Tim Laue</a>
*/

#ifndef __BH2009LEDSymbols_h_
#define __BH2009LEDSymbols_h_

#include "../../Symbols.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/LEDRequest.h"
#include "Representations/Infrastructure/SensorData.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/ObstacleModel.h"
#include "Representations/Infrastructure/TeamMateData.h"
#include "Tools/RingBufferWithSum.h"

/**
* The Xabsl symbols that are defined in "led_symbols.xabsl"
*
* @author Judith Müller
*/ 
class BH2009LEDSymbols : public Symbols
{
public:
  /* Constructor */
  BH2009LEDSymbols(LEDRequest& ledRequest, const FilteredSensorData& theFilteredSensorData,
    const BallModel& theBallModel, const FrameInfo& theFrameInfo, const TeamMateData& theTeamMateData): 
  ledRequest(ledRequest), theFilteredSensorData(theFilteredSensorData), theBallModel(theBallModel),
    theFrameInfo(theFrameInfo), theTeamMateData(theTeamMateData)
  {
    theInstance = this;
  }

  /** Registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  /** Update function, called every cycle */
  void update();

private:
  /** Points to the only instance of this class in this process or is 0 if there is none. */
  PROCESS_WIDE_STORAGE_STATIC BH2009LEDSymbols* theInstance; 
  /** A reference to the LEDRequest */
  LEDRequest& ledRequest;
  /** A reference to the sensor data (for battery status) */
  const FilteredSensorData& theFilteredSensorData;
  /** A reference to the ball model (for ball statistics) */
  const BallModel& theBallModel;
  /** A reference to the current frame information */
  const FrameInfo& theFrameInfo;
  /** A reference to the current team mate data */
  const TeamMateData& theTeamMateData;
  /** Keep ball observations during last 10 cycles */
  RingBufferWithSum<int, 10> ballStatistics;

  static void setFaceLeftRed(int ledState);
  static int getFaceLeftRed() { return (int)theInstance->ledRequest.ledStates[LEDRequest::faceLeftRed0Deg]; }

  static void setFaceLeftGreen(int ledState);
  static int getFaceLeftGreen() { return (int)theInstance->ledRequest.ledStates[LEDRequest::faceLeftGreen0Deg]; }

  static void setFaceLeftBlue(int ledState);
  static int getFaceLeftBlue() { return (int)theInstance->ledRequest.ledStates[LEDRequest::faceLeftBlue0Deg]; }

  static void setFaceRightRed(int ledState);
  static int getFaceRightRed() { return (int)theInstance->ledRequest.ledStates[LEDRequest::faceRightRed0Deg]; }

  static void setFaceRightGreen(int ledState);
  static int getFaceRightGreen() { return (int)theInstance->ledRequest.ledStates[LEDRequest::faceRightGreen0Deg]; }

  static void setFaceRightBlue(int ledState);
  static int getFaceRightBlue() { return (int)theInstance->ledRequest.ledStates[LEDRequest::faceRightBlue0Deg]; }

  static void setEarLeftComplete(int ledState);
  static void setEarRightComplete(int ledState);
  static void setEarRightBattery(int ledState);
  static void setEarLeftBallSeen(int ledState);
  static void setEarLeftConntectedPlayers(int ledState);
  
  static int getFirstRightEarLED() {return (int)theInstance->ledRequest.ledStates[LEDRequest::earsRight0Deg];}
  static int getFirstLeftEarLED() {return (int)theInstance->ledRequest.ledStates[LEDRequest::earsLeft0Deg];}
};


#endif // __LEDSymbols_h_
