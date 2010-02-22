/**
 * @file Material.h
 * 
 * Definition of class Material
 *
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 */ 

#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <string>
#include <Parser/ParserUtilities.h>
#include <Tools/Errors.h>

/** 
 * @class CoefficientDefinition
 * A class describing a coefficient definition
 */
class CoefficientDefinition
{
public:
  /** The other material of the pair the coefficient is defined for */
  std::string otherMaterial;
  /** The value of the coefficient */
  double coefficient;
};

/**
 * @class Material
 * A class describing the material of a physical object
 */
class Material
{
public:
  /** Constructor */
  Material(){name=""; index=-1;};
  //TESTKAI
  Material(std::string materialName, int materialIndex){name = materialName; index = materialIndex;};
  /**
   * Constructor
   * @param attributes The attributes of the material
   */
  Material(const AttributeSet& attributes) 
  {
    name = ParserUtilities::getValueFor(attributes, "name");
    index=-1;
  };

  /** 
   * Returns the name of the Material
   * @return The name
   */
  std::string getName(){return name;};
  /** Sets the index in list of metarial definitions for this material 
   *  @return The index in list of material definitions
   */
  int getIndex(){return index;};
  /** Sets the index in list of metarial definitions for this material
   *  @param newIndex The new index in list of material definitions
   */
  void setIndex(int newIndex){index = newIndex;};
  /**
   * Returns the friction coefficent defined for this material
   * @return A pointer to the friction coefficient definitions
   */
  const std::vector<CoefficientDefinition>* getFrictionCoefficients(){return &frictionCoefficients;};
  /**
   * Returns the rolling friction coefficent defined for this material
   * @return A pointer to the rolling friction coefficient definitions
   */
  const std::vector<CoefficientDefinition>* getRollingFrictionCoefficients(){return &rollingFrictionCoefficients;};
  /**
   * Returns the coefficents of restitution defined for this material
   * @return A pointer to the coefficient of restitution definitions
   */
  const std::vector<CoefficientDefinition>* getRestitutionCoefficients(){return &restitutionCoefficients;};

  /**
  * Parses the attributes of a simulated object which are
  * defined in seperate elements
  * @param name The name of the attribute element
  * @param attributes The attributes of the element
  * @param line The line in the scene description
  * @param column The column in the scene description
  * @param errorManager An object managing errors
  */
  void parseAttributeElements(const std::string& name, 
                              const AttributeSet& attributes,
                              int line, int column, 
                              ErrorManager* errorManager)
  {
    if(name == "FrictionCoefficient")
    {
      CoefficientDefinition frictionCoefficient;
      frictionCoefficient.otherMaterial = ParserUtilities::getValueFor(attributes, "otherMaterial");
      frictionCoefficient.coefficient = ParserUtilities::toDouble(ParserUtilities::getValueFor(attributes, "value"));

      this->frictionCoefficients.push_back(frictionCoefficient);
    }
    else if(name == "RollingFrictionCoefficient")
    {
      CoefficientDefinition frictionCoefficient;
      frictionCoefficient.otherMaterial = ParserUtilities::getValueFor(attributes, "otherMaterial");
      frictionCoefficient.coefficient = ParserUtilities::toDouble(ParserUtilities::getValueFor(attributes, "value"));

      this->rollingFrictionCoefficients.push_back(frictionCoefficient);
    }
    else if(name == "RestitutionCoefficient")
    {
      CoefficientDefinition restitutionCoefficient;
      restitutionCoefficient.otherMaterial = ParserUtilities::getValueFor(attributes, "otherMaterial");
      restitutionCoefficient.coefficient = ParserUtilities::toDouble(ParserUtilities::getValueFor(attributes, "value"));

      this->restitutionCoefficients.push_back(restitutionCoefficient);
    }

  };

private:
  /** The name of the material */
  std::string name;
  /** The index in the list of materials */
  int index;
  /** The friction coefficients defined for this material */
  std::vector<CoefficientDefinition> frictionCoefficients;
  /** The rolling friction coefficients defined for this material */
  std::vector<CoefficientDefinition> rollingFrictionCoefficients;
  /** The coefficients of restitution defined for this material */
  std::vector<CoefficientDefinition> restitutionCoefficients;
};

#endif //MATERIAL_H_

