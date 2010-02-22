/** 
* @file ModuleManager.h
* Declaration of a class representing the module manager.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef __ModuleManager_h_
#define __ModuleManager_h_

#include "Module.h"
#include "Tools/Streams/InOut.h"
#include <map>

class DefaultModule;

/** 
* @class ModuleManager
* A class representing the module manager.
*/
class ModuleManager
{
private:
  /**
  * The class represents the current state of a module.
  */
  class ModuleState
  {
  public:
    ModuleBase* module; /**< A pointer to the module base that is able to create an instance of the module. */
    Blackboard* instance; /**< A pointer to the instance of the module if it was created. Otherwise the pointer is 0. */
    bool required; /**< A flag that is required when determining whether a module is currently required or not. */

    /**
    * Constructor.
    * @param module A pointer to the module base that is able to create an instance of the module.
    */
    ModuleState(ModuleBase* module) : module(module), instance(0), required(false) {}
  };

  std::list<ModuleState> modules; /**< The current state of all modules. */

  /**
  * The class represents a provider of information, i.e. a method of a module that updates a representation.
  */
  class Provider
  {
  public:
    std::string representation; /**< The representation that will be provided. */
    const ModuleState* moduleState; /**< The moduleState that will give access to the module that provides the information. */
    void (*update)(Blackboard&); /**< The update handler within the module. */
    void (*create)(); /**< The method to create a new instance of the representation. */
    void (*free)(); /**< The method to delete an instance of the representation. */

    /**
    * Constructor.
    * @param representation The name of the representation provided.
    * @param moduleState The moduleState that will give access to the module that provides the information.
    * @param update The update handler within the module.
    * @param create The create handler for the representation.
    * @param free The free handler for the representation.
    */
    Provider(const std::string& representation, const ModuleState* moduleState, 
             void (*update)(Blackboard&), void (*create)(), void (*free)()) :
      representation(representation),
      moduleState(moduleState),
      update(update),
      create(create),
      free(free)
    {
    }

    /**
    * Comparison operator. Only uses the representation for comparison.
    * @param other The other provider this one is compared to.
    * @return Are the representation names the same?
    */
    bool operator==(const Provider& other) const {return representation == other.representation;}

    /**
    * Comparison operator. Only uses the representation for comparison.
    * @param representation The representation this one is compared to.
    * @return Does this provider provide the given representation?
    */
    bool operator==(const std::string& representation) const {return this->representation == representation;}
  };

  std::list<Provider> providers; /**< The list of providers that will be executed. */
  std::map<const char*, const char*> selected; /**< The providers selected. This is always the last configuration set. It may not work. */

  class Shared
  {
  public:
    std::string representation; /**< The name of the shared representation. */
    void (*create)(); /**< The method to create a new instance of the representation. */
    void (*free)(); /**< The method to delete an instance of the representation. */
    void (*out)(Out&); /**< The function that is able to write the representation to a stream. */
    void (*in)(In&); /**< The function that is able to read the representation from a stream. */

    /**
    * Constructor.
    * @param representation The name of the shared representation.
    */
    Shared(const std::string& representation) : representation(representation), create(0), free(0), out(0), in(0) {}

    /**
    * Comparison operator. Only uses the representation for comparison.
    * @param other The representaion name this one is compared to.
    * @return Are the representation names the same?
    */
    bool operator==(const std::string& other) const {return representation == other;}
  };

  std::list<Shared> shared; /**< The list of all shared representations. */

  unsigned timeStamp; /**< The timeStamp of the last module request. Communication is only possible if both sides use the same timestamp. */

  DefaultModule* defaultModule; /**< A module that can provide everything. */

  /**
  * The method brings the providers in the correct sequence.
  * @return Is the set of providers consistent?
  */
  bool sortProviders();

public:
  /**
  * Default constructor. Used on the robot.
  * Collects data on the available modules, but does not instantiate any of them.
  */
  ModuleManager();

  /**
  * Constructor used under Windows.
  * Under Windows, each process sees all modules, because the are all declared in the 
  * same address space. Therefore, this constructor filters them through a hack.
  * @param process The name of the process.
  */
  ModuleManager(const std::string& process);

  /**
  * Destructor.
  * Destructs all modules currently constructed.
  */
  ~ModuleManager();

  /**
  * The method loads the selection of solutions from a configuration file.
  */
  void load();

  /**
  * The method destroys all modules. It can be called to destroy the modules
  * before the constructor is called.
  */
  void destroy();

  /**
  * The method updates the list of the currently created modules.
  * @param stream The stream the new configuration is read from. 
  * @param timeStamp The timeStamp of the last module request. 
  */
  void update(In& stream, unsigned timeStamp = 0);

  /**
  * The method executes all selected modules.
  */
  void execute();

  /**
  * The method reads a package from a stream.
  * @param stream A stream containing representations received from another process.
  */
  void readPackage(In& stream);

  /**
  * The method writes a package to a stream.
  * @param stream A stream that will be filled with representations that are sent
  *               to another process.
  */
  void writePackage(Out& stream) const;

  friend class DefaultModule; /**< Allowed to access local class ModuleState. */
};

/**
* The class defines the default module that can provide everything.
* The representations provided remain in their initial state.
*/
class DefaultModule : public ModuleBase
{
private:
  /**
  * The method normally creates an instance of the module.
  * However, there is no instance of the default module
  * @return The address of the newly created instance.
  */
  Blackboard* createNew()
  {
    return 0;
  }

public:
  /**
  * Default constructor.
  */
  DefaultModule();

  /**
  * The method lets this module provide all the representations that any
  * of the other modules also provide.
  * @param modules The list of all module states.
  */
  void setRepresentations(const std::list<ModuleManager::ModuleState>& modules);
};

#endif //__ModuleManager_h_
