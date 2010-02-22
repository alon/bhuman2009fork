/** 
* @file  Platform/linux/SoundPlayer.h
*
* Declaration of class SoundPlayer.
*/

#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include <deque>
#include <string>
#include "Thread.h"

class SoundPlayer: Thread<SoundPlayer>
{
public:
  /**
   * Put a filename into play sound queue.
   * If you want to play GT_DIR\Sounds\bla.wav use play("bla.wav");
   * @param name The filename of the sound file.
   * @return The amound of files in play sound queue.
   */
  static int play(const std::string& name);

private:
  static SoundPlayer soundPlayer; /**< The only instance of this class. */

  /**
  * Constructor.
  */
  SoundPlayer();
  
  /**
   * play all sounds in queue and block until finished.
   */
  void flush();

  /**
   * main function of this thread
   */
  void main();

  /**
   * start thread
   */
  void start();

  std::string cmd;
  std::deque< std::string > queue;
  std::string filePrefix;
  SyncObject lockQueue;
  bool started;
  
  void playDirect(const std::string& basename);
};

#endif
