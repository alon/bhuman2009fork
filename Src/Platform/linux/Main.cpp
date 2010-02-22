/**
* @file Platform/linux/Main.cpp
* Implementation of the main() function for starting and stopping the module framework.
* @author Colin Graf
*/

#include <signal.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/file.h> // flock
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "Robot.h"
#include "NaoBody.h"
#include "Tools/Global.h"
#include "Tools/Settings.h"
#include "libbhuman/bhuman.h"

static pid_t bhumanPid = 0;
static Robot* robot = 0;
static NaoBody naoBody;
static bool run = true;

static void bhumanStart(bool recover)
{
  fprintf(stderr, "BHuman: Start.\n");
  
  robot = new Robot();
  if(recover)
    Global::getSettings().recover = true;
  robot->start();
}

static void bhumanStop()
{
  fprintf(stderr, "BHuman: Stop.\n");
  robot->announceStop();
  usleep(1000000);
  delete robot;
  robot = 0;
  //fprintf(stderr, "BHuman: Stopped.\n");
}

static void sighandlerShutdown(int sig)
{
  if(run)
    printf("Caught signal %i\nShutting down...\n", sig);
  run = false;
}

static void sighandlerRedirect(int sig)
{
  //if(bhumanPid != 0)
    //kill(bhumanPid, sig);
  run = false;
}

int main(int argc, char *argv[]) 
{
  {
    // parse command-line arguments
    bool background = false;
    bool recover = false;
    bool watchdog = false;
    const char* gtDir = "/media/userdata";

    for(int i = 1; i < argc; ++i)
      if(!strcmp(argv[i], "-b"))
        background = true;
      else if(!strcmp(argv[i], "-w"))
        watchdog = true;
      else if(!strcmp(argv[i], "-c") && i + 1 < argc)
        gtDir = argv[++i];
      else 
      {
        fprintf(stderr, "Usage: %s [-b] [-c <dir>] [-w]\n\
    -b            run in background (as daemon)\n\
    -c <dir>      used gt directory (default is /media/userdata)\n\
    -w            use a watchdog for crash recovery and creating trace dumps\n", argv[0]);
        exit(EXIT_FAILURE);
      }

    // avoid duplicated instances
    int fd = open("/tmp/bhuman", O_CREAT, 0600);
    if(fd == -1 || flock(fd, LOCK_EX | LOCK_NB) == -1)
    {
      fprintf(stderr, "There is already an instance of this process!\n");
      exit(EXIT_FAILURE);
    }
    
    // start as deamon
    if(background)
    {
      fprintf(stderr, "Starting as daemon...\n");
      pid_t childPid = fork();
      if(childPid == -1)
        exit(EXIT_FAILURE);
      if(childPid != 0)
        exit(EXIT_SUCCESS);
    }

    // change working directory
    if(*gtDir && chdir(gtDir) != 0)
    {
      fprintf(stderr, "chdir to config directory failed!\n");
      exit(EXIT_FAILURE);
    }

    // the watchdog
    if(watchdog)
      for(;;)
      {
        bhumanPid = fork();
        if(bhumanPid == -1)
          exit(EXIT_FAILURE);
        if(bhumanPid != 0)
        {
          int status;
          signal(SIGTERM, sighandlerRedirect);
          signal(SIGINT, sighandlerRedirect);
          if(waitpid(bhumanPid, &status, 0) != bhumanPid)
            exit(EXIT_FAILURE);
          signal(SIGTERM, SIG_DFL);
          signal(SIGINT, SIG_DFL);
         
          // detect requested or normal exit
          if(!run || (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS ))
            exit(WIFEXITED(status) ? WEXITSTATUS(status) : EXIT_FAILURE);

          // trace dump
          NaoBody naoBody;
          if(naoBody.init())
          {
            naoBody.setCrashed(WIFSIGNALED(status) ? int(WTERMSIG(status)) : int(abnormalTermination));
            naoBody.dumpTrace();
          }

          // don't restart if we got killed
          if(WIFSIGNALED(status) && WTERMSIG(status) == SIGKILL)
            exit(EXIT_FAILURE);

          // restart in release mode only
#ifndef RELEASE
          exit(EXIT_FAILURE);
#endif
       
          // deactivate the pre-initial state
          recover = true;

          usleep(2000 * 1000);
        }
        else
          break;
      }

    // wait for NaoQi/libbhuman
    if(!naoBody.init())
    {
      fprintf(stderr, "BHuman: Waiting for NaoQi/libbhuman...\n");
      do
      {
        usleep(1000000);
      } while(!naoBody.init());
    }

    // register signal handler for strg+c and termination signal
    signal(SIGTERM, sighandlerShutdown);
    signal(SIGINT, sighandlerShutdown);
  
    //
    bhumanStart(recover);
  }

  while(run)
    pause();

  bhumanStop();

  return EXIT_SUCCESS;
}
