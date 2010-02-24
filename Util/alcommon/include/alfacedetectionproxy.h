/**
 * This file was generated by ALProxyGenerator
 */

#ifndef ALFACEDETECTIONPROXY_H_
# define ALFACEDETECTIONPROXY_H_
# include "alproxy.h"
# include "alfacedetectionproxyposthandler.h"

namespace AL
{
  /**
   * ALFaceDetectionProxy gives acces to all bound methods of the module.
   */
  class ALFaceDetectionProxy : public ALProxy
    {
       public:

       /**
        * Local Constructor
        */
       ALFaceDetectionProxy(ALPtr<ALBroker> pBroker) : ALProxy(pBroker, "ALFaceDetection")
       { post.setParent( (ALProxy*)this );}

       /**
        * Remote Constructor
        */
       ALFaceDetectionProxy(std::string pIP, int pPort) : ALProxy("ALFaceDetection", pIP, pPort)
       { post.setParent( (ALProxy*)this );}

       /**
        * \brief: Implements thread wrappers around methods
       */
       ALFaceDetectionProxyPostHandler post;

       /**
        * \brief dataChanged : Called by ALMemory when subscribed data has been modified.
        * \param dataName Name of the modified data
        * \param value value of the data
        * \param message associate message.
        */
       void dataChanged ( std::string dataName, ALValue value, std::string message )
       {
         callVoid( "dataChanged", dataName, value, message );
       }

       /**
        * \brief exit : Exit and unregister module, with no params and no return value.
        */
       void exit (  )
       {
         callVoid( "exit" );
       }

       /**
        * \brief getBrokerName : return the broker name
        * \return broker name : string that contain broker name
        */
       std::string getBrokerName (  )
       {
         return call<std::string>( "getBrokerName" );
       }

       /**
        * \brief getCurrentPeriod : gets the parameters chosen by the Extractor 
        * \return period : refresh period (in millisecond)
        */
       int getCurrentPeriod (  )
       {
         return call<int>( "getCurrentPeriod" );
       }

       /**
        * \brief getCurrentPrecision : gets the parameters chosen by the Extractor 
        * \return precision : precision of the extractor
        */
       float getCurrentPrecision (  )
       {
         return call<float>( "getCurrentPrecision" );
       }

       /**
        * \brief getMethodHelp : Retrieve a method's description
        * \param methodName the name of the method
        * \return getMethodHelp : a structure containing the method description
        */
       ALValue getMethodHelp ( std::string methodName )
       {
         return call<ALValue>( "getMethodHelp", methodName );
       }

       /**
        * \brief getMethodList : Retrieve the module's method list.
        * \return methodList : an array containing methods name.
        */
       std::vector<std::string>  getMethodList (  )
       {
         return call<std::vector<std::string> >( "getMethodList" );
       }

       /**
        * \brief getMyPeriod :  gets the parameters given by the module 
        * \param name name of the module which has subscribed
        * \return period : refresh period (in millisecond)
        */
       int getMyPeriod ( std::string name )
       {
         return call<int>( "getMyPeriod", name );
       }

       /**
        * \brief getMyPrecision :  gets the parameters given by the module 
        * \param name name of the module which has subscribed
        * \return precision : precision of the extractor
        */
       float getMyPrecision ( std::string name )
       {
         return call<float>( "getMyPrecision", name );
       }

       /**
        * \brief getOutputNames : get the list of values updated in ALMemory
        * \return outputNames : array of values updated by this extractor in ALMemory
        */
       std::vector<std::string>  getOutputNames (  )
       {
         return call<std::vector<std::string> >( "getOutputNames" );
       }

       /**
        * \brief getSubscribersInfo :  gets the parameters given by the module 
        * \return subscribers' info : array of names and parameters of all subscribers.
        */
       ALValue getSubscribersInfo (  )
       {
         return call<ALValue>( "getSubscribersInfo" );
       }

