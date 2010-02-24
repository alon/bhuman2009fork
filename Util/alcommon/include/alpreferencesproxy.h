/**
 * This file was generated by ALProxyGenerator
 */

#ifndef ALPREFERENCESPROXY_H_
# define ALPREFERENCESPROXY_H_
# include "alproxy.h"
# include "alpreferencesproxyposthandler.h"

namespace AL
{
  /**
   * ALPreferencesProxy gives acces to all bound methods of the module.
   */
  class ALPreferencesProxy : public ALProxy
    {
       public:

       /**
        * Local Constructor
        */
       ALPreferencesProxy(ALPtr<ALBroker> pBroker) : ALProxy(pBroker, "ALPreferences")
       { post.setParent( (ALProxy*)this );}

       /**
        * Remote Constructor
        */
       ALPreferencesProxy(std::string pIP, int pPort) : ALProxy("ALPreferences", pIP, pPort)
       { post.setParent( (ALProxy*)this );}

       /**
        * \brief: Implements thread wrappers around methods
       */
       ALPreferencesProxyPostHandler post;

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
        * \brief readPrefFile : Reads all preferences from an xml files and stores them in an ALValue.
        * \param fileName Name of the module associate to the preference.
        * \param autoGenerateMemoryNames If true a memory name will be generated for each non-array preference.
        * \return prefs : array reprenting the whole file.
        */
       ALValue readPrefFile ( std::string fileName, bool autoGenerateMemoryNames )
       {
         return call<ALValue>( "readPrefFile", fileName, autoGenerateMemoryNames );
       }

       /**
        * \brief saveToMemory : Writes all preferences from ALValue to an xml file.
        * \param pPrefs array reprenting the whole file.
        * \return result : True upon success
        */
       bool saveToMemory ( ALValue pPrefs )
       {
         return call<bool>( "saveToMemory", pPrefs );
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
        * \brief writePrefFile : Writes all preferences from ALValue to an xml file.
        * \param fileName Name of the module associate to the preference.
        * \param prefs array reprenting the whole file.
        * \param ignoreMemoryNames If true all memory names will be removed before saving.
        */
       void writePrefFile ( std::string fileName, ALValue prefs, bool ignoreMemoryNames )
       {
         callVoid( "writePrefFile", fileName, prefs, ignoreMemoryNames );
       }

    };
};

#endif
