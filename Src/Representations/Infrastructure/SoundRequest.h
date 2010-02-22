/**
 * @file SoundRequest.h
 * Declaration of class SoundRequest.
 * @author Philippe Schober
 */ 

#ifndef __SoundRequest_h_
#define __SoundRequest_h_

#include "Tools/Streams/Streamable.h"

class SoundRequest : public Streamable
{
private:
  virtual void serialize(In* in, Out* out)
  {
    STREAM_REGISTER_BEGIN();
      STREAM_ENUM(sound, numOfSounds, SoundRequest::getSoundName);
      STREAM(once);
    STREAM_REGISTER_FINISH();
  }

public:
  enum Sound
  {
    none,
    red,
    blue,
    ownKickoff,
    opponentKickoff,
    penalized,
    notPenalized,

    numOfSounds
  };

  Sound sound; /**< The requested sound to be played. */
  bool once; /**< Play sound only once. */
 
  static const char* getSoundName(Sound sound)
  {
    switch(sound)
    {
      case none: return "none";
      case red: return "red";
      case blue: return "blue";
      case ownKickoff: return "ownKickoff";
      case opponentKickoff: return "opponentKickoff";
      case penalized: return "penalized";
      case notPenalized: return "notPenalized";
      default: return "unknown";
    }
  };

  /**
  * The function returns the filename of the sound to play.
  * @param sound The sound.
  * @return The filename of this sound.
  */
  static const char* getSoundFilename(Sound sound)
  {
    switch(sound)
    {
      case red: return "red.wav";
      case blue: return "blue.wav";
      case ownKickoff: return "own_kickoff.wav";
      case opponentKickoff: return "opponent_kickoff.wav";
      case penalized: return "penalized.wav";
      case notPenalized: return "notPenalized.wav";

      default: return 0;
    }
  };

  /**
  * Default constructor.
  */
  SoundRequest() : sound(none), once(true) {}
};

/**
* A dummy class so SoundControl is selectable.
*/
class SoundOutput : public Streamable
{
private:
  virtual void serialize(In* in, Out* out)
  {
    STREAM_REGISTER_BEGIN();
      char dummy(0);
      STREAM(dummy);
    STREAM_REGISTER_FINISH();
  }
};

#endif //__SoundRequest_h_
