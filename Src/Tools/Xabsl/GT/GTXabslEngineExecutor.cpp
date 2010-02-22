/**
* @file GTXabslEngineExecutor.cpp
* 
* Implementation of class GTXabslEngineExecutor.
*
* @author Martin Lötzsch
*/

#include "GTXabslEngineExecutor.h"
#include "Tools/Debugging/Debugging.h"
#include "Platform/GTAssert.h"
#include "Platform/SystemCall.h"

GTXabslEngineExecutor::GTXabslEngineExecutor (const char* id) 
: pEngine(0), errorHandler(id), id(id)
{
}

GTXabslEngineExecutor::~GTXabslEngineExecutor()
{
  if (pEngine!=0) delete pEngine;
}

void GTXabslEngineExecutor::init()
{
  char filename[100];
  sprintf(filename,"Xabsl/%s-ic.dat", id);
  XabslFileInputSource input(filename);
  init(input);
}

void GTXabslEngineExecutor::init(xabsl::InputSource& input)
{
  if (pEngine != 0) delete pEngine;
  watchedBooleanInputSymbols.clear();
  watchedDecimalInputSymbols.clear();
  watchedEnumeratedInputSymbols.clear();
  watchedDecimalOutputSymbols.clear();
  watchedBooleanOutputSymbols.clear();
  watchedEnumeratedOutputSymbols.clear();
  setDecimalOutputSymbols.clear();
  setDecimalOutputSymbolValues.clear();
  setBooleanOutputSymbols.clear();
  setBooleanOutputSymbolValues.clear();
  setEnumeratedOutputSymbols.clear();
  setEnumeratedOutputSymbolValues.clear();
  
  errorHandler.errorsOccurred = false; // reset the error handler
  
  #ifdef LINUX
  pEngine = new xabsl::Engine(errorHandler,(unsigned int (*)(void))(&SystemCall::getCurrentSystemTime));
  #else
  pEngine = new xabsl::Engine(errorHandler,(unsigned int (__cdecl *)(void))(&SystemCall::getCurrentSystemTime));
  #endif
  registerSymbolsAndBasicBehaviors();
  
  pEngine->createOptionGraph(input);

  if (!errorHandler.errorsOccurred)
  {
    errorHandler.message("created a new Engine");
    
  }
}

void GTXabslEngineExecutor::executeEngine()
{
  if (errorHandler.errorsOccurred)
  {
    executeIfEngineCouldNotBeCreated();
    return;
  }
  
  // execute the option graph beginning from the current root option
  // which was set to a specific option or basic behavior
  pEngine->execute();
  
  // Set the output symbols that were requested by the Xabsl Dialog
  for (int i=0; i<setDecimalOutputSymbols.getSize(); i++)
  {
    setDecimalOutputSymbols[i]->setValue(setDecimalOutputSymbolValues[i]);
  }
  for (int i=0; i<setBooleanOutputSymbols.getSize(); i++)
  {
    setBooleanOutputSymbols[i]->setValue(setBooleanOutputSymbolValues[i]);
  }
  for (int i=0; i<setEnumeratedOutputSymbols.getSize(); i++)
  {
    setEnumeratedOutputSymbols[i]->setValue(setEnumeratedOutputSymbolValues[i]);
  }
  
  DEBUG_RESPONSE("automated requests:xabsl:debugMessages", sendDebugMessage(Global::getDebugOut().bin););
  DEBUG_RESPONSE("automated requests:xabsl:debugSymbols", sendDebugSymbols(Global::getDebugOut().bin););
}

