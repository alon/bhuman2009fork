/** 
* @file XabslBooleanExpression.cpp
*
* Implementation of BooleanExpression and derivates
* 
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/

#include "XabslBooleanExpression.h"
#include "XabslOption.h"

namespace xabsl 
{
  
BooleanExpression::~BooleanExpression()
{
}

BooleanExpression* BooleanExpression::create(InputSource& input, 
                                                         Array<Action*>& actions,
                                                         ErrorHandler& errorHandler,
                                                         OptionParameters& parameters,
                                                         Symbols& symbols,
                                                         unsigned& timeOfOptionExecution,
                                                         unsigned& timeOfStateExecution)
{
  char c[100];
  BooleanExpression* booleanOperand = 0;
  DecimalExpression* decimalOperand1 = 0;
  DecimalExpression* decimalOperand2 = 0;
  RelationalAndEqualityOperator* relationalAndEqualityOperator = 0;
  input.readString(c,1);
  
  switch (*c)
  {
  case 'v':
    return new BooleanValue(input,errorHandler);
  case 'p':
    return new BooleanOptionParameterRef(input,errorHandler,parameters);
  case 'i':
    return new BooleanInputSymbolRef(input,actions,errorHandler,parameters,symbols,timeOfOptionExecution,timeOfStateExecution);
  case 'o':
    return new BooleanOutputSymbolRef(input,errorHandler,symbols);
  case 't':
    return new SubsequentOptionReachedTargetStateCondition(actions, errorHandler);
  case 'c':
    return new EnumeratedInputSymbolComparison(input, actions, errorHandler, parameters, symbols, timeOfOptionExecution, timeOfStateExecution);
  case '&':
    {
      XABSL_DEBUG_INIT(errorHandler.message("creating and operator"));
      
      int numberOfOperands = (int)input.readValue();
      
      AndOperator* andOperator = new AndOperator();
      
      for (int i=0; i<numberOfOperands; i++)
      {
        if (!BooleanExpression::createOperand(booleanOperand, input, actions, errorHandler, parameters, symbols, timeOfOptionExecution, timeOfStateExecution))
        {
          delete andOperator;
          return 0;
        }
        
        andOperator->addOperand(booleanOperand);
      }
      
      return andOperator;
    }
  case '|':
    {
      XABSL_DEBUG_INIT(errorHandler.message("creating or operator"));
      
      int numberOfOperands = (int)input.readValue();
      
      OrOperator* orOperator = new OrOperator();
      
      for (int i=0; i<numberOfOperands; i++)
      {
        if (!BooleanExpression::createOperand(booleanOperand, input, actions, errorHandler, parameters, symbols, timeOfOptionExecution, timeOfStateExecution))
        {
          delete orOperator;
          return 0;
        }
        orOperator->addOperand(booleanOperand);
      }
      
      return orOperator;
    }
  case '!':
    XABSL_DEBUG_INIT(errorHandler.message("creating not operator"));
    
    if (!BooleanExpression::createOperand(booleanOperand, input, actions, errorHandler, parameters, symbols, timeOfOptionExecution, timeOfStateExecution))
      return 0;
    
    return new NotOperator(booleanOperand);
  case '=':
    XABSL_DEBUG_INIT(errorHandler.message("creating == operator"));
    
    if (!DecimalExpression::createOperand(decimalOperand1,input, actions, errorHandler, parameters, symbols, timeOfOptionExecution, timeOfStateExecution))
      return 0;
    
    if (!DecimalExpression::createOperand(decimalOperand2,input, actions, errorHandler, parameters, symbols, timeOfOptionExecution, timeOfStateExecution))
      return 0;
    
    relationalAndEqualityOperator = new EqualToOperator();
    relationalAndEqualityOperator->create(decimalOperand1,decimalOperand2);
    return relationalAndEqualityOperator;
  case 'n':
    XABSL_DEBUG_INIT(errorHandler.message("creating != operator"));
    
    if (!DecimalExpression::createOperand(decimalOperand1,input,actions,errorHandler, parameters, symbols, timeOfOptionExecution, timeOfStateExecution))
      return 0;
    
    if (!DecimalExpression::createOperand(decimalOperand2,input,actions,errorHandler, parameters, symbols, timeOfOptionExecution, timeOfStateExecution))
      return 0;
    
    relationalAndEqualityOperator = new NotEqualToOperator();
    relationalAndEqualityOperator->create(decimalOperand1,decimalOperand2);
    return relationalAndEqualityOperator;
  case '<':
    XABSL_DEBUG_INIT(errorHandler.message("creating < operator"));
    
    if (!DecimalExpression::createOperand(decimalOperand1,input,actions,errorHandler, parameters, symbols, timeOfOptionExecution, timeOfStateExecution))
      return 0;
    
    if (!DecimalExpression::createOperand(decimalOperand2,input,actions,errorHandler, parameters, symbols, timeOfOptionExecution, timeOfStateExecution))
      return 0;
    
    relationalAndEqualityOperator = new LessThanOperator();
    relationalAndEqualityOperator->create(decimalOperand1,decimalOperand2);
    return relationalAndEqualityOperator;
  case 'l':
    XABSL_DEBUG_INIT(errorHandler.message("creating <= operator"));
    
    if (!DecimalExpression::createOperand(decimalOperand1,input,actions,errorHandler, parameters, symbols, timeOfOptionExecution, timeOfStateExecution))
      return 0;
    
    if (!DecimalExpression::createOperand(decimalOperand2,input,actions,errorHandler, parameters, symbols, timeOfOptionExecution, timeOfStateExecution))
      return 0;
    
    relationalAndEqualityOperator = new LessThanOrEqualToOperator();
    relationalAndEqualityOperator->create(decimalOperand1,decimalOperand2);
    return relationalAndEqualityOperator;
  case '>':
    XABSL_DEBUG_INIT(errorHandler.message("creating > operator"));
    
    if (!DecimalExpression::createOperand(decimalOperand1,input,actions,errorHandler, parameters, symbols, timeOfOptionExecution, timeOfStateExecution))
      return 0;
    
    if (!DecimalExpression::createOperand(decimalOperand2,input,actions,errorHandler, parameters, symbols, timeOfOptionExecution, timeOfStateExecution))
      return 0;
    
    relationalAndEqualityOperator = new GreaterThanOperator();
    relationalAndEqualityOperator->create(decimalOperand1,decimalOperand2);
    return relationalAndEqualityOperator;
  case 'g':
    XABSL_DEBUG_INIT(errorHandler.message("creating >= operator"));
    
    if (!DecimalExpression::createOperand(decimalOperand1,input,actions,errorHandler, parameters, symbols, timeOfOptionExecution, timeOfStateExecution))
      return 0;
    
    if (!DecimalExpression::createOperand(decimalOperand2,input,actions,errorHandler, parameters, symbols, timeOfOptionExecution, timeOfStateExecution))
      return 0;
    
    relationalAndEqualityOperator = new GreaterThanOrEqualToOperator();
    relationalAndEqualityOperator->create(decimalOperand1,decimalOperand2);
    return relationalAndEqualityOperator;
  default:
    errorHandler.error("XabslBooleanExpression::create(): unknown expression type \"%c\"",*c);
  }
  return 0;
}

bool BooleanExpression::createOperand(BooleanExpression*& operand,
                                            InputSource& input, 
                                            Array<Action*>& actions,
                                            ErrorHandler& errorHandler,
                                            OptionParameters& parameters,
                                            Symbols& symbols,
                                            unsigned& timeOfOptionExecution,
                                            unsigned& timeOfStateExecution)
{
  operand = BooleanExpression::create(input,actions,errorHandler,
    parameters, symbols, timeOfOptionExecution,timeOfStateExecution);
  
  if (operand == 0) 
  {
    errorHandler.error("XabslBooleanExpression::createOperand(): created operand is 0");
    return false;
  }
  
  if (errorHandler.errorsOccurred)
  {
    errorHandler.error("XabslBooleanExpression::createOperand(): could not create operand");
    if (operand != 0) delete operand;
    return false;
  }
  
  return true;
}

BooleanValue::BooleanValue(InputSource& input, 
                                       ErrorHandler& errorHandler)
{
  char buf[6];
  input.readString(buf,5);

  value = (strcmp("true",buf) == 0);
  
  XABSL_DEBUG_INIT(errorHandler.message("created boolean value: \"%s\"",value?"true":"false"));
}

bool BooleanValue::getValue() const
{
  return value;
}

BooleanOptionParameterRef::BooleanOptionParameterRef(InputSource& input, 
                                                   ErrorHandler& errorHandler,
                                                   OptionParameters& parameters)
{
  char buf[100];
  input.readString(buf,99);
  
  XABSL_DEBUG_INIT(errorHandler.message("creating a reference to boolean option parameter \"%s\"",buf));
  
  if (!parameters.boolean.exists(buf))
  {
    errorHandler.error("XabslBooleanOptionParameterRef::BooleanOptionParameterRef(): boolean option parameter \"%s\" does not exist",buf);
    return;
  }
  
  parameter = parameters.boolean.getPElement(buf)->e;
}

bool BooleanOptionParameterRef::getValue() const
{
  return *parameter;
}

AndOperator::AndOperator() 
{
  operands.clear();
}

AndOperator::~AndOperator()
{
  for (int i=0; i< operands.getSize(); i++)
  {
    if (operands[i]!=0) delete operands[i];
  }
}

bool AndOperator::getValue() const
{
  for (int i=0; i< operands.getSize(); i++)
  {
    if (operands[i]->getValue() == false) return false;
  }
  return true;
}

void AndOperator::addOperand(BooleanExpression* operand)
{
  operands.append("",operand);
}

OrOperator::OrOperator() 
{
  operands.clear();
}

OrOperator::~OrOperator()
{
  for (int i=0; i< operands.getSize(); i++)
  {
    if (operands[i]!=0) delete operands[i];
  }
}

bool OrOperator::getValue() const
{
  for (int i=0; i< operands.getSize(); i++)
  {
    if (operands[i]->getValue() == true) return true;
  }
  return false;
}

void OrOperator::addOperand(BooleanExpression* operand)
{
  operands.append("",operand);
}

NotOperator::NotOperator(BooleanExpression* operand1) :
operand1(operand1)
{
}

NotOperator::~NotOperator()
{
  if (operand1!=0) delete operand1;
}

bool NotOperator::getValue() const
{
  return !(operand1->getValue());
}

BooleanInputSymbolRef::BooleanInputSymbolRef(InputSource& input, 
                                                               Array<Action*>& actions,
                                                               ErrorHandler& errorHandler,
                                                               OptionParameters& optionParameters,
                                                               Symbols& symbols,
                                                               unsigned& timeOfOptionExecution,
                                                               unsigned& timeOfStateExecution) :
  symbol(0), parameters(0)
{
  char buf[100];
  input.readString(buf,99);
  
  XABSL_DEBUG_INIT(errorHandler.message("creating reference to boolean input symbol \"%s\"",buf));
  
  if (!symbols.booleanInputSymbols.exists(buf))
  {
    errorHandler.error("XabslBooleanInputSymbolRef::XabslBooleanInputSymbolRef(): boolean input symbol \"%s\" was not registered at the engine",buf);
    return;
  }
  
  symbol = symbols.booleanInputSymbols[buf];

  parameters = new ParameterAssignment(&symbol->parameters, errorHandler);

  parameters->create(input, optionParameters, symbols, timeOfOptionExecution, timeOfStateExecution, actions);
}
  
BooleanInputSymbolRef::~BooleanInputSymbolRef()
{
  if (parameters != 0)
    delete parameters;
}

bool BooleanInputSymbolRef::getValue() const
{
  // set the symbol parameters
  if (parameters->set())
    symbol->parametersChanged();

  return symbol->getValue();
}

BooleanOutputSymbolRef::BooleanOutputSymbolRef(InputSource& input, 
                                                               ErrorHandler& errorHandler,
                                                               Symbols& symbols)
{
  char buf[100];
  input.readString(buf,99);
  
  XABSL_DEBUG_INIT(errorHandler.message("creating a reference to a boolean output symbol \"%s\"",buf));
  
  if (!symbols.booleanOutputSymbols.exists(buf))
  {
    errorHandler.error("XabslBooleanOutputSymbolRef::BooleanOutputSymbolRef(): boolean output symbol \"%s\" was not registered",buf);
    return;
  }
  
  symbol = symbols.booleanOutputSymbols[buf];
}

bool BooleanOutputSymbolRef::getValue() const
{
  return symbol->getValue();
}

SubsequentOptionReachedTargetStateCondition::SubsequentOptionReachedTargetStateCondition(Array<Action*>& actions,
                                                   ErrorHandler& errorHandler)
                                                   : actions(actions)
{
  XABSL_DEBUG_INIT(errorHandler.message("creating a \"subsequent-option-reached-target-state\" element"));
}

bool SubsequentOptionReachedTargetStateCondition::getValue() const
{
  bool anySubsequentBehaviorReachedTargetState = false;
  for (int i = 0; i < actions.getSize(); i++)
    if (ActionOption* subsequentAction = dynamic_cast<ActionOption*>(actions[i]))
      if (subsequentAction->option->getOptionReachedATargetState())
        anySubsequentBehaviorReachedTargetState = true;
  return anySubsequentBehaviorReachedTargetState;
}

EnumeratedInputSymbolComparison::EnumeratedInputSymbolComparison(InputSource& input,
  Array<Action*>& actions,
  ErrorHandler& errorHandler,
  OptionParameters& parameters,
  Symbols& symbols,
  unsigned& timeOfOptionExecution,
  unsigned& timeOfStateExecution)
{
  if (!EnumeratedExpression::createOperand(operand1,NULL,input,actions,errorHandler,parameters,symbols,timeOfOptionExecution,timeOfStateExecution))
  {
    errorHandler.error("XabslEnumeratedInputSymbolComparison::EnumeratedInputSymbolComparison(): could not create enumerated expression");
    operand1 = operand2 = 0;
    return;
  }
  if (!EnumeratedExpression::createOperand(operand2,operand1->enumeration,input,actions,errorHandler,parameters,symbols,timeOfOptionExecution,timeOfStateExecution))
  {
    errorHandler.error("XabslEnumeratedInputSymbolComparison::EnumeratedInputSymbolComparison(): could not create enumerated expression");
    operand2 = 0;
    return;
  }
}

EnumeratedInputSymbolComparison::~EnumeratedInputSymbolComparison()
{
  if (operand1!=0) delete operand1;
  if (operand2!=0) delete operand2;
}

bool EnumeratedInputSymbolComparison::getValue() const
{
  return (operand1->getValue() == operand2->getValue());
}

void RelationalAndEqualityOperator::create(DecimalExpression* operand1,
                                                 DecimalExpression* operand2)
{
  this->operand1 = operand1;
  this->operand2 = operand2;
}

RelationalAndEqualityOperator ::~RelationalAndEqualityOperator ()
{
  if (operand1!=0) delete operand1;
  if (operand2!=0) delete operand2;
}

bool EqualToOperator::getValue() const
{
  return (operand1->getValue() == operand2->getValue());
}

bool NotEqualToOperator::getValue() const
{
  return (operand1->getValue() != operand2->getValue());
}

bool LessThanOperator::getValue() const
{
  return (operand1->getValue() < operand2->getValue());
}

bool LessThanOrEqualToOperator::getValue() const
{
  return (operand1->getValue() <= operand2->getValue());
}

bool GreaterThanOperator::getValue() const
{
  return (operand1->getValue() > operand2->getValue());
}

bool GreaterThanOrEqualToOperator::getValue() const
{
  return (operand1->getValue() >= operand2->getValue());
}

} // namespace

