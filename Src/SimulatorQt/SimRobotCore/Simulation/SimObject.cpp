/**
 * @file SimObject.cpp
 * 
 * Implementation of class SimObject
 *
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 */ 


#include "SimObject.h"
#include "Simulation.h"
#include <Tools/SimGeometry.h>
#include <OpenGL/GLHelper.h>
#include <Controller/Controller3DDrawing.h>


SimObject* SimObject::rootNode = 0;
Simulation* SimObject::simulation = 0;
GraphicsManager* SimObject::graphicsManager = 0;


SimObject::SimObject():name("(no name)"), fullName("(no name)"), boundingBox(position)
{
  parentNode = 0;
  movableID = 0;
  boundLight = false;
  mbMethod = MOTIONBLUR_VELOCITY_BUFFER_2;
}

SimObject::~SimObject()
{
  ObjectList::const_iterator pos;
  for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
  {
    delete *pos;
  }
  childNodes.clear();
  controllerDrawings.clear();
}

unsigned int SimObject::getNumOfPrimitives() const
{
  ObjectList::const_iterator pos;
  unsigned int numPrim = getNumPrim();
  for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
  {
    numPrim += (*pos)->getNumOfPrimitives();
  }
  return numPrim;
}

unsigned int SimObject::getNumOfVertices() const
{
  ObjectList::const_iterator pos;
  unsigned int numVert = getNumVert();
  for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
  {
    numVert += (*pos)->getNumOfVertices();
  }
  return numVert;
}

void SimObject::draw(const VisualizationParameterSet& visParams)  
{
  //Draw the object itself:
  drawObject(visParams);
  //Check for controller drawings connected to this object:
  drawControllerDrawings(visParams);
  //Draw child nodes:
  ObjectList::const_iterator pos;
  for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
  {
    (*pos)->draw(visParams);
  }  
}

void SimObject::draw(const VisualizationParameterSet& visParams, bool skipFirstChild)  
{
  //Draw the object itself:
  drawObject(visParams);
  //Check for controller drawings connected to this object:
  drawControllerDrawings(visParams);
  //Draw child nodes:
  ObjectList::const_iterator pos = childNodes.begin();
  for(pos != childNodes.end() && skipFirstChild ? pos++ : pos; pos != childNodes.end(); ++pos)
  {
    (*pos)->draw(visParams);
  }  
}

void SimObject::quickDraw()
{
  // Draw the object really simple
  quickDrawObject();
  //Draw child nodes:
  ObjectList::const_iterator pos;
  for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
  {
    (*pos)->quickDraw();
  }
}

inline void SimObject::drawControllerDrawings(const VisualizationParameterSet& visParams)  
{
  if(visParams.enableControllerDrawings && controllerDrawings.size() != 0)
  {
    std::list<Controller3DDrawing*>::iterator dPos;
    for(dPos = controllerDrawings.begin(); dPos != controllerDrawings.end(); ++dPos)
    {
      glPushMatrix();
      GLHelper::getGLH()->setMatrix(position, rotation);
      (*dPos)->draw();
      glPopMatrix();
    }  
  }
}

void SimObject::attachDrawing(Controller3DDrawing* drawing)
{
  controllerDrawings.push_back(drawing);
}

void SimObject::removeDrawing(Controller3DDrawing* drawing)
{
  controllerDrawings.remove(drawing);
}

void SimObject::setPosition(const Vector3d& pos)
{
  Vector3d translation(pos);
  translation -= position;
  position = pos;
  ObjectList::const_iterator listPos;
  for(listPos = childNodes.begin(); listPos != childNodes.end(); ++listPos)
  {
    (*listPos)->translate(translation);
  }
}

void SimObject::translate(const Vector3d& translation)
{
  position+=translation;
  ObjectList::const_iterator pos;
  for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
  {
    (*pos)->translate(translation);
  }
}

void SimObject::setRotation(const Matrix3d& rot)
{
  Matrix3d childRotation(this->rotation);
  childRotation.transpose();
  childRotation = rot * childRotation;
  rotation = rot;
  ObjectList::const_iterator listPos;
  for(listPos = childNodes.begin(); listPos != childNodes.end(); ++listPos)
  {
    (*listPos)->rotate(childRotation, position);
  }
}

void SimObject::rotate(const Vector3d& angles, const Vector3d& origin)
{
  Matrix3d matrix(angles);
  rotate(matrix, origin);
}

