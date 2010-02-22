/**
* @file ControllerQt/Representations/XabslInfo.h
*
* Declaration of class XabslInfo
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef __XabslInfo_h_
#define __XabslInfo_h_

#include <list>
#include <string>
#include "Tools/MessageQueue/InMessage.h"

class XabslInfo;

/**
* Streaming operator that writes a Xabsl debug request to a stream.
* @param stream The stream from which is read.
* @param info The XabslInfo object.
* @return The stream.
*/ 
Out& operator<<(Out& stream, const XabslInfo& info);

/**
* @class XabslInfo 
* 
* A class to represent information about a Xabsl behavior.
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/
class XabslInfo
{
public:
  /** The type of a symbol. */
  enum SymbolType {decimal, boolean, enumerated};

  /** A pair of a name and a value. */
  struct NameValue
  {
    std::string name,
                value;
  };

  /** An enumeration. */
  struct Enumeration : public NameValue
  {
    std::list<std::string> elements;
  };

  /** Base class for parameterized objects. */
  struct ParameterizedObject
  {
    std::list<std::string> decimalParameters; /**< The parameters of the symbol. */
    std::list<std::string> booleanParameters; /**< The parameters of the symbol. */
    std::list<std::string> enumeratedParameters; /**< The parameters of the symbol. */
    std::list<Enumeration*> enumeratedParameterEnums; /**< The associated enumerations for enumerated parameters. */
  };

  /** An input symbol. */
  struct InputSymbol : public NameValue, public ParameterizedObject
  {
    bool show; /**< Show this symbol? */
    SymbolType type; /**< The type of the symbol. */
    Enumeration* enumeration; /**< The associated enumeration in case of an enumerated symbol. */

    /**
    * Default constructor.
    */
    InputSymbol() : show(false) {}
  };

  /** An output symbol. */
  struct OutputSymbol : public NameValue
  {
    bool show; /**< Show this symbol? */
    SymbolType type; /**< The type of the symbol. */
    Enumeration* enumeration; /**< The associated enumeration in case of an enumerated symbol. */
    bool set; /**< Set the symbol value? */
    std::list<std::string>::const_iterator selectedAlternative; /**< The currently selected alternative if symbol is enumerated. */
    bool booleanValue; /** The selected value if symbol is boolean. */
    double decimalValue; /** The selected value if symbol is decimal. */

    /**
    * Default constructor.
    */
    OutputSymbol() : show(false), set(false) {value = "0";}
  };

  /** An option. */
  struct Option : public ParameterizedObject
  {
    std::string name; /**< The name of the option. */
  };

  /** A basic behavior. */
  struct BasicBehavior : public ParameterizedObject
  {
    std::string name; /**< The name of the basic behavior. */
  };

  /** An active option. */
  struct ActiveOption
  {
    std::string name, /**< The name of the active option. */
                durationOfActivation, /**< The duration this option is active. */
                activeState, /**< The active state. */
                durationOfStateActivation; /**< The duration the state is active. */
    int depth; /**< Depth in option activation tree. */
    std::list<NameValue> parameters; /**< The parameters of the option. */
  };

  std::string id, /**< The id of the behavior. */
              selectedAgentName, /**< The name of the agent. */
              executedMotionRequest; /**< The motion request currently executed. */
  std::list<std::string> agents; /**< The enumerations. */
  std::list<Enumeration> enumerations; /**< The enumerations. */
  std::list<InputSymbol> inputSymbols; /**< The input symbols. */
  std::list<OutputSymbol> outputSymbols; /**< The output symbols. */
  std::list<Option> options; /**< The options. */
  std::list<BasicBehavior> basicBehaviors; /**< The basic behaviors. */
  std::list<ActiveOption> activeOptions; /**< The active options. */
  const Option* selectedOption; /**< The option selected by the user. */
  const BasicBehavior* selectedBasicBehavior; /**< The basic behavior selected by the user. */
  double decimalParameters[10]; /**< The decimal parameters of the basic behavior/option selected. */
  bool booleanParameters[10]; /**< The decimal parameters of the basic behavior/option selected. */
  std::string enumeratedParameters[10]; /**< The decimal parameters of the basic behavior/option selected. */
  unsigned timeStamp; /**< The timestamp when this information was received. */
  bool reload; /**< If set, the behavior is forced to be reloaded. */

  /**
  * Default constructor.
  */
  XabslInfo() {reset();}

  /**
  * The function handles a Xabsl debug message.
  * @param message The message.
  * @return Was is actually a Xabsl message?
  */
  bool handleMessage(InMessage& message);

  /**
  * The function empties the Xabsl info object.
  */
  void reset();

  /**
  * The function forces a reload of the behavior.
  */
  void forceReload() {reload = true;}

private:
  /**
  * The function finds an enumeration by its name.
  * Note: the function throws an exception if the enumeration is not found.
  * @param search The name of the enumeration to search for.
  * @return A reference to the enumeration found.
  */
  Enumeration& findEnumeration(const std::string& search);

  /**
  * The function finds an input symbol by its name.
  * Note: the function throws an exception if the symbol is not found.
  * @param search The name of the symbol to search for.
  * @return A reference to the symbol found.
  */
  InputSymbol& findInputSymbol(const std::string& search);

  /**
  * The function finds an output symbol by its name.
  * Note: the function throws an exception if the symbol is not found.
  * @param search The name of the symbol to search for.
  * @return A reference to the symbol found.
  */
  OutputSymbol& findOutputSymbol(const std::string& search);

  /**
  * The method reads the Xabsl debug symbols.
  * @param stream The stream the Xabsl debug symbols are read from.
  */
  void readDebugSymbols(In& stream);

  /**
  * The method reads a Xabsl debug message.
  * @param stream The stream the Xabsl debug message is read from.
  */
  void readDebugMessage(In& stream);

  /**
  * The method reads the parameters of an input value or option.
  * @param stream The stream the parameters are read from.
  * @param po The input value or option.
  */
  void readParams(In& stream, ParameterizedObject& po);

  friend Out& operator<<(Out& stream, const XabslInfo& info);
};

#endif //__XabslInfo_h_
