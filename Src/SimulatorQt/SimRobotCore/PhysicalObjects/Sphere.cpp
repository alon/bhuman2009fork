/**
 * @file Sphere.cpp
 * 
 * Implementation of class Sphere
 *
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 */ 

#include <Tools/Surface.h>
#include <Tools/ODETools.h>
#include <Tools/SimGraphics.h>
#include <Tools/SimGeometry.h>
#include <Simulation/Simulation.h>
#include <Simulation/PhysicsParameterSet.h>
#include "Sphere.h"


Sphere::Sphere(const AttributeSet& attributes):radius(0), lastAngularVelocity(0,0,0)
{
  for(unsigned int i=0; i<attributes.size(); i++)
  {
    std::string attributeName(attributes[i].attribute);
    if(attributeName == "name")
    {
      name = attributes[i].value;
    }
    else if(attributeName == "radius")
    {
      radius = ParserUtilities::toDouble(attributes[i].value);
    }
    else if(attributeName == "canCollide")
    {
      collideBit = ParserUtilities::toBool(attributes[i].value);
    }
  }
  graphicsHandle = graphicsManager->createNewSphere(radius);
}


void Sphere::drawObject(const VisualizationParameterSet& visParams) 
{
  if((invisible && !visParams.drawForSensor) || visParams.mb_skip == mbMethod)
    return;

  if(shaderProgram)
    graphicsManager->shader->enableLocalShader(this);
  else if(visParams.provideUniforms)
    graphicsManager->shader->handleUniforms(this);

  graphicsManager->setMBInfos(previous_positions, previous_rotations, cycle_order_offset);
  graphicsManager->drawPrimitive(graphicsHandle,position, rotation, *surface,
                                 inverted, visParams.drawForSensor, visParams);

  if(shaderProgram)
    graphicsManager->shader->disableLocalShader();
}


void Sphere::quickDrawObject() 
{
  if(!invisible)
  {
    graphicsManager->quickDrawPrimitive(graphicsHandle,position, rotation, *surface);
  }
}


SimObject* Sphere::clone() const
{
  Sphere* newSphere = new Sphere();
  newSphere->copyStandardMembers(this);
  bool createsNewMovID = newSphere->adjustMovableID();
  newSphere->radius = radius;
  newSphere->graphicsHandle = graphicsHandle;

  std::list<SimObject*>::const_iterator pos;
  for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
  {
    SimObject* childNode = (*pos)->clone();
    newSphere->addChildNode(childNode, false);
  }

  if(createsNewMovID)
    simulation->removeCurrentMovableID();

  SimObject* newObject = newSphere;
  return newObject;
}


double Sphere::getMaxDistanceTo(const Vector3d& base) const
{
  double dist(0.0);
  Vector3d vec(base - position);
  double vecLen(vec.getLength());
  if(vecLen < radius)
  {
    dist = vecLen + radius;
  }
  else
  {
    dist = vecLen + 2.0*radius;
  }
  std::list<SimObject*>::const_iterator pos;
  for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
  {
    double childDist = (*pos)->getMaxDistanceTo(base);
    if(dist < childDist)
    {
      dist = childDist;
    }
  }
  return dist;
}


bool Sphere::intersectWithRay(const Vector3d& pos, const Vector3d& ray, Vector3d& intersectPos) 
{
  return SimGeometry::intersectRayAndSphere(pos, ray, position, radius, intersectPos);
}


void Sphere::updateLocalBoundingBox()
{
  boundingBox.mini = position;
  boundingBox.maxi = position;
  boundingBox.expandMaxX(position.v[0]+radius);
  boundingBox.expandMinX(position.v[0]-radius);
  boundingBox.expandMaxY(position.v[1]+radius);
  boundingBox.expandMinY(position.v[1]-radius);
  boundingBox.expandMaxZ(position.v[2]+radius);
  boundingBox.expandMinZ(position.v[2]-radius);
}


void Sphere::createPhysics()
{
  //create physic engine stuff
  setPointerToPhysicalWorld(simulation->getPhysicalWorld());
  setPointerToCollisionSpace(simulation->getCurrentSpace());
  //create geometry
  if(movableID)
  {
    createGeometry(*simulation->getMovableSpace());
    createBody();
    dGeomSetBody(geometry, body); //connect body and geometry
  }
  else
  {
    createGeometry(*simulation->getStaticSpace());
  }
}


void Sphere::createBody()
{
  if(body == 0)
  {
    body = dBodyCreate(*simulation->getPhysicalWorld());
    dMass m;
    dMassSetSphere(&m, 1, dReal(radius));
    dMassAdjust(&m, dReal(mass));
    dBodySetMass(body, &m);
    dBodySetPosition(body, dReal(position.v[0]), dReal(position.v[1]), dReal(position.v[2]));
    dMatrix3 rotationMatrix;
    ODETools::convertMatrix(rotation, rotationMatrix);
    dBodySetRotation(body, rotationMatrix);
  }
}


void Sphere::createGeometry(dSpaceID space)
{
  if(geometry == 0)
  {
    setGeometry(dCreateSphere(space, dReal(radius)));
    dGeomSetPosition(geometry, dReal(position.v[0]), dReal(position.v[1]), dReal(position.v[2]));
    dMatrix3 rotationMatrix;
    ODETools::convertMatrix(rotation, rotationMatrix);
    dGeomSetRotation(geometry, rotationMatrix);
    //set user data pointer of ODE geometry object to this physical object
    dGeomSetData(geometry, this);
    //set collide bitfield
    PhysicalObject::setCollideBitfield();
  }
}


void Sphere::computeMassProperties(dMass& m)
{
  dMassSetSphere(&m, 1, dReal(radius));
  dMassAdjust(&m, dReal(mass));
}


void Sphere::determineRollingFrictionFlag()
{
  bool result = false;
  if(movableID > 0)
  {
    if(compoundPhysicalObject->isSimple())
    {
      result = true;
      SimObject* parent = getParentNode();
      while(parent != simulation->getObjectTreeRoot() && result == true)
      {
        if(parent->getKind() != "compound")
        {
          result = false;
          return;
        }
        parent  = parent->getParentNode();
      }
    }
  }
  rollingFrictionFlag = result;
}


void Sphere::applySimpleRollingFriction(double rfc)
{
  const dReal* curAngVel = dBodyGetAngularVel(body);
  Vector3d angularVel(curAngVel[0], curAngVel[1], curAngVel[2]);
  if(angularVel.getLength() < 0.001)
    dBodySetAngularVel(body, 0.0, 0.0, 0.0);
  else
    dBodyAddTorque(body, (curAngVel[0] * (dReal)(-rfc)), (curAngVel[1] * (dReal)(-rfc)), (curAngVel[2] * (dReal)(-rfc)));
}


void Sphere::applyRollingFriction(double cosAlpha, double rfc)
{
}
