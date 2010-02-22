/** 
* @file BH2009LEDSymbols.cpp
*
* Implementation of class BH2009LEDSymbols.
*
* @author Judith Müller
* @author André Schreck
* @author <a href="mailto:Tim.Laue@dfki.de">Tim Laue</a>
*/

#include "BH2009LEDSymbols.h"
#include "BH2009TeamSymbols.h"
#include "Tools/Xabsl/GT/GTXabslTools.h"

PROCESS_WIDE_STORAGE BH2009LEDSymbols* BH2009LEDSymbols::theInstance = 0;

void BH2009LEDSymbols::registerSymbols(xabsl::Engine& engine)
{
  char s[256];

  // "led.request" Nao
  engine.registerEnumeratedOutputSymbol("led_request.faceLeftRed", "led_request.state", &setFaceLeftRed, &getFaceLeftRed);
  engine.registerEnumeratedOutputSymbol("led_request.faceLeftGreen", "led_request.state", &setFaceLeftGreen, &getFaceLeftGreen);
  engine.registerEnumeratedOutputSymbol("led_request.faceLeftBlue", "led_request.state", &setFaceLeftBlue, &getFaceLeftBlue);

  engine.registerEnumeratedOutputSymbol("led_request.faceRightRed", "led_request.state", &setFaceRightRed, &getFaceRightRed);
  engine.registerEnumeratedOutputSymbol("led_request.faceRightGreen", "led_request.state", &setFaceRightGreen, &getFaceRightGreen);
  engine.registerEnumeratedOutputSymbol("led_request.faceRightBlue", "led_request.state", &setFaceRightBlue, &getFaceRightBlue);

  engine.registerEnumeratedOutputSymbol("led_request.chestRed", "led_request.state", (int*)&ledRequest.ledStates[LEDRequest::chestRed]);
  engine.registerEnumeratedOutputSymbol("led_request.chestGreen", "led_request.state", (int*)&ledRequest.ledStates[LEDRequest::chestGreen]);
  engine.registerEnumeratedOutputSymbol("led_request.chestBlue", "led_request.state", (int*)&ledRequest.ledStates[LEDRequest::chestBlue]);

  engine.registerEnumeratedOutputSymbol("led_request.footLeftRed", "led_request.state", (int*)&ledRequest.ledStates[LEDRequest::footLeftRed]);
  engine.registerEnumeratedOutputSymbol("led_request.footLeftGreen", "led_request.state", (int*)&ledRequest.ledStates[LEDRequest::footLeftGreen]);
  engine.registerEnumeratedOutputSymbol("led_request.footLeftBlue", "led_request.state", (int*)&ledRequest.ledStates[LEDRequest::footLeftBlue]);

  engine.registerEnumeratedOutputSymbol("led_request.footRightRed", "led_request.state", (int*)&ledRequest.ledStates[LEDRequest::footRightRed]);
  engine.registerEnumeratedOutputSymbol("led_request.footRightGreen", "led_request.state", (int*)&ledRequest.ledStates[LEDRequest::footRightGreen]);
  engine.registerEnumeratedOutputSymbol("led_request.footRightBlue", "led_request.state", (int*)&ledRequest.ledStates[LEDRequest::footRightBlue]);

  engine.registerEnumeratedOutputSymbol("led_request.earLeft.complete", "led_request.state", &setEarLeftComplete, &getFirstLeftEarLED);
  engine.registerEnumeratedOutputSymbol("led_request.earRight.complete", "led_request.state", &setEarRightComplete, &getFirstRightEarLED);
  engine.registerEnumeratedOutputSymbol("led_request.earRight.battery", "led_request.state", &setEarRightBattery, &getFirstRightEarLED);
  engine.registerEnumeratedOutputSymbol("led_request.earLeft.ballSeen", "led_request.state", &setEarLeftBallSeen, &getFirstLeftEarLED);
  engine.registerEnumeratedOutputSymbol("led_request.earLeft.connectedPlayers", "led_request.state", &setEarLeftConntectedPlayers, &getFirstLeftEarLED);
  
  for(int i = 0; i < LEDRequest::numOfLEDStates; ++i)
  {
    sprintf(s, "led_request.state.");
    getXabslString(s + strlen(s), LEDRequest::getLEDStateName((LEDRequest::LEDState) i));
    engine.registerEnumElement("led_request.state", s, i);
  }
}


