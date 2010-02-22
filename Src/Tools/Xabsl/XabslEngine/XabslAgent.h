/**
* @file XabslAgent.h
* 
* Definition of class Agent
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/

#ifndef __XabslAgent_h_
#define __XabslAgent_h_

#include "XabslBehavior.h"
#include "XabslTools.h"

namespace xabsl 
{

/** 
* @class Agent
* 
* Combines some options to an agent
*
* @author Martin Lötzsch
*/
class Agent : public NamedItem
{
public:
  /**
  * Constructor
  * @param name The name of the agent
  * @param rootOption A pointer to the initial option of the agent
  * @param errorHandler Is invoked when errors occur
  */
  Agent(const char* name, Behavior* rootOption,
    ErrorHandler& errorHandler);

  /** Returns the root option */
  Behavior* getRootOption() const;
private:

  /** A pointer to the root option */
  Behavior* rootOption;

  /** Is invoked when errors occur */
  ErrorHandler& errorHandler;
};

} // namespace

#endif // __XabslAgent_h_
