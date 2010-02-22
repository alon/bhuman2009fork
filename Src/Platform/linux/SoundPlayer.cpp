/** 
* @file  Platform/linux/SoundPlayer.cpp
* Implementation of class SoundPlayer.
* @attention this is the Linux implementation
*/

#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#include "SoundPlayer.h"
#include "File.h"

SoundPlayer SoundPlayer::soundPlayer;

SoundPlayer::SoundPlayer(): Thread<SoundPlayer>(),
                            cmd("aplay"),
                            started(false)
{
}

void SoundPlayer::start()
{
  Thread<SoundPlayer>::start(this, &SoundPlayer::main);
}

void SoundPlayer::main()
{
  while(isRunning())
  {
    flush();
    usleep(1000); // bah !!!
  }
}

void SoundPlayer::playDirect(const std::string& basename)
{
  std::string fileName(filePrefix);
  fileName += basename;
 
  int r = fork();
  if (r == -1)
    perror("SoundPlayer: fork() failed");
  else if (r != 0) //parent
  {
    int status;
    waitpid(r, &status, 0);
  }
  else //child
  {
    if (-1 == execlp(cmd.c_str(),
                     cmd.c_str(), "-q", fileName.c_str(),
                     (char*)0))
    {
      perror("SoundPlayer: exec failed");
    }
  }
}


void SoundPlayer::flush()
{
  for(;;)
  {
    lockQueue.enter();
    if (0 == queue.size())
    {
      lockQueue.leave();
      break;
    }

    std::string first = queue.front();
    queue.pop_front();
    lockQueue.leave();

    playDirect(first);
  } 
}


int SoundPlayer::play(const std::string& name)
{
  int queuelen;

  soundPlayer.lockQueue.enter();
  if(!soundPlayer.started)
  {
    soundPlayer.started = true;
    soundPlayer.filePrefix = File::getGTDir(); 
    soundPlayer.filePrefix += "/Config/Sounds/";
    soundPlayer.start();
  }
  soundPlayer.queue.push_back(name);
  queuelen = soundPlayer.queue.size();
  soundPlayer.lockQueue.leave();

  return queuelen;
}

