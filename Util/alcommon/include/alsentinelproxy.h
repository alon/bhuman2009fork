/**
 * This file was generated by ALProxyGenerator
 */

#ifndef ALSENTINELPROXY_H_
# define ALSENTINELPROXY_H_
# include "alproxy.h"
# include "alsentinelproxyposthandler.h"

namespace AL
{
  /**
   * ALSentinelProxy gives acces to all bound methods of the module.
   */
  class ALSentinelProxy : public ALProxy
    {
       public:

       /**
        * Local Constructor
        */
       ALSentinelProxy(ALPtr<ALBroker> pBroker) : ALProxy(pBroker, "ALSentinel")
       { post.setParent( (ALProxy*)this );}

       /**
        * Remote Constructor
        */
       ALSentinelProxy(std::string pIP, int pPort) : ALProxy("ALSentinel", pIP, pPort)
       { post.setParent( (ALProxy*)this );}

       /**
        * \brief: Implements thread wrappers around methods
       */
       ALSentinelProxyPostHandler post;

       /**
        * \brief batteryPowerChange : callback for change in battery power level.(internal use).
        * \param dataName Name of the modified data
        * \param value value of the data
        * \param message associate message.
        */
       void batteryPowerChange ( std::string dataName, ALValue value, std::string message )
       {
         callVoid( "batteryPowerChange", dataName, value, message );
       }

