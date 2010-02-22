/** 
* @file XabslDecimalExpression.cpp
*
* Implementation of DecimalExpression and derivates
* 
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/

#include "XabslOption.h"
#include "XabslDecimalExpression.h"
#include "XabslBooleanExpression.h"
#include "XabslEnumeratedExpression.h"

namespace xabsl 
{
  
DecimalExpression::~DecimalExpression()
{
}

DecimalExpression* DecimalExpression::create(InputSource& input, 
                                                         Array<Action*>& actions,
                                                         ErrorHandler& errorHandler,
                                                         OptionParameters& parameters,
                                                         Symbols& symbols,
                                                         unsigned& timeOfOptionExecution,
                                                         unsigned& timeOfStateExecution)
{
  char c[100];
  input.readString(c,1);
  ArithmeticOperator* arithmeticOperator;
  DecimalExpression* operand1;
  DecimalExpression* operand2;
  
  switch (*c)
  {
  case 'i':
    return new DecimalInputSymbolRef(input,actions,errorHandler,parameters,symbols,timeOfOptionExecution,timeOfStateExecution);
  case 'o':
    return new DecimalOutputSymbolRef(input,errorHandler,symbols);
  case 'c':
    // constants are treates as decimal values (there is no difference from the engines point of view.)
  case 'v':
    return new DecimalValue(input,errorHandler);
  case 'p':
    return new DecimalOptionParameterRef(input,errorHandler,parameters);
  case '+':
    if (!DecimalExpression::createOperand(operand1,input,actions,errorHandler,parameters,symbols,timeOfOptionExecution,timeOfStateExecution))
      return 0;
    if (!DecimalExpression::createOperand(operand2,input,actions,errorHandler,parameters,symbols,timeOfOptionExecution,timeOfStateExecution))
      return 0;
    
    XABSL_DEBUG_INIT(errorHandler.message("creating + operator"));
    arithmeticOperator = new PlusOperator();
    arithmeticOperator->create(operand1,operand2);
    return arithmeticOperator;
  case '-':
    if (!DecimalExpression::createOperand(operand1,input,actions,errorHandler,parameters,symbols,timeOfOptionExecution,timeOfStateExecution))
      return 0;
    if (!DecimalExpression::createOperand(operand2,input,actions,errorHandler,parameters,symbols,timeOfOptionExecution,timeOfStateExecution))
      return 0;
    
    XABSL_DEBUG_INIT(errorHandler.message("creating - operator"));
    arithmeticOperator = new MinusOperator();
    arithmeticOperator->create(operand1,operand2);
    return arithmeticOperator;
  case '*':
    if (!DecimalExpression::createOperand(operand1,input,actions,errorHandler,parameters,symbols,timeOfOptionExecution,timeOfStateExecution))
      return 0;
    if (!DecimalExpression::createOperand(operand2,input,actions,errorHandler,parameters,symbols,timeOfOptionExecution,timeOfStateExecution))
      return 0;
    
    XABSL_DEBUG_INIT(errorHandler.message("creating * operator"));
    arithmeticOperator = new MultiplyOperator();
    arithmeticOperator->create(operand1,operand2);
    return arithmeticOperator;
  case 'd':
    if (!DecimalExpression::createOperand(operand1,input,actions,errorHandler,parameters,symbols,timeOfOptionExecution,timeOfStateExecution))
      return 0;
    if (!DecimalExpression::createOperand(operand2,input,actions,errorHandler,parameters,symbols,timeOfOptionExecution,timeOfStateExecution))
      return 0;
    
    XABSL_DEBUG_INIT(errorHandler.message("creating / operator"));
    arithmeticOperator = new DivideOperator();
    arithmeticOperator->create(operand1,operand2);
    return arithmeticOperator;
    
  case '%':
    if (!DecimalExpression::createOperand(operand1,input,actions,errorHandler,parameters,symbols,timeOfOptionExecution,timeOfStateExecution))
      return 0;
    if (!DecimalExpression::createOperand(operand2,input,actions,errorHandler,parameters,symbols,timeOfOptionExecution,timeOfStateExecution))
      return 0;
    
    XABSL_DEBUG_INIT(errorHandler.message("creating % operator"));
    arithmeticOperator = new ModOperator();
    arithmeticOperator->create(operand1,operand2);
    return arithmeticOperator;
  case 's':
    return new TimeRef(errorHandler,timeOfStateExecution);
  case 't':
    return new TimeRef(errorHandler,timeOfOptionExecution);
  case 'q':
    return new ConditionalDecimalExpression(input,actions,errorHandler,parameters,symbols,timeOfOptionExecution,timeOfStateExecution);
  default:
    errorHandler.error("XabslDecimalExpression::create(): unknown expression type: \"%c\"",*c);
    return 0;
  }
}

bool DecimalExpression::createOperand(DecimalExpression*& operand,
                                            InputSource& input, 
                                            Array<Action*>& actions,
                                            ErrorHandler& errorHandler,
                                            OptionParameters& parameters,
                                            Symbols& symbols,
                                            unsigned& timeOfOptionExecution,
                                            unsigned& timeOfStateExecution)
{
  operand = DecimalExpression::create(input,actions,errorHandler,parameters, symbols,
    timeOfOptionExecution,timeOfStateExecution);
  
  if (operand == 0) 
  {
    errorHandler.error("XabslDecimalExpression::createOperand(): created operand is 0");
    return false;
  }
  
  if (errorHandler.errorsOccurred)
  {
    errorHandler.error("XabslDecimalExpression::createOperand(): could not create operand");
    if (operand != 0) delete operand;
    return false;
  }
  
  return true;
}

DecimalValue::DecimalValue(InputSource& input, 
                                       ErrorHandler& errorHandler)
{
  value = input.readValue();
  
  XABSL_DEBUG_INIT(errorHandler.message("created decimal value: \"%.2f\"",value));
}

double DecimalValue::getValue() const
{
  return value;
}

DecimalOptionParameterRef::DecimalOptionParameterRef(InputSource& input, 
                                                   ErrorHandler& errorHandler,
                                                   OptionParameters& parameters)
{
  char buf[100];
  input.readString(buf,99);
  
  XABSL_DEBUG_INIT(errorHandler.message("creating a reference to decimal option parameter \"%s\"",buf));
  
  if (!parameters.decimal.exists(buf))
  {
    errorHandler.error("XabslDecimalOptionParameterRef::DecimalOptionParameterRef(): decimal option parameter \"%s\" does not exist",buf);
    return;
  }
  
  parameter = parameters.decimal.getPElement(buf)->e;
}

double DecimalOptionParameterRef::getValue() const
{
  return *parameter;
}

void ArithmeticOperator::create(DecimalExpression* operand1, DecimalExpression* operand2)
{
  this->operand1 = operand1;
  this->operand2 = operand2;
}

ArithmeticOperator::~ArithmeticOperator()
{
  if (operand1 != 0) delete operand1;
  if (operand2 != 0) delete operand2;
}

double PlusOperator::getValue() const
{
  return operand1->getValue() + operand2->getValue();
}

double MinusOperator::getValue() const
{
  return operand1->getValue() - operand2->getValue();
}

double MultiplyOperator::getValue() const
{
  return operand1->getValue() * operand2->getValue();
}

double DivideOperator::getValue() const
{
  double o2 = operand2->getValue();
  if (o2==0) 
    return operand1->getValue() / 0.0000001;
  else
    return operand1->getValue() / o2;
}

double ModOperator::getValue() const
{
  return (int)operand1->getValue() % (int)operand2->getValue();
}

TimeRef::TimeRef(ErrorHandler& errorHandler,
                             unsigned& time) :
time(time)
{
  XABSL_DEBUG_INIT(errorHandler.message("creating a reference to state or option execution time"));
}

double TimeRef::getValue() const
{
  return time;
}

DecimalInputSymbolRef::DecimalInputSymbolRef(InputSource& input, 
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
  
  XABSL_DEBUG_INIT(errorHandler.message("creating a reference to decimal input symbol \"%s\"",buf));
  
  if (!symbols.decimalInputSymbols.exists(buf))
  {
    errorHandler.error("XabslDecimalInputSymbolRef::DecimalInputSymbolRef(): decimal input symbol \"%s\" was not registered",buf);
    return;
  }
  
  symbol = symbols.decimalInputSymbols[buf];

  parameters = new ParameterAssignment(&symbol->parameters, errorHandler);

  parameters->create(input, optionParameters, symbols, timeOfOptionExecution, timeOfStateExecution, actions);
}

DecimalInputSymbolRef::~DecimalInputSymbolRef()
{
  if (parameters != 0)
    delete parameters;
}

double DecimalInputSymbolRef::getValue() const
{
  // set the symbol parameters
  if (parameters->set())
    symbol->parametersChanged();

  return symbol->getValue();
}

DecimalOutputSymbolRef::DecimalOutputSymbolRef(InputSource& input, 
                                                               ErrorHandler& errorHandler,
                                                               Symbols& symbols)
{
  char buf[100];
  input.readString(buf,99);
  
  XABSL_DEBUG_INIT(errorHandler.message("creating a reference to a decimal output symbol \"%s\"",buf));
  
  if (!symbols.decimalOutputSymbols.exists(buf))
  {
    errorHandler.error("XabslDecimalOutputSymbolRef::DecimalOutputSymbolRef(): decimal output symbol \"%s\" was not registered",buf);
    return;
  }
  
  symbol = symbols.decimalOutputSymbols[buf];
}

double DecimalOutputSymbolRef::getValue() const
{
  return symbol->getValue();
}

ConditionalDecimalExpression::ConditionalDecimalExpression(InputSource& input, 
    Array<Action*>& actions,
    ErrorHandler& errorHandler,
    OptionParameters& parameters,
    Symbols& symbols,
    unsigned& timeOfOptionExecution,
    unsigned& timeOfStateExecution)
{
  XABSL_DEBUG_INIT(errorHandler.message("creating a question mark operator"));

  condition = BooleanExpression::create(input,actions,errorHandler,parameters, symbols,
    timeOfOptionExecution,timeOfStateExecution);

  if (condition == 0) 
  {
    errorHandler.error("XabslQuestionMarkOperator::QuestionMarkOperator(): created condition is 0");
    return;
  }
  else if (errorHandler.errorsOccurred)
  {
    errorHandler.error("XabslQuestionMarkOperator::QuestionMarkOperator(): could not create condition");
    delete condition;
    return;
  }

  if (!DecimalExpression::createOperand(expression1,input,actions,errorHandler,parameters,symbols,timeOfOptionExecution,timeOfStateExecution))
  {
    errorHandler.error("XabslQuestionMarkOperator::QuestionMarkOperator(): could not create decimal expression1");
    return;
  }
  
  if (!DecimalExpression::createOperand(expression2,input,actions,errorHandler,parameters,symbols,timeOfOptionExecution,timeOfStateExecution))
  {
    errorHandler.error("XabslQuestionMarkOperator::QuestionMarkOperator(): could not create decimal expression2");
    return;
  }
  
}

ConditionalDecimalExpression::~ConditionalDecimalExpression()
{
  if (condition!=0) delete condition;
  if (expression1!=0) delete expression1;
  if (expression2!=0) delete expression2;
}

double ConditionalDecimalExpression::getValue() const
{
  if (condition->getValue())
  {
    return expression1->getValue();
  }
  else
  {
    return expression2->getValue();
  }
}

} // namespace

