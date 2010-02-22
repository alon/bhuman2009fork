/**
 * @file FPS_Counter.h
 *
 * Definition of class FPS Counter
 *
 * @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
 */

#ifndef FPS_COUNTER_H_
#define FPS_COUNTER_H_

class FPS_Counter
{
protected:
  unsigned int frames,
               averageFrames,
               frameTicks;
  int frameBeginTick,
      secondBeginTick,
      tenSecondBeginTick;

public:
  FPS_Counter();
  ~FPS_Counter()
  {}

  void begin();
  void end();
  void reset();
  
  float current,
        average,
        possible,
        frametime;
};

#endif //FPS_COUNTER_H_
