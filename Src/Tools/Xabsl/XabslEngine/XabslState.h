/**
* @file XabslState.h
* 
* Definition of class State and Helper classes
*
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/

#ifndef __XabslState_h_
#define __XabslState_h_

#include "XabslStatement.h"
#include "XabslBasicBehavior.h"
#include "XabslDecimalExpression.h"
#include "XabslBooleanExpression.h"
#include "XabslEnumeratedExpression.h"
#include "XabslAction.h"

namespace xabsl 
{

// class prototype needed for declaration of State
class Option;

/**
* @class State
*
* Represents a single state written in 
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class State : public NamedItem
{
public:
  /** 
  * Constructor. Does not create the state.
  * @param name The name of the state. For debugging purposes.
  * @param errorHandler A reference to a ErrorHandler instance
  * @param pTimeFunction a pointer to a function that returns the system time in ms.
  */
  State(const char* name, 
    ErrorHandler& errorHandler,
    unsigned (*pTimeFunction)());
  
  /** Destructor */
  ~State();
  
  /** 
  * Creates the state and it's subelements from the intermediate code.
  * @param input An input source for the intermediate code. It must be opened and read until 
  *              A position where a state starts.
  * @param options All available options
  * @param basicBehaviors All available basicBehaviors
  * @param states All states of the option
  * @param parameters The parameters of the option
  * @param symbols All available symbols
  * @param timeOfOptionExecution The time how long the option is already active
  */
  void create(InputSource& input,    
    Array<Option*>& options,
    Array<BasicBehavior*>& basicBehaviors,
    Array<State*>& states,
    OptionParameters& parameters,
    Symbols& symbols,
    unsigned& timeOfOptionExecution);
  
  /** 
  * Executes the decision tree and determines the next active state (can be the same). 
  */
  State* getNextState();
  
  /** The actions of the state */
  Array<Action*> actions;

  /** 
  * The first subsequent option that is executed after that option if the state is active. 
  * If 0, no subsequent option is executed after that option. 
  * This pointer is present just for compatibility reasons, please use getactions instead
  */
  Option* subsequentOption;  

  /** The time how long the state is already active */
  unsigned timeOfStateExecution;
  
  /** Sets the time when the state was activated to 0 */
  void reset();

  /** Returns wheter the state is a target state */
  bool isTargetState() const;

private:

  /** The time, when the state was activated */
  unsigned timeWhenStateWasActivated;

  /** If true, the state is a target state */
  bool targetState;

  /** Used for error handling */
  ErrorHandler& errorHandler;
  
  /** The root element of the decision tree */
  Statement* decisionTree;

  /** A pointer to a function that returns the system time in ms. */
  unsigned (*pTimeFunction)();
};

} // namespace

#endif //__XabslState_h_
