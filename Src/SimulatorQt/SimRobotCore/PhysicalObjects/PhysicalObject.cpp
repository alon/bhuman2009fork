/**
 * @file PhysicalObject.cpp
 * 
 * Implementation of class PhysicalObject
 *
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 */ 

#include "PhysicalObject.h"
#include <Simulation/Simulation.h>
#include <Tools/ODETools.h>


PhysicalObject::PhysicalObject() :  surface(0), shaderProgram(0), invisible(false), inverted(false), body(0), geometry(0), mass(0), pWorldObject(0),
                                   transformedGeometry(false), compoundPhysicalObject(0),
                                   physicsDisabled(false), material(""), materialIndex(-1),
                                   collideBit(true), rollingFrictionFlag(false), cycle_order_offset(-1)
{
}

void PhysicalObject::replaceSurface(Surface* newSurface)
{
  setSurface(newSurface);
  SimObject::replaceSurface(newSurface);
}

void PhysicalObject::replaceSurface(Surface* oldSurface, Surface* newSurface)
{
  if(surface == oldSurface)
  {
    setSurface(newSurface);
  }
  SimObject::replaceSurface(oldSurface, newSurface);
}

void PhysicalObject::replaceShaderProgram(ShaderProgram* newShaderProgram)
{
  setShaderProgram(newShaderProgram);
  SimObject::replaceShaderProgram(newShaderProgram);
}

void PhysicalObject::replaceShaderProgram(ShaderProgram* oldShaderProgram, ShaderProgram* newShaderProgram)
{
  if(shaderProgram == oldShaderProgram)
  {
    setShaderProgram(newShaderProgram);
  }
  SimObject::replaceShaderProgram(oldShaderProgram, newShaderProgram);
}

void PhysicalObject::setBody(dBodyID body)
{
  this->body = body;
}

void PhysicalObject::setGeometry(dGeomID geometry)
{
  this->geometry = geometry;
}

void PhysicalObject::writeBackPhysicalResult(const bool& store_last_results)
{
  if(isMovable())
  {
    if(transformedGeometry)
    {
      const dReal* geomPos = dGeomGetPosition(geometry);
      const dReal* bodyPos = dBodyGetPosition(body);
      const dReal* geomRot = dGeomGetRotation(geometry);
      const dReal* bodyRot = dBodyGetRotation(body);
      Matrix3d bodyR, geomR;
      ODETools::convertMatrix(bodyRot, bodyR);
      ODETools::convertMatrix(geomRot, geomR);

      Vector3d temp(bodyPos[0], bodyPos[1], bodyPos[2]);
      Vector3d temp2(geomPos[0], geomPos[1], geomPos[2]);
      temp2.rotate(bodyR);
      temp += temp2;
      setPosition(temp);
      setRotation(bodyR*geomR);
    }
    else
    {
      //position
      const dReal* bodyPos = dBodyGetPosition(body);
      setPosition(Vector3d(bodyPos[0], bodyPos[1], bodyPos[2]));
      //rotation
      const dReal* bodyRot = dBodyGetRotation(body);
      Matrix3d rot;
      ODETools::convertMatrix(bodyRot, rot);
      setRotation(rot);
    }
  }

  if(store_last_results)
  {
    cycle_order_offset = (++cycle_order_offset)%12;
    previous_positions[cycle_order_offset] = position;
    previous_rotations[cycle_order_offset] = rotation;
  }
}

void PhysicalObject::copyStandardMembers(const SimObject* other)
{
  SimObject::copyStandardMembers(other);
  PhysicalObject* otherPhys = (PhysicalObject*)other;
  pWorldObject = otherPhys->pWorldObject;
  pCollisionSpace = otherPhys->pCollisionSpace;
  surface = otherPhys->surface;
  shaderProgram = otherPhys->shaderProgram;
  invisible = otherPhys->invisible;
  inverted = otherPhys->inverted;
  mass = otherPhys->mass;
  material = otherPhys->material;
  materialIndex = otherPhys->materialIndex;
  collideBit = otherPhys->collideBit;
  cycle_order_offset = otherPhys->cycle_order_offset;
}

void PhysicalObject::parseMass(const AttributeSet& attributes)
{
  setMass(ParserUtilities::toDouble(attributes[0].value));
}

void PhysicalObject::parseFriction(const AttributeSet& attributes)
{
  //Friction is not implemented yet
}

void PhysicalObject::parseCenterOfMass(const AttributeSet& attributes)
{
  //Center of Mass is not implemented yet
}

void PhysicalObject::parseMaterial(const AttributeSet& attributes,
                                     int line, int column, 
                                     ErrorManager* errorManager)
{
  for(unsigned int i=0; i < attributes.size(); i++)
  {
    if(attributes[i].attribute == "name")
    {
      int matIndex = simulation->getMaterialIndex(attributes[i].value);
      if(matIndex != -1)
      {
        this->material = attributes[i].value;
        this->materialIndex = matIndex;
      }
      else
      {
          errorManager->addError("Unknown Material",
                                  attributes[i].value+" is not a valid material attribute.",
                                  line, column);
      }
    }
  }
}

