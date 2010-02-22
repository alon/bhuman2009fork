/** 
* @file XabslBooleanExpression.h
*
* Definition of BooleanExpression and derivates
* 
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/

#ifndef __XabslBooleanExpression_h_
#define __XabslBooleanExpression_h_

#include "XabslDecimalExpression.h"

namespace xabsl 
{

/** 
* @class BooleanExpression
* 
* Base class for all boolean expressions inside an option graph.
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/
class BooleanExpression
{
public:
  /** Evaluates the boolean expression. */
  virtual bool getValue() const = 0;
  
  /**
  * Creates a boolean expression depending on the input.
  * @param input An input source for the intermediate code. It must be opened and read until 
  *              A position where a boolean expression starts.
  * @param actions The subsequent behaviors i.e options and basic behaviors of the state.
  * @param errorHandler A reference to a ErrorHandler instance
  * @param parameters The parameters of the option
  * @param symbols All available symbols
  * @param timeOfOptionExecution The time how long the option is already active
  * @param timeOfStateExecution The time how long the state is already active
  */
  static BooleanExpression* create(InputSource& input, 
    Array<Action*>& actions,
    ErrorHandler& errorHandler,
    OptionParameters& parameters,
    Symbols& symbols,
    unsigned& timeOfOptionExecution,
    unsigned& timeOfStateExecution);
  
  /** Destructor */
  virtual ~BooleanExpression() = 0;

  /** 
  * Creates a boolean expression depending on the input. 
  * Uses the create() function to create boolean operands.
  * @param operand The expression to be created
  * @param input An input source for the intermediate code. It must be opened and read until 
  *              A position where a boolean operand starts.
  * @param actions The subsequent behaviors i.e options and basic behaviors of the state.
  * @param errorHandler A reference to a ErrorHandler instance
  * @param parameters The parameters of the option
  * @param symbols All available symbols
  * @param timeOfOptionExecution The time how long the option is already active
  * @param timeOfStateExecution The time how long the state is already active
  * @return If the creation was successful
  */
  static bool createOperand(
    BooleanExpression*& operand,
    InputSource& input, 
    Array<Action*>& actions,
    ErrorHandler& errorHandler,
    OptionParameters& parameters,
    Symbols& symbols,
    unsigned& timeOfOptionExecution,
    unsigned& timeOfStateExecution);
};

/** 
* @class BooleanValue
* 
* Represents a boolean value.
*
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/
class BooleanValue : public BooleanExpression
{
public:
  /**
  * Constructor. Creates the value
  * @param input An input source for the intermediate code. It must be opened and read until 
  *              A position where a value starts.
  * @param errorHandler A reference to a ErrorHandler instance
  */
  BooleanValue(InputSource& input, 
    ErrorHandler& errorHandler);
  
  /**
  * Constructor. Creates an expression for a fixed boolean value
  * @param value The boolean value
  */
  BooleanValue(bool value) : value(value) {}

  /** Calculates the value of the decimal expression. */
  virtual bool getValue() const;
  
private:
  /** The value */
  bool value;
};

/** 
* @class BooleanOptionParameterRef
* 
* Represents a reference to a decimal option parameter.
*
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/
class BooleanOptionParameterRef : public BooleanExpression
{
public:
/**
* Constructor. Creates the reference 
* @param input An input source for the intermediate code. It must be opened and read until 
*              A position where the expression starts.
* @param errorHandler A reference to a ErrorHandler instance
* @param parameters The parameters of the option
  */
  BooleanOptionParameterRef(InputSource& input, 
    ErrorHandler& errorHandler,
    OptionParameters& parameters);
  
  /** Calculates the value of the boolean expression. */
  virtual bool getValue() const;
  
private:
  /** A pointer to the parameter */
  bool* parameter;
};

