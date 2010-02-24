/**
* @author Aldebaran-Robotics
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/


#ifndef AL_TASK_H_
# define AL_TASK_H_

// External headers
# include <string>
#include "alptr.h"

// Internal headers
# include "althreadtypes.h"

namespace AL
{
  class ALTask
  {
  public:

    /**
    * \brief Indicate the state of the Task
    *
    * Indicate the state of the task, it is either IDLE, RUNNING or ENDED
    */

    typedef enum { IDLE, RUNNING, ENDED } ALTPState;

    /**
    * \brief Constructor. User defined. Must stock in private fields run parameters.
    *
    * Defined by users. If a user wants to access to some parameters during execution,
    * he should stock them as private fields.
    */

    ALTask();

    /**
    * \brief Destructor.
    *
    * Users may define their own destructors while constructing their class.
    */

    virtual ~ALTask();

    /**
    * \brief run. User defined. Contains the code to run.
    *
    * Instruction to run in the thread. parameters should be private members of the
    * class. User should use privete members to get the result too.
    */

    virtual void run () = 0 ;

    virtual void exitTask () {};

#ifdef DEBUG
    virtual std::string getMethodName(){return "unknown";}; 
#endif

    /* Setters */

    /**
    * setCreationTime 
    * @params task creation time
    *
    */
    void setCreationTime();

    /**
    * setExecutionTime 
    * Execution task time
    *
    */
    void setExecutionTime();

    /**
    * setEndTime 
    * End task time
    *
    */
    void setEndTime() ;

    /**
    * setThread
    * Thread affectation
    *
    */
    void setThread( ALPtr<ALThread> pThread ) ;

    /**
    * setState
    * set state machine
    *
    */
    void setState( ALTPState pState ) {fState=pState;}


    /**
    * getIdleTime
    * return IdleTime in usec.
    *
    */
    int getIdleTime() ;


    /**
    * getExecutionTime
    * return execution time in usec.
    *
    */
    int getExecutionTime() ;


    /**
    * getState
    * return machine state
    *
    */
    ALTPState getState() { return fState; }

    std::string toString() const;


    /**
    * fAutoDelete
    * Tell if the task must be deleted after the treatment
    *
    */
    //! 
    bool fAutoDelete;

  private:

    //! Creation time
    struct timeval fCreationTime;

    //! Execution time
    struct timeval fExecutionTime;

    //! End time
    struct timeval fEndTime;



    //! Unique ID of the task.
    int fId;

    //! State of the task : IDLE / RUNNING / ENDED
    volatile ALTPState fState;

  protected:
    //! Thread treating this task
    ALWeakPtr<ALThread> fThread;

  };

}// namespace AL
#endif // !AL_TASK_H_
