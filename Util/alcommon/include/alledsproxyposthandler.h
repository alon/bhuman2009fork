/**
 * This file was generated by ALProxyGenerator
 */

#ifndef ALLEDSPROXYPOSTHANDLER_H_
# define ALLEDSPROXYPOSTHANDLER_H_
# include "alproxy.h"
namespace AL
{
  class ALLedsProxy;

  /**
   * ALLedsProxyPostHandler gives acces to all bound methods of the module, using a thread around each call.
   * Long running tasks can be stopped by calling stop(id) with the id returned by the post. 
   */
  class ALLedsProxyPostHandler
    {
       friend class ALLedsProxy;

       protected:
          AL::ALProxy* fParent;

       private:
          void setParent(AL::ALProxy* pParent) { fParent = pParent;}

       public:

       ALLedsProxyPostHandler() : fParent(NULL)
       {}

       /**
        * \brief createGroup : Makes a group name for ease of setting multiple LEDs.
        * \param pGroupName The name of the group.
        * \param pLedNames A vector of the names of the LEDs in the group.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int createGroup ( std::string pGroupName, std::vector<std::string>  pLedNames )
       {
         return fParent->pCall( "createGroup", pGroupName, pLedNames );
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
        * \brief earLedsSetAngle : An animation to show a direction with the ears.
        * \param pDegrees The angle you want to show in degrees (int). 0 is up, 90 is forwards, 180 is down and 270 is back.
        * \param pDuration The duration in seconds of the animation.
        * \param pLeaveOnAtEnd If true the last led is left on at the end of the animation.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int earLedsSetAngle ( int pDegrees, float pDuration, bool pLeaveOnAtEnd )
       {
         return fParent->pCall( "earLedsSetAngle", pDegrees, pDuration, pLeaveOnAtEnd );
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
        * \brief fade : Sets the intensity of a LED or Group of LEDs within a given time.
        * \param pName The name of the LED or Group.
        * \param pIntensity The intensity of the LED or Group (a value between 0 and 1).
        * \param pDuration The duration of the fade in seconds
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int fade ( std::string pName, float pIntensity, float pDuration )
       {
         return fParent->pCall( "fade", pName, pIntensity, pDuration );
       }

       /**
        * \brief fadeRGB : Sets the intensity of a led. If the name matches an RGB led, all channels are set to the same value.
        * \param pName The name of the LED or Group.
        * \param nRGB The RGB value led, RGB as seen in hexa-decimal: 0x00RRGGBB.
        * \param pDuration Time used to fade in seconds.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int fadeRGB ( std::string pName, int nRGB, float pDuration )
       {
         return fParent->pCall( "fadeRGB", pName, nRGB, pDuration );
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
        * \brief getIntensity : Sets the intensity of a LED or Group of LEDs.
        * \param pName The name of the LED or Group.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getIntensity ( std::string pName )
       {
         return fParent->pCall( "getIntensity", pName );
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
        * \brief isRunning : return true if method is currently running
        * \param ID ID of the method to wait
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int isRunning ( int ID )
       {
         return fParent->pCall( "isRunning", ID );
       }

       /**
        * \brief listGroup : Lists the devices in the group.
        * \param pGroupName The name of the Group.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int listGroup ( std::string pGroupName )
       {
         return fParent->pCall( "listGroup", pGroupName );
       }

       /**
        * \brief listGroups : Lists available group names.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int listGroups (  )
       {
         return fParent->pCall( "listGroups" );
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
        * \brief off : Switch to a minimum intensity a LED or Group of LEDs.
        * \param pName The name of the LED or Group.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int off ( std::string pName )
       {
         return fParent->pCall( "off", pName );
       }

       /**
        * \brief on : Switch to a maximum intensity a LED or Group of LEDs.
        * \param pName The name of the LED or Group.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int on ( std::string pName )
       {
         return fParent->pCall( "on", pName );
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
        * \brief randomEyes : Launch a random animation in eyes
        * \param pDuration Approximate duration of the animation in seconds.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int randomEyes ( float pDuration )
       {
         return fParent->pCall( "randomEyes", pDuration );
       }

       /**
        * \brief rasta : Launch a green/yellow/red rasta animation on all body.
        * \param pDuration Approximate duration of the animation in seconds.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int rasta ( float pDuration )
       {
         return fParent->pCall( "rasta", pDuration );
       }

       /**
        * \brief rotateEyes : Launch a rotation using the leds of the eyes.
        * \param nRGB the RGB value led, RGB as seen in hexa-decimal: 0x00RRGGBB.
        * \param rTimeForTurn Approximate time to make one turn.
        * \param pDuration Approximate duration of the animation in seconds.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int rotateEyes ( int nRGB, float rTimeForTurn, float pDuration )
       {
         return fParent->pCall( "rotateEyes", nRGB, rTimeForTurn, pDuration );
       }

       /**
        * \brief setIntensity : Sets the intensity of a LED or Group of LEDs.
        * \param pName The name of the LED or Group.
        * \param pIntensity The intensity of the LED or Group (a value between 0 and 1).
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int setIntensity ( std::string pName, float pIntensity )
       {
         return fParent->pCall( "setIntensity", pName, pIntensity );
       }

       /**
        * \brief stop : callback when user call stop(id) on ALLeds method
        * \param stoppedID function ID allow to deduce function name, module name...
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int stop ( int stoppedID )
       {
         return fParent->pCall( "stop", stoppedID );
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