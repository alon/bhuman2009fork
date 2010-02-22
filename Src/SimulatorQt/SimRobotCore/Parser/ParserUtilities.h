/**
 * @file ParserUtilities.h
 * 
 * Definition of a set of utility functions and classes
 *
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 * @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
 */ 

#ifndef PARSER_UTILITIES_H_
#define PARSER_UTILITIES_H_

#include <string>
#include <vector>
#include "../Tools/SimMath.h"


/*
* @class AttributeValuePair
* A class representing the name of an
* attribute and its content
*/
class AttributeValuePair
{
public:
  /** Empty Constructor*/
  AttributeValuePair() {}

  /** Initializing Constructor
  * @param attributeName The name of the attribute
  * @param attributeValue The value of the attribute
  */
  AttributeValuePair(const std::string& attributeName,
                     const std::string& attributeValue):
                     attribute(attributeName), value(attributeValue)
  {}

  /** Copy-Constructor
  * @param other Another AttributeValuePair
  */
  AttributeValuePair(const AttributeValuePair& other)
  {
    attribute = other.attribute;
    value = other.value;
  }

  /** The name of the attribute*/
  std::string attribute;
  /** The content of the attribute*/
  std::string value;
};

/** A new notation for a list of attribute value pairs*/
typedef std::vector<AttributeValuePair> AttributeSet;


/**
* @class ParserUtilities
* A set of useful parsing functions
*/
class ParserUtilities
{
public:
  static double toDouble(const std::string& numberString);
  static int toInt(const std::string& numberString);
  static bool toBool(const std::string& boolString);
  static Vector3d toPoint(const AttributeSet& attributes);
  static Vertex toVertex(const AttributeSet& attributes);
  static void toUnnormalizedPoint(const AttributeSet& attributes, float* point);
  static void toNormalizedPoint(const AttributeSet& attributes, float* normpoint);
  static Vector3d toRGBTriple(const AttributeSet& attributes);
  static void toRGB(const AttributeSet& attributes, float* rgb);
  static void toRGBA(const AttributeSet& attributes, float* rgba);
  static int toTextureWrapMode(const std::string& modeString);
  static int getIndexOf(const AttributeSet& attributes, const std::string& name);
  static std::string getValueFor(const AttributeSet& attributes, const std::string& name);
  static void parseXYIntPair(const AttributeSet& attributes, int& x, int& y);
  static void parseXYDoublePair(const AttributeSet& attributes, double& x, double& y);
  static void parseRange(const AttributeSet& attributes, double& lowerBoundary, double& upperBoundary);
};


#endif //PARSER_UTILITIES_H_