       /**
        * \brief innerTest : Run an internal test to this module.
        * \return Test_return : true if internal tests are ok
        */
       bool innerTest (  )
       {
         return call<bool>( "innerTest" );
       }

       /**
        * \brief isRunning : return true if method is currently running
        * \param ID ID of the method to wait
        * \return isRunning : True method is currently running
        */
       bool isRunning ( int ID )
       {
         return call<bool>( "isRunning", ID );
       }

       /**
        * \brief moduleHelp : Retrieve the module's description
        * \return moduleHelp : a structure describing the module
        */
       ALValue moduleHelp (  )
       {
         return call<ALValue>( "moduleHelp" );
       }

       /**
        * \brief ping : Just a ping, with no params and returning always true
        * \return alwaysTrue : returns always true
        */
       bool ping (  )
       {
         return call<bool>( "ping" );
       }

       /**
        * \brief run : protected method
        */
       void run (  )
       {
         callVoid( "run" );
       }

       /**
        * \brief setParam : Set face detection parameters.
        * \param paramName Name of the parameter to be changed. Among "("FRAME_RATE",[int]");  "("FACE_EDGES_DETECTION",[int]"); "("HIGH_ACCURACY",[int]");  "("FALSE_DETECTION_RATIO",[int]");   "("STEP_SIZE",[int]"); "("MINIMUM_FACE_SIZE_PERCENTAGE",[int]"); "("MAXIMUM_FACE_ROLL",[int]");    "("MAXIMUM_FACE_YAW",[int]"); "("FACE_RECOGNITION_USING_ALL_FEATURES",[int]"); "("RECOGNITION_THRESHOLD",[int]");     "("RECOGNITION_CANDIDATE_THRESHOLD",[int]"); "("USER_DATA_VALIDITY_JUDGMENT_THRESHOLD",[int]");      "("REQUIRED_USER_DATA_CORRECT_FRAMES",[int]")
        * \param paramValue Value(s) of the parameter to be changed.
        */
       void setParam ( std::string paramName, ALValue paramValue )
       {
         callVoid( "setParam", paramName, paramValue );
       }

       /**
        * \brief subscribe :  subscribes to the extractor 
        * \param name name of the module which subscribes
        * \param period refresh period (in millisecond)
        * \param precision precision of the extractor
        */
       void subscribe ( std::string name, int period, float precision )
       {
         callVoid( "subscribe", name, period, precision );
       }

       /**
        * \brief unsubscribe :  unsubscribes to the extractor 
        * \param name name of the module which has subscribed
        */
       void unsubscribe ( std::string name )
       {
         callVoid( "unsubscribe", name );
       }

       /**
        * \brief updatePeriod :  updates the parameters 
        * \param name name of the module which has subscribed
        * \param period refresh period (in millisecond)
        */
       void updatePeriod ( std::string name, int period )
       {
         callVoid( "updatePeriod", name, period );
       }

       /**
        * \brief updatePrecision :  updates the parameters 
        * \param name name of the module which has subscribed
        * \param precision precision of the extractor
        */
       void updatePrecision ( std::string name, float precision )
       {
         callVoid( "updatePrecision", name, precision );
       }

       /**
        * \brief version : Returns the revision of the module.
        * \return version_string : the string containing the version
        */
       std::string version (  )
       {
         return call<std::string>( "version" );
       }

       /**
        * \brief wait : wait end of execution method if method was called with pCall
        * \param ID ID of the method to wait
        * \param timeout timeout of the wait in ms. 0 if no timeout
        * \return timeout : True if wait exit with timeout
        */
       bool wait ( int ID, int timeout )
       {
         return call<bool>( "wait", ID, timeout );
       }

       /**
        * \brief waitMethod : wait end of execution method if method was called with pCall
        * \param name name of the method to wait
        * \param timeout timeout of the wait. 0 if no timeout
        * \return timeout : True if wait exit with timeout
        */
       int waitMethod ( std::string name, int timeout )
       {
         return call<int>( "waitMethod", name, timeout );
       }

    };
};

#endif
