/**
 * This file was generated by ALProxyGenerator
 */

#ifndef ALLOGGERPROXY_H_
# define ALLOGGERPROXY_H_
# include "alproxy.h"
# include "alloggerproxyposthandler.h"

namespace AL
{
  /**
   * ALLoggerProxy gives acces to all bound methods of the module.
   */
  class ALLoggerProxy : public ALProxy
    {
       public:

       /**
        * Local Constructor
        */
       ALLoggerProxy(ALPtr<ALBroker> pBroker) : ALProxy(pBroker, "ALLogger")
       { post.setParent( (ALProxy*)this );}

       /**
        * Remote Constructor
        */
       ALLoggerProxy(std::string pIP, int pPort) : ALProxy("ALLogger", pIP, pPort)
       { post.setParent( (ALProxy*)this );}

       /**
        * \brief: Implements thread wrappers around methods
       */
       ALLoggerProxyPostHandler post;

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
        * \brief debug : Log a debug message.
        * \param moduleName Name of the module.
        * \param message Log Message.
        */
       void debug ( std::string moduleName, std::string message )
       {
         callVoid( "debug", moduleName, message );
       }

       /**
        * \brief error : Log an error.
        * \param moduleName Name of the module.
        * \param message Log Message.
        */
       void error ( std::string moduleName, std::string message )
       {
         callVoid( "error", moduleName, message );
       }

       /**
        * \brief exit : Exit and unregister module, with no params and no return value.
        */
       void exit (  )
       {
         callVoid( "exit" );
       }

       /**
        * \brief fatal : Log a fatal error.
        * \param moduleName Name of the module.
        * \param message Log Message.
        */
       void fatal ( std::string moduleName, std::string message )
       {
         callVoid( "fatal", moduleName, message );
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
        * \brief info : Log a info message.
        * \param moduleName Name of the module.
        * \param message Log Message.
        */
       void info ( std::string moduleName, std::string message )
       {
         callVoid( "info", moduleName, message );
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
        * \brief logInALMemory : Allow logger to store log in the ALMemory.
        * \param pOnOff True to start, false to stop
        */
       void logInALMemory ( bool pOnOff )
       {
         callVoid( "logInALMemory", pOnOff );
       }

       /**
        * \brief logInFile : Allows the logger to store logs in a file.
        * \param pOnOff True to start, false to stop
        * \param fileName Absolute file name of the output text file. 
        */
       void logInFile ( bool pOnOff, std::string fileName )
       {
         callVoid( "logInFile", pOnOff, fileName );
       }

       /**
        * \brief logInStd : Allows the logger to output logs to the standard output.
        * \param pOnOff True to start, false to stop
        */
       void logInStd ( bool pOnOff )
       {
         callVoid( "logInStd", pOnOff );
       }

       /**
        * \brief lowDebug : Log a low priority debug message.
        * \param moduleName Name of the module.
        * \param message Log Message.
        */
       void lowDebug ( std::string moduleName, std::string message )
       {
         callVoid( "lowDebug", moduleName, message );
       }

       /**
        * \brief lowInfo : Log a low priority information message.
        * \param moduleName Name of the module.
        * \param message Log Message.
        */
       void lowInfo ( std::string moduleName, std::string message )
       {
         callVoid( "lowInfo", moduleName, message );
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
        * \brief separator : output a simple line separator
        */
       void separator (  )
       {
         callVoid( "separator" );
       }

       /**
        * \brief setVerbosity : set verbosity lowDebug (max verbostity), debug, lowInfo, info, warning, error, fatal
        * \param verbosity verbosity value
        */
       void setVerbosity ( std::string verbosity )
       {
         callVoid( "setVerbosity", verbosity );
       }

       /**
        * \brief setVerbosityByModuleName : set verbosity by module
        * \param pModuleName Name of the module
        * \param pVerbosity verbosity level
        */
       void setVerbosityByModuleName ( std::string pModuleName, std::string pVerbosity )
       {
         callVoid( "setVerbosityByModuleName", pModuleName, pVerbosity );
       }

       /**
        * \brief sleep : just sleep an amount of time (for test purpose)
        * \param nTimeInMilliSeconds time to sleep in milli seconds.
        */
       void sleep ( int nTimeInMilliSeconds )
       {
         callVoid( "sleep", nTimeInMilliSeconds );
       }

       /**
        * \brief stop : callback when user call stop(id) on allogger method
        * \param stoppedID function ID allow to deduce function name, module name...
        */
       void stop ( int stoppedID )
       {
         callVoid( "stop", stoppedID );
       }

       /**
        * \brief testReturnValue : just return the value received (for test purpose)
        * \param arg1 arg
        * \return value : the value inputted
        */
       ALValue testReturnValue ( ALValue arg1 )
       {
         return call<ALValue>( "testReturnValue", arg1 );
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

       /**
        * \brief warn : Log a warning.
        * \param moduleName Name of the module.
        * \param message Log Message.
        */
       void warn ( std::string moduleName, std::string message )
       {
         callVoid( "warn", moduleName, message );
       }

    };
};

#endif
