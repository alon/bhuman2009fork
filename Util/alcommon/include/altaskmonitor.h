/**
* @author Aldebaran Robotics 
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/


#ifndef AL_TASKMONITOR_H
#define AL_TASKMONITOR_H
#include "alvalue.h"
#include "alcriticalsection.h"
#include <map>
#include "alptr.h"

#define INDEX_NUMBER 5
#define INDEX_UNIQUE_ID_BROKER 0   // ID created at pcall
#define INDEX_METHODE_NAME 1
#define INDEX_MODULE_NAME 2
#define INDEX_REQUIRE_STOP 3
#define INDEX_THREAD_ID 4     // ID created when thread found for task   


namespace AL
{

class ALBroker;

class ALTaskMonitor
{
  
  // Attributes
private:
  //all running task ID are stored here
  std::map<int , ALValue> fMonitor;

  ALPtr<ALMutex> fMutex;
  
  // Operations
public:
  ALTaskMonitor(ALPtr<ALBroker> pBroker);

  ~ALTaskMonitor ();

  ALWeakPtr<ALBroker> parentBroker;

  
    /**
   * getID from monitoring information
   * 
   **/
  static int getID(ALValue monitorTask)
  {
    if ((monitorTask.getSize() < 3) || (monitorTask[0].getType() != ALValue::TypeInt))
    {
      return -1;
    }
    return monitorTask[0];
  }

    /**
   * get method name from monitoring information
   * 
   **/
  static std::string getMethodName(ALValue monitorTask)
  {
    if ((monitorTask.getSize() < 3) || (monitorTask[1].getType() != ALValue::TypeString))
    {
      return "";
    }
    return monitorTask[1];
  }

  /**
   * set thread ID when thread found for task
   * 
   **/
  void setThreadID(ALValue taskID);

  /**
   * get method ID, call it from thread. Carrefull, call it only one time.
   * 
   **/
  int getMethodID();

    /**
   * get method ID from module, call it from thread. Carrefull, call it only one time.
   * 
   **/
  int getMethodID(const std::string &strModule) ;

 /**
   * get module name from monitoring information
   * 
   **/
  static std::string getModuleName(ALValue monitorTask)
  {
    if ((monitorTask.getSize() < 3) || (monitorTask[2].getType() != ALValue::TypeString))
    {
      return "";
    }
    return monitorTask[2];
  }


  /**
   * getParentBroker give access to broker information
   * 
   **/
  inline ALPtr<ALBroker> getParentBroker(void)
  {
    ALPtr<ALBroker> returnPtr = parentBroker.lock();
    AL_ASSERT(returnPtr != NULL);
    return returnPtr;
  }
  /**
   * add : add a task ID into the map ( called when a new task start running)
   * @param pId tast ID
   **/
  void add (const ALValue& pId);
  

  /**
   * getNumberOfTask 
   * @return number of task running (only pcall)
   **/
  int getNumberOfTask(void)
  {
    return fMonitor.size();
  }

  /**
   * remove : remove a task ID into the map( called when a task finished)
   * @param pIndex tast id
   **/
  void remove (int pIndex);
  
  /**
   * show : display map
   **/
  void show();
  
  /**
   * getDebugTaskList return an alvalue containing info on all running task
   **/
  ALValue getDebugTaskList();
  
  /**
   * isRunning return true if the task pId is running
   * @param pId tast id
   **/
  bool isRunning(int pId);

  /**
   * wait end of method pIp
   * @param pId task id
   **/
  bool wait(int pIp, int pTimeout = 0);

  /**
   * wait end of method pIp
   * @param pId task id
   **/
  int waitMethod(std::string methodName, int pTimeout = 0);
  

  /**
   * isRunning return true if one instance of this task is running
   * @param pTaskName tast id
   **/
  bool isRunning(const std::string& pTaskName);
  
  /**
   * return true if user call stop(id) or stop(method) on id method
   * @param  task id
   **/
  bool isRequireStop(int id);

  /**
   * find the the right module corresponding to the task and call the stop method 
   * @param pId tast id
   **/
  int stop(int pId);
  
  /**
   * return list of task running
   **/
  std::string getTaskStringRunning();
  
  /**
   * return xml formatted of running tasks string 
   **/
  std::string getXMLTaskStringRunning();
 

   /**
   * stop return the ALValue ID of a task id
   * @param pId tast id
   **/
  int stop(const std::string& pTaskName);
  ALValue getID(int id);
  ALValue getID(const std::string& pTaskName);
  
  //#ifdef DEBUG
  std::string fStmPrefix;
  ///return a free Id in fStmMonitorStatus
  int getFreeId();
  std::map<int,int> fIdToStmMonitor;
  std::vector<AL::ALValue> fStmMonitorStatus;
  //#endif
};



/**
 * ALMonitoredSection has same behavior than alcriticalsection. While ALMonitoredSection variable is in call stack, function is monitored
 * 
 **/
class ALMonitoredSection
{
public:
    ALMonitoredSection(ALPtr<ALBroker> pBroker, const ALValue &ID);
    ALMonitoredSection(ALPtr<ALBroker> pBroker,const std::string &pModule, const std::string pDesc);
    ~ALMonitoredSection();


private:
    ALPtr<ALBroker> parentBroker;
    ALValue valueID;
};

}
#endif
