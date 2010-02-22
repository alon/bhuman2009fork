/**
 * @file Box.cpp
 * 
 * Implementation of class Box
 *
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 */ 

#include <Tools/SimGraphics.h>
#include <Tools/ODETools.h>
#include <Tools/SimGeometry.h>
#include <Simulation/Simulation.h>
#include "Box.h"


Box::Box(const AttributeSet& attributes)
{
  for(unsigned int i=0; i<attributes.size(); i++)
  {
    std::string attributeName(attributes[i].attribute);
    if(attributeName == "name")
    {
      name = attributes[i].value;
    }
    else if(attributeName == "length")
    {
      length = ParserUtilities::toDouble(attributes[i].value);
    }
    else if(attributeName == "height")
    {
      height = ParserUtilities::toDouble(attributes[i].value);
    }
    else if(attributeName == "width")
    {
      width = ParserUtilities::toDouble(attributes[i].value);
    }
    else if(attributeName == "canCollide")
    {
      collideBit = ParserUtilities::toBool(attributes[i].value);
    }
  }
  createGraphics();
}


void Box::drawObject(const VisualizationParameterSet& visParams) 
{
  if((invisible && !visParams.drawForSensor) || visParams.mb_skip == mbMethod)
    return;

  if(shaderProgram)
    graphicsManager->shader->enableLocalShader(this);
  else if(visParams.provideUniforms)
    graphicsManager->shader->handleUniforms(this);

  graphicsManager->setMBInfos(previous_positions, previous_rotations, cycle_order_offset);
  graphicsManager->drawPrimitive(graphicsHandle, position, rotation, *surface,
                                 inverted, visParams.drawForSensor, visParams);

  if(shaderProgram)
    graphicsManager->shader->disableLocalShader();
}


void Box::quickDrawObject()  
{
  if(!invisible)
  {
    graphicsManager->quickDrawPrimitive(graphicsHandle, position, rotation, *surface);
  }
}


SimObject* Box::clone() const
{
  Box* newBox = new Box();
  newBox->copyStandardMembers(this);
  bool createsNewMovID = newBox->adjustMovableID();
  newBox->length = length;
  newBox->width = width;
  newBox->height = height;
  newBox->graphicsHandle = graphicsHandle;
  memcpy(&(newBox->corners), &corners, sizeof corners);

  std::list<SimObject*>::const_iterator pos;
  for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
  {
    SimObject* childNode = (*pos)->clone();
    newBox->addChildNode(childNode, false);
  }

  if(createsNewMovID)
    simulation->removeCurrentMovableID();

  SimObject* newObject = newBox;
  return newObject;
}


bool Box::intersectWithRay(const Vector3d& pos, const Vector3d& ray, Vector3d& intersectPos)
{
  Vector3d boxIntersectionPos;
  if(SimGeometry::intersectRayAndBox(pos, ray, position, rotation, length, width, height, boxIntersectionPos))
  {
    intersectPos = boxIntersectionPos;
    return true;
  }
  return false;
}


double Box::getMaxDistanceTo(const Vector3d& base) const
{
  double maxQuadDist(0.0);
  Vector3d transBase(base-position);
  //Test all eight corners:
  for(int i=0; i<8; i++)
  {
    double currentQuadDist((corners[i] - transBase).getQuadLength());  
    if(currentQuadDist > maxQuadDist)
    {
      maxQuadDist = currentQuadDist;
    }
  }
  return sqrt(maxQuadDist);
}


void Box::createPhysics()
{
  //create physic engine stuff
  setPointerToPhysicalWorld(simulation->getPhysicalWorld());
  setPointerToCollisionSpace(simulation->getCurrentSpace());
  //create geometry
  if(movableID)
  {
    createGeometry(*simulation->getMovableSpace());
    createBody();
    dGeomSetBody(geometry, body);     //connect body and geometry
  }
  else
  {
    createGeometry(*simulation->getStaticSpace());
  }
}


void Box::createBody()
{
  if(body == 0)
  {
    body = dBodyCreate(*this->simulation->getPhysicalWorld());
    dMass m;
    dMassSetBox(&m, 1, dReal(length), dReal(width), dReal(height));
    dMassAdjust(&m, dReal(mass));
    dBodySetMass(body, &m);
    dBodySetPosition(body, dReal(position.v[0]), dReal(position.v[1]), dReal(position.v[2]));
    dMatrix3 rotationMatrix;
    ODETools::convertMatrix(rotation, rotationMatrix);
    dBodySetRotation(body, rotationMatrix);
  }
}


void Box::createGeometry(dSpaceID space)
{
  if(geometry == 0)
  {
    setGeometry(dCreateBox(space, dReal(length), dReal(width), dReal(height)));
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


void Box::computeMassProperties(dMass& m)
{
  dMassSetBox(&m, 1, dReal(length), dReal(width), dReal(height));
  dMassAdjust(&m, dReal(mass));
}


void Box::setDimensions(double newLength, double newWidth, double newHeight)
{
  length = newLength; 
  width = newWidth; 
  height = newHeight;
  calculateCorners();
}


void Box::calculateCorners()
{
  //Set corner points:
  double length_2(length * 0.5);
  double width_2(width * 0.5);
  double height_2(height * 0.5);
  corners[0] = Vector3d(-length_2, -width_2, height_2);
  corners[1] = Vector3d(length_2, -width_2, height_2);
  corners[2] = Vector3d(length_2, width_2, height_2);
  corners[3] = Vector3d(-length_2, width_2, height_2);
  corners[4] = Vector3d(-length_2, width_2, -height_2);
  corners[5] = Vector3d(length_2, width_2, -height_2);
  corners[6] = Vector3d(length_2, -width_2, -height_2);
  corners[7] = Vector3d(-length_2, -width_2, -height_2);
}


void Box::updateLocalBoundingBox()
{
  boundingBox.mini = position;
  boundingBox.maxi = position;
  for(int i=0; i<8; i++)
  {
    Vector3d p(corners[i]);
    p.rotate(rotation);
    p += position;
    boundingBox.expand(p);
  }
}


void Box::createGraphics()
{
  calculateCorners();
  graphicsHandle = graphicsManager->createNewBox(corners);
}