void GTXabslEngineExecutor::sendActiveOptionsToStream(Out &out) const
{
  int i, j, k;
  xabsl::Array<const xabsl::Action*> activeActions;
  xabsl::Array<int> activeDepth;

  // depth-first-traversal of active options and basic behaviors
  xabsl::Array<const xabsl::Action*> currentActions;
  xabsl::Array<int> currentDepth;
  for (i=pEngine->getRootActions().getSize()-1;i>=0;i--)
  {
    currentActions.append("", pEngine->getRootActions()[i]);
    currentDepth.append("",0);
  }

  while(currentActions.getSize() > 0)
  {
    const xabsl::Action* last = currentActions[currentActions.getSize()-1];
    int lastDepth = currentDepth[currentDepth.getSize()-1];
    activeActions.append("", last);
    activeDepth.append("", lastDepth);
    currentActions.removeLast();
    currentDepth.removeLast();
    if (const xabsl::Option* option = last->getOption())
    {
      const xabsl::Array<xabsl::Action*>& actions = 
        option->activeState->actions;

      for(j=actions.getSize()-1;j>=0;j--)
      {
        currentActions.append("", actions[j]);
        currentDepth.append("", lastDepth + 1);
      }
    }
  }

  // number of active options and basic behaviors
  out << activeActions.getSize();

  // For each active option/basic behavior: 
  // name, depth, duration of activation, active state, duration of state activation, parameters
  
  for (i=0; i < activeActions.getSize(); i++)
  {
    out << activeDepth[i];
    if (const xabsl::Behavior* behavior = activeActions[i]->getBehavior())
    {
      const xabsl::Option* option = activeActions[i]->getOption();
      const xabsl::ParameterAssignment* parameters = activeActions[i]->getParameters();

      out << (option ? "o" : "a");
      out << behavior->n << behavior->timeOfExecution;
      if (option)
        out << option->activeState->n << option->activeState->timeOfStateExecution;

      // the number of parameters
      char numberOfParameters = parameters->decimalValues.getSize() + parameters->booleanValues.getSize() + parameters->enumeratedValues.getSize();
      out << numberOfParameters;
      
      for (j = 0; j < parameters->decimalValues.getSize(); j++)
      {
        out << "d" << parameters->decimalValues.getName(j);
        out << parameters->decimalValues[j];
      }
      for (j = 0; j < parameters->booleanValues.getSize(); j++)
      {
        out << "b" << parameters->booleanValues.getName(j);
        out << (char)parameters->booleanValues[j];
      }
      for (j = 0; j < parameters->enumeratedValues.getSize(); j++)
      {
        out << "e" << parameters->enumeratedValues.getName(j);
        const xabsl::Enumeration* enumeration = parameters->enumeratedExpressions[j]->enumeration;

        for (k = 0; k < enumeration->enumElements.getSize(); k++)
          if (enumeration->enumElements[k]->v == parameters->enumeratedValues[j])
          {
            out << enumeration->enumElements[k]->n;
            break;
          }
        if (k == enumeration->enumElements.getSize())
          out << "";
      }
    }
    else if (const xabsl::DecimalOutputSymbol* decimalOutputSymbol = activeActions[i]->getDecimalOutputSymbol())
    {
      out << "d" << decimalOutputSymbol->n;
      out << activeActions[i]->getDecimalOutputSymbolValue();
    }
    else if (const xabsl::BooleanOutputSymbol* booleanOutputSymbol = activeActions[i]->getBooleanOutputSymbol())
    {
      out << "b" << booleanOutputSymbol->n;
      out << (char)activeActions[i]->getBooleanOutputSymbolValue();
    }
    else if (const xabsl::EnumeratedOutputSymbol* enumeratedOutputSymbol = activeActions[i]->getEnumeratedOutputSymbol())
    {
      out << "e" << enumeratedOutputSymbol->n;
      const xabsl::Enumeration* enumeration = enumeratedOutputSymbol->enumeration;

      for (k = 0; k < enumeration->enumElements.getSize(); k++)
        if (enumeration->enumElements[k]->v == activeActions[i]->getEnumeratedOutputSymbolValue())
        {
          out << enumeration->enumElements[k]->n;
          break;
        }
      if (k == enumeration->enumElements.getSize())
        out << "";
    }
  }
}

