/**
* @file Platform/Win32/SoundPlayer.h
* The file declares a static class for playing sound files.
* @author Colin Graf
*/

#ifndef SoundPlayer_H
#define SoundPlayer_H

#include <string>
/**
* @class SoundPlayer
* The class provides a static function for playing sound files under Win32.
*/
class SoundPlayer
{
public:
  /**
  * Plays a sound file.
  * If you want to play GT_DIR\Config\Sounds\bla.wav use "play("bla.wav");"
  * @param name The filename of the sound file.
  * @return The amound of files in play sound queue.
  */
  static int play(const std::string& name);
};

#endif //SoundPlayer_H
