/**
* @file Tools/Debugging/DebugDrawings.cpp
*
* Functions for Debugging
* 
* @author Michael Spranger
*/ 

#include "DebugDrawings.h"
#include "Platform/GTAssert.h"
#include "Tools/Debugging/DebugDataTable.h" // HACK: this file needs to be included because something doesn't compile otherwise


In& operator>>(In& stream, ColorRGBA& color)
{
  stream >> color.r;
  stream >> color.g;
  stream >> color.b;
  stream >> color.a;
  return stream;
}

Out& operator<<(Out& stream, const ColorRGBA& color)
{
  stream << color.r;
  stream << color.g;
  stream << color.b;
  stream << color.a;  
  return stream;
}


void DrawingManager::addDrawingId(const char* name, const char* typeName)
{
  if(drawingNameIdTable.find(name) == drawingNameIdTable.end())
  {
    drawingNameIdTable[name].id=(char)drawingNameIdTable.size();
    drawingNameIdTable[name].processIdentifier = processIdentifier;

    // String pooling seems not to work for drawing types on the AIBO.
    // So we use string comparison here instead of find(), which only matches addresses.
    DrawingNameTypeTable::const_iterator i;
    for(i = drawingNameTypeTable.begin(); i != drawingNameTypeTable.end(); ++i)
      if(!strcmp(i->first, typeName))
        break;
    if(i == drawingNameTypeTable.end())
      drawingNameIdTable[name].type = drawingNameTypeTable[typeName] = (char)drawingNameTypeTable.size();
    else
      drawingNameIdTable[name].type = i->second;
  }
}

void DrawingManager::clear()
{
  drawingNameTypeTable.clear();
  drawingNameIdTable.clear();
  stringTable.clear();
}

const char* DrawingManager::getString(const std::string& string)
{
  StringTable::iterator i;
  i = stringTable.find(string);
  if(i == stringTable.end())
  {
    stringTable[string];
    i = stringTable.find(string);
  }
  return i->first.c_str();
}

In& operator>>(In& stream, DrawingManager& drawingManager)
{
  // note that this operator appends the data read to the drawingManager
  // clear() has to be called first to replace the existing data

  int size;
  stream >> size;;
  for(int i = 0; i < size; ++i)
  {
    std::string name;
    int id;
    stream >> id >> name;
    drawingManager.drawingNameTypeTable[drawingManager.getString(name)] = (char) id;
  }

  stream >> size; 
  for(int i = 0; i < size; ++i)
  {
    DrawingManager::DrawingNameIdTableEntry entry;
    std::string name;
    int id, 
        type;
    stream >> id >> type >> name;
    entry.id = (char) id;
    entry.type = (char) type;
    entry.processIdentifier = drawingManager.processIdentifier;
    drawingManager.drawingNameIdTable[drawingManager.getString(name)] = entry;
  }
  
  return stream;
}

Out& operator<<(Out& stream, const DrawingManager& drawingManager)
{
  stream << (int)drawingManager.drawingNameTypeTable.size();
  
  for (DrawingManager::DrawingNameTypeTable::const_iterator iter = drawingManager.drawingNameTypeTable.begin();
       iter != drawingManager.drawingNameTypeTable.end(); ++iter)
  {
    stream << (int) iter->second;
    stream << iter->first;
  }

  stream << (int)drawingManager.drawingNameIdTable.size();
  for (DrawingManager::DrawingNameIdTable::const_iterator iter = drawingManager.drawingNameIdTable.begin();
       iter != drawingManager.drawingNameIdTable.end(); ++iter)
  {
    stream << (int) iter->second.id;
    stream << (int) iter->second.type;
    stream << iter->first;
  }
  
  return stream;
}
