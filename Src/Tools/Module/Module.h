/** 
* @file Module.h
* Definition of the module handling schema.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*
* This file provides the ability to specify the requirements of and the 
* representations provided by a module. Example:
*
* MODULE(MyImageProcessor)
*   REQUIRES(Image)                               // has to be updated before
*   REQUIRES(CameraMatrix)                        // has to be updated before
*   USES(RobotPose)                               // is used, but has not to be updated before
*   PROVIDES_WITH_MODIFY(BallPercept)             // class provides a method to update BallPercept. Representation can be MODIFYed.
*   PROVIDES_WITH_OUTPUT(BeaconsPercept)          // class provides a method to update BeaconsPercept. Representation can be requested.
*   PROVIDES_WITH_MODIFY_AND_OUTPUT(LinesPercept) // class provides a method to update LinesPercept. Representation can be MODIFYed and requested to be sent with given message.
*   PROVIDES(GoalsPercept)                        // class provides a method to update GoalsPercept.
* END_MODULE
*
* This block defines a base class MyImageProcessorBase. The module MyImageProcessor has to be
* derived from that class:
*
* class MyImageProcessor : public MyImageProcessorBase
* {
*   void update(BallPercept& ballPercept);
*   void update(BeaconsPercept& beaconsPercept);
*   void update(GoalsPercept& goalsPercept);
* };
*
* In the implementation file, the existence of the module has to be announced:
*
* MAKE_MODULE(MyImageProcessor)
*
* Currently, the header file of each module has to be included either in
* Modules/CognitionModules.h or Modules/MotionModules.h to force the build
* system to include it in the executable.
*/

#ifndef __Module_h_
#define __Module_h_

#include "Representations/Blackboard.h"
#include "Tools/Debugging/Modify.h"
#include "Tools/Debugging/Stopwatch.h"
#include "Platform/BHTrace.h"
#include "Platform/GTAssert.h"
#include <list>
#include <string>

/**
* @class Requirements
* The class collects all requirements of a certain module, i.e. the representations 
* that have to be updated before the module is executed.
* Its contents are only temporary and will be created and deleted for
* each module.
*/
class Requirements
{
public:
  /**
  * A class for representing information about a representation.
  */
  class Entry
  {
  public:
    const char* name; /**< The name of the representation. */
    void (*create)(); /**< The handler that is called to create a new instance of the representation. */
    void (*free)(); /**< The handler that is called to delete the instance of the representation. */
    void (*in)(In&); /**< The handler that is called to read the instance of the representation from a stream. */

    /**
    * Constructor.
    * @param name The name of the representation.
    * @param create The handler that is called to create a new instance of the representation.
    * @param free The handler that is called to delete the instance of the representation.
    * @param in The handler that is called to read the instance of the representation from a stream.
    */
    Entry(const char* name, void (*create)(), void (*free)(), void (*in)(In&)) :
      name(name),
      create(create),
      free(free),
      in(in) 
      {}

    /**
    * Comparison operator. Only uses the name for comparison.
    * @param other The representaion name this one is compared to.
    * @return Are the representation names the same?
    */
    bool operator==(const std::string& other) const {return other == name;}
  };

  typedef std::list<Entry> List; /**< Type of the list of all requirements. */
  static List entries; /**< The list of all requirements. */
  static bool record; /**< A flag that determines whether requirements are currently recorded or not. */

protected:
  /**
  * The method adds a new requirement to the list but only if the class
  * is currently in recording mode.
  * @param name The name of the requirement.
  * @param create The handler that is called to create a new instance of the representation.
  * @param free The handler that is called to delete the instance of the representation.
  * @param in The handler that is called to read the instance of the representation from a stream.
  */
  void add(const char* name, void (*create)(), void (*free)(), void (*in)(In&));
};

