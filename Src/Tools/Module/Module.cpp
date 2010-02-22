/** 
* @file Module.cpp
* The class attributes of the module handling schema.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#include "Module.h"

void Requirements::add(const char* name, void (*create)(),void (*free)(), void (*in)(In&))
{
  if(record)
    entries.push_back(Entry(name, create, free, in));
}

void Representations::add(const char* name, void (*update)(Blackboard&), void (*create)(),void (*free)(), void (*out)(Out&))
{
  if(record)
    entries.push_back(Entry(name, update, create, free, out));
}

// init before all other global variables
#ifdef _MSC_VER
#pragma warning(disable:4073)
#pragma init_seg(lib)
#define FIRST
#else
#ifdef __GNUC__
#define FIRST __attribute__ ((init_priority (101)))
#else
#define FIRST
#endif
#endif

std::list<Requirements::Entry> Requirements::entries FIRST;
bool Requirements::record = 0;
std::list<Representations::Entry> Representations::entries FIRST;
bool Representations::record;
std::list<ModuleBase*> ModuleBase::modules FIRST;
