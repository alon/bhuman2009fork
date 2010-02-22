/**
* @file ParserUtilities.cpp
* 
* Implementation of a set of utility functions and classes.
*
* @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
* @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
*/ 

#include <cstring>
#include <cstdlib>
#include "ParserUtilities.h"
#include <GL/glew.h>


double ParserUtilities::toDouble(const std::string& numberString)
{
  return atof(numberString.c_str());
}

int ParserUtilities::toInt(const std::string& numberString)
{
  return atoi(numberString.c_str());
}

bool ParserUtilities::toBool(const std::string& boolString)
{
  return (boolString == "true");
}

Vector3d ParserUtilities::toPoint(const AttributeSet& attributes)
{
  Vector3d returnValue;
  for(unsigned int i=0; i< attributes.size(); i++)
  {
    double value(ParserUtilities::toDouble(attributes[i].value));
    switch(attributes[i].attribute[0])
    {
    case 'x': returnValue.v[0] = value; break;
    case 'y': returnValue.v[1] = value; break;
    case 'z': returnValue.v[2] = value; break;
    default: break;
    }
  }
  return returnValue;
}

Vertex ParserUtilities::toVertex(const AttributeSet& attributes)
{
  Vertex returnValue;
  for(unsigned int i=0; i< attributes.size(); i++)
  {
    double value(ParserUtilities::toDouble(attributes[i].value));
    switch(attributes[i].attribute[0])
    {
    case 'x': returnValue.pos.v[0] = value; break;
    case 'y': returnValue.pos.v[1] = value; break;
    case 'z': returnValue.pos.v[2] = value; break;
    case 's': returnValue.tex.s = value; break;
    case 't': returnValue.tex.t = value; break;
    default: break;
    }
  }
  return returnValue;
}

void ParserUtilities::toUnnormalizedPoint(const AttributeSet& attributes, float* point)
{
  for(unsigned int i=0; i< attributes.size(); i++)
  {
    double value(ParserUtilities::toDouble(attributes[i].value));
    switch(attributes[i].attribute[0])
    {
    case 'x': point[0] = (float)value; break;
    case 'y': point[1] = (float)value; break;
    case 'z': point[2] = (float)value; break;
    case 'w':
      point[3] = (float)value;
      if(point[3] == 0.0)
        point[3] = 1.0;
      break;
    default: break;
    }
  }
}

void ParserUtilities::toNormalizedPoint(const AttributeSet& attributes, float* normpoint)
{
  for(unsigned int i=0; i< attributes.size(); i++)
  {
    double value(ParserUtilities::toDouble(attributes[i].value));
    switch(attributes[i].attribute[0])
    {
    case 'x': normpoint[0] = (float)value; break;
    case 'y': normpoint[1] = (float)value; break;
    case 'z': normpoint[2] = (float)value; break;
    default: break;
    }
  }
}

Vector3d ParserUtilities::toRGBTriple(const AttributeSet& attributes)
{
  Vector3d returnValue;
  for(unsigned int i=0; i< attributes.size(); i++)
  {
    double value(ParserUtilities::toDouble(attributes[i].value));
    if(attributes[i].attribute == "r")
    {
      returnValue.v[0] = value;
    }
    else if(attributes[i].attribute == "g")
    {
      returnValue.v[1] = value;
    }
    else if(attributes[i].attribute == "b")
    {
      returnValue.v[2] = value;
    }
  }
  returnValue /= 255.0;
  return returnValue;
}

void ParserUtilities::toRGB(const AttributeSet& attributes, float* rgb)
{
  for(unsigned int i=0; i< attributes.size(); i++)
  {
    double value(ParserUtilities::toDouble(attributes[i].value));
    switch(attributes[i].attribute[0])
    {
    case 'r': rgb[0] = (float)value; break;
    case 'g': rgb[1] = (float)value; break;
    case 'b': rgb[2] = (float)value; break;
    default: break;
    }
  }
}

void ParserUtilities::toRGBA(const AttributeSet& attributes, float* rgba)
{
  for(unsigned int i=0; i< attributes.size(); i++)
  {
    double value(ParserUtilities::toDouble(attributes[i].value));
    switch(attributes[i].attribute[0])
    {
    case 'r': rgba[0] = (float)value; break;
    case 'g': rgba[1] = (float)value; break;
    case 'b': rgba[2] = (float)value; break;
    case 'a': rgba[3] = (float)value; break;
    default: break;
    }
  }
}

int ParserUtilities::toTextureWrapMode(const std::string& modeString)
{
  if(strcmp(modeString.c_str(), "GL_CLAMP") == 0)
    return GL_CLAMP;
  else if(strcmp(modeString.c_str(), "GL_CLAMP_TO_EDGE") == 0)
  {
    if(GL_VERSION_1_2)
      return GL_CLAMP_TO_EDGE;
    else
      return GL_CLAMP;
  }
  else if(strcmp(modeString.c_str(), "GL_CLAMP_TO_BORDER") == 0)
  {
    if(GL_VERSION_1_3)
      return GL_CLAMP_TO_BORDER;
    else
      return GL_CLAMP;
  }
  else if(strcmp(modeString.c_str(), "GL_MIRRORED_REPEAT") == 0)
  {
    if(GL_VERSION_1_4)
      return GL_MIRRORED_REPEAT;
    else
      return GL_REPEAT;
  }
  else if(strcmp(modeString.c_str(), "GL_REPEAT") == 0)
    return GL_REPEAT;
  else
    return GL_NONE;
}

int ParserUtilities::getIndexOf(const AttributeSet& attributes, 
                                const std::string& name)
{
  for(unsigned int i=0; i< attributes.size(); i++)
  {
    if(attributes[i].attribute == name)
    {
      return i;
    }
  }
  return -1;
}

std::string ParserUtilities::getValueFor(const AttributeSet& attributes, 
                                         const std::string& name)
{
  int index(ParserUtilities::getIndexOf(attributes, name));
  if(index == -1)
    return std::string("");
  else
    return attributes[(unsigned int)index].value;
}

void ParserUtilities::parseXYIntPair(const AttributeSet& attributes, int& x, int& y)
{
  for(unsigned int i=0; i< attributes.size(); i++)
  {
    int value(ParserUtilities::toInt(attributes[i].value));
    if(attributes[i].attribute == "x")
    {
      x = value;
    }
    else
    {
      y = value;
    }
  }
}

void ParserUtilities::parseXYDoublePair(const AttributeSet& attributes, double& x, double& y)
{
  for(unsigned int i=0; i< attributes.size(); i++)
  {
    double value(ParserUtilities::toDouble(attributes[i].value));
    if(attributes[i].attribute == "x")
    {
      x = value;
    }
    else
    {
      y = value;
    }
  }
}

void ParserUtilities::parseRange(const AttributeSet& attributes, double& lowerBoundary, double& upperBoundary)
{
  for(unsigned int i=0; i< attributes.size(); i++)
  {
    double value(ParserUtilities::toDouble(attributes[i].value));
    if(attributes[i].attribute == "near")
    {
      lowerBoundary = value;
    }
    else
    {
      upperBoundary = value;
    }
  }
}