void GTXabslEngineExecutor::sendDebugMessage(Out& out) const
{
  int i, j;
  
  // send the id of the behavior contol solution
  out << id;
  
  // send the name of the selected agent 
  out << pEngine->getSelectedAgentName();
  
  // watched decimal input symbols;
  out << watchedDecimalInputSymbols.getSize();
  for (i = 0; i < watchedDecimalInputSymbols.getSize(); i++)
    out << watchedDecimalInputSymbols[i]->n << watchedDecimalInputSymbols[i]->getValue();
  
  // watched boolean input symbols;
  out << watchedBooleanInputSymbols.getSize();
  for (i = 0; i < watchedBooleanInputSymbols.getSize(); i++)
    out << watchedBooleanInputSymbols[i]->n << (char)watchedBooleanInputSymbols[i]->getValue();
  
  // watched enumerated input symbols;
  out << watchedEnumeratedInputSymbols.getSize();
  for (i = 0; i < watchedEnumeratedInputSymbols.getSize(); i++)
  {
    const xabsl::EnumeratedInputSymbol* s = watchedEnumeratedInputSymbols[i];
    out << s->n;
    int v = s->getValue();
    for (j = 0; j < s->enumeration->enumElements.getSize();j++)
    {
      if (s->enumeration->enumElements[j]->v == v) break;
    }
    if (j==s->enumeration->enumElements.getSize())
    {
      out << " ";
    }
    else
    {
      out << s->enumeration->enumElements[j]->n; 
    }
  }
  
  // watched decimal output symbols;
  out << watchedDecimalOutputSymbols.getSize();
  for (i = 0; i < watchedDecimalOutputSymbols.getSize(); i++)
    out << watchedDecimalOutputSymbols[i]->n << watchedDecimalOutputSymbols[i]->getValue();
  
  // watched boolean output symbols;
  out << watchedBooleanOutputSymbols.getSize();
  for (i = 0; i < watchedBooleanOutputSymbols.getSize(); i++)
    out << watchedBooleanOutputSymbols[i]->n << (char)watchedBooleanOutputSymbols[i]->getValue();

  // watched enumerated output symbols;
  out << watchedEnumeratedOutputSymbols.getSize();
  for (i=0; i<watchedEnumeratedOutputSymbols.getSize(); i++)
  {
    const xabsl::EnumeratedOutputSymbol* s = watchedEnumeratedOutputSymbols[i];
    out << s->n;
    for (j = 0; j < s->enumeration->enumElements.getSize();j++)
    {
      if (s->enumeration->enumElements[j]->v == s->getValue()) break;
    }
    if (j==s->enumeration->enumElements.getSize())
    {
      out << " ";
    }
    else
    {
      out << s->enumeration->enumElements[j]->n; 
    }
  }
  
  sendActiveOptionsToStream(out);

  // send the generated main action as a string
  char buf[200];
  printGeneratedMainActionToString(buf);
  out << buf;
  
  Global::getDebugOut().finishMessage(idXabslDebugMessage);
}

