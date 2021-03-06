/**
 * This file was generated by ALProxyGenerator
 */

#ifndef ALSENTINELPROXYPOSTHANDLER_H_
# define ALSENTINELPROXYPOSTHANDLER_H_
# include "alproxy.h"
namespace AL
{
  class ALSentinelProxy;

  /**
   * ALSentinelProxyPostHandler gives acces to all bound methods of the module, using a thread around each call.
   * Long running tasks can be stopped by calling stop(id) with the id returned by the post. 
   */
  class ALSentinelProxyPostHandler
    {
       friend class ALSentinelProxy;

       protected:
          AL::ALProxy* fParent;

       private:
          void setParent(AL::ALProxy* pParent) { fParent = pParent;}

       public:

       ALSentinelProxyPostHandler() : fParent(NULL)
       {}

       /**
        * \brief batteryPowerChange : callback for change in battery power level.(internal use).
        * \param dataName Name of the modified data
        * \param value value of the data
        * \param message associate message.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int batteryPowerChange ( std::string dataName, ALValue value, std::string message )
       {
         return fParent->pCall( "batteryPowerChange", dataName, value, message );
       }

       /**
        * \brief crouchAndRemoveStiffness : Crouch and remove stiffness, this is the method called when low energy and when stopping Nao, this binding permits to test this pose, for test/fun...
        * \param bEvenIfStiffnessNotPresent if false Nao will crouch only if stiffness is on
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int crouchAndRemoveStiffness ( bool bEvenIfStiffnessNotPresent )
       {
         return fParent->pCall( "crouchAndRemoveStiffness", bEvenIfStiffnessNotPresent );
       }

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
        * \brief enableBatteryMeasure : check the battery information, voltage and alarm flag
        * \param bSetOrUnset true of false to respectively enable or disable (default: true)
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int enableBatteryMeasure ( bool bSetOrUnset )
       {
         return fParent->pCall( "enableBatteryMeasure", bSetOrUnset );
       }

       /**
        * \brief enableCheckRemainingRam : check the remaining physical memory
        * \param bSetOrUnset true of false to respectively enable or disable (default: true)
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int enableCheckRemainingRam ( bool bSetOrUnset )
       {
         return fParent->pCall( "enableCheckRemainingRam", bSetOrUnset );
       }

       /**
        * \brief enableDefaultActionDoubleClick : enable or disable the "ouch" action after two pushes.
        * \param bSetOrUnset true of false to respectively enable or disable
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int enableDefaultActionDoubleClick ( bool bSetOrUnset )
       {
         return fParent->pCall( "enableDefaultActionDoubleClick", bSetOrUnset );
       }

       /**
        * \brief enableDefaultActionSimpleClick : enable or disable the "hello, i'm nao..." after one short push of the Power button. In both cases the ALMemory ALSentinel/SimpleClickOccured key will be set to true, so users can read/registerOnChange it, then reset to false after handling the event.
        * \param bSetOrUnset true of false to respectively enable or disable
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int enableDefaultActionSimpleClick ( bool bSetOrUnset )
       {
         return fParent->pCall( "enableDefaultActionSimpleClick", bSetOrUnset );
       }

       /**
        * \brief enableDefaultActionTripleClick : enable or disable the "restart naoqi" action after three pushes.
        * \param bSetOrUnset true of false to respectively enable or disable
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int enableDefaultActionTripleClick ( bool bSetOrUnset )
       {
         return fParent->pCall( "enableDefaultActionTripleClick", bSetOrUnset );
       }

       /**
        * \brief enableElectricCurrentSecurity : enable or disable the 'too much current security' (when a motor is straining against something)
        * \param bSetOrUnset true of false to respectively enable or disable (default: false)
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int enableElectricCurrentSecurity ( bool bSetOrUnset )
       {
         return fParent->pCall( "enableElectricCurrentSecurity", bSetOrUnset );
       }

       /**
        * \brief enableHeatMonitoring : Enable or Disable heat monitoring.
        * \param bTrueOrFalse true to enable, false to disable, default: true
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int enableHeatMonitoring ( bool bTrueOrFalse )
       {
         return fParent->pCall( "enableHeatMonitoring", bTrueOrFalse );
       }

       /**
        * \brief enablePowerMonitoring : Enable or Disable power monitoring.
        * \param bTrueOrFalse true to enable, false to disable, default: true
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int enablePowerMonitoring ( bool bTrueOrFalse )
       {
         return fParent->pCall( "enablePowerMonitoring", bTrueOrFalse );
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
        * \brief getBatteryLevel : Get the battery level (0..5) 5: full 0: totally empty (never reached).
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getBatteryLevel (  )
       {
         return fParent->pCall( "getBatteryLevel" );
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
        * \brief innerTest : Run an internal test to this module.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int innerTest (  )
       {
         return fParent->pCall( "innerTest" );
       }

       /**
        * \brief isElectricCurrentSecurityEnabled : return the state of ElectricCurrentSecurity (true means enabled)
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int isElectricCurrentSecurityEnabled (  )
       {
         return fParent->pCall( "isElectricCurrentSecurityEnabled" );
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
        * \brief onAccX : callback for internal use.
        * \param dataName Name of the modified data
        * \param value value of the data
        * \param message associate message.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int onAccX ( std::string dataName, ALValue value, std::string message )
       {
         return fParent->pCall( "onAccX", dataName, value, message );
       }

       /**
        * \brief onAccY : callback for internal use.
        * \param dataName Name of the modified data
        * \param value value of the data
        * \param message associate message.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int onAccY ( std::string dataName, ALValue value, std::string message )
       {
         return fParent->pCall( "onAccY", dataName, value, message );
       }

       /**
        * \brief onAccZ : callback for internal use.
        * \param dataName Name of the modified data
        * \param value value of the data
        * \param message associate message.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int onAccZ ( std::string dataName, ALValue value, std::string message )
       {
         return fParent->pCall( "onAccZ", dataName, value, message );
       }

       /**
        * \brief onBatteryMeasure : callback for internal use.
        * \param dataName Name of the modified data
        * \param value value of the data
        * \param message associate message.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int onBatteryMeasure ( std::string dataName, ALValue value, std::string message )
       {
         return fParent->pCall( "onBatteryMeasure", dataName, value, message );
       }

       /**
        * \brief onElectricCurrentMeasure : callback for change in battery power level. (internal use).
        * \param dataName Name of the modified data
        * \param value value of the data
        * \param message associate message.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int onElectricCurrentMeasure ( std::string dataName, ALValue value, std::string message )
       {
         return fParent->pCall( "onElectricCurrentMeasure", dataName, value, message );
       }

       /**
        * \brief onMotorError : callback for change in battery power level. (internal use).
        * \param dataName Name of the modified data
        * \param value value of the data
        * \param message associate message.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int onMotorError ( std::string dataName, ALValue value, std::string message )
       {
         return fParent->pCall( "onMotorError", dataName, value, message );
       }

       /**
        * \brief onTemperatureMeasure : callback for change in battery power level. (internal use).
        * \param dataName Name of the modified data
        * \param value value of the data
        * \param message associate message.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int onTemperatureMeasure ( std::string dataName, ALValue value, std::string message )
       {
         return fParent->pCall( "onTemperatureMeasure", dataName, value, message );
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
        * \brief powerButtonOnPress : callback when the power button is pressed. (internal use).
        * \param dataName Name of the modified data
        * \param value value of the data
        * \param message associate message.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int powerButtonOnPress ( std::string dataName, ALValue value, std::string message )
       {
         return fParent->pCall( "powerButtonOnPress", dataName, value, message );
       }

       /**
        * \brief presentation : Launch a small presentation of Nao: name, ip, battery. It's the default behavior launched when user press on the chest
        * \param bFastCheckJoints if true, Nao will quickly check that his joints are moving properly.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int presentation ( bool bFastCheckJoints )
       {
         return fParent->pCall( "presentation", bFastCheckJoints );
       }

       /**
        * \brief run : Launch the Sentinel's job (never ending method)
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int run (  )
       {
         return fParent->pCall( "run" );
       }

       /**
        * \brief setPowerLimit : Change the power threshold; below it, Nao will sit down and remove power from his motors.
        * \param rPowerLimit power limit in percent, default: 0.07; 0.00 to disable this feature.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int setPowerLimit ( float rPowerLimit )
       {
         return fParent->pCall( "setPowerLimit", rPowerLimit );
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
