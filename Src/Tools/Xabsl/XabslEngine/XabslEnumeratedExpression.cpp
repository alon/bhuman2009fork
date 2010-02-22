/** 
* @file XabslEnumeratedExpression.cpp
*
* Implementation of EnumeratedExpression and derivates
* 
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/

#include "XabslEnumeratedExpression.h"
#include "XabslOption.h"

namespace xabsl 
{
  
EnumeratedExpression::~EnumeratedExpression()
{
}

EnumeratedExpression* EnumeratedExpression::create(const Enumeration* enumeration,
                                                         InputSource& input, 
                                                         Array<Action*>& actions,
                                                         ErrorHandler& errorHandler,
                                                         OptionParameters& parameters,
                                                         Symbols& symbols,
                                                         unsigned& timeOfOptionExecution,
                                                         unsigned& timeOfStateExecution)
{
  char c[2];
  input.readString(c,1);
  
  switch (*c)
  {
  case 'v':
    return new EnumeratedValue(enumeration, input,errorHandler);
  case 'p':
    return new EnumeratedOptionParameterRef(enumeration, input,errorHandler,parameters);
  case 'i':
    return new EnumeratedInputSymbolRef(enumeration, input,actions,errorHandler,parameters,symbols,timeOfOptionExecution,timeOfStateExecution);
  case 'o':
    return new EnumeratedOutputSymbolRef(enumeration, input,errorHandler,symbols);
  case 'q':
    return new ConditionalEnumeratedExpression(enumeration, input,actions,errorHandler,parameters,symbols,timeOfOptionExecution,timeOfStateExecution);
  default:
    errorHandler.error("XabslEnumeratedExpression::create(): unknown expression type \"%c\"",*c);
  }
  return 0;
}

bool EnumeratedExpression::createOperand(const EnumeratedExpression*& operand,
                                            const Enumeration* enumeration,
                                            InputSource& input, 
                                            Array<Action*>& actions,
                                            ErrorHandler& errorHandler,
                                            OptionParameters& parameters,
                                            Symbols& symbols,
                                            unsigned& timeOfOptionExecution,
                                            unsigned& timeOfStateExecution)
{
  operand = EnumeratedExpression::create(enumeration,input,actions,errorHandler,
    parameters, symbols, timeOfOptionExecution,timeOfStateExecution);
  
  if (operand == 0) 
  {
    errorHandler.error("XabslEnumeratedExpression::createOperand(): created operand is 0");
    return false;
  }
  
  if (errorHandler.errorsOccurred)
  {
    errorHandler.error("XabslEnumeratedExpression::createOperand(): could not create operand");
    if (operand != 0) delete operand;
    return false;
  }
  
  return true;
}

EnumeratedValue::EnumeratedValue(const Enumeration* enumeration,
                                       InputSource& input, 
                                       ErrorHandler& errorHandler)
{
  char buf[100];
  input.readString(buf,99);

  if (enumeration == NULL)
  {
    errorHandler.error("XabslEnumeratedValue::EnumeratedValue(): enumerated value can not be created without specifying enumeration");
    return;
  }

  if (!enumeration->enumElements.exists(buf))
  {
    errorHandler.error("XabslEnumeratedValue::EnumeratedValue(): enum element \"%s\" of enumeration \"%s\" was not registered", buf, enumeration->n);
    return;
  }
  
  value = enumeration->enumElements[buf]->v;
  this->enumeration = enumeration;
}

int EnumeratedValue::getValue() const
{
  return value;
}

EnumeratedOptionParameterRef::EnumeratedOptionParameterRef(const Enumeration* enumeration,
                                                   InputSource& input, 
                                                   ErrorHandler& errorHandler,
                                                   OptionParameters& parameters)
{
  char buf[100];
  input.readString(buf,99);
  
  XABSL_DEBUG_INIT(errorHandler.message("creating a reference to enumerated option parameter \"%s\"",buf));
  
  if (!parameters.enumerated.exists(buf))
  {
    errorHandler.error("XabslEnumeratedOptionParameterRef::EnumeratedOptionParameterRef(): enumerated option parameter \"%s\" does not exist",buf);
    return;
  }
  
  parameter = parameters.enumerated.getPElement(buf)->e;
  this->enumeration = parameters.enumerations[buf];
  if (enumeration != NULL && enumeration != this->enumeration)
  {
    errorHandler.error("XabslEnumeratedOptionParameterRef::EnumeratedOptionParameterRef(): enumeration input symbol \"%s\" does not match enumeration type \"%s\"", buf, enumeration->n);
  }
}

int EnumeratedOptionParameterRef::getValue() const
{
  return *parameter;
}

EnumeratedInputSymbolRef::EnumeratedInputSymbolRef(const Enumeration* enumeration,
                                                               InputSource& input, 
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
  
  XABSL_DEBUG_INIT(errorHandler.message("creating reference to enumerated input symbol \"%s\"",buf));
  
  if (!symbols.enumeratedInputSymbols.exists(buf))
  {
    errorHandler.error("XabslEnumeratedInputSymbolRef::XabslEnumeratedInputSymbolRef(): enumerated input symbol \"%s\" was not registered at the engine",buf);
    return;
  }
  
  symbol = symbols.enumeratedInputSymbols[buf];
  this->enumeration = symbol->enumeration;

  if (enumeration != NULL && enumeration != this->enumeration)
  {
    errorHandler.error("XabslEnumeratedInputSymbolRef::XabslEnumeratedInputSymbolRef(): enumeration input symbol \"%s\" does not match enumeration type \"%s\"", buf, enumeration->n);
  }

  parameters = new ParameterAssignment(&symbol->parameters, errorHandler);

  parameters->create(input, optionParameters, symbols, timeOfOptionExecution, timeOfStateExecution, actions);
}
  
EnumeratedInputSymbolRef::~EnumeratedInputSymbolRef()
{
  if (parameters != 0)
    delete parameters;
}

int EnumeratedInputSymbolRef::getValue() const
{
  // set the symbol parameters
  if (parameters->set())
    symbol->parametersChanged();

  return symbol->getValue();
}

EnumeratedOutputSymbolRef::EnumeratedOutputSymbolRef(const Enumeration* enumeration,
                                                               InputSource& input, 
                                                               ErrorHandler& errorHandler,
                                                               Symbols& symbols)
{
  char buf[100];
  input.readString(buf,99);
  
  XABSL_DEBUG_INIT(errorHandler.message("creating a reference to an enumerated output symbol \"%s\"",buf));
  
  if (!symbols.enumeratedOutputSymbols.exists(buf))
  {
    errorHandler.error("XabslEnumeratedOutputSymbolRef::EnumeratedOutputSymbolRef(): enumerated output symbol \"%s\" was not registered",buf);
    return;
  }
  
  symbol = symbols.enumeratedOutputSymbols[buf];
  this->enumeration = symbol->enumeration;

  if (enumeration != NULL && enumeration != this->enumeration)
  {
    errorHandler.error("XabslEnumeratedInputSymbolRef::XabslEnumeratedInputSymbolRef(): enumeration input symbol \"%s\" does not match enumeration type \"%s\"", buf, enumeration->n);
  }
}

int EnumeratedOutputSymbolRef::getValue() const
{
  return symbol->getValue();
}

ConditionalEnumeratedExpression::ConditionalEnumeratedExpression(const Enumeration* enumeration,
    InputSource& input, 
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

  if (!EnumeratedExpression::createOperand(expression1,enumeration,input,actions,errorHandler,parameters,symbols,timeOfOptionExecution,timeOfStateExecution))
  {
    errorHandler.error("XabslQuestionMarkOperator::QuestionMarkOperator(): could not create decimal expression1");
    return;
  }
  
  if (!EnumeratedExpression::createOperand(expression2,enumeration,input,actions,errorHandler,parameters,symbols,timeOfOptionExecution,timeOfStateExecution))
  {
    errorHandler.error("XabslQuestionMarkOperator::QuestionMarkOperator(): could not create decimal expression2");
    return;
  }
  this->enumeration = enumeration;
}

ConditionalEnumeratedExpression::~ConditionalEnumeratedExpression()
{
  if (condition!=0) delete condition;
  if (expression1!=0) delete expression1;
  if (expression2!=0) delete expression2;
}

int ConditionalEnumeratedExpression::getValue() const
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