void GTXabslEngineExecutor::sendDebugSymbols(Out& out) const
{
  int i,j;

  // send the id of the behavior contol solution
  out << id;

  // send agent names
  out << pEngine->getAgents().getSize();
  for (i = 0; i < pEngine->getAgents().getSize(); i++)
    out << pEngine->getAgents()[i]->n;

  // send enumerations
  out << pEngine->enumerations.getSize();
  for (i = 0; i < pEngine->enumerations.getSize(); i++)
  {
    out << pEngine->enumerations[i]->n;
    // send number of elements
    out << pEngine->enumerations[i]->enumElements.getSize();
    // send elements
    for (j = 0; j < pEngine->enumerations[i]->enumElements.getSize(); j++)
      out << pEngine->enumerations[i]->enumElements[j]->n;
  }

  // send decimal input symbols
  out << pEngine->decimalInputSymbols.getSize();
  for (i = 0; i < pEngine->decimalInputSymbols.getSize(); i++)
  {
    const xabsl::DecimalInputSymbol* pSymbol = pEngine->decimalInputSymbols[i];
    out << pSymbol->n;
    // decimal parameters
    out << pSymbol->parameters.decimal.getSize();
    for (j = 0; j < pSymbol->parameters.decimal.getSize(); j++)
      out << pSymbol->parameters.decimal.getName(j);
    // boolean parameters
    out << pSymbol->parameters.boolean.getSize();
    for (j = 0; j < pSymbol->parameters.boolean.getSize(); j++)
      out << pSymbol->parameters.boolean.getName(j);;
    // enumerated parameters
    out << pSymbol->parameters.enumerated.getSize();
    for (j = 0; j < pSymbol->parameters.enumerated.getSize(); j++)
    {
      // enumeration name
      out << pSymbol->parameters.enumerations[j]->n;
      // parameter name
      out << pSymbol->parameters.enumerated.getName(j);;
    }
  }

  // send boolean input symbols
  out << pEngine->booleanInputSymbols.getSize();
  for (i = 0; i < pEngine->booleanInputSymbols.getSize(); i++)
  {
    xabsl::BooleanInputSymbol* pSymbol = pEngine->booleanInputSymbols[i];
    out << pSymbol->n;
    // decimal parameters
    out << pSymbol->parameters.decimal.getSize();
    for (j = 0; j < pSymbol->parameters.decimal.getSize(); j++)
      out << pSymbol->parameters.decimal.getName(j);
    // boolean parameters
    out << pSymbol->parameters.boolean.getSize();
    for (j = 0; j < pSymbol->parameters.boolean.getSize(); j++)
      out << pSymbol->parameters.boolean.getName(j);;
    // enumerated parameters
    out << pSymbol->parameters.enumerated.getSize();
    for (j = 0; j < pSymbol->parameters.enumerated.getSize(); j++)
    {
      // enumeration name
      out << pSymbol->parameters.enumerations[j]->n;
      // parameter name
      out << pSymbol->parameters.enumerated.getName(j);;
    }
  }

  // send enumerated input symbols
  out << pEngine->enumeratedInputSymbols.getSize();
  for (i = 0; i < pEngine->enumeratedInputSymbols.getSize(); i++)
  {
    xabsl::EnumeratedInputSymbol* pSymbol = pEngine->enumeratedInputSymbols[i];
    // enumeration name
    out << pSymbol->enumeration->n;
    // symbol name
    out << pSymbol->n;
    // decimal parameters
    out << pSymbol->parameters.decimal.getSize();
    for (j = 0; j < pSymbol->parameters.decimal.getSize(); j++)
      out << pSymbol->parameters.decimal.getName(j);
    // boolean parameters
    out << pSymbol->parameters.boolean.getSize();
    for (j = 0; j < pSymbol->parameters.boolean.getSize(); j++)
      out << pSymbol->parameters.boolean.getName(j);;
    // enumerated parameters
    out << pSymbol->parameters.enumerated.getSize();
    for (j = 0; j < pSymbol->parameters.enumerated.getSize(); j++)
    {
      // enumeration name
      out << pSymbol->parameters.enumerations[j]->n;
      // parameter name
      out << pSymbol->parameters.enumerated.getName(j);;
    }
  }

  // send decimal output symbols
  out << pEngine->decimalOutputSymbols.getSize();
  for (i = 0; i < pEngine->decimalOutputSymbols.getSize(); i++)
  {
    xabsl::DecimalOutputSymbol* pSymbol = pEngine->decimalOutputSymbols[i];
    out << pSymbol->n;
  }

  // send boolean output symbols
  out << pEngine->booleanOutputSymbols.getSize();
  for (i = 0; i < pEngine->booleanOutputSymbols.getSize(); i++)
  {
    xabsl::BooleanOutputSymbol* pSymbol = pEngine->booleanOutputSymbols[i];
    out << pSymbol->n;
  }

  // send enumerated output symbols
  out << pEngine->enumeratedOutputSymbols.getSize();
  for (i = 0; i < pEngine->enumeratedOutputSymbols.getSize(); i++)
  {
    xabsl::EnumeratedOutputSymbol* pSymbol = pEngine->enumeratedOutputSymbols[i];
    // enumeration name
    out << pSymbol->enumeration->n;
    // symbol name
    out << pSymbol->n;
  }

  // send options
  out << pEngine->getOptions().getSize();
  for (i = 0; i < pEngine->getOptions().getSize(); i++)
  {
    const xabsl::Option* pOption = pEngine->getOptions()[i];
    out << pOption->n;
    // decimal parameters
    out << pOption->parameters->decimal.getSize();
    for (j = 0; j < pOption->parameters->decimal.getSize(); j++)
      out << pOption->parameters->decimal.getName(j);
    // boolean parameters
    out << pOption->parameters->boolean.getSize();
    for (j = 0; j < pOption->parameters->boolean.getSize(); j++)
      out << pOption->parameters->boolean.getName(j);;
    // enumerated parameters
    out << pOption->parameters->enumerated.getSize();
    for (j = 0; j < pOption->parameters->enumerated.getSize(); j++)
    {
      // enumeration name
      out << pOption->parameters->enumerations[j]->n;
      // parameter name
      out << pOption->parameters->enumerated.getName(j);
    }
  }

  // send basic behaviors
  out << pEngine->getBasicBehaviors().getSize();
  for (i = 0; i < pEngine->getBasicBehaviors().getSize(); i++)
  {
    const xabsl::BasicBehavior* pBasicBehavior = pEngine->getBasicBehaviors()[i];
    out << pBasicBehavior->n;
    // decimal parameters
    out << pBasicBehavior->parameters->decimal.getSize();
    for (j = 0; j < pBasicBehavior->parameters->decimal.getSize(); j++)
      out << pBasicBehavior->parameters->decimal.getName(j);
    // boolean parameters
    out << pBasicBehavior->parameters->boolean.getSize();
    for (j = 0; j < pBasicBehavior->parameters->boolean.getSize(); j++)
      out << pBasicBehavior->parameters->boolean.getName(j);;
    // enumerated parameters
    out << pBasicBehavior->parameters->enumerated.getSize();
    for (j = 0; j < pBasicBehavior->parameters->enumerated.getSize(); j++)
    {
      // enumeration name
      out << pBasicBehavior->parameters->enumerations[j]->n;
      // parameter name
      out << pBasicBehavior->parameters->enumerated.getName(j);
    }
  }
  
  Global::getDebugOut().finishMessage(idXabslDebugSymbols);
}