void PhysicalObject::parseAppearance(const AttributeSet& attributes,
                                     int line, int column, 
                                     ErrorManager* errorManager)
{
  for(unsigned int i=0; i< attributes.size(); i++)
  {
    if(attributes[i].attribute == "ref")
    {
      Surface* surface = simulation->getSurface(attributes[i].value);
      if(surface)
      {
        setSurface(surface);
      }
      else
      {
          errorManager->addError("Unknown Surface",
                                  attributes[i].value+" is not a valid surface element.",
                                  line, column);
      }
    }
    else //attributes[i].attribute == "visible-to-user"
    {
      invisible = !(ParserUtilities::toBool(attributes[i].value));
    }
  }
}

void PhysicalObject::parseShaderProgram(const AttributeSet& attributes,
                                        int line, int column, 
                                        ErrorManager* errorManager)
{
  for(unsigned int i=0; i< attributes.size(); i++)
  {
    if(attributes[i].attribute == "ref")
    {
      ShaderProgram* shader = simulation->getShaderProgram(attributes[i].value);
      if(shader)
      {
        setShaderProgram(shader);
      }
      else
      {
          errorManager->addError("Unknown Shader Program",
                                  attributes[i].value+" is not a valid shader program.",
                                  line, column);
      }
    }
  }
}

bool PhysicalObject::parseAttributeElements(const std::string& name, 
                                            const AttributeSet& attributes,
                                            int line, int column, 
                                            ErrorManager* errorManager)
{
  if(SimObject::parseAttributeElements(name,attributes,line,column, errorManager))
  {
    return true;
  }
  else if(name == "Appearance")
  {
    parseAppearance(attributes, line, column, errorManager);
  }
  else if(name == "Shader")
  {
    parseShaderProgram(attributes, line, column, errorManager);
  }
  else if(name == "Mass")
  {
    parseMass(attributes);
  }
  else if(name == "Friction")
  {
    parseFriction(attributes);
  }
  else if(name == "CenterOfMass")
  {
    parseCenterOfMass(attributes);
  }
  else if(name == "Material")
  {
    parseMaterial(attributes, line, column, errorManager);
  }
  else if(name == "PhysicalAttributes")
  {
    // This element is ignored, it is just used to structure
    // the description in XML
  }
  else
  {
    return false;
  }
  return true;
}

void PhysicalObject::postProcessAfterParsing()
{
  if(surface == 0)
  {
    surface = simulation->getDefaultSurface();
  }
}

void PhysicalObject::invertColors()
{
  inverted = (!inverted);
  SimObject::invertColors();
}

void PhysicalObject::rotateBody(dMatrix3 rotationMatrix)
{
  dBodySetRotation(body, rotationMatrix);
}

void PhysicalObject::disablePhysics(bool disableGeometry)
{
  if(!physicsDisabled)
  {
    if(geometry && disableGeometry)
    {
      if(dGeomIsEnabled(geometry))
        dGeomDisable(geometry);
    }
    if(body)
    {
      if(dBodyIsEnabled(body))
        dBodyDisable(body);
    }

    physicsDisabled = true;
  }
}

void PhysicalObject::enablePhysics()
{
  if(physicsDisabled)
  {
    if(geometry)
    {
      if(!dGeomIsEnabled(geometry))
        dGeomEnable(geometry);
    }
    if(body)
    {
      if(!dBodyIsEnabled(body))
        dBodyEnable(body);
    }

    physicsDisabled = false;
  }
}

void PhysicalObject::setCollideBitfield()
{
  if(geometry)
  {
    if(collideBit)
      dGeomSetCollideBits(geometry, 1);
    else
      dGeomSetCollideBits(geometry, 0);
      //dGeomDisable(geometry);
  }
}

void PhysicalObject::translateDnD(const Vector3d& translation)
{
  if(transformedGeometry)
  {
    if(!compoundPhysicalObject->isSynchronized())
    {
      compoundPhysicalObject->translateDnD(translation, false);
    }
    SimObject::translateDnD(translation);
  }
  else
  {
    position+=translation;

    if(geometry)
    {
      //position
      dGeomSetPosition(geometry, dReal(position.v[0]), dReal(position.v[1]), dReal(position.v[2]));
      if(body)
      {
        dBodySetPosition(body, dReal(position.v[0]), dReal(position.v[1]), dReal(position.v[2]));
      }
    }

    ObjectList::const_iterator pos;
    for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
    {
      (*pos)->translateDnD(translation);
    }
  }
}

void PhysicalObject::rotateDnD(const Matrix3d& rot, const Vector3d& origin)
{
  SimObject::rotateDnD(rot, origin);
  if(transformedGeometry)
  {
    if(!compoundPhysicalObject->isSynchronized())
      compoundPhysicalObject->rotateDnD(rot, origin, false);
  }
  else
  {
    if(geometry)
    {
      //position
      dGeomSetPosition(geometry, dReal(position.v[0]), dReal(position.v[1]), dReal(position.v[2]));
      //rotation
      dMatrix3 rotationMatrix;
      ODETools::convertMatrix(rotation, rotationMatrix);  
      dGeomSetRotation(geometry, rotationMatrix);
      if(body)
      {
        //position
        dBodySetPosition(body, dReal(position.v[0]), dReal(position.v[1]), dReal(position.v[2]));
        dBodySetRotation(body, rotationMatrix);
      }
    }
  }
}
