/**
 * This file was generated by ALProxyGenerator
 */

#ifndef ALPYTHONBRIDGEPROXYPOSTHANDLER_H_
# define ALPYTHONBRIDGEPROXYPOSTHANDLER_H_
# include "alproxy.h"
namespace AL
{
  class ALPythonBridgeProxy;

  /**
   * ALPythonBridgeProxyPostHandler gives acces to all bound methods of the module, using a thread around each call.
   * Long running tasks can be stopped by calling stop(id) with the id returned by the post. 
   */
  class ALPythonBridgeProxyPostHandler
    {
       friend class ALPythonBridgeProxy;

       protected:
          AL::ALProxy* fParent;

       private:
          void setParent(AL::ALProxy* pParent) { fParent = pParent;}

       public:

       ALPythonBridgeProxyPostHandler() : fParent(NULL)
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
        * \brief eval : eval script
        * \param pToEval string to eval
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int eval ( std::string pToEval )
       {
         return fParent->pCall( "eval", pToEval );
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