bool GTXabslEngineExecutor::handleXabslMessage(InMessage& message)
{
  switch (message.getMessageID())
  {
  case idXabslDebugRequest:
    {
      // temporary variables
      std::string buf;
      std::string buf2;
      std::string buf3;
      double valueDouble;
      int valueInt;
      char c;
      int i, j, numberOfWatchedInputSymbols, numberOfWatchedOutputSymbols;
      
      message.bin >> buf;
      
      if (strcmp(buf.c_str(), id) != 0)
      {
        // the request was sent for another engine
        return true;
      }

      // reset xabsl engine if requested
      bool resetEngine;
      message.bin >> resetEngine;
      if (resetEngine) pEngine->reset();

      // clear the old pointer arrays
      watchedDecimalInputSymbols.clear();
      watchedBooleanInputSymbols.clear();
      watchedEnumeratedInputSymbols.clear();
      watchedDecimalOutputSymbols.clear();
      watchedBooleanOutputSymbols.clear();
      watchedEnumeratedOutputSymbols.clear();
      setDecimalOutputSymbols.clear();
      setDecimalOutputSymbolValues.clear();
      setBooleanOutputSymbols.clear();
      setBooleanOutputSymbolValues.clear();
      setEnumeratedOutputSymbols.clear();
      setEnumeratedOutputSymbolValues.clear();

      // input symbols
      message.bin >> numberOfWatchedInputSymbols;
      
      for (i=0; i< numberOfWatchedInputSymbols; i++)
      {
        message.bin >> c;
        message.bin >> buf;
        switch(c)
        {
        case 'd':
          if (pEngine->decimalInputSymbols.exists(buf.c_str()))
            watchedDecimalInputSymbols.append(buf.c_str(),pEngine->decimalInputSymbols[buf.c_str()]);
          break;
        case 'b':
          if (pEngine->booleanInputSymbols.exists(buf.c_str()))
            watchedBooleanInputSymbols.append(buf.c_str(),pEngine->booleanInputSymbols[buf.c_str()]);
          break;
        case 'e':
          if (pEngine->enumeratedInputSymbols.exists(buf.c_str()))
            watchedEnumeratedInputSymbols.append(buf.c_str(),pEngine->enumeratedInputSymbols[buf.c_str()]);
          break;
        }
      }
      
      // output symbols
      message.bin >> numberOfWatchedOutputSymbols;
      for (i=0; i< numberOfWatchedOutputSymbols; i++)
      {
        message.bin >> c;
        message.bin >> buf;
        switch(c)
        {
          case 'd':
            if (pEngine->decimalOutputSymbols.exists(buf.c_str()))
              watchedDecimalOutputSymbols.append(buf.c_str(),pEngine->decimalOutputSymbols[buf.c_str()]);
            break;
          case 'b':
            if (pEngine->booleanOutputSymbols.exists(buf.c_str()))
              watchedBooleanOutputSymbols.append(buf.c_str(),pEngine->booleanOutputSymbols[buf.c_str()]);
            break;
          case 'e':
            if (pEngine->enumeratedOutputSymbols.exists(buf.c_str()))
              watchedEnumeratedOutputSymbols.append(buf.c_str(),pEngine->enumeratedOutputSymbols[buf.c_str()]);
            break;
        }
      }

      char numberOfActions;
      message.bin >> numberOfActions;
      
      if (numberOfActions == 0)
      {
        pEngine->setRootAction();
      }
      else
      {
        pEngine->clearRootActions();

        for (j = 0; j < numberOfActions; j++)
        {
          char mode;
          char numberOfParameters;
          message.bin >> mode;
          switch (mode)
          {
          case 'a':
            message.bin >> buf;
            if(!pEngine->addRootAction(buf.c_str(), false))
            {
              OUTPUT(idText, text, "BasicBehavior " << buf << " does not exist");
            }
          
            message.bin >> numberOfParameters;
            for(i = 0; i < numberOfParameters; i++)
            {
              message.bin >> c;
              message.bin >> buf2;
              switch(c)
              {
                case 'd':
                  message.bin  >> valueDouble;
                  pEngine->getRootAction(j)->getParameters()->setDecimalParameter(buf2.c_str(), valueDouble);
                  break;
                case 'b':
                  message.bin  >> valueInt;
                  pEngine->getRootAction(j)->getParameters()->setBooleanParameter(buf2.c_str(), valueInt != 0);
                  break;
                case 'e':
                  message.bin >> buf3;
                  pEngine->getRootAction(j)->getParameters()->setEnumeratedParameter(buf2.c_str(), buf3.c_str());
                  break;
              }
            }
            break;
          case 'o':
            message.bin >> buf;
            if(!pEngine->addRootAction(buf.c_str()))
            {
              OUTPUT(idText, text, "Option " << buf << " does not exist");
            }
            message.bin >> numberOfParameters;
            for(i = 0; i < numberOfParameters; i++)
            {
              message.bin >> c;
              message.bin >> buf2;
              switch(c)
              {
                case 'd':
                  message.bin  >> valueDouble;
                  pEngine->getRootAction(j)->getParameters()->setDecimalParameter(buf2.c_str(), valueDouble);
                  break;
                case 'b':
                  message.bin  >> valueInt;
                  pEngine->getRootAction(j)->getParameters()->setBooleanParameter(buf2.c_str(), valueInt != 0);
                  break;
                case 'e':
                  message.bin >> buf3;
                  pEngine->getRootAction(j)->getParameters()->setEnumeratedParameter(buf2.c_str(), buf3.c_str());
                  break;
              }
            }
            break;
          case 'd':
            message.bin >> buf;
            message.bin >> valueDouble;
            pEngine->addRootActionDecimalOutputSymbol(buf.c_str(), valueDouble);
            break;
          case 'b':
            message.bin >> buf;
            message.bin >> valueInt;
            pEngine->addRootActionBooleanOutputSymbol(buf.c_str(), valueInt != 0);
            break;
          case 'e':
            message.bin >> buf;
            message.bin >> buf2;
            pEngine->addRootActionEnumeratedOutputSymbol(buf.c_str(), buf2.c_str());
            break;
          }
        }
      }

      // changed output symbols
      char numberOfChangedOutputSymbols;
      message.bin >> numberOfChangedOutputSymbols;
      for(i = 0; i < numberOfChangedOutputSymbols; i++)
      {
        message.bin >> c;
        message.bin >> buf;  //name
        switch (c)
        {
          case 'd':
            if (!pEngine->decimalOutputSymbols.exists(buf.c_str()))
            {
              OUTPUT(idText,text,"Decimal output symbol " << buf << " does not exist");
            }
            else
            {
              xabsl::DecimalOutputSymbol* pSymbol = pEngine->decimalOutputSymbols[buf.c_str()];
              double value;
              message.bin >> value;
              setDecimalOutputSymbols.append(buf.c_str(),pSymbol);
              setDecimalOutputSymbolValues.append(buf.c_str(),value);
            }
            break;
          case 'b':
            if (!pEngine->booleanOutputSymbols.exists(buf.c_str()))
            {
              OUTPUT(idText,text,"Boolean output symbol " << buf << " does not exist");
            }
            else
            {
              xabsl::BooleanOutputSymbol* pSymbol = pEngine->booleanOutputSymbols[buf.c_str()];
              char value;
              message.bin >> value;
              setBooleanOutputSymbols.append(buf.c_str(),pSymbol);
              setBooleanOutputSymbolValues.append(buf.c_str(), value != 0);
            }
            break;
          case 'e':
            if (!pEngine->enumeratedOutputSymbols.exists(buf.c_str()))
            {
              OUTPUT(idText,text,"Enumerated output symbol " << buf << " does not exist");
            }
            else
            {
              xabsl::EnumeratedOutputSymbol* pSymbol = pEngine->enumeratedOutputSymbols[buf.c_str()];
              int value = 0;
              
              setEnumeratedOutputSymbols.append(buf.c_str(),pSymbol);
              
              message.bin >> buf2; // value
              if (!pSymbol->enumeration->enumElements.exists(buf2.c_str()))
              {
                OUTPUT(idText,text,"Enum element " << buf2 << " for symbol " << buf 
                  << " does not exist");
              }
              else
              {
                value = pSymbol->enumeration->enumElements[buf2.c_str()]->v;
              }
              setEnumeratedOutputSymbolValues.append(buf.c_str(),value);
            }
            break;
        }
      }      
      return true;
    }
  case idXabslIntermediateCode:
    {
      std::string buf;
      message.bin >> buf;
      
      if (strcmp(buf.c_str(), id) != 0)
      {
        // the request was sent for another engine
        return true;
      }

      XabslMessageInputSource input(message.config);
      init(input);
      return true;
    }
  default:
    return false;
  }
}

