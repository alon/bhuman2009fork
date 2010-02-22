/** 
* @file XabslState.cpp
*
* Implementation of class State and helper classes
*
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/

#include "XabslState.h"
#include "XabslOption.h"

namespace xabsl 
{

State* TransitionToState::getNextState()
{
  return nextState;
}

State::State(const char* name, ErrorHandler& errorHandler,
                         unsigned (*pTimeFunction)())
: NamedItem(name), 
targetState(false), 
errorHandler(errorHandler), 
decisionTree(0),
pTimeFunction(pTimeFunction)
{
}

State::~State()
{
  if (decisionTree != 0) delete decisionTree;
  for (int i=0; i<actions.getSize(); i++)
    delete actions[i];
}

void State::create(InputSource& input,
                         Array<Option*>& options,
                         Array<BasicBehavior*>& basicBehaviors,
                         Array<State*>& states,
                         OptionParameters& parameters,
                         Symbols& symbols,
                         unsigned& timeOfOptionExecution)
{ 
  XABSL_DEBUG_INIT(errorHandler.message("creating state \"%s\"",n));

  char c[100]; 
  int i;
  
  // target state or not
  input.readString(c,1);
  if (*c == '1') 
  {
    targetState = true;
  }
  
  // subsequent option, basic behaviors and output symbol assignments
  int numberOfActions = (int)input.readValue();
  
  subsequentOption = 0;
  for(i = 0; i< numberOfActions; i++)
  {
    Action* action = 
      Action::create(input, 
                      options, 
                      basicBehaviors, 
                      parameters, 
                      symbols, 
                      timeOfOptionExecution, 
                      timeOfStateExecution,
                      actions,
                      errorHandler,
                      pTimeFunction);
    if (errorHandler.errorsOccurred)
    {
      errorHandler.error("XabslState::create(): could not create action for state \"%s\"",n);
      return;
    }
    if (subsequentOption == 0)
      if (ActionOption* actionOption = dynamic_cast<ActionOption*>(action)) 
        subsequentOption = actionOption->option;
    actions.append("", action);
  }
  
  // transition to state or if / else block
  decisionTree = Statement::createStatement(input,actions,errorHandler,
    states,parameters, symbols, timeOfOptionExecution,timeOfStateExecution);
  if (errorHandler.errorsOccurred)
    errorHandler.error("XabslState::create(): could not create decision tree for state \"%s\"",n);
}

State* State::getNextState()
{
  timeOfStateExecution = pTimeFunction() - timeWhenStateWasActivated;
  
  State* nextState = decisionTree->getNextState();
  
  return nextState;
}

void State::reset()
{
  timeWhenStateWasActivated = pTimeFunction();
  timeOfStateExecution = 0;
}

bool State::isTargetState() const
{
  return targetState;
}

} // namespace