/**
* @class Requirement
* The class adds a single requirement to the list of requirements.
* This is somewhat tricky: since the instance of this object
* is an attribute of abstact class, it cannot be created. But through
* a brute force type-cast, it can be assigned. Therefore, the assignment
* operator is used to add the requirement.
* @param getName A function which returns the name of the representation.
* @param create The handler that is called to create a new instance of the representation.
* @param free The handler that is called to delete the instance of the representation.
* @param in The handler that is called to read the instance of the representation from a stream.
*/
template<const char* (*getName)(), void (*create)(), void (*free)(), void (*in)(In&)> class Requirement : private Requirements
{
public:
  /**
  * The assignment operator add the name of the template parameter
  * as a requirement.
  */
  void operator=(const Requirement&) {add(getName(), create, free, in);}
};

/**
* @class Representations
* The class collects all representations a certain module provides.
* Its contents are only temporary and will be created and deleted for
* each module.
*/
class Representations
{
public:
  /**
  * A class for representing information about a representation.
  */
  class Entry
  {
  public:
    const char* name; /**< The name of the representation. */
    void (*update)(Blackboard&); /**< The handler that is called to update the representation. */
    void (*create)(); /**< The handler that is called to create a new instance of the representation. */
    void (*free)(); /**< The handler that is called to delete the instance of the representation. */
    void (*out)(Out&); /**< The handler that is called to write the instance of the representation to a stream. */

    /**
    * Constructor.
    * @param name The name of the representation.
    * @param update The handler that is called to update the representation.
    * @param create The handler that is called to create a new instance of the representation.
    * @param free The handler that is called to delete the instance of the representation.
    * @param out The handler that is called to write the instance of the representation to a stream.
    */
    Entry(const char* name, void (*update)(Blackboard&), void (*create)(), void (*free)(), void (*out)(Out&)) :
      name(name),
      update(update),
      create(create),
      free(free),
      out(out) 
      {}

    /**
    * Comparison operator. Only uses the name for comparison.
    * @param other The representaion name this one is compared to.
    * @return Are the representation names the same?
    */
    bool operator==(const std::string& other) const {return other == name;}
  };

  typedef std::list<Entry> List; /**< Type of the list of all representations. */
  static List entries; /**< The list of a representations provided. */
  static bool record; /**< A flag that determines whether requirements are currently recorded or not. */

protected:
  /**
  * The method adds a new representation to the list but only if the class
  * is currently in recording mode.
  * @param name The name of the representation.
  * @param update The handler that is called to update the representation.
  * @param create The handler that is called to create a new instance of the representation.
  * @param free The handler that is called to delete the instance of the representation.
  * @param out The handler that is called to write the instance of the representation to a stream.
  */
  void add(const char* name, void (*update)(Blackboard&), void (*create)(),void (*free)(), void (*out)(Out&));
};

/**
* @class Representation
* The class adds a single representation to the list of representations provided.
* This is somewhat tricky: since the instance of this object
* is an attribute of abstact class, it cannot be created. But through
* a brute force type-cast, it can be assigned. Therefore, the assignment
* operator is used to add the representation.
* @param getName A function which returns the name of the representation.
* @param update The handler that is called to update the representation.
* @param create The handler that is called to create a new instance of the representation.
* @param free The handler that is called to delete the instance of the representation.
* @param out The handler that is called to write the instance of the representation to a stream.
*/
template<const char* (*getName)(), void (*update)(Blackboard&), void (*create)(), void (*free)(), void (*out)(Out&)>
class Representation : private Representations
{
public:
  /**
  * The assignment operator add the name of the template parameter
  * as a representation provided.
  */
  void operator=(const Representation&) {add(getName(), update, create, free, out);}
};

class ModuleManager;
class DefaultModule;

/**
* @class ModuleBase
* The class is the abstract base of all template classes that create modules.
*/
class ModuleBase
{
private:
  static std::list<ModuleBase*> modules; /**< The list of all modules available. */
  const char* name, /**< The name of the module that can be created by this instance. */
            * category; /**< The name of the category of this module. */

protected:
  Requirements::List requirements; /**< The list of all requirements of the module created by this instance. */
  Representations::List representations; /**< The list of all representations provided by the module created by this instance. */

