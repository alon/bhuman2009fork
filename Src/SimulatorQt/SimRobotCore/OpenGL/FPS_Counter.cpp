/**
 * @file FPS_Counter.cpp
 *
 * Definition of class FPS Counter
 *
 * @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
 */

#include <time.h>
#include "FPS_Counter.h"

FPS_Counter::FPS_Counter()
{
  reset();
}

void FPS_Counter::reset()
{
  secondBeginTick = clock();
  tenSecondBeginTick = clock();

  frames = 0;
  frameTicks = 0;
  averageFrames = 0;
  current = 0.0f;
  average = 0.0f;
  possible = 0.0f;
  frametime = 0.0f;
}

void FPS_Counter::begin()
{
  frameBeginTick = clock();
}

void FPS_Counter::end()
{
  frames++;
  averageFrames++;
  frameTicks += clock() - frameBeginTick;

  if(clock() - secondBeginTick > 1000)
  {
    current = (float)(frames);
    frames = 0;
    secondBeginTick = clock();
  }
  if(clock() - tenSecondBeginTick > 10000)
  {
    // average fps over 10 seconds
    average = (float)averageFrames/10.0f;
    tenSecondBeginTick = clock();

    // per frame time
    frametime = (float)frameTicks/averageFrames;
    frameTicks = 0;
    averageFrames = 0;

    // possible frames per second
    possible = 1000.0f/frametime;
  }
}