void SimObject::rotate(const Matrix3d& rotation, 
                       const Vector3d& origin)
{
  Vector3d v = position - origin;
  v.rotate(rotation);
  position = origin + v;
  Matrix3d dummy = rotation;
  this->rotation = dummy * (this->rotation);
  ObjectList::const_iterator pos;
  for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
  {
    (*pos)->rotate(rotation, origin);
  }
}

void SimObject::rotateAroundAxis(double angle, Vector3d axis)
{
  Matrix3d m;
  axis.normalize();
  m.setRotationAroundAxis(axis, angle);
  rotate(m, position);
}

void SimObject::addChildNode(SimObject* child, bool move)
{
  childNodes.push_back(child);
  child->setParentNode(this);
  if(move)
  {
    child->translate(position);
    child->rotate(rotation, position);
  }
}

SimObject* SimObject::getChildNode(int num) const
{
  if((unsigned int)num >= childNodes.size() || num < 0)
  {
    return 0;
  }
  else
  {
    ObjectList::const_iterator pos;
    int counter = 0;
    for(pos = childNodes.begin(); counter < num; ++counter)
    {
      ++pos;
    }
    return (*pos);
  }
}

void SimObject::findObject(SimObject*& object, const std::string& searchName, bool useFullName) 
{
  object = 0;
  if(useFullName)
  {
    //The name of this object has to be the first part of the given name
    if(searchName.find(fullName) == 0)
    {
      if(fullName == searchName)
      {
        object = this;
      }
      else
      {
        ObjectList::const_iterator pos;
        for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
        {
          (*pos)->findObject(object, searchName);
          if(object)
          {
            break;
          }
        }
      }
    }
  }
  else
  {
    if(name == searchName)
    {
      object = this;
    }
    else
    {
      ObjectList::const_iterator pos;
      for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
      {
        (*pos)->findObject(object, searchName, false);
        if(object)
        {
          break;
        }
      }
    }
  }
}

void SimObject::findObject(SimObject*& object, const std::vector<std::string>& partsOfName)
{
  std::string::size_type index = 0;
  for(unsigned i = 0; i < partsOfName.size(); ++i)
  {
    index = fullName.find(partsOfName[i], index);
    if(index == std::string::npos)
      break;
    else
      index += partsOfName[i].size();
  }
  if(index != std::string::npos)
  {
    object = this;
  }
  else
  {
    ObjectList::const_iterator pos;
    object = 0;
    for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
    {
      (*pos)->findObject(object, partsOfName);
      if(object)
        return;
    }
  }
}

SimObject* SimObject::clone() const
{
  SimObject* newObject = new SimObject();
  newObject->copyStandardMembers(this);
  ObjectList::const_iterator pos;
  for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
  {
    SimObject* childNode = (*pos)->clone();
    newObject->addChildNode(childNode, false);
  }
  return newObject;
}

void SimObject::copyStandardMembers(const SimObject* other)
{
  setName(other->name);
  setFullName(other->fullName);
  setPosition(other->position);
  rotation = other->rotation;
  parentNode = other->parentNode;
  movableID = other->movableID;
  boundLight = other->boundLight;
  mbMethod = other->mbMethod;
}

void SimObject::addToDescriptions(std::vector<ObjectDescription>& objectDescriptionTree,
                                  int depth) 
{
  ObjectDescription myDesc;
  myDesc.kind = !depth ? "scene" : getKind();
  myDesc.name = name;
  myDesc.fullName = fullName;
  myDesc.depth = depth;
  objectDescriptionTree.push_back(myDesc);
  ObjectList::const_iterator pos;
  for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
  {
    (*pos)->addToDescriptions(objectDescriptionTree, depth + 1);
  }
}

bool SimObject::intersect(std::vector<Intersection*>& intersections, 
                          const Vector3d& pos, const Vector3d& ray) 
{
  bool result = false;
  //Does the ray intersect the bounding box?
  if(SimGeometry::intersectRayAndAxisAlignedBoundingBox(pos, ray, boundingBox))
  {
    Vector3d intersectionPos;
    if(intersectWithRay(pos, ray, intersectionPos))
    {
      result = true;
      Intersection* intersection = new Intersection();
      intersection->object = this;
      intersection->hasPosition = true;
      intersection->position = intersectionPos;
      intersections.push_back(intersection);
    }
    ObjectList::const_iterator child;
    for(child = childNodes.begin(); child != childNodes.end(); ++child)
    {
      if((*child)->intersect(intersections, pos, ray))
      {
        result = true;
      };
    }
  }
  return result;
}

