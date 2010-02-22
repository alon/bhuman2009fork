/**
 * @file CoefficientTable.cpp
 * 
 * Implementation of class CoefficientTable
 *
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 */ 

#include "CoefficientTable.h"

CoefficientTable::CoefficientTable() : table(NULL), numberOfMaterials(0)
{
}

CoefficientTable::~CoefficientTable()
{
  if(table != NULL)
    delete[] table;
}

void CoefficientTable::createTable(double defaultCoeff)
{
  unsigned int i=0;
  std::map<std::string, int>::const_iterator materialsIter;

  for(i=0; i < tableEntries.size(); i++)
  {
    materialsIter = materials.find(tableEntries[i].material1);
    if(materialsIter == materials.end())
    {
      materials[tableEntries[i].material1] = materials.size();
    }
    materialsIter = materials.find(tableEntries[i].material2);
    if(materialsIter == materials.end())
    {
      materials[tableEntries[i].material2] = materials.size();
    }
  }

  const int tableSize = materials.size();
  this->numberOfMaterials = tableSize;
  table = new double[tableSize*tableSize];
  for(i=0; i < (unsigned int)tableSize*tableSize; i++)
    table[i] = defaultCoeff;

  for(i=0; i < tableEntries.size(); i++)
  {
    int temp1 = materials.find(tableEntries[i].material1)->second;
    int temp2 = materials.find(tableEntries[i].material2)->second;
    int index = temp1 * tableSize + temp2;
    table[index] = tableEntries[i].coefficient;
    index = temp2*tableSize + temp1;
    table[index] = tableEntries[i].coefficient;
  }
};

void CoefficientTable::createTable(const std::vector<Material*>* materialDefs, double defaultCoeff)
{
  std::vector<Material*>::const_iterator iter;
  for(iter = materialDefs->begin(); iter != materialDefs->end(); ++iter)
  {
    materials[(*iter)->getName()] = (*iter)->getIndex();
  }

  const int tableSize = materialDefs->size();
  this->numberOfMaterials = tableSize;
  table = new double[tableSize*tableSize];

  unsigned int i=0;
  for(i=0; i < tableEntries.size(); i++)
  {
    int temp1 = materials.find(tableEntries[i].material1)->second;
    int temp2 = materials.find(tableEntries[i].material2)->second;
    int index = temp1 * tableSize + temp2;
    table[index] = tableEntries[i].coefficient;
    index = temp2*tableSize + temp1;
    table[index] = tableEntries[i].coefficient;
  }
};


int CoefficientTable::getMaterialIndex(const std::string materialName) const
{
  int result = -1;
  std::map<std::string, int>::const_iterator materialsIter;
  materialsIter = materials.find(materialName);
  if(materialsIter != materials.end())
    result = materialsIter->second;

  return result;
}

void CoefficientTable::addCoefficientTableEntry(const CoefficientTableEntry& coeffTE)
{
  this->tableEntries.push_back(coeffTE);
}

