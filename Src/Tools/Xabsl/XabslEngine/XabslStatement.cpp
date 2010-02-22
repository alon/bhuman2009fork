/** 
* @file XabslStatement.cpp
*
* Implementation of class Statement and helper classes
*
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/

#include "XabslStatement.h"
#include "XabslState.h"

namespace xabsl 
{

Statement* Statement::createStatement(InputSource& input,    
                                                  Array<Action*>& actions,
                                                  ErrorHandler& errorHandler,
                                                  Array<State*>& states,
                                                  OptionParameters& parameters,
                                                  Symbols& symbols,    
                                                  unsigned& timeOfOptionExecution,
                                                  unsigned& timeOfStateExecution)
{
  char c[100]; 
  
  input.readString(c,1);
  
  switch (*c)
  {
  case 't':
    return new TransitionToState(input, errorHandler, states);
  case 'i':
    return new IfElseBlock(input, actions, errorHandler, states, 
      parameters, symbols, timeOfOptionExecution, timeOfStateExecution);
  default:
    errorHandler.error("XabslStatement::create(): unknown type \"%c\"",*c);
    return 0;
  }
}

Statement::~Statement()
{
}

IfElseBlock::IfElseBlock(InputSource& input,    
                                     Array<Action*>& actions,
                                     ErrorHandler& errorHandler,
                                     Array<State*>& states,
                                     OptionParameters& parameters,
                                     Symbols& symbols,    
                                     unsigned& timeOfOptionExecution,
                                     unsigned& timeOfStateExecution) :
ifCondition(0), ifStatement(0), elseStatement(0)
{
  
  // if case
  XABSL_DEBUG_INIT(errorHandler.message("creating if statement"));
  
  ifCondition = BooleanExpression::create(input, actions, errorHandler, 
    parameters, symbols, timeOfOptionExecution, timeOfStateExecution);
  if (errorHandler.errorsOccurred)
  {
    errorHandler.error("XabslIfElseBlock::IfElseBlock(): could not create if condition");
    return;
  }
  
  ifStatement = Statement::createStatement(input,actions, errorHandler,
    states,parameters, symbols, timeOfOptionExecution, timeOfStateExecution);
  if (errorHandler.errorsOccurred)
  {
    errorHandler.error("XabslIfElseBlock::IfElseBlock(): could not create if statement");
    return;
  }
    
  // else case
  XABSL_DEBUG_INIT(errorHandler.message("creating else statement"));
  
  elseStatement = Statement::createStatement(input, actions,
    errorHandler, states, parameters, symbols, timeOfOptionExecution, timeOfStateExecution);
  if (errorHandler.errorsOccurred)
    errorHandler.error("XabslIfElseBlock::IfElseBlock(): could not create else statement");
  
}

IfElseBlock::~IfElseBlock()
{
  if (ifCondition != 0) delete ifCondition;
  if (ifStatement != 0) delete ifStatement;
  
  int i;
  for (i=0; i<elseIfStatements.getSize(); i++)
  {
    if (elseIfConditions[i] != 0)
      delete elseIfConditions[i];
    if (elseIfStatements[i] != 0) 
      delete elseIfStatements[i];
  }
  
  if (elseStatement != 0) delete elseStatement;
}

State* IfElseBlock::getNextState()
{
  if (ifCondition->getValue()) return ifStatement->getNextState();
  
  for (int i=0; i<elseIfConditions.getSize(); i++)
  {
    if (elseIfConditions[i]->getValue()) return elseIfStatements[i]->getNextState();
  }
  
  return elseStatement->getNextState();
}

TransitionToState::TransitionToState(InputSource& input,    
                                                 ErrorHandler& errorHandler,
                                                 Array<State*>& states)
{
  char buf[100];
  input.readString(buf,99);
  
  nextState = states[buf];
  
  XABSL_DEBUG_INIT(errorHandler.message("creating a transition to state \"%s\"",nextState->n));
}

} // namespace