double SimObject::getMaxDistanceTo(const Vector3d& base) const
{
  double dist = 0;
  ObjectList::const_iterator pos;
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

bool SimObject::isMovableOrClickable(SimObject*& object,
                                     bool testClickable)
{
  if(movableID)
  {
    object = this;
    if(parentNode)
    {
      return parentNode->isMovableOrClickable(object,testClickable);
    }
  }
  return object != 0;
}

void SimObject::invertColors()
{
  ObjectList::const_iterator child;
  for(child = childNodes.begin(); child != childNodes.end(); ++child)
    (*child)->invertColors();
}

bool SimObject::parseAttributeElements(const std::string& name, 
                                       const AttributeSet& attributes,
                                       int line, int column, 
                                       ErrorManager* errorManager)
{
  if(name == "Translation")
  {
    position = parentNode->position;
    translate(parentNode->getRotation() * (ParserUtilities::toPoint(attributes)));
  }
  else if(name == "Rotation")
  {
    Vector3d rot(ParserUtilities::toPoint(attributes));
    rot.toRad();
    rotate(rot, position);
  }
  else if(name == "Elements" || name == "Element")
  {
    // These elements are ignored, they are just used to structure
    // the description in XML
  }
  else
  {
    return false;
  }
  return true;
}

bool SimObject::isMovable() const
{
  return (movableID != 0);
}

void SimObject::replaceSurface(Surface* newSurface)
{
  ObjectList::iterator iter(childNodes.begin());
  while(iter != childNodes.end())
    (*(iter++))->replaceSurface(newSurface);
}

void SimObject::replaceSurface(Surface* oldSurface, Surface* newSurface)
{
  ObjectList::iterator iter(childNodes.begin());
  while(iter != childNodes.end())
    (*(iter++))->replaceSurface(oldSurface, newSurface);
}

void SimObject::replaceShaderProgram(ShaderProgram* newShaderProgram)
{
  ObjectList::iterator iter(childNodes.begin());
  while(iter != childNodes.end())
    (*(iter++))->replaceShaderProgram(newShaderProgram);
}

void SimObject::replaceShaderProgram(ShaderProgram* oldShaderProgram, ShaderProgram* newShaderProgram)
{
  ObjectList::iterator iter(childNodes.begin());
  while(iter != childNodes.end())
    (*(iter++))->replaceShaderProgram(oldShaderProgram, newShaderProgram);
}

bool SimObject::adjustMovableID()
{
  bool newMovableIDOnStack = false;
  if(movableID != 0 && simulation->getCurrentMovableID() == 0)
  {
    simulation->createNewMovableID();
    newMovableIDOnStack = true;
  }
  movableID = simulation->getCurrentMovableID();

  return newMovableIDOnStack;
}

void SimObject::changeAbilityToCollide(bool collide)
{
  setCollideBit(collide);

  ObjectList::iterator iter(childNodes.begin());
  while(iter != childNodes.end())
    (*(iter++))->changeAbilityToCollide(collide);
}

void SimObject::translateDnD(const Vector3d& translation)
{
  position+=translation;
  ObjectList::const_iterator pos;
  for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
  {
    (*pos)->translateDnD(translation);
  }
}

void SimObject::rotateDnD(const Matrix3d& rot, const Vector3d& origin)
{
  Vector3d v = position - origin;
  v.rotate(rot);
  position = origin + v;
  Matrix3d dummy = rot;
  this->rotation = dummy * (this->rotation);
  ObjectList::const_iterator pos;
  for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
  {
    (*pos)->rotateDnD(rot, origin);
  }
}

void SimObject::rotateAroundAxisDnD(double angle, Vector3d axis)
{
  Matrix3d m;
  axis.normalize();
  m.setRotationAroundAxis(axis, angle);
  rotateDnD(m, position);
}

void SimObject::updateLocalBoundingBox()
{
  boundingBox.maxi = position;
  boundingBox.mini = position;
}

void SimObject::expandAxisAlignedBoundingBox(AxisAlignedBoundingBox& box)
{
  updateLocalBoundingBox();
  ObjectList::iterator pos;
  for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
  {
    (*pos)->expandAxisAlignedBoundingBox(boundingBox);
  }
  box.expand(boundingBox);
}

void SimObject::setForcedMotionBlurMethod(const MotionBlurMode method)
{
  mbMethod = method;
  ObjectList::iterator pos;
  for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
  {
    (*pos)->setForcedMotionBlurMethod(method);
  }
}