  /**
  * Abstract method to create an instance of a module.
  * @return The address of the instance created.
  */
  virtual Blackboard* createNew() = 0;

public:
  /**
  * Constructor.
  * @param name The name of the module that can be created by this instance.
  * @param category The name of the category of this module.
  */
  ModuleBase(const char* name, const char* category) : 
    name(name),
    category(category)
  {
    modules.push_back(this);
  }

  friend class ModuleManager; /**< ModuleManager requires access to private data. */
  friend class DefaultModule; /**< DefaultModule requires access to private data. */
};

/**
* @class Module
* The template class provides a method to create a certain module, and it
* registers all requirements and representations provided by that class.
* @param M The type of the module created.
* @param B The base class of the module.
*/
template<class M, class B> class Module : public ModuleBase
{
private:
  /**
  * The method creates an instance of the module.
  * @return The address of the newly created instance.
  */
  Blackboard* createNew()
  {
    return (Blackboard*) new M;
  }

public:
  /**
  * Constructor.
  * The constructor registers all requirements and representations provided
  * of the module that can be created by this instance.
  * @attention Since the module itself should not be created to just register
  * its requirements and representations, because the constructor of the module
  * may do time-consuming operations, the base class has to be used directly
  * to perform the registration. However, the base class is abstract, so it
  * cannot be constructed. To overcome this limitation, an assignment is faked,
  * and it is used to do the registration of the information required.
  * @param name The name of the module that can be created by this instance.
  * @param category The name of the category of this module.
  */
  Module(const char* name, const char* category)
    : ModuleBase(name, category)
  {
    Representations::entries.clear();
    Requirements::entries.clear();
    Representations::record = Requirements::record = true;
    char buf[sizeof(B)];
    // executes assignment operators -> recording information!
    (B&) *buf = (const B&) *buf;
    Representations::record = Requirements::record = false;
    requirements = Requirements::entries;
    representations = Representations::entries;
  }
}; 