       /**
        * \brief crouchAndRemoveStiffness : Crouch and remove stiffness, this is the method called when low energy and when stopping Nao, this binding permits to test this pose, for test/fun...
        * \param bEvenIfStiffnessNotPresent if false Nao will crouch only if stiffness is on
        */
       void crouchAndRemoveStiffness ( bool bEvenIfStiffnessNotPresent )
       {
         callVoid( "crouchAndRemoveStiffness", bEvenIfStiffnessNotPresent );
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
        * \brief enableBatteryMeasure : check the battery information, voltage and alarm flag
        * \param bSetOrUnset true of false to respectively enable or disable (default: true)
        */
       void enableBatteryMeasure ( bool bSetOrUnset )
       {
         callVoid( "enableBatteryMeasure", bSetOrUnset );
       }

       /**
        * \brief enableCheckRemainingRam : check the remaining physical memory
        * \param bSetOrUnset true of false to respectively enable or disable (default: true)
        */
       void enableCheckRemainingRam ( bool bSetOrUnset )
       {
         callVoid( "enableCheckRemainingRam", bSetOrUnset );
       }

       /**
        * \brief enableDefaultActionDoubleClick : enable or disable the "ouch" action after two pushes.
        * \param bSetOrUnset true of false to respectively enable or disable
        */
       void enableDefaultActionDoubleClick ( bool bSetOrUnset )
       {
         callVoid( "enableDefaultActionDoubleClick", bSetOrUnset );
       }

       /**
        * \brief enableDefaultActionSimpleClick : enable or disable the "hello, i'm nao..." after one short push of the Power button. In both cases the ALMemory ALSentinel/SimpleClickOccured key will be set to true, so users can read/registerOnChange it, then reset to false after handling the event.
        * \param bSetOrUnset true of false to respectively enable or disable
        */
       void enableDefaultActionSimpleClick ( bool bSetOrUnset )
       {
         callVoid( "enableDefaultActionSimpleClick", bSetOrUnset );
       }

       /**
        * \brief enableDefaultActionTripleClick : enable or disable the "restart naoqi" action after three pushes.
        * \param bSetOrUnset true of false to respectively enable or disable
        */
       void enableDefaultActionTripleClick ( bool bSetOrUnset )
       {
         callVoid( "enableDefaultActionTripleClick", bSetOrUnset );
       }

       /**
        * \brief enableElectricCurrentSecurity : enable or disable the 'too much current security' (when a motor is straining against something)
        * \param bSetOrUnset true of false to respectively enable or disable (default: false)
        */
       void enableElectricCurrentSecurity ( bool bSetOrUnset )
       {
         callVoid( "enableElectricCurrentSecurity", bSetOrUnset );
       }

       /**
        * \brief enableHeatMonitoring : Enable or Disable heat monitoring.
        * \param bTrueOrFalse true to enable, false to disable, default: true
        */
       void enableHeatMonitoring ( bool bTrueOrFalse )
       {
         callVoid( "enableHeatMonitoring", bTrueOrFalse );
       }

       /**
        * \brief enablePowerMonitoring : Enable or Disable power monitoring.
        * \param bTrueOrFalse true to enable, false to disable, default: true
        */
       void enablePowerMonitoring ( bool bTrueOrFalse )
       {
         callVoid( "enablePowerMonitoring", bTrueOrFalse );
       }

       /**
        * \brief exit : Exit and unregister module, with no params and no return value.
        */
       void exit (  )
       {
         callVoid( "exit" );
       }

       /**
        * \brief getBatteryLevel : Get the battery level (0..5) 5: full 0: totally empty (never reached).
        * \return int : the battery level (0..5): 5: full 0: totally empty (never reached)
        */
       int getBatteryLevel (  )
       {
         return call<int>( "getBatteryLevel" );
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
        * \brief isElectricCurrentSecurityEnabled : return the state of ElectricCurrentSecurity (true means enabled)
        * \return array : the state
        */
       bool isElectricCurrentSecurityEnabled (  )
       {
         return call<bool>( "isElectricCurrentSecurityEnabled" );
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
        * \brief onAccX : callback for internal use.
        * \param dataName Name of the modified data
        * \param value value of the data
        * \param message associate message.
        */
       void onAccX ( std::string dataName, ALValue value, std::string message )
       {
         callVoid( "onAccX", dataName, value, message );
       }

       /**
        * \brief onAccY : callback for internal use.
        * \param dataName Name of the modified data
        * \param value value of the data
        * \param message associate message.
        */
       void onAccY ( std::string dataName, ALValue value, std::string message )
       {
         callVoid( "onAccY", dataName, value, message );
       }

       /**
        * \brief onAccZ : callback for internal use.
        * \param dataName Name of the modified data
        * \param value value of the data
        * \param message associate message.
        */
       void onAccZ ( std::string dataName, ALValue value, std::string message )
       {
         callVoid( "onAccZ", dataName, value, message );
       }

       /**
        * \brief onBatteryMeasure : callback for internal use.
        * \param dataName Name of the modified data
        * \param value value of the data
        * \param message associate message.
        */
       void onBatteryMeasure ( std::string dataName, ALValue value, std::string message )
       {
         callVoid( "onBatteryMeasure", dataName, value, message );
       }

       /**
        * \brief onElectricCurrentMeasure : callback for change in battery power level. (internal use).
        * \param dataName Name of the modified data
        * \param value value of the data
        * \param message associate message.
        */
       void onElectricCurrentMeasure ( std::string dataName, ALValue value, std::string message )
       {
         callVoid( "onElectricCurrentMeasure", dataName, value, message );
       }

       /**
        * \brief onMotorError : callback for change in battery power level. (internal use).
        * \param dataName Name of the modified data
        * \param value value of the data
        * \param message associate message.
        */
       void onMotorError ( std::string dataName, ALValue value, std::string message )
       {
         callVoid( "onMotorError", dataName, value, message );
       }

       /**
        * \brief onTemperatureMeasure : callback for change in battery power level. (internal use).
        * \param dataName Name of the modified data
        * \param value value of the data
        * \param message associate message.
        */
       void onTemperatureMeasure ( std::string dataName, ALValue value, std::string message )
       {
         callVoid( "onTemperatureMeasure", dataName, value, message );
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
        * \brief powerButtonOnPress : callback when the power button is pressed. (internal use).
        * \param dataName Name of the modified data
        * \param value value of the data
        * \param message associate message.
        */
       void powerButtonOnPress ( std::string dataName, ALValue value, std::string message )
       {
         callVoid( "powerButtonOnPress", dataName, value, message );
       }

       /**
        * \brief presentation : Launch a small presentation of Nao: name, ip, battery. It's the default behavior launched when user press on the chest
        * \param bFastCheckJoints if true, Nao will quickly check that his joints are moving properly.
        */
       void presentation ( bool bFastCheckJoints )
       {
         callVoid( "presentation", bFastCheckJoints );
       }

       /**
        * \brief run : Launch the Sentinel's job (never ending method)
        */
       void run (  )
       {
         callVoid( "run" );
       }

       /**
        * \brief setPowerLimit : Change the power threshold; below it, Nao will sit down and remove power from his motors.
        * \param rPowerLimit power limit in percent, default: 0.07; 0.00 to disable this feature.
        */
       void setPowerLimit ( float rPowerLimit )
       {
         callVoid( "setPowerLimit", rPowerLimit );
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