bool GTXabslEngineExecutor::setSelectedAgent(const char* name)
{
  if (pEngine != 0) 
    return pEngine->setSelectedAgent(name);
  else
    return false;
}

GTXabslErrorHandler::GTXabslErrorHandler(const char* id)
: id (id)
{
}

void GTXabslErrorHandler::printError(const char* text)
{
  OUTPUT(idText, text, "(" 
    << id
    << " xabsl::Engine) error: " << text);
}

void GTXabslErrorHandler::printMessage(const char* text)
{
  OUTPUT(idText, text, "(" 
    << id
    << " xabsl::Engine): " << text);
}

XabslFileInputSource::XabslFileInputSource(const char* fileName)
: xabsl::NamedItem(fileName), file(0)
{
}

XabslFileInputSource::~XabslFileInputSource()
{
  if (file!=0) delete file;
}

bool XabslFileInputSource::open()
{
  if (file==0) file=new InConfigFile(n);
  return (file!=0 && file->exists());
}


void XabslFileInputSource::close()
{
  if (file!=0) delete file;
  file = 0;
}

double XabslFileInputSource::readValue()
{
  double d=0;
  if (file!=0) *file >> d;
  return d;
}

bool XabslFileInputSource::readString(char* destination, int maxLength)
{
  std::string buf;
  if (file!=0) *file >> buf;
  strcpy(destination, buf.c_str());
  return true;
}

XabslMessageInputSource::XabslMessageInputSource(InConfigMessage& message)
: message(message)
{
}

double XabslMessageInputSource::readValue()
{
  double d;
  message >> d;
  return d;
}

bool XabslMessageInputSource::readString(char* destination, int maxLength)
{
  std::string buf;
  message >> buf;
  strcpy(destination, buf.c_str());
  return true;
}

