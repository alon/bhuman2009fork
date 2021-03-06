/**
 * This file was generated by ALProxyGenerator
 */

#ifndef ALFACEDETECTIONPROXYPOSTHANDLER_H_
# define ALFACEDETECTIONPROXYPOSTHANDLER_H_
# include "alproxy.h"
namespace AL
{
  class ALFaceDetectionProxy;

  /**
   * ALFaceDetectionProxyPostHandler gives acces to all bound methods of the module, using a thread around each call.
   * Long running tasks can be stopped by calling stop(id) with the id returned by the post. 
   */
  class ALFaceDetectionProxyPostHandler
    {
       friend class ALFaceDetectionProxy;

       protected:
          AL::ALProxy* fParent;

       private:
          void setParent(AL::ALProxy* pParent) { fParent = pParent;}

       public:

       ALFaceDetectionProxyPostHandler() : fParent(NULL)
       {}

       /**
        * \brief dataChanged : Called by ALMemory when subscribed data has been modified.
        * \param dataName Name of the modified data
        * \param value value of the data
        * \param message associate message.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int dataChanged ( std::string dataName, ALValue value, std::string message )
       {
         return fParent->pCall( "dataChanged", dataName, value, message );
       }

       /**
        * \brief exit : Exit and unregister module, with no params and no return value.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int exit (  )
       {
         return fParent->pCall( "exit" );
       }

       /**
        * \brief getBrokerName : return the broker name
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getBrokerName (  )
       {
         return fParent->pCall( "getBrokerName" );
       }

       /**
        * \brief getCurrentPeriod : gets the parameters chosen by the Extractor 
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getCurrentPeriod (  )
       {
         return fParent->pCall( "getCurrentPeriod" );
       }

       /**
        * \brief getCurrentPrecision : gets the parameters chosen by the Extractor 
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getCurrentPrecision (  )
       {
         return fParent->pCall( "getCurrentPrecision" );
       }

       /**
        * \brief getMethodHelp : Retrieve a method's description
        * \param methodName the name of the method
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getMethodHelp ( std::string methodName )
       {
         return fParent->pCall( "getMethodHelp", methodName );
       }

       /**
        * \brief getMethodList : Retrieve the module's method list.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getMethodList (  )
       {
         return fParent->pCall( "getMethodList" );
       }

       /**
        * \brief getMyPeriod :  gets the parameters given by the module 
        * \param name name of the module which has subscribed
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getMyPeriod ( std::string name )
       {
         return fParent->pCall( "getMyPeriod", name );
       }

       /**
        * \brief getMyPrecision :  gets the parameters given by the module 
        * \param name name of the module which has subscribed
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getMyPrecision ( std::string name )
       {
         return fParent->pCall( "getMyPrecision", name );
       }

       /**
        * \brief getOutputNames : get the list of values updated in ALMemory
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getOutputNames (  )
       {
         return fParent->pCall( "getOutputNames" );
       }

       /**
        * \brief getSubscribersInfo :  gets the parameters given by the module 
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getSubscribersInfo (  )
       {
         return fParent->pCall( "getSubscribersInfo" );
       }

       /**
        * \brief innerTest : Run an internal test to this module.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int innerTest (  )
       {
         return fParent->pCall( "innerTest" );
       }

       /**
        * \brief isRunning : return true if method is currently running
        * \param ID ID of the method to wait
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int isRunning ( int ID )
       {
         return fParent->pCall( "isRunning", ID );
       }

       /**
        * \brief moduleHelp : Retrieve the module's description
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int moduleHelp (  )
       {
         return fParent->pCall( "moduleHelp" );
       }

       /**
        * \brief ping : Just a ping, with no params and returning always true
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int ping (  )
       {
         return fParent->pCall( "ping" );
       }

       /**
        * \brief run : protected method
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int run (  )
       {
         return fParent->pCall( "run" );
       }

       /**
        * \brief setParam : Set face detection parameters.
        * \param paramName Name of the parameter to be changed. Among "("FRAME_RATE",[int]");  "("FACE_EDGES_DETECTION",[int]"); "("HIGH_ACCURACY",[int]");  "("FALSE_DETECTION_RATIO",[int]");   "("STEP_SIZE",[int]"); "("MINIMUM_FACE_SIZE_PERCENTAGE",[int]"); "("MAXIMUM_FACE_ROLL",[int]");    "("MAXIMUM_FACE_YAW",[int]"); "("FACE_RECOGNITION_USING_ALL_FEATURES",[int]"); "("RECOGNITION_THRESHOLD",[int]");     "("RECOGNITION_CANDIDATE_THRESHOLD",[int]"); "("USER_DATA_VALIDITY_JUDGMENT_THRESHOLD",[int]");      "("REQUIRED_USER_DATA_CORRECT_FRAMES",[int]")
        * \param paramValue Value(s) of the parameter to be changed.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int setParam ( std::string paramName, ALValue paramValue )
       {
         return fParent->pCall( "setParam", paramName, paramValue );
       }

       /**
        * \brief subscribe :  subscribes to the extractor 
        * \param name name of the module which subscribes
        * \param period refresh period (in millisecond)
        * \param precision precision of the extractor
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int subscribe ( std::string name, int period, float precision )
       {
         return fParent->pCall( "subscribe", name, period, precision );
       }

       /**
        * \brief unsubscribe :  unsubscribes to the extractor 
        * \param name name of the module which has subscribed
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int unsubscribe ( std::string name )
       {
         return fParent->pCall( "unsubscribe", name );
       }

       /**
        * \brief updatePeriod :  updates the parameters 
        * \param name name of the module which has subscribed
        * \param period refresh period (in millisecond)
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int updatePeriod ( std::string name, int period )
       {
         return fParent->pCall( "updatePeriod", name, period );
       }

       /**
        * \brief updatePrecision :  updates the parameters 
        * \param name name of the module which has subscribed
        * \param precision precision of the extractor
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int updatePrecision ( std::string name, float precision )
       {
         return fParent->pCall( "updatePrecision", name, precision );
       }

       /**
        * \brief version : Returns the revision of the module.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int version (  )
       {
         return fParent->pCall( "version" );
       }

       /**
        * \brief wait : wait end of execution method if method was called with pCall
        * \param ID ID of the method to wait
        * \param timeout timeout of the wait in ms. 0 if no timeout
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int wait ( int ID, int timeout )
       {
         return fParent->pCall( "wait", ID, timeout );
       }

       /**
        * \brief waitMethod : wait end of execution method if method was called with pCall
        * \param name name of the method to wait
        * \param timeout timeout of the wait. 0 if no timeout
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int waitMethod ( std::string name, int timeout )
       {
         return fParent->pCall( "waitMethod", name, timeout );
       }

    };
};

#endif