void BH2009LEDSymbols::update()
{
  // Reset all LEDs:
  for(int i = 0; i < ledRequest.numOfLEDs; ++i)
    ledRequest.ledStates[i] = LEDRequest::off;
  // Update ball statistics:
  ballStatistics.add(theFrameInfo.getTimeSince(theBallModel.timeWhenLastSeen) == 0 ? 1 : 0);
}


void BH2009LEDSymbols::setFaceLeftRed(int state)
{
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceLeftRed0Deg]))   = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceLeftRed45Deg]))  = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceLeftRed90Deg]))  = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceLeftRed135Deg])) = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceLeftRed180Deg])) = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceLeftRed225Deg])) = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceLeftRed270Deg])) = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceLeftRed315Deg])) = state;
}


void BH2009LEDSymbols::setFaceLeftGreen(int state)
{
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceLeftGreen0Deg]))   = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceLeftGreen45Deg]))  = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceLeftGreen90Deg]))  = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceLeftGreen135Deg])) = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceLeftGreen180Deg])) = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceLeftGreen225Deg])) = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceLeftGreen270Deg])) = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceLeftGreen315Deg])) = state;
}


void BH2009LEDSymbols::setFaceLeftBlue(int state)
{
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceLeftBlue0Deg]))   = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceLeftBlue45Deg]))  = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceLeftBlue90Deg]))  = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceLeftBlue135Deg])) = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceLeftBlue180Deg])) = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceLeftBlue225Deg])) = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceLeftBlue270Deg])) = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceLeftBlue315Deg])) = state;
}


void BH2009LEDSymbols::setFaceRightRed(int state)
{
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceRightRed0Deg]))   = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceRightRed45Deg]))  = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceRightRed90Deg]))  = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceRightRed135Deg])) = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceRightRed180Deg])) = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceRightRed225Deg])) = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceRightRed270Deg])) = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceRightRed315Deg])) = state;
}


void BH2009LEDSymbols::setFaceRightGreen(int state)
{
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceRightGreen0Deg]))   = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceRightGreen45Deg]))  = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceRightGreen90Deg]))  = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceRightGreen135Deg])) = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceRightGreen180Deg])) = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceRightGreen225Deg])) = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceRightGreen270Deg])) = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceRightGreen315Deg])) = state;
}


void BH2009LEDSymbols::setFaceRightBlue(int state)
{
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceRightBlue0Deg]))   = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceRightBlue45Deg]))  = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceRightBlue90Deg]))  = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceRightBlue135Deg])) = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceRightBlue180Deg])) = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceRightBlue225Deg])) = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceRightBlue270Deg])) = state;
  *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::faceRightBlue315Deg])) = state;
}


void BH2009LEDSymbols::setEarLeftComplete(int ledState)
{
  for(int i=0; i<10; ++i)
    *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::earsLeft0Deg + i])) = ledState;
}


void BH2009LEDSymbols::setEarRightComplete(int ledState)
{
  for(int i=0; i<10; ++i)
    *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::earsRight0Deg + i])) = ledState;
}


void BH2009LEDSymbols::setEarRightBattery(int ledState)
{
  int onLEDs = static_cast<int>(theInstance->theFilteredSensorData.data[FilteredSensorData::batteryLevel] / 0.1F);
  if(onLEDs >= 10)
    onLEDs = 9;
  for(int i=0; i<=onLEDs; ++i)
    *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::earsRight0Deg + i])) = ledState;
}


void BH2009LEDSymbols::setEarLeftBallSeen(int ledState)
{
  int onLEDs = theInstance->ballStatistics.getSum();
  for(int i=0; i<onLEDs; ++i)
    *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::earsLeft0Deg + i])) = ledState;
}


void BH2009LEDSymbols::setEarLeftConntectedPlayers(int ledState)
{
  for(unsigned i=0; i<theInstance->theTeamMateData.numOfConnectedPlayers*5; ++i)
    *((int*)(&theInstance->ledRequest.ledStates[LEDRequest::earsLeft0Deg + i])) = ledState;
}
