/**
* @file ControllerQt/Representations/XabslInfo.cpp
*
* Implementation of class XabslInfo
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#include "XabslInfo.h"
#include "Platform/SystemCall.h"

void XabslInfo::reset()
{
  selectedAgentName = "";
  executedMotionRequest = "";
  selectedBasicBehavior = 0;
  selectedOption = 0;
  for(int i = 0; i < 10; ++i)
  {
    decimalParameters[i] = 0.0;
    booleanParameters[i] = false;
    enumeratedParameters[i] = "";
  }
  agents.clear();
  inputSymbols.clear();
  outputSymbols.clear();
  options.clear();
  basicBehaviors.clear();
  activeOptions.clear();
  reload = false;
}

void XabslInfo::readDebugSymbols(In& stream)
{
  reset();

  std::string buffer;
  int count,
      count2;

  // read id;
  stream >> id;

  // read agents
  stream >> count;
  for(int i = 0; i < count; ++i)
  {
    std::string name;
    stream >> name;
    agents.push_back(name);
  }

  // read enumerations
  stream >> count;
  for(int i = 0; i < count; ++i)
  {
    XabslInfo::Enumeration en;
    stream >> en.name >> count2;
    for(int j = 0; j < count2; ++j)
    {
      stream >> buffer;
      en.elements.push_back(buffer);
    }
    enumerations.push_back(en);
  }

  // read decimal input symbols
  stream >> count;
  for(int i = 0; i < count; ++i)
  {
    XabslInfo::InputSymbol is;
    stream >> is.name;
    is.type = XabslInfo::decimal;
    readParams(stream, is);
    inputSymbols.push_back(is);
  }

  // read boolean input symbols
  stream >> count;
  for(int i = 0; i < count; ++i)
  {
    XabslInfo::InputSymbol is;
    stream >> is.name;
    is.type = XabslInfo::boolean;
    readParams(stream, is);
    inputSymbols.push_back(is);
  }

  // read enumeration input symbols
  stream >> count;
  for(int i = 0; i < count; ++i)
  {
    XabslInfo::InputSymbol is;
    stream >> buffer >> is.name;
    is.enumeration = &findEnumeration(buffer);
    is.type = XabslInfo::enumerated;
    readParams(stream, is);
    inputSymbols.push_back(is);
  }

  // read decimal output symbols
  stream >> count;
  for(int i = 0; i < count; ++i)
  {
    XabslInfo::OutputSymbol os;
    stream >> os.name;
    os.type = XabslInfo::decimal;
    outputSymbols.push_back(os);
  }

  // read boolean output symbols
  stream >> count;
  for(int i = 0; i < count; ++i)
  {
    XabslInfo::OutputSymbol os;
    stream >> os.name;
    os.type = XabslInfo::boolean;
    outputSymbols.push_back(os);
  }

  // read enumeration output symbols
  stream >> count;
  for(int i = 0; i < count; ++i)
  {
    XabslInfo::OutputSymbol os;
    stream >> buffer >> os.name;
    os.enumeration = &findEnumeration(buffer);
    os.type = XabslInfo::enumerated;
    outputSymbols.push_back(os);
  }
  
  // read options
  stream >> count;
  for(int i = 0; i < count; ++i)
  {
    XabslInfo::Option o;
    stream >> o.name;
    readParams(stream, o);
    options.push_back(o);
  }

  // read basic behaviors
  stream >> count;
  for(int i = 0; i < count; ++i)
  {
    XabslInfo::BasicBehavior bb;
    stream >> bb.name;
    readParams(stream, bb);
    basicBehaviors.push_back(bb);
  }
}

void XabslInfo::readParams(In& stream, XabslInfo::ParameterizedObject& po)
{
  std::string buffer;
  int count;

  // read decimal input symbols
  stream >> count;
  for(int i = 0; i < count; ++i)
  {
    stream >> buffer;
    po.decimalParameters.push_back(buffer);
  }

  // read boolean input symbols
  stream >> count;
  for(int i = 0; i < count; ++i)
  {
    stream >> buffer;
    po.booleanParameters.push_back(buffer);
  }

  // read enumeration input symbols
  stream >> count;
  for(int i = 0; i < count; ++i)
  {
    stream >> buffer;
    po.enumeratedParameterEnums.push_back(&findEnumeration(buffer));
    stream >> buffer;
    po.enumeratedParameters.push_back(buffer);
  }
}

bool XabslInfo::handleMessage(InMessage& message)
{
  switch(message.getMessageID())
  {
    case idXabslDebugSymbols:
      readDebugSymbols(message.bin);
      return true;

    case idXabslDebugMessage:
      readDebugMessage(message.bin);
      return true;

    default:
      return false;
  }
}

void XabslInfo::readDebugMessage(In& stream)
{
  std::string buffer,
              type;
  int count,
      i;
  double valueDouble;
  char valueChar, valueChar2;
  unsigned valueUnsignedLong;

  std::string id;
  stream >> id;
  if(id != this->id)
    return;

  activeOptions.clear();

  // read active agent
  stream >> selectedAgentName;
  try
  {
    // read watched decimal input symbols
    stream >> count;
    for(i = 0; i < count; ++i)
    {
      stream >> buffer >> valueDouble;
      char buf[100];
      sprintf(buf,"%.2f",valueDouble);
      findInputSymbol(buffer).value = buf;
    }

    // read watched boolean input symbols
    stream >> count; 
    for(i = 0; i < count; ++i)
    {
      stream >> buffer >> valueChar;
      findInputSymbol(buffer).value = valueChar ? "true" : "false";
    }

    // read watched enumeration input symbols
    stream >> count; 
    for(i = 0; i < count; ++i)
    {
      stream >> buffer;
      stream >> findInputSymbol(buffer).value;
    }

    // read watched decimal output symbols
    stream >> count;
    for(i = 0; i < count; ++i)
    {
      stream >> buffer >> valueDouble;
      char buf[100];
      sprintf(buf,"%.2f",valueDouble);
      findOutputSymbol(buffer).value = buf;
    }

    // read watched boolean output symbols
    stream >> count; 
    for(i = 0; i < count; ++i)
    {
      stream >> buffer >> valueChar;
      findOutputSymbol(buffer).value = valueChar ? "true" : "false";
    }

    // read watched enumeration output symbols
    stream >> count; 
    for(i = 0; i < count; ++i)
    {
      stream >> buffer;
      stream >> findOutputSymbol(buffer).value;
    }
  }
  catch(std::string&) {}

  // read active options and basic behaviors
  stream >> count;
  for(i = 0; i < count; ++i)
  { 
    ActiveOption ao;
    stream >> ao.depth;
    char buf[100];
    stream >> type >> ao.name;
    if (type[0] == 'o' || type[0] == 'a')
    {
      stream >> valueUnsignedLong;
      sprintf(buf, "%.1f", valueUnsignedLong / 1000.0);
      ao.durationOfActivation = buf;
      if (type[0] == 'o')
      {
        stream >> ao.activeState >> valueUnsignedLong;
        sprintf(buf, "%.1f", valueUnsignedLong / 1000.0);
        ao.durationOfStateActivation = buf;
      }
      stream >> valueChar;
      for(int j = 0; j < valueChar; ++j)
      {
        NameValue nv;
        stream >> type >> nv.name;
        switch(type[0])
        {
          case 'd':
            stream >> valueDouble;
            sprintf(buf,"%.2f",valueDouble);
            nv.value = buf;
            break;
          case 'b':
            stream >> valueChar2;
            sprintf(buf,"%s", valueChar2 ? "true" : "false");
            nv.value = buf;
            break;
          case 'e':
            stream >> nv.value;
            break;
        }
        ao.parameters.push_back(nv);
      }
      activeOptions.push_back(ao);
    }
    else
    {
      NameValue nv;
      nv.name = ao.name;
      switch(type[0])
      {
        case 'd':
          stream >> valueDouble;
          sprintf(buf,"%.2f",valueDouble);
          nv.value = buf;
          break;
        case 'b':
          stream >> valueChar2;
          sprintf(buf,"%s", valueChar2 ? "true" : "false");
          nv.value = buf;
          break;
        case 'e':
          stream >> nv.value;
          break;
      }
      ao.parameters.push_back(nv);
    }
    //currently output symbol assignments are not displayed
  }

  stream >> executedMotionRequest;
  timeStamp = SystemCall::getCurrentSystemTime();
}

XabslInfo::Enumeration& XabslInfo::findEnumeration(const std::string& search)
{
  for(std::list<Enumeration>::iterator i = enumerations.begin(); i != enumerations.end(); ++i)
    if(i->name == search)
      return *i;
  throw std::string(search) + " not found";
}

XabslInfo::InputSymbol& XabslInfo::findInputSymbol(const std::string& search)
{
  for(std::list<InputSymbol>::iterator i = inputSymbols.begin(); i != inputSymbols.end(); ++i)
    if(i->name == search)
      return *i;
  throw std::string(search) + " not found";
}

XabslInfo::OutputSymbol& XabslInfo::findOutputSymbol(const std::string& search)
{
  for(std::list<OutputSymbol>::iterator i = outputSymbols.begin(); i != outputSymbols.end(); ++i)
    if(i->name == search)
      return *i;
  throw std::string(search) + " not found";
}

Out& operator<<(Out& stream,const XabslInfo& info)
{
  stream << info.id;

  //do not reset engine
  stream << false;

  int count = 0;
  for(std::list<XabslInfo::InputSymbol>::const_iterator i = info.inputSymbols.begin(); i != info.inputSymbols.end(); ++i)
    if(i->show)
      ++count;
  stream << count;
  for(std::list<XabslInfo::InputSymbol>::const_iterator i = info.inputSymbols.begin(); i != info.inputSymbols.end(); ++i)
    if(i->show)
    {
      switch(i->type)
      {
        case XabslInfo::decimal: 
          stream << 'd'; 
          break;
        case XabslInfo::boolean:
          stream << 'b';
          break;
        case XabslInfo::enumerated:
          stream << 'e';
          break;
      }
      stream << i->name.c_str();
    }

  count = 0;
  for(std::list<XabslInfo::OutputSymbol>::const_iterator i = info.outputSymbols.begin(); i != info.outputSymbols.end(); ++i)
    if(i->show)
      ++count;
  stream << count;
  for(std::list<XabslInfo::OutputSymbol>::const_iterator i = info.outputSymbols.begin(); i != info.outputSymbols.end(); ++i)
    if(i->show)
    {
      switch(i->type)
      {
        case XabslInfo::decimal: 
          stream << 'd'; 
          break;
        case XabslInfo::boolean:
          stream << 'b';
          break;
        case XabslInfo::enumerated:
          stream << 'e';
          break;
      }
      stream << i->name.c_str();
    }

  if(info.selectedBasicBehavior)
  {
    stream << char(1);
    const XabslInfo::BasicBehavior& bb = *info.selectedBasicBehavior;
    stream << char('a') << bb.name.c_str() << char(bb.decimalParameters.size() + bb.booleanParameters.size() + bb.enumeratedParameters.size());
    int j = 0;
    for(std::list<std::string>::const_iterator i = bb.decimalParameters.begin(); i != bb.decimalParameters.end(); ++i)
      stream << char('d') << i->c_str() << info.decimalParameters[j++];
    j = 0;
    for(std::list<std::string>::const_iterator i = bb.booleanParameters.begin(); i != bb.booleanParameters.end(); ++i)
      stream << char('b') << i->c_str() << info.booleanParameters[j++];
    j = 0;
    for(std::list<std::string>::const_iterator i = bb.enumeratedParameters.begin(); i != bb.enumeratedParameters.end(); ++i)
      stream << char('e') << i->c_str() << info.enumeratedParameters[j++].c_str();
  }
  else if(info.selectedOption)
  {
    stream << char(1);
    const XabslInfo::Option& o = *info.selectedOption;
    stream << char('o') << o.name.c_str() << char(o.decimalParameters.size() + o.booleanParameters.size() + o.enumeratedParameters.size());
    int j = 0;
    for(std::list<std::string>::const_iterator i = o.decimalParameters.begin(); i != o.decimalParameters.end(); ++i)
      stream << char('d') << i->c_str() << info.decimalParameters[j++];
    j = 0;
    for(std::list<std::string>::const_iterator i = o.booleanParameters.begin(); i != o.booleanParameters.end(); ++i)
      stream << char('b') << i->c_str() << info.booleanParameters[j++];
    j = 0;
    for(std::list<std::string>::const_iterator i = o.enumeratedParameters.begin(); i != o.enumeratedParameters.end(); ++i)
      stream << char('e') << i->c_str() << info.enumeratedParameters[j++].c_str();
  }
  else
    stream << char(0);

  count = 0;
  for(std::list<XabslInfo::OutputSymbol>::const_iterator i = info.outputSymbols.begin(); i != info.outputSymbols.end(); ++i)
    if(i->set)
      ++count;

  stream << char(count);
  for(std::list<XabslInfo::OutputSymbol>::const_iterator i = info.outputSymbols.begin(); i != info.outputSymbols.end(); ++i)
    if(i->set)
    {
      if (i->type == XabslInfo::decimal)
        stream << char('d') << i->name.c_str() << i->decimalValue;
      else if (i->type == XabslInfo::boolean)
        stream << char('b') << i->name.c_str() << char(i->booleanValue);
      else
        stream << char('e') << i->name.c_str() << i->selectedAlternative->c_str();
    }

  return stream;
}
