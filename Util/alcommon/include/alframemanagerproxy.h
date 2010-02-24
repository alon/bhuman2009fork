/**
 * This file was generated by ALProxyGenerator
 */

#ifndef ALFRAMEMANAGERPROXY_H_
# define ALFRAMEMANAGERPROXY_H_
# include "alproxy.h"
# include "alframemanagerproxyposthandler.h"

namespace AL
{
  /**
   * ALFrameManagerProxy gives acces to all bound methods of the module.
   */
  class ALFrameManagerProxy : public ALProxy
    {
       public:

       /**
        * Local Constructor
        */
       ALFrameManagerProxy(ALPtr<ALBroker> pBroker) : ALProxy(pBroker, "ALFrameManager")
       { post.setParent( (ALProxy*)this );}

       /**
        * Remote Constructor
        */
       ALFrameManagerProxy(std::string pIP, int pPort) : ALProxy("ALFrameManager", pIP, pPort)
       { post.setParent( (ALProxy*)this );}

       /**
        * \brief: Implements thread wrappers around methods
       */
       ALFrameManagerProxyPostHandler post;

       /**
        * \brief cleanBehaviors : Stop playing any behavior in FrameManager, and delete all of them.
        */
       void cleanBehaviors (  )
       {
         callVoid( "cleanBehaviors" );
       }

       /**
        * \brief completeBehavior : It will play a behavior and block until the behavior is finished. Note that it can block forever if the behavior output is never called.
        * \param pId The id of the box.
        */
       void completeBehavior ( std::string pId )
       {
         callVoid( "completeBehavior", pId );
       }

       /**
        * \brief createAndPlayTimeline : Creates and play completely a timeline
        * \param pXmlFile The choregraphe project (in plain text for the moment).
        * \return pId : return a unique identifier for the created box that contains the timeline. You must call deleteBehavior on it at some point.
        */
       std::string createAndPlayTimeline ( std::string pXmlFile )
       {
         return call<std::string>( "createAndPlayTimeline", pXmlFile );
       }

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
        * \brief deleteBehavior : Deletes a behavior (meaning a box). Stop the whole behavior contained in this box first.
        * \param pId The id of the box to delete.
        */
       void deleteBehavior ( std::string pId )
       {
         callVoid( "deleteBehavior", pId );
       }

       /**
        * \brief exit : Exit and unregister module, with no params and no return value.
        */
       void exit (  )
       {
         callVoid( "exit" );
       }