/**
* Macro for the first line of the module definition block.
* See beginning of this file.
* @param module The class implementing the module.
*/
#define MODULE(module) \
  class module##Base : private Blackboard \
  { \
    private: typedef module##Base _Me; \
    private: bool _initialized; \
    public: module##Base() : Blackboard(*Blackboard::theInstance), _initialized(false) {} \
    public: virtual void init() {} \
    using Blackboard::operator new; \
    using Blackboard::operator delete;

/**
* The macro defines a requirement.
* See beginning of this file.
* @param representation The representation that has to be updated before executing this module.
*/
#define REQUIRES(representation) \
  protected: using Blackboard::the##representation; \
  \
  /** \
  * The method creates the representation in the blackboard. \
  */ \
  static void create2##representation() \
  { \
    if(!&((_Me*) Blackboard::theInstance)->the##representation) \
      replace2##representation((representation**) Blackboard::theInstance, new representation); \
  } \
  \
  /** \
  * The method deletes the representation from the blackboard. \
  */ \
  static void free2##representation() \
  { \
    if(&((_Me*) Blackboard::theInstance)->the##representation) \
    { \
      delete &((_Me*) Blackboard::theInstance)->the##representation; \
      replace2##representation((representation**) Blackboard::theInstance, 0); \
    } \
  } \
  \
  /** \
  * The method replaces the reference (!) to the representation in the blackboard by a new one. \
  * Since references cannot be overwritten, this is done by a search and replace operation. \
  * @param r A pointer to the blackboard as array of pointers. \
  * @param rNew The new entry. \
  */ \
  static void replace2##representation(representation** r, representation* rNew) \
  { \
    for(unsigned i = 0; i < sizeof(Blackboard) / sizeof(representation*); ++i) \
    { \
      representation* temp = r[i]; \
      r[i] = rNew; \
      distract(); /* This call forces the compiler to consider possible changes to global data. */ \
      if(&((_Me*) Blackboard::theInstance)->the##representation == rNew) \
        return; \
      r[i] = temp; \
    } \
    ASSERT(false); \
  } \
  \
  /** \
  * The method reads the representation from a stream. \
  * @param stream The stream that is read from. \
  */ \
  static void in##representation(In& stream) \
  { \
    stream >> const_cast<representation&>(((_Me*) Blackboard::theInstance)->the##representation); \
  } \
  private: \
  /** \
  * The method returns the name of the representation. \
  */ \
  static const char* getName2##representation() {return #representation;}\
  \
  Requirement<&_Me::getName2##representation, &_Me::create2##representation, &_Me::free2##representation, &_Me::in##representation> z##representation;

/**
* The macro defines a usage, i.e. a representation that is accessed but does not need to be up to date.
* See beginning of this file.
* @param representation The representation that is used.
*/
#define USES(representation) \
  protected: using Blackboard::the##representation;

/**
* The macro defines a representation that is updated by this module.
* See beginning of this file.
* @param representation The representation that can be updated by this module.
*/
#define PROVIDES_WITH_MODIFY(representation) \
  _PROVIDES(representation, MODIFY("representation:" #representation, const_cast<representation&>(r));)

/**
* The macro defines a representation that is updated by this module.
* See beginning of this file.
* The macro will additionally call the representations draw method. 
* @param representation The representation that can be updated by this module.
*/
#define PROVIDES_WITH_DRAW(representation) \
  _PROVIDES(representation, EXECUTE_ONLY_IN_DEBUG( \
      DEBUG_RESPONSE_NOT("representations:disable drawing generation:" #representation, \
        const_cast<representation&>(r).draw(); \
      ); \
    ); \
  )

/**
* The macro defines a representation that is updated by this module.
* See beginning of this file.
* The macro will additionally call the representations draw method. 
* @param representation The representation that can be updated by this module.
*/
#define PROVIDES_WITH_MODIFY_AND_DRAW(representation) \
  _PROVIDES(representation, MODIFY("representation:" #representation, const_cast<representation&>(r)); \
    EXECUTE_ONLY_IN_DEBUG( \
      DEBUG_RESPONSE_NOT("representations:disable drawing generation:" #representation, \
        const_cast<representation&>(r).draw(); \
      ); \
    ); \
  )

/**
* The macro defines a representation that is updated by this module.
* See beginning of this file.
* The macro will generate an OUTPUT statement as well as a MODIFY statement for the representation.
* The macro will additionally call the representations draw method. 
* @param representation The representation that can be updated by this module.
*/
#define PROVIDES_WITH_MODIFY_AND_OUTPUT_AND_DRAW(representation) \
  _PROVIDES(representation, MODIFY("representation:" #representation, const_cast<representation&>(r)); \
    DEBUG_RESPONSE("representation:" #representation, OUTPUT(id##representation, bin, r); ); \
    EXECUTE_ONLY_IN_DEBUG( \
      DEBUG_RESPONSE_NOT("representations:disable drawing generation:" #representation, \
        const_cast<representation&>(r).draw(); \
      ); \
    ); \
  )

/**
* The macro defines a representation that is updated by this module.
* The macro will generate an OUTPUT statement instead of a MODIFY statement for the representation.
* @param representation The representation that can be updated by this module.
*/
#define PROVIDES_WITH_OUTPUT(representation) \
  _PROVIDES(representation, DEBUG_RESPONSE("representation:" #representation, OUTPUT(id##representation, bin, r); );)

/**
* The macro defines a representation that is updated by this module.
* The macro will generate an OUTPUT statement as well as a MODIFY statement for the representation.
* @param representation The representation that can be updated by this module.
*/
#define PROVIDES_WITH_MODIFY_AND_OUTPUT(representation) \
  _PROVIDES(representation, \
    MODIFY("representation:" #representation, const_cast<representation&>(r)); \
    DEBUG_RESPONSE("representation:" #representation, OUTPUT(id##representation, bin, r); ); \
  )

/**
* The macro defines a representation that is updated by this module.
* The macro will not generate an OUTPUT or MODIFY statement for the representation.
* @param representation The representation that can be updated by this module.
*/
#define PROVIDES(representation) \
  _PROVIDES(representation, )

/**
* The macro defines a representation that is updated by this module.
* @attention Don't use this macro directly.
* This macro also define methods to create and delete this representation in the
* blackboard. Please note that replacing the references in the blackboard is
* really dirty, but it works and abstracts from the fact that not all representations
* exist all the time. In fact, only the representations currently in use are actually
* constructed.
* @param representation The representation that can be updated by this module.
* @param mod Either a MODIFY expression or nothing.
*/
#define _PROVIDES(representation, mod) \
  /** \
  * The derived class must implement an update method for this representation. \
  * @param repName The representation that is updated. \
  */ \
  protected: virtual void update(representation& the##representation) = 0; \
  \
  /** \
  * The method is called to update the representation by this module. \
  * @param b The module. \
  */ \
  private: static void update##representation(Blackboard& b) \
  { \
    const representation& r = ((_Me*) Blackboard::theInstance)->the##representation; \
    ASSERT(&r); \
    if(!((_Me&) b)._initialized) \
    { \
      BH_TRACE; \
      ((_Me&) b).init(); \
      ((_Me&) b)._initialized = true; \
    } \
    BH_TRACE; \
    STOP_TIME_ON_REQUEST(#representation, ((_Me&) b).update(const_cast<representation&>(r)); ); \
    mod \
  } \
  \
  /** \
  * The method creates the representation in the blackboard. \
  */ \
  static void create##representation() \
  { \
    if(!&((_Me*) Blackboard::theInstance)->the##representation) \
      replace##representation((representation**) Blackboard::theInstance, new representation); \
  } \
  \
  /** \
  * The method deletes the representation from the blackboard. \
  */ \
  static void free##representation() \
  { \
    if(&((_Me*) Blackboard::theInstance)->the##representation) \
    { \
      delete &((_Me*) Blackboard::theInstance)->the##representation; \
      replace##representation((representation**) Blackboard::theInstance, 0); \
    } \
  } \
  \
  /** \
  * The method replaces the reference (!) to the representation in the blackboard by a new one. \
  * Since references cannot be overwritten, this is done by a search and replace operation. \
  * @param r A pointer to the blackboard as array of pointers. \
  * @param rNew The new entry. \
  */ \
  static void replace##representation(representation** r, representation* rNew) \
  { \
    for(unsigned i = 0; i < sizeof(Blackboard) / sizeof(representation*); ++i) \
    { \
      representation* temp = r[i]; \
      r[i] = rNew; \
      distract(); /* This call forces the compiler to consider possible changes to global data. */ \
      if(&((_Me*) Blackboard::theInstance)->the##representation == rNew) \
        return; \
      r[i] = temp; \
    } \
    ASSERT(false); \
  } \
  \
  /** \
  * The method writes the representation to a stream. \
  * @param stream The stream that is written to. \
  */ \
  static void out##representation(Out& stream) \
  { \
    stream << ((_Me*) Blackboard::theInstance)->the##representation; \
  } \
  \
  \
  /** \
  * The method returns the name of the representation. \
  */ \
  static const char* getName##representation() {return #representation;}\
  \
  Representation<&_Me::getName##representation, &_Me::update##representation, &_Me::create##representation, \
                 &_Me::free##representation, &_Me::out##representation> zz##representation;

#define END_MODULE };

/**
* The macro creates a creator for the module which can be ony of multiple solutions of a common base class.
* See beginning of this file.
* It has to be part of the implementation file.
* @param module The name of the module that can be created.
* @param base The name of the base class of the module.
* @param category The name of the category of this module.
*/
#define MAKE_SOLUTION(module, base, category) Module<module, base##Base> the##module##Module(#module, #category);

/**
* The macro creates a creator for the module.
* See beginning of this file.
* It has to be part of the implementation file.
* @param module The name of the module that can be created.
* @param category The name of the category of this module.
*/
#define MAKE_MODULE(module, category) MAKE_SOLUTION(module, module, category)

#endif //__Module_h_
