/**
 * @file CoefficientTable.h
 * 
 * Definition of class CoefficientTable
 *
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 */ 

#ifndef COEFFICIENTTABLE_H_
#define COEFFICIENTTABLE_H_

#include <vector>
#include <string>
#include <map>

#include "Material.h"

/** 
 * A class describing an entry in the coefficient table, containing the material pair 
 * and the belonging coefficient
 */
class CoefficientTableEntry
{
public:
  /** The name of the first material the coefficient is defined for */
  std::string material1;
  /** The name of the second material the coefficient is defined for */
  std::string material2;
  /** The friction coefficient of the material pair */
  double coefficient;
};


/** A class describing a coefficient table*/
class CoefficientTable
{
private:
  /** A list of all coefficient table entries */
  std::vector<CoefficientTableEntry> tableEntries;
  /** A list of all materials defined in the table */
  std::map<std::string, int> materials;
  /** The coefficient table */
  double* table;
  /** The number of materials defined in the table */
  int numberOfMaterials;

public:
  /** Constructor */
  CoefficientTable();
  /** Destructor */
  virtual ~CoefficientTable();
  /** 
   * Creates the coefficient table from the coefficient definitions 
   * @param defaultCoeff The default coefficient
   */
  void createTable(double defaultCoeff);
  /** 
   * Creates the coefficient table from the coefficient definitions 
   * @param materialDefs The list of material definitions
   * @param defaultCoeff The default coefficient
   */
  void createTable(const std::vector<Material*>* materialDefs, double defaultCoeff);
  /** 
   * Returns the index of the given material in the material list of the coefficient table
   * @param materialName The name of the material
   * @return The index in the material list of the table
   */
  int getMaterialIndex(const std::string materialName) const;
  /**
   * Adds a coefficent table entry
   * @param coeffTE The coefficient table entry
   */
  void addCoefficientTableEntry(const CoefficientTableEntry& coeffTE);
  /**
   * Returns a pointer to the coefficent table for fast index access
   * @return A pointer to the coefficent table
   */
  const double* getTable(){return table;};
  /**
   * Returns the number of materials stored in the coefficent table
   * @return The number of materials
   */
  int getNumberOfMaterialsInTable(){return materials.size();};
};

#endif //COEFFICIENTTABLE_H_

