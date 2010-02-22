/**
* @file Tools/Debugging/DebugDrawings3D.cpp
*
* Functions for Debugging
* 
* @author Michael Spranger
*/ 

#include "DebugDrawings3D.h"
#include "Platform/GTAssert.h"


void DrawingManager3D::addDrawingId(const char* name, const char* typeName)
{
  if(drawingNameIdTable.find(name) == drawingNameIdTable.end())
  {
    drawingNameIdTable[name].id=(char)drawingNameIdTable.size();
    Drawing3DNameTypeTable::const_iterator i;
    for(i = drawing3DNameTypeTable.begin(); i != drawing3DNameTypeTable.end(); ++i)
      if(!strcmp(i->first, typeName))
        break;
    if(i == drawing3DNameTypeTable.end())
      drawingNameIdTable[name].type = drawing3DNameTypeTable[typeName] = (char) drawing3DNameTypeTable.size();
    else
      drawingNameIdTable[name].type = i->second;
  }
}

void DrawingManager3D::clear()
{
  drawingNameIdTable.clear();
  stringTable.clear();
}

const char* DrawingManager3D::getString(const std::string& string)
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

In& operator>>(In& stream, DrawingManager3D& drawingManager)
{
  // note that this operator appends the data read to the drawingManager
  // clear() has to be called first to replace the existing data
  int size;
  stream >> size; 
  for(int i = 0; i < size; ++i)
  {
    DrawingManager3D::DrawingNameIdTableEntry entry;
    std::string name;
    int id, type;
    stream >> id >> type >> name;
    entry.id = (char) id;
    entry.type = (char) type;
    drawingManager.drawingNameIdTable[drawingManager.getString(name)] = entry;
  } 
  return stream;
}

Out& operator<<(Out& stream, const DrawingManager3D& drawingManager)
{
  stream << (int)drawingManager.drawingNameIdTable.size();
  for (DrawingManager3D::DrawingNameIdTable::const_iterator iter = drawingManager.drawingNameIdTable.begin();
       iter != drawingManager.drawingNameIdTable.end(); ++iter)
  {
    stream << (int) iter->second.id;
    stream << (int) iter->second.type;
    stream << iter->first;
  }
  
  return stream;
}