       /**
        * \brief exitBehavior : Exit the reading of a timeline contained in a given box
        * \param pId The id of the box.
        */
       void exitBehavior ( std::string pId )
       {
         callVoid( "exitBehavior", pId );
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
        * \brief getDefaultProject : Get the default project (running when the robot is turned on).
        * \return pProject : return Choregraphe's project in plain text
        */
       std::string getDefaultProject (  )
       {
         return call<std::string>( "getDefaultProject" );
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
        * \brief getMotionLength : Returns in seconds, the duration of a given movement stored in a box. Returns 0 if the behavior has no motion layers.
        * \param pId The id of the box.
        * \return pLength : Returns the time in seconds.
        */
       float getMotionLength ( std::string pId )
       {
         return call<float>( "getMotionLength", pId );
       }

       /**
        * \brief gotoAndPlay : Goes to a certain frame and continue playing
        * \param pId The id of the box containing the box.
        * \param pFrame The frame we want the timeline to go to.
        */
       void gotoAndPlay ( std::string pId, int pFrame )
       {
         callVoid( "gotoAndPlay", pId, pFrame );
       }

       /**
        * \brief gotoAndStop : Goes to a certain frame and pause
        * \param pId The id of the box containing the box.
        * \param pFrame The frame we want the timeline to go to.
        */
       void gotoAndStop ( std::string pId, int pFrame )
       {
         callVoid( "gotoAndStop", pId, pFrame );
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
        * \brief newBehavior : Creates a new behavior, from a box found in an xml file. Note that you have to give the xml file contents, so this method is not very user friendly. You have to open the file, and send the string that matches the file contents if you are working from a file. You probably want to use newBehaviorFromFile instead.
        * \param pPath The path to reach the box to instantiate in the project ("" if root).
        * \param pXmlFile The choregraphe project (in plain text for the moment).
        * \return pId : return a unique identifier for the created box.
        */
       std::string newBehavior ( std::string pPath, std::string pXmlFile )
       {
         return call<std::string>( "newBehavior", pPath, pXmlFile );
       }

       /**
        * \brief newBehaviorFromFile : Creates a new behavior, from a box found in an xml file stored in the robot.
        * \param pXmlFilePath Path to Xml file, ex : "/home/youhou/mybehavior.xar".
        * \param pPath The path to reach the box to instantiate in the project ("" if root).
        * \return pId : return a unique identifier for the created box, that can be used by a playBehavior to 
        */
       std::string newBehaviorFromFile ( std::string pXmlFilePath, std::string pPath )
       {
         return call<std::string>( "newBehaviorFromFile", pXmlFilePath, pPath );
       }

       /**
        * \brief onPythonError : Transmit Python error to Choregraphe or logger. Should not be called manually.
        * \param Boxname Name of the box raising the error
        * \param Methodname Name of the method
        * \param Error Error string
        */
       void onPythonError ( std::string Boxname, std::string Methodname, std::string Error )
       {
         callVoid( "onPythonError", Boxname, Methodname, Error );
       }

       /**
        * \brief onPythonPrint : Print Python message in Choregraphe or logger. Should not be called manually.
        * \param message Message to print
        */
       void onPythonPrint ( std::string message )
       {
         callVoid( "onPythonPrint", message );
       }

       /**
        * \brief onRubyError : Transmit Ruby error to Choregraphe or logger. Should not be called manually.
        * \param Boxname Name of the box raising the error
        * \param Methodname Name of the method
        * \param Error Error string
        */
       void onRubyError ( std::string Boxname, std::string Methodname, std::string Error )
       {
         callVoid( "onRubyError", Boxname, Methodname, Error );
       }

       /**
        * \brief onRubyPrint : Print Ruby message in Choregraphe or logger. Should not be called manually.
        * \param message Message to print
        */
       void onRubyPrint ( std::string message )
       {
         callVoid( "onRubyPrint", message );
       }

       /**
        * \brief onUrbiError : Transmit Urbi error to Choregraphe or logger. Should not be called manually.
        * \param Boxname Name of the box raising the error
        * \param Methodname Name of the method
        * \param Location Location of the error
        * \param Error Error string
        */
       void onUrbiError ( std::string Boxname, std::string Methodname, std::string Location, std::string Error )
       {
         callVoid( "onUrbiError", Boxname, Methodname, Location, Error );
       }

       /**
        * \brief onUrbiPrint : Print Urbi message in Choregraphe or logger. Should not be called manually.
        * \param message Message to print
        */
       void onUrbiPrint ( std::string message )
       {
         callVoid( "onUrbiPrint", message );
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
        * \brief playBehavior : Starts playing a timeline contained in a given box
        * \param pId The id of the box.
        */
       void playBehavior ( std::string pId )
       {
         callVoid( "playBehavior", pId );
       }

       /**
        * \brief playDefaultProject : Play default behavior
        */
       void playDefaultProject (  )
       {
         callVoid( "playDefaultProject" );
       }

       /**
        * \brief resetDefaultProject : Set the default project to an empty one.
        */
       void resetDefaultProject (  )
       {
         callVoid( "resetDefaultProject" );
       }

       /**
        * \brief setDefaultProject : Set the default project (running when the robot is turned on).
        * \param pProject Choregraphe's project in plain text.
        */
       void setDefaultProject ( std::string pProject )
       {
         callVoid( "setDefaultProject", pProject );
       }

       /**
        * \brief stopBehavior : Stops playing a timeline contained in a given box, at the current frame
        * \param pId The id of the box.
        */
       void stopBehavior ( std::string pId )
       {
         callVoid( "stopBehavior", pId );
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