/**
 * This file was generated by ALProxyGenerator
 */

#ifndef ALTEXTTOSPEECHPROXYPOSTHANDLER_H_
# define ALTEXTTOSPEECHPROXYPOSTHANDLER_H_
# include "alproxy.h"
namespace AL
{
  class ALTextToSpeechProxy;

  /**
   * ALTextToSpeechProxyPostHandler gives acces to all bound methods of the module, using a thread around each call.
   * Long running tasks can be stopped by calling stop(id) with the id returned by the post. 
   */
  class ALTextToSpeechProxyPostHandler
    {
       friend class ALTextToSpeechProxy;

       protected:
          AL::ALProxy* fParent;

       private:
          void setParent(AL::ALProxy* pParent) { fParent = pParent;}

       public:

       ALTextToSpeechProxyPostHandler() : fParent(NULL)
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
        * \brief getAvailableLanguages : Lists the languages installed on the system.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getAvailableLanguages (  )
       {
         return fParent->pCall( "getAvailableLanguages" );
       }

       /**
        * \brief getAvailableVoices : Lists the available voices. The displayed list contains the voice IDs. This method does not take any parameter.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getAvailableVoices (  )
       {
         return fParent->pCall( "getAvailableVoices" );
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
        * \brief getLanguage : Gets the language of the current voice.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getLanguage (  )
       {
         return fParent->pCall( "getLanguage" );
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
        * \brief getParam : Returns the value of one of the parameters of the voice. The available parameters are: "pitchShift", "doubleVoice","doubleVoiceLevel" and "doubleVoiceTimeShift"
        * \param pParameterName Name of the parameter.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int getParam ( std::string pParameterName )
       {
         return fParent->pCall( "getParam", pParameterName );
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
        * \brief loadVoicePreference : Loads a set of voice parameters defined in a xml file contained in the preferences folder.The name of the voice in the xml filename must be preceded by ALTextToSpeech_Voice_ 
        * \param pPreferenceName Name of the voice preference.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int loadVoicePreference ( std::string pPreferenceName )
       {
         return fParent->pCall( "loadVoicePreference", pPreferenceName );
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
        * \brief say : Performs the text-to-speech operation : it takes a string as input and outputs a sound in both speakers. It logs an error if the string is empty. String encoding must be UTF8.
        * \param pStringToSay text to say, encoded in UTF-8.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int say ( std::string pStringToSay )
       {
         return fParent->pCall( "say", pStringToSay );
       }

       /**
        * \brief sayToFile : Performs the text-to-speech operation: it takes a string as input and outputs a sound in a file.
        * \param pStringToSay Text to say, encoded in UTF-8.
        * \param pFileName RAW file where to store the sound. The sound is encoded with samplerate 22050, format S16_LE, 2 channels.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int sayToFile ( std::string pStringToSay, std::string pFileName )
       {
         return fParent->pCall( "sayToFile", pStringToSay, pFileName );
       }

       /**
        * \brief sayToFileAndPlay : This method performs the text-to-speech operation: it takes a string, outputs the synthesis result sound in a file, then it reads the file. The file is deleted afterwards. It is useful when you want to perform a short synthesis, when few CPU is available. Do not use it if you want a low-latency synthesis start or to synthesize a long string.
        * \param pStringToSay Text to say, encoded in UTF-8.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int sayToFileAndPlay ( std::string pStringToSay )
       {
         return fParent->pCall( "sayToFileAndPlay", pStringToSay );
       }

       /**
        * \brief setLanguage : Changes the Text-to-Speech language. It automatically changes the basic voice of the speech generation since each of them is related to a unique language. The basic voice that is taken is the first one of the language list obtained with the  getAvailableVoices method. If the language is not available, the current language remains unchanged. No exception is thrown in this case.
        * \param pLanguage Language name. Must belong to the languages available in TTS.  It should be an identifier string.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int setLanguage ( std::string pLanguage )
       {
         return fParent->pCall( "setLanguage", pLanguage );
       }

       /**
        * \brief setParam : Changes the parameters of the voice. The available parameters are: 
 	 pitchShift: applies a pitch shifting to the voice. The value indicates the ratio between the new fundamental frequencies and the old ones (examples: 2.0: octave above, 1.5: quint above) . correct range is (1.0 -- 4), or 0 to disable effect.
 	 doubleVoice: adds a second voice to the first one. The value indicates the ratio between the second voice fundamental frequency and the first one. correct range is (1.0 -- 4), or 0 to disable effect 
 	 doubleVoiceLevel: the corresponding value is the level of the double voice (1.0: equal to the main voice one) correct range is (0 -- 4). 
 	 doubleVoiceTimeShift: the corresponding value is the delay between the double voice and the main one. correct range is (0 -- 0.5) 
 If the effect value is not available, the effect parameter remains unchanged.
        * \param pEffectName Name of the parameter.
        * \param pEffectValue Value of the parameter.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int setParam ( std::string pEffectName, float pEffectValue )
       {
         return fParent->pCall( "setParam", pEffectName, pEffectValue );
       }

       /**
        * \brief setSystemVolume : Sets the system volume
        * \param volume volume [0-100].
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int setSystemVolume ( int volume )
       {
         return fParent->pCall( "setSystemVolume", volume );
       }

       /**
        * \brief setVoice : Changes the basic voice of the speech generation. The voice identifier must belong to the installed voices, that can be listed using the getAvailableVoices method. If the voice is not available, it remains unchanded. No exception is thrown in this case.
        * \param pVoiceID The voice ID.
        * \return brokerTaskID : The ID of the task assigned to it by the broker.
        */
       int setVoice ( std::string pVoiceID )
       {
         return fParent->pCall( "setVoice", pVoiceID );
       }

       /**
        * \brief stop : Stops the synthesis engine.
        * \param stoppedID ID of the process to stop.
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
