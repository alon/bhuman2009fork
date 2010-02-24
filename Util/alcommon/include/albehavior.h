/**
 * @author Aldebaran Robotics
 * Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
 *
 * Version : $Id$
 */

/**
* The class that implements a behavior.
* A behavior is an entity that have inputs and outputs.
* So he can be enabled and produce events.
* It's like a box in Telepathe, but it's not containing the inner box (present in itself).
*/


#ifndef AL_BEHAVIOR_H
#define AL_BEHAVIOR_H

#include "altypes.h"
#include "almodule.h"
#include "alptr.h"


namespace AL
{
  class ALBroker;
  class ALMemoryProxy;
  class ALProxy;
  class ALMutexRW;

  class ALBehavior: public AL::ALModule
  {
  protected:
    enum State //state of the behavior : enabled (all subscription done), disable (none done), partial (some of them done)
    {
      ENABLED,
      DISABLED,
      PARTIAL
    };

  public :

    //
    // ALBehavior type definition
    //

    class IO
    {
      // a link/input/output class.
      // It stores the name of the IO, and the type.

    public:

      IO( const std::string & pName, bool pIsBang )
        : fName   ( pName )
        , fIsBang ( pIsBang )
      {
        // NOTHING
      }

      bool operator<(const IO& io) const {return fName < io.fName;}

    public:

      const std::string   fName;
      const bool          fIsBang;
    };

    typedef IO                              TInput;
    typedef IO                              TOutput;

    typedef std::list<std::string>          TListValues;
    typedef TListValues::iterator           ITListValues;
    typedef TListValues::const_iterator     CTListValues;

    typedef std::map<TInput, TListValues >  TInputMap;
    typedef TInputMap::iterator             ITInputMap;
    typedef TInputMap::const_iterator       CITInputMap;

    typedef std::map<TOutput, TListValues > TOutputMap;
    typedef TOutputMap::iterator            ITOutputMap;
    typedef TOutputMap::const_iterator      CITOutputMap;


    //
    // ALBehavior method definition
    //

  public :

    /*
    * Creates an ALModule. An ALModule has a name.
    * @param pName the behavior name
    */
	  ALBehavior( ALPtr<ALBroker> pBroker, const std::string& pName );

    /**
    * Destructor
    */
    virtual ~ALBehavior();
      
    virtual void init();

    /**
     * Will enable or disable a behavior.
     * When switching from one state to another, all the subscription will be done/undone.
     */
    void setEnabled(const bool& pEnabled);
    
    /**
     * return true if state is ENABLED, false otherwise
     */
    bool isEnabled();

    const TInputMap& getInputs( void ) const
    {
      return fInputList;
    }

    const TOutputMap& getOutputs( void ) const
    {
      return fOutputList;
    }

	/**
	 * addInput. Add a new input to this behavior
   * @param pInputName input name !
   * @param pbIsBang is it a bang type ?
   * @return true if input added (false if an in put with the same name already exists
	 */
    bool addInput( const std::string & pInputName, bool pbIsBang );

	/**
	 * addOutput. Add a new output to this behavior
   * @param pOutputName output name !
   * @param pbIsBang is it a bang type ?
   * @return true if input added (false if an in put with the same name already exists
	 */
    bool addOutput( const std::string & pOutputName, bool pbIsBang );

	/**
	 * getOutputStmVal/convertOutputNameToStmName/getALMemoryNameOfOutput. return the ALMemory key name related to an output of this behavior
   * @param pOutputName name of an output
   * @return the name of the ALMemory key linked to the output
	 */
    std::string getALMemoryNameOfOutput( const std::string & pOutputName ) const;


	/**
	 * addLink. Add a connection between an ALMemory value and one input, it's a kind of trigger for this behavior: when a value in ALMemory change, the value is send to an input of this behavior.
   * @param pInputName name of an input
   * @param pALMemoryValueName name of the ALMemory that will 
   * @return true if everything's ok
	 */
    bool addLink( const std::string & pInputName, const std::string & pALMemoryValueName );

	/**
	 * removeLink. remove a link created by the addLink method
   * @param pInputName name of an input
   * @param pALMemoryValueName name of the ALMemory that will be removed
   * @return true if everything's ok
	 */
    bool removeLink( const std::string & pInputName, const std::string & pALMemoryValueName );

    bool addOutputLink( const std::string & pOutputName, const std::string & pALMemoryValueName );
 
    /**
    * dataChanged. Called by stm when subcription
    * has been modified.
    * @param pDataName Name of the modified data
    * @param pValue Name of the modified data
    * @param pMessage Name of the modified data
    */
    virtual void dataChanged( const std::string& pDataName, const AL::ALValue& pValue, const std::string& pMessage );


    std::string version (  ) 
    {
      return "unused";
    }

    bool innerTest( void ) 
    {
      return true;
    }

    /**
     * Log call be called from a behavior. It will log in Choregraphe if there is a Choregraphe, 
     * or print it otherwise.
     */
    void log(const std::string& pStr, moduleType pType);

  protected:
    /**
     * Subscribe or unsubscribe all
     */
    void xSubscribe(bool pSubs);

    //
    // ALBehavior attribut definition
    //

  private:

    ALPtr<ALMemoryProxy>  fStm;
    ALPtr<ALProxy>        fToMe;
    TInputMap             fInputList;
    ALPtr<ALMutexRW>      fInputListMutex;       
    TOutputMap            fOutputList;
    ALPtr<ALMutexRW>      fOutputListMutex;       

    State                 fState;
    ALPtr<ALMutexRW>      fStateMutex;       
  };

}; // namespace AL

#endif // AL_BEHAVIOR_H

