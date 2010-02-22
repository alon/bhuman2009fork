/*
* @file Tools/Debugging/DebugDataTable.h
* Table for generic handling of debug data
*
* @author Michael Spranger
* @author Tobias Oberlies
*/

#ifndef __DebugRegistry_h_
#define __DebugRegistry_h_


#include "Tools/Streams/Streamable.h"
#include "Tools/Debugging/Debugging.h"
#include <vector>
#include <string>

class DebugDataHeader
{
public:
  enum { write, unchanged } request;
  virtual void changeData(In&)=0;
  virtual void unchangeData()=0;
  DebugDataHeader() : request(unchanged) {}
  virtual ~DebugDataHeader() {}
};

template< class T>
class DebugData : public DebugDataHeader
{
public:
  T* data;
  
  DebugData()
    : data(NULL)
  {
  }
  ~DebugData()
  {
    unchangeData();
  }
  virtual void changeData(In& in)
  {
    if (data == NULL) data = new T;
    in >> *data;
  }
  virtual void unchangeData()
  {
    if (data != NULL)
    {
      delete data;
      data = NULL;
    }
  }
};

template< class T>
class EnumDebugData : public DebugDataHeader
{
public:
  T* data;

  EnumDebugData()
    : data(NULL)
  {
  }
  ~EnumDebugData()
  {
    unchangeData();
  }
  virtual void changeData(In& in)
  {
    if (data == NULL) data = new T;
    streamEnum( in, *data);
  }
  virtual void unchangeData()
  {
    if (data != NULL)
    { 
      delete data;
      data = NULL;
    }
  }
};


/** 
* @class DebugDataTable
*
* A singleton class that maintains the debug data table. 
*/
class DebugDataTable
{
private:
  hash_map< std::string, DebugDataHeader*> table;

  /**
   * Default constructor.
   * No other instance of this class is allowed except the one accessible via getDebugDataTable
   * therefore the constructor is private.
   */
  DebugDataTable() {}

  /**
   * Copy constructor.
   * Copying instances of this class is not allowed
   * therefore the copy constructor is private. 
   */
  DebugDataTable(const DebugDataTable&) {}

  /*
   * Only a process is allowed to create the instance.
   */
  friend class Process;

public:
  ~DebugDataTable()
  {
    hash_map< std::string, DebugDataHeader*>::iterator iter;
    iter = table.begin();
    for(; iter != table.end(); ++iter)
      delete iter->second;
  }

  /**
   * Checks if the object with the given name has been modified from RobotControl.
   */
  bool isModified( const char* name)
  {
    hash_map< std::string, DebugDataHeader*>::iterator iter = table.find( name);
    return ( iter != table.end() && iter->second->request == DebugDataHeader::write);
  }

  /**
   * Registers the object with the debug data table and updates the object if the 
   * respective entry in the table has been modified through RobotControl. 
   */
  template< class T>
  void updateObject( const char* name, T& t)
  {
    // Find entry in debug data table
    hash_map< std::string, DebugDataHeader*>::iterator iter = table.find( name);
    
    if( iter == table.end())
    {
      // Create a copy of the object (marked as unchanged) and store in debug data table
      table[ name] = new DebugData<T>();
    }
    else if( iter->second->request == DebugDataHeader::write)
    {
      // Assign the changed entry in the debug data table to the object
      DebugData<T>* debugData = dynamic_cast<DebugData<T>*>(iter->second);
      if( debugData != NULL)
        if (debugData->data != NULL)
          t = *debugData->data;
    }
  }

  /**
   * Registers the enum value with the debug data table and updates the value if it has 
   * been modified through RobotControl. 
   */
  template< class T>
  void updateEnum( const char* name, T& t)
  {
    // Find entry in debug data table
    hash_map< std::string, DebugDataHeader*>::iterator iter = table.find( name);

    if( iter == table.end())
    {
      // Create a copy of the enum (marked as unchanged) and store in debug data table
      table[ name] = new EnumDebugData<T>();
    }
    else if( iter->second->request == DebugDataHeader::write)
    {
      // Assign the changed entry in the debug data table to the object
      EnumDebugData<T>* debugData = dynamic_cast<EnumDebugData<T>*>(iter->second);
      if( debugData != NULL)
        if (debugData->data != NULL)
          t = *debugData->data;
    }
  }
  
  bool processChangeRequest( In& in)
  {
    std::string name;
    in >> name;
    
    if( name == "disable all")
    {
      for( 
        hash_map< std::string, DebugDataHeader*>::iterator iter = table.begin();
        iter != table.end();
        ++iter
          )
      {
        iter->second->request = DebugDataHeader::unchanged;
        iter->second->unchangeData();
      }
    }
    else
    {
      char change;
      in >> change;
      hash_map< std::string, DebugDataHeader*>::iterator iter = table.find( name);
      if( iter != table.end())
      {
        if( change)
        {
          iter->second->request = DebugDataHeader::write;
          iter->second->changeData( in);
        }
        else
        {
          iter->second->request = DebugDataHeader::unchanged;
          iter->second->unchangeData();
        }
      }
    }
    return true;

  }

  /**
   * Functions for ensuring that object is streamable at compile time.
   */
  static inline void testForStreamable(const ImplicitlyStreamable& object) {}
  static inline void testForStreamable(const bool& object) {}
  static inline void testForStreamable(const int& object) {}
  static inline void testForStreamable(const unsigned int& object) {}
  static inline void testForStreamable(const long& object) {}
  static inline void testForStreamable(const unsigned long& object) {}
  static inline void testForStreamable(const float& object) {}
  static inline void testForStreamable(const double& object) {}
  static inline void testForStreamable(const std::string& object) {}
};

In& operator>>(In& in, DebugDataTable& debugDataTable);

#endif // DebugRegistry