/** 
* @class AndOperator
*
* Represents an 'and' element of the option graph 
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class AndOperator : public BooleanExpression
{
public:
  /** Constructor */
  AndOperator();
  
  /** Destructor. Deletes the two operands */
  ~AndOperator();
  
  /** Evaluates the boolean expression.*/
  virtual bool getValue() const;
  
  /** Adds an operand to the operands array */
  void addOperand(BooleanExpression* operand);

private:
  /** the 2+n operands of the operator */
  Array<BooleanExpression*> operands;
};

/** 
* @class OrOperator
*
* Represents an 'or' element of the option graph 
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class OrOperator : public BooleanExpression
{
public:
  /** Constructor */
  OrOperator();
  
  /** Destructor. Deletes the two operands */
  ~OrOperator();
  
  /** Evaluates the boolean expression. */
  virtual bool getValue() const;
  
  /** Adds an operand to the operands array */
  void addOperand(BooleanExpression* operand);

private:
  /** the 2+n operands of the operator */
  Array<BooleanExpression*> operands;
};


/** 
* @class NotOperator
*
* Represents an 'not' element of the option graph 
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class NotOperator : public BooleanExpression
{
public:
/** 
* Constructor. Creates the element.
* @param operand1 A boolean expression
  */
  NotOperator(BooleanExpression* operand1);
  
  /** Destructor. Deletes the operand */
  ~NotOperator();
  
  /** Evaluates the boolean expression. */
  virtual bool getValue() const;
  
private:
  /** operand 1 */
  BooleanExpression* operand1;
};

/** 
* @class BooleanInputSymbolRef
*
* Represents an 'boolean-input-symbol-ref' element of the option graph 
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class BooleanInputSymbolRef : public BooleanExpression
{
public:
  /**
  * Constructor. Creates the element
  * @param input An input source for the intermediate code. It must be opened and read until 
  *              A position where a expression starts.
  * @param actions The subsequent behaviors i.e options and basic behaviors of the state.
  * @param errorHandler A reference to a ErrorHandler instance
  * @param optionParameters The parameters of the option
  * @param symbols All available symbols
  * @param timeOfOptionExecution The time how long the option is already active
  * @param timeOfStateExecution The time how long the state is already active
  */
  BooleanInputSymbolRef(InputSource& input, 
    Array<Action*>& actions,
    ErrorHandler& errorHandler,
    OptionParameters& optionParameters,
    Symbols& symbols,
    unsigned& timeOfOptionExecution,
    unsigned& timeOfStateExecution);
  
  /** Destructor */
  ~BooleanInputSymbolRef();

  /** Evaluates the boolean expression. */
  virtual bool getValue() const;
  
private:
  /** The referenced symbol */
  BooleanInputSymbol* symbol;

  /** The parameter assignments of the referenced symbol */
  ParameterAssignment* parameters;
};

/** 
* @class BooleanOutputSymbolRef
* 
* Represents a reference to a boolean input symbol.
*
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/
class BooleanOutputSymbolRef : public BooleanExpression
{
public:
  /** Calculates the value of the boolean expression. */
  virtual bool getValue() const;
  
  /**
  * Constructor. Creates the function call depending on the input.
  * @param input An input source for the intermediate code. It must be opened and read until 
  *              A position where the function reference starts.
  * @param errorHandler A reference to a ErrorHandler instance
  * @param symbols All available symbols
  */
  BooleanOutputSymbolRef(InputSource& input, 
    ErrorHandler& errorHandler,
    Symbols& symbols);

private:
  /** The referenced symbol */
  BooleanOutputSymbol* symbol;
};

/** 
* @class SubsequentOptionReachedTargetStateCondition
*
* Represents an 'subsequent-option-reached-target-state' element of the option graph
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class SubsequentOptionReachedTargetStateCondition : public BooleanExpression
{
public:
/** 
* Constructor. Creates the element.
  * @param actions The subsequent behaviors i.e options and basic behaviors of the state.
  * @param errorHandler A reference to a ErrorHandler instance
  */
  SubsequentOptionReachedTargetStateCondition(
    Array<Action*>& actions,
    ErrorHandler& errorHandler);
  
  /** Evaluates the boolean expression. */
  virtual bool getValue() const;
  
private:
  /** The subsequent behaviors of that state */
  Array<Action*>& actions;
};

/** 
* @class EnumeratedInputSymbolComparison
*
* Represents an 'enumerated-input-symbol-comparison' element of the option graph 
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class EnumeratedInputSymbolComparison : public BooleanExpression
{
public:
  /**
  * Constructor. Creates the element
  * @param input An input source for the intermediate code. It must be opened and read until 
  *              A position where a expression starts.
  * @param actions The subsequent behaviors i.e options and basic behaviors of the state.
  * @param errorHandler A reference to a ErrorHandler instance
  * @param parameters The parameters of the option
  * @param symbols All available symbols
  * @param timeOfOptionExecution The time how long the option is already active
  * @param timeOfStateExecution The time how long the state is already active
  */
  EnumeratedInputSymbolComparison(InputSource& input, 
    Array<Action*>& actions,
    ErrorHandler& errorHandler,
    OptionParameters& parameters,
    Symbols& symbols,
    unsigned& timeOfOptionExecution,
    unsigned& timeOfStateExecution);
  
  /** Destructor. Deletes the two operands */
  ~EnumeratedInputSymbolComparison();
  
  /** Evaluates the boolean expression.*/
  virtual bool getValue() const;
  
protected:
  /** operand 1 */
  const EnumeratedExpression* operand1;
  
  /** operand 2 */
  const EnumeratedExpression* operand2;
};

/**
* @class RelationalAndEqualityOperator
*
* Base class for the operators <, <=, >, >=, == and !=
* 
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class RelationalAndEqualityOperator : public BooleanExpression
{
public:
/** 
* Creates the element.
* @param operand1 A decimal expression
* @param operand2 A decimal expression
  */
  void create(DecimalExpression* operand1, 
    DecimalExpression* operand2);
  
  /** Destructor. Deletes the two operands */
  ~RelationalAndEqualityOperator();
  
  /** Evaluates the boolean expression.*/
  virtual bool getValue() const = 0;
  
protected:
  /** operand 1 */
  DecimalExpression* operand1;
  
  /** operand 2 */
  DecimalExpression* operand2;
};

/** 
* @class EqualToOperator
*
* Represents an 'equal-to' element of the option graph 
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class EqualToOperator : public RelationalAndEqualityOperator
{
public:
  /** Evaluates the boolean expression.*/
  virtual bool getValue() const;
};

/** 
* @class NotEqualToOperator
*
* Represents an 'not-equal-to' element of the option graph 
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class NotEqualToOperator : public RelationalAndEqualityOperator
{
public:
  /** Evaluates the boolean expression.*/
  virtual bool getValue() const;
};

/** 
* @class LessThanOperator
*
* Represents an 'less-than' element of the option graph 
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class LessThanOperator : public RelationalAndEqualityOperator
{
public:
  /** Evaluates the boolean expression.*/
  virtual bool getValue() const;
};

/** 
* @class LessThanOrEqualToOperator
*
* Represents an 'less-than-or-equal-to' element of the option graph 
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class LessThanOrEqualToOperator : public RelationalAndEqualityOperator
{
public:
  /** Evaluates the boolean expression.*/
  virtual bool getValue() const;
};

/** 
* @class GreaterThanOperator
*
* Represents an 'greater-than' element of the option graph 
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class GreaterThanOperator : public RelationalAndEqualityOperator
{
public:
  /** Evaluates the boolean expression.*/
  virtual bool getValue() const;
};

/** 
* @class GreaterThanOrEqualToOperator
*
* Represents an 'greater-than-or-equal-to' element of the option graph 
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class GreaterThanOrEqualToOperator : public RelationalAndEqualityOperator
{
public:
  /** Evaluates the boolean expression.*/
  virtual bool getValue() const;
};


} // namespace

#endif //__XabslBooleanExpression_h_
