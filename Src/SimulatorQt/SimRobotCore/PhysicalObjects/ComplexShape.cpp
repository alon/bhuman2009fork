/**
* @file ComplexShape.cpp
* 
* Implementation of class ComplexShape
*
* @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
* @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
* @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
*/ 

#include <Tools/ODETools.h>
#include <Tools/Surface.h>
#include <Tools/SimGraphics.h>
#include <Simulation/Simulation.h>
#include "ComplexShape.h"

#include "Box.h"

#include "gsl/gsl_math.h"
#include "gsl/gsl_eigen.h"

#if defined(__cplusplus)
extern "C"
{
#endif
#include <stdio.h>
#include <stdlib.h>
#include <qhull.h>
#include <mem.h>
#include <qset.h>
#include <geom.h>
#include <merge.h>
#include <poly.h>
#include <io.h>
#include <stat.h>

#if defined(__cplusplus)
}
#endif


ComplexShape::ComplexShape() : physicsVertices(0), physicsIndices(0), physVertexCount(0), physIndexCount(0), 
physicsNormals(0), physNormalsCount(0), parsingPhysicalRepresentation(false), 
useTriangleMesh(false), useOBB(false), useABB(false), averageNormals(false), flipNormal(false),
combinePrimitives(true)
{
}


ComplexShape::ComplexShape(const AttributeSet& attributes) : physicsVertices(0), physicsIndices(0), 
physVertexCount(0), physIndexCount(0), 
physicsNormals(0), physNormalsCount(0), 
parsingPhysicalRepresentation(false), 
useTriangleMesh(false), useOBB(false), useABB(false), 
averageNormals(false), flipNormal(false), combinePrimitives(true)
{
  for(unsigned int i=0; i<attributes.size(); i++)
  {
    std::string attributeName(attributes[i].attribute);
    if(attributeName == "name")
    {
      name = attributes[i].value;
    }
    else if(attributeName == "canCollide")
    {
      collideBit = ParserUtilities::toBool(attributes[i].value);
    }
  }
}


ComplexShape::~ComplexShape()
{
  std::vector<PhysicalObject*>::iterator iter;
  for(iter = physicalRepresentation.begin(); iter != physicalRepresentation.end(); ++iter)
    delete (*iter);

  if(physicsVertices)
    delete[] physicsVertices;
  if(physicsIndices)
    delete[] physicsIndices;
  if(physicsNormals)
    delete[] physicsNormals;
}

unsigned int ComplexShape::getNumPrim() const
{
  return graphicsPrimitives.size();
}

unsigned int ComplexShape::getNumVert() const
{
  std::list<GraphicsPrimitiveHandle>::const_iterator iter;
  unsigned int numVert = 0;
  for(iter = graphicsPrimitives.begin(); iter != graphicsPrimitives.end(); iter++)
  {
    numVert += graphicsManager->getNumVertForCS(*iter);
  }
  return numVert;
}

void ComplexShape::finishShape()
{
  if(flipNormal)
  {
    graphicsManager->flipVertexNormals();
    graphicsManager->flipVertexOrder();
  }
  if(averageNormals)
    graphicsManager->calculateVertexNormals();
  if(combinePrimitives)
    graphicsManager->combinePrePrimitives();
  while(graphicsManager->hasPrePrimitives())
  {
    graphicsPrimitives.push_back(graphicsManager->convertPrePrimitive(averageNormals));
  }
  graphicsManager->clearVertexNormals();
}

bool ComplexShape::parseAttributeElements(const std::string& name, 
                                          const AttributeSet& attributes,
                                          int line, int column, 
                                          ErrorManager* errorManager)
{
  if(name == "Vertex")
  {
    graphicsManager->addVertexToCurrentPrePrimitive(ParserUtilities::getValueFor(attributes,"ref"));
  }
  else if(name == "Polygon")
  {
    graphicsManager->createNewPrePrimitive(POLYGON);
  }
  else if(name == "TriangleStrip")
  {
    graphicsManager->createNewPrePrimitive(TRIANGLE_STRIP);
  }
  else if(name == "Quad")
  {
    graphicsManager->createNewQuadPrePrimitive(attributes, false);
  }
  else if(name == "fQuad")
  {
    graphicsManager->createNewQuadPrePrimitive(attributes, true);
  }
  else if(name == "Triangle")
  {
    graphicsManager->createNewTrianglePrePrimitive(attributes, false);
  }
  else if(name == "fTriangle")
  {
    graphicsManager->createNewTrianglePrePrimitive(attributes, true);
  }
  else if(PhysicalObject::parseAttributeElements(name,attributes,line,column,errorManager))
  {
    return true;
  }
  else if(name == "PhysicalRepresentation")
  {
    this->parsingPhysicalRepresentation = true;
    // This element is ignored, it is just used to structure
    // the description in XML
  }
  else if(name == "GraphicalRepresentation")
  {
    std::string vertexListName(ParserUtilities::getValueFor(attributes,"vertexList"));
    graphicsManager->setActiveVertexList(vertexListName);
  }
  else if(name == "GraphicalAttributes")
  {
    // This element is ignored, it is just used to structure
    // the description in XML
  }
  else if(name == "SmoothSurface")
  {
    averageNormals = ParserUtilities::toBool(attributes[0].value);
  }
  else if(name == "FlipNormal")
  {
    flipNormal = ParserUtilities::toBool(attributes[0].value);
  }
  else if(name == "CombinePrimitives")
  {
    combinePrimitives = ParserUtilities::toBool(attributes[0].value);
  }
  else if(name == "TriangleMesh")
  {
    useTriangleMesh = true;
  }
  else if(name == "ObjectOrientedBoundingBox")
  {
    useOBB = true;
    Box* obb = new Box();
    obb->setDimensions(1.0, 1.0, 1.0);
    obb->setName("OBB");
    obb->setMass(ParserUtilities::toDouble(ParserUtilities::getValueFor(attributes, "mass")));
    this->physicalRepresentation.push_back(obb);
  }
  else if(name == "AxisAlignedBoundingBox")
  {
    useABB = true;
    Box* abb = new Box();
    abb->setDimensions(1.0, 1.0, 1.0);
    abb->setName("ABB");
    abb->setMass(ParserUtilities::toDouble(ParserUtilities::getValueFor(attributes, "mass")));
    this->physicalRepresentation.push_back(abb);
  }
  else 
  {
    return false;
  }
  return true;
}


void ComplexShape::finishedParsingCurrentElement(const std::string& name)
{
  if(name[0] == 'V')
  {
    return;
  }
  else if(name == "Polygon" || name == "TriangleStrip")
  {
    graphicsManager->currentPrePrimitiveHasBeenFinished();
  }
  else if(name == "GraphicalRepresentation")
  {
    finishShape();
  }
  else if(name == "PhysicalRepresentation")
  {
    this->parsingPhysicalRepresentation = false;
  }
}


void ComplexShape::drawObject(const VisualizationParameterSet& visParams) 
{  
  if((invisible && !visParams.drawForSensor) || visParams.mb_skip == mbMethod)
    return;

  if(shaderProgram)
    graphicsManager->shader->enableLocalShader(this);
  else if(visParams.provideUniforms)
    graphicsManager->shader->handleUniforms(this);

  graphicsManager->setMBInfos(previous_positions, previous_rotations, cycle_order_offset);

  //Draw appearance of object:
  if(visParams.surfaceStyle == VisualizationParameterSet::WIREFRAME ||
    visParams.surfaceStyle == VisualizationParameterSet::FLAT_SHADING ||
    visParams.surfaceStyle == VisualizationParameterSet::SMOOTH_SHADING || 
    visParams.surfaceStyle == VisualizationParameterSet::TEXTURE_SHADING ||
    visParams.surfaceStyle == VisualizationParameterSet::CAMERA_SIMULATION ||
    useTriangleMesh)
  {
    graphicsManager->drawPrimitives(graphicsPrimitives,position, rotation, *surface,
      inverted, visParams.drawForSensor, visParams);
  }
  //Draw physical representation of object:
  else if(visParams.surfaceStyle == VisualizationParameterSet::PHYSICS_WIREFRAME ||
    visParams.surfaceStyle == VisualizationParameterSet::PHYSICS_FLAT ||
    visParams.surfaceStyle == VisualizationParameterSet::PHYSICS_SMOOTH)
  {
    std::vector<PhysicalObject*>::iterator iter;
    for(iter = physicalRepresentation.begin(); iter != physicalRepresentation.end(); ++iter)
      (*iter)->draw(visParams);
  }
  //Draw object in mixed mode:
  else
  {
    if(visParams.surfaceStyle == VisualizationParameterSet::MIXED_APPEARANCE)
    {
      //Draw appearance:
      graphicsManager->drawPrimitives(graphicsPrimitives,position, rotation, *surface,
        inverted, visParams.drawForSensor, visParams);
      //Draw physics:
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      glShadeModel(GL_FLAT);
      GLfloat colorBackup[] = {surface->color[0], surface->color[1],
        surface->color[2], surface->color[3]};
      surface->setColor(1.0, 1.0, 1.0, 1.0);
      std::vector<PhysicalObject*>::iterator iter;
      for(iter = physicalRepresentation.begin(); iter != physicalRepresentation.end(); ++iter)
        (*iter)->draw(visParams);
      surface->setColorv(colorBackup);
      //Reset old state:
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      glShadeModel(GL_SMOOTH);
    }
    else //(visParams.surfaceStyle == VisualizationParameterSet::MIXED_PHYSICS)
    {
      //Draw appearance:
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      glShadeModel(GL_FLAT);
      GLfloat colorBackup[] = {surface->color[0], surface->color[1],
        surface->color[2], surface->color[3]};
      surface->setColor(1.0, 1.0, 1.0, 1.0);
      graphicsManager->drawPrimitives(graphicsPrimitives,position, rotation, *surface,
        inverted, visParams.drawForSensor, visParams);
      surface->setColorv(colorBackup);
      //Draw physics:
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      glShadeModel(GL_SMOOTH);
      std::vector<PhysicalObject*>::iterator iter;
      for(iter = physicalRepresentation.begin(); iter != physicalRepresentation.end(); ++iter)
        (*iter)->draw(visParams);
    }
  }

  if(shaderProgram)
    graphicsManager->shader->disableLocalShader();

}

void ComplexShape::quickDrawObject() 
{  
  if(!invisible)
  {
    graphicsManager->quickDrawPrimitives(graphicsPrimitives,position, rotation, *surface);
  }
}


double ComplexShape::getMaxDistanceTo(const Vector3d& base) const
{
  double distBasePosition((position-base).getLength());
  double shapeRadius(graphicsManager->distanceToFarthestPoint(graphicsPrimitives));
  return distBasePosition + shapeRadius;
}


bool ComplexShape::intersectWithRay(const Vector3d& pos, const Vector3d& ray, Vector3d& intersectPos) 
{
  return graphicsManager->intersectSetOfPrimitivesWithRay(pos, ray, position, rotation, graphicsPrimitives, intersectPos);
}


SimObject* ComplexShape::clone() const
{
  ComplexShape* newComplexShape = new ComplexShape();
  newComplexShape->copyStandardMembers(this);
  bool createsNewMovID = newComplexShape->adjustMovableID();
  newComplexShape->graphicsPrimitives = graphicsPrimitives;
  newComplexShape->useTriangleMesh = useTriangleMesh;
  newComplexShape->useOBB = useOBB;
  newComplexShape->useABB = useABB;
  std::vector<PhysicalObject*>::const_iterator physRepIter;
  for(physRepIter = physicalRepresentation.begin(); physRepIter != physicalRepresentation.end(); ++physRepIter)
  {
    SimObject* physRepClone = (*physRepIter)->clone();
    PhysicalObject* physObj = physRepClone->castToPhysicalObject();
    assert(physObj != NULL);
    newComplexShape->physicalRepresentation.push_back(physObj);
  }

  std::list<SimObject*>::const_iterator pos;
  for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
  {
    SimObject* childNode = (*pos)->clone();
    newComplexShape->addChildNode(childNode, false);
  }

  if(createsNewMovID)
    simulation->removeCurrentMovableID();

  SimObject* newObject = newComplexShape;
  return newObject;
}


void ComplexShape::createPhysics()
{
  if(movableID)
  {
    //create physic engine stuff
    setPointerToPhysicalWorld(simulation->getPhysicalWorld());
    setPointerToCollisionSpace(simulation->getCurrentSpace());
    //create body and geometry
    createBody();
    createGeometry(*simulation->getMovableSpace());

    //connect body and geometry
    if(physicalRepresentation.size() == 1)
      dGeomSetBody(geometry, body);
  }
  else
  {
    createGeometry(*simulation->getStaticSpace());
  }
}


void ComplexShape::createBody()
{
  if(body == 0)
  {
    if(physicalRepresentation.size() == 0)
    {
      // this case is not defined
    }
    else if(physicalRepresentation.size() == 1)
    {
      PhysicalObject* tempObj = physicalRepresentation.front();
      tempObj->createBody();
      body = *(tempObj->getBody());
      bodyPositionOffset = position - tempObj->getPosition();

      Matrix3d invRotation(rotation);
      invRotation.transpose();
      bodyRotationOffset = tempObj->getRotation() * invRotation;

      initialBodyRotation = tempObj->getRotation();
      initialRotation = rotation;
    }
    else
    {
      //create body for multiple physical objects
      unsigned int i=0;

      dMass singleMass;
      dMass compoundMass;

      //compute body position (->center of mass)
      Vector3d centerOfMass(computeCenterOfMass());

      //create body
      body = dBodyCreate(*(simulation->getPhysicalWorld()));

      //set body position
      dBodySetPosition(body, dReal(centerOfMass.v[0]), dReal(centerOfMass.v[1]), dReal(centerOfMass.v[2]));

      for(i=0; i<physicalRepresentation.size(); i++)
      {
        PhysicalObject* physObj = physicalRepresentation[i];

        //compute mass properties
        physObj->computeMassProperties(singleMass);

        //compute geometry offset relativ to the center of mass
        Vector3d offset = physObj->getPosition() - centerOfMass;

        //rotate mass
        Matrix3d rotationMatrix;
        dMatrix3 odeRotationMatrix;
        rotationMatrix = physObj->getRotation();
        ODETools::convertMatrix(rotationMatrix, odeRotationMatrix);
        dMassRotate(&singleMass, odeRotationMatrix);

        //translate mass 
        dMassTranslate(&singleMass, dReal(offset.v[0]), dReal(offset.v[1]), dReal(offset.v[2]));

        //add mass to compound mass
        if(singleMass.mass > 0)
          dMassAdd(&compoundMass, &singleMass);
      }

      dBodySetPosition(body, dReal(centerOfMass.v[0] + compoundMass.c[0]), dReal(centerOfMass.v[1] + compoundMass.c[1]), dReal(centerOfMass.v[2] + compoundMass.c[2]));
      geometryOffset = Vector3d(-compoundMass.c[0], -compoundMass.c[1], -compoundMass.c[2]);

      //set body to objects
      for(i=0; i<physicalRepresentation.size(); i++)
      {
        physicalRepresentation[i]->setBody(body);
      }

      compoundMass.c[0] = compoundMass.c[1] = compoundMass.c[2] = 0;
      dBodySetMass(body, &compoundMass);

      //set body offsets
      const dReal* bodyP = dBodyGetPosition(body);
      Vector3d bodyPos(bodyP[0], bodyP[1], bodyP[2]);
      bodyPositionOffset = position - bodyPos;

      Matrix3d invRotation(rotation);
      invRotation.transpose();

      const dReal* bodyR = dBodyGetRotation(body);
      Matrix3d bodyRot;
      ODETools::convertMatrix(bodyR, bodyRot);
      bodyRotationOffset = bodyRot * invRotation;

      initialBodyRotation = bodyRot;
      initialRotation = rotation;
    }
  }
}


void ComplexShape::computeMassProperties(dMass& m)
{
  if(body)
  {
    dMass currentMass;
    dBodyGetMass(body, &currentMass);
    m = currentMass;
  }
}


void ComplexShape::createGeometry(dSpaceID space)
{
  if(geometry == 0)
  {
    if(physicalRepresentation.size() > 0 || useTriangleMesh)
    {
      if(useTriangleMesh)
      {
        if(physicsVertices == NULL)
          prepareTrianglesForPhysics();

        if(useTriangleMesh)
        {
          dTriMeshDataID Data = dGeomTriMeshDataCreate();
          dGeomTriMeshDataBuildSimple(Data, (dReal*)physicsVertices, physVertexCount, physicsIndices, physIndexCount);
          geometry = dCreateTriMesh(*simulation->getStaticSpace(), Data, 0, 0, 0);
        }

        //set user data pointer of ODE geometry object to this physical object
        dGeomSetData(geometry, this);
      }
      else // basic physical representation objects
      {
        if(physicalRepresentation.size() == 1)
        {
          PhysicalObject* temp = physicalRepresentation.front();
          temp->createGeometry(space);
          geometry = *(temp->getGeometry());
        }
        else // multiple basic physical representation objects
        {
          //create geometry for multiple physical objects
          unsigned int i=0;

          //compute body position (->center of mass)
          Vector3d centerOfMass(position);
          if(movableID)
            centerOfMass = computeCenterOfMass();

          for(i=0; i<physicalRepresentation.size(); i++)
          {
            PhysicalObject* physObj = physicalRepresentation[i];
            //create geometry transform
            dGeomID transform = dCreateGeomTransform(*(simulation->getMovableSpace()));
            dGeomTransformSetCleanup(transform,1);
            //set collision response to attached geometry, not to transform object
            dGeomTransformSetInfo(transform, 1);
            //create geometry
            physObj->createGeometry(0);
            //attach geometry to geometry transform
            dGeomTransformSetGeom(transform, *(physObj->getGeometry()));

            //set transformed geometry flag for physical object
            physObj->setTransformedGeometry(true);

            //compute geometry offset relativ to the center of mass
            Vector3d offset = physObj->getPosition() + geometryOffset;
            if(movableID)
              offset = physObj->getPosition() - centerOfMass + geometryOffset;
            //set geometry offset for transform object
            dGeomSetPosition(*(physObj->getGeometry()), dReal(offset.v[0]), dReal(offset.v[1]), dReal(offset.v[2]));
            //rotate geometry
            dMatrix3 rotationMatrix;
            dQuaternion rotationQuat;
            dGeomGetQuaternion(*(physObj->getGeometry()), rotationQuat);
            dQtoR(rotationQuat, rotationMatrix);
            dGeomSetRotation(*(physObj->getGeometry()), rotationMatrix);

            dGeomSetBody(transform, body);
            physObj->setBody(body);

            geometryTransforms.push_back(transform);
          }
          transformedGeometry = true;
        }
      }
    }
    setCollideBitfield();
  }
}



void ComplexShape::addChildNode(SimObject* child, bool move)
{
  if(parsingPhysicalRepresentation)
  {
    PhysicalObject* physObj = child->castToPhysicalObject();
    if(physObj)
    {
      physObj->setMaterial(material);
      physObj->setMaterialIndex(materialIndex);
      physObj->setMovableID(movableID);
      physicalRepresentation.push_back(physObj);
      physObj->setParentNode(this);
      if(move)
      {
        physObj->translate(position);
        physObj->rotate(rotation, position);
      }
    }
  }
  else
  {
    SimObject::addChildNode(child, move);
  }
}


double ComplexShape::computeTotalMass()
{
  double totalMass = 0.0;
  if(!physicalRepresentation.empty())
  {
    std::vector<PhysicalObject*>::const_iterator pos;

    for(pos = physicalRepresentation.begin(); pos != physicalRepresentation.end(); ++pos)
    {
      totalMass += (*pos)->getMass();
    }
  }
  return totalMass;
}


Vector3d ComplexShape::computeCenterOfMass()
{
  Vector3d centerOfMass;
  if(!physicalRepresentation.empty())
  {
    std::vector<PhysicalObject*>::const_iterator pos;

    for(pos = physicalRepresentation.begin(); pos != physicalRepresentation.end(); ++pos)
    {
      centerOfMass += ((*pos)->getPosition() * (*pos)->getMass());
    }
    centerOfMass /= computeTotalMass();
  }
  return centerOfMass;
}


void ComplexShape::writeBackPhysicalResult(const bool& store_last_results)
{
  if(isMovable())
  {
    if(physicalRepresentation.size() == 1)
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

        Matrix3d initRot(initialBodyRotation);
        initRot.transpose();
        Matrix3d posOffsetRot = bodyR * initRot;
        Vector3d positionOffset(bodyPositionOffset);
        positionOffset.rotate(posOffsetRot);

        SimObject::setRotation(posOffsetRot * initialRotation);
        SimObject::setPosition(temp + positionOffset);

        temp2.rotate(bodyR);
        temp += temp2;
        physicalRepresentation.front()->setPosition(temp);
        physicalRepresentation.front()->setRotation(bodyR*geomR);
      }
      else
      {
        const dReal* tempPos = dBodyGetPosition(body);
        Vector3d bodyPos(tempPos[0], tempPos[1], tempPos[2]);
        physicalRepresentation.front()->setPosition(bodyPos);
        const dReal* tempRot = dBodyGetRotation(body);
        Matrix3d bodyRot;
        ODETools::convertMatrix(tempRot, bodyRot);
        physicalRepresentation.front()->setRotation(bodyRot);

        Matrix3d initRot(initialBodyRotation);
        initRot.transpose();
        Matrix3d posOffsetRot = bodyRot * initRot;
        Vector3d positionOffset(bodyPositionOffset);
        positionOffset.rotate(posOffsetRot);
        bodyPos += positionOffset;
        bodyRot = posOffsetRot * initialRotation;

        SimObject::setRotation(bodyRot);
        SimObject::setPosition(bodyPos);
      }
    }
    else
    {
      std::vector<PhysicalObject*>::const_iterator pos;
      for(pos = physicalRepresentation.begin(); pos != physicalRepresentation.end(); ++pos)
      {
        const dReal* geomPos = dGeomGetPosition(*(*pos)->getGeometry());
        const dReal* bodyPos = dBodyGetPosition(body);

        const dReal* geomRot = dGeomGetRotation(*(*pos)->getGeometry());
        const dReal* bodyRot = dBodyGetRotation(body);
        Matrix3d bodyR, geomR;
        ODETools::convertMatrix(bodyRot, bodyR);
        ODETools::convertMatrix(geomRot, geomR);

        Vector3d temp(bodyPos[0], bodyPos[1], bodyPos[2]);
        Vector3d temp2(geomPos[0], geomPos[1], geomPos[2]);

        temp2.rotate(bodyR);
        temp += temp2;
        (*pos)->setPosition(temp);
        (*pos)->setRotation(bodyR*geomR);
        if(store_last_results)
        {
          (*pos)->cycle_order_offset = ((*pos)->cycle_order_offset + 1)%12;
          (*pos)->previous_positions[(*pos)->cycle_order_offset] = position;
          (*pos)->previous_rotations[(*pos)->cycle_order_offset] = rotation;
        }
      }

      const dReal* bodyPos = dBodyGetPosition(body);
      Vector3d temp(bodyPos[0], bodyPos[1], bodyPos[2]);

      const dReal* bodyRot = dBodyGetRotation(body);
      Matrix3d bodyR;
      ODETools::convertMatrix(bodyRot, bodyR);

      Matrix3d initRot(initialBodyRotation);
      initRot.transpose();
      Matrix3d posOffsetRot = bodyR * initRot;
      Vector3d positionOffset = bodyPositionOffset;
      positionOffset.rotate(posOffsetRot);

      SimObject::setRotation(posOffsetRot * initialRotation);
      SimObject::setPosition(temp + positionOffset);
    }
    if(store_last_results)
    {
      cycle_order_offset = (++cycle_order_offset)%12;
      previous_positions[cycle_order_offset] = position;
      previous_rotations[cycle_order_offset] = rotation;
    }
  }
}


void ComplexShape::disablePhysics(bool disableGeometry)
{
  PhysicalObject::disablePhysics(disableGeometry);

  if(this->physicalRepresentation.size() > 0)
  {
    std::vector<PhysicalObject*>::const_iterator physRepIter;
    for(physRepIter = physicalRepresentation.begin(); physRepIter != physicalRepresentation.end(); ++physRepIter)
    {
      (*physRepIter)->disablePhysics(disableGeometry);
    }
    if(disableGeometry)
    {
      std::vector<dGeomID>::const_iterator geomTransIter;
      for(geomTransIter = geometryTransforms.begin(); geomTransIter != geometryTransforms.end(); ++geomTransIter)
      {
        if(dGeomIsEnabled(dGeomID(*geomTransIter)))
          dGeomDisable(dGeomID(*geomTransIter));
      }
    }
  }
}

void ComplexShape::enablePhysics()
{
  PhysicalObject::enablePhysics();
  if(physicalRepresentation.size())
  {
    std::vector<PhysicalObject*>::const_iterator physRepIter;
    for(physRepIter = physicalRepresentation.begin(); physRepIter != physicalRepresentation.end(); ++physRepIter)
    {
      (*physRepIter)->enablePhysics();
    }
    std::vector<dGeomID>::const_iterator geomTransIter;
    for(geomTransIter = geometryTransforms.begin(); geomTransIter != geometryTransforms.end(); ++geomTransIter)
    {
      if(!dGeomIsEnabled(dGeomID(*geomTransIter)))
        dGeomEnable(dGeomID(*geomTransIter));
    }
  }
}

void ComplexShape::createAABB()
{
  double minX, minY, minZ, maxX, maxY, maxZ;
  minX = minY = minZ = DBL_MAX;
  maxX = maxY = maxZ = -DBL_MAX;
  Vector3d center;

  std::list<GraphicsPrimitiveHandle>::const_iterator handle;
  for(handle = graphicsPrimitives.begin(); handle != graphicsPrimitives.end(); ++handle)
  {
    const GraphicsPrimitiveDescription* primitive = this->graphicsManager->getGraphicsPrimitive(*handle);
    unsigned int i(0);
    while(i < primitive->numberOfVertices*3 )
    {
      if(primitive->vertices[i] < minX)
        minX = primitive->vertices[i];
      else if(primitive->vertices[i] > maxX)
        maxX = primitive->vertices[i];

      if(primitive->vertices[i+1] < minY)
        minY = primitive->vertices[i+1];
      else if(primitive->vertices[i+1] > maxY)
        maxY = primitive->vertices[i+1];

      if(primitive->vertices[i+2] < minZ)
        minZ = primitive->vertices[i+2];
      else if(primitive->vertices[i+2] > maxZ)
        maxZ = primitive->vertices[i+2];

      i += 3;
    }
  }

  double length = (maxX - minX);
  double width = (maxY - minY);
  double height = (maxZ - minZ);

  center.v[0] = minX + (length*0.5);
  center.v[1] = minY + (width*0.5);
  center.v[2] = minZ + (height*0.5);

  Box* dummy = new Box();
  dummy->setSurface(simulation->getDefaultSurface());
  dummy->setDimensions(length, width, height);
  dummy->setMass(1.0);
  dummy->setPosition(center + position);
  physicalRepresentation.push_back(dummy);
}

void ComplexShape::translate(const Vector3d& translation)
{
  position+=translation;
  ObjectList::const_iterator pos;
  //translate child objects
  for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
  {
    (*pos)->translate(translation);
  }
  //translate physical representation
  if(physicalRepresentation.size())
  {
    std::vector<PhysicalObject*>::const_iterator physRepIter;
    for(physRepIter = physicalRepresentation.begin(); physRepIter != physicalRepresentation.end(); ++physRepIter)
    {
      (*physRepIter)->translate(translation);
    }
  }
}

void ComplexShape::rotate(const Matrix3d& rotation, const Vector3d& origin)
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

  //rotate physical representation
  if(physicalRepresentation.size())
  {
    std::vector<PhysicalObject*>::const_iterator physRepIter;
    for(physRepIter = physicalRepresentation.begin(); physRepIter != physicalRepresentation.end(); ++physRepIter)
    {
      (*physRepIter)->rotate(rotation, origin);
    }
  }
}

void ComplexShape::postProcessAfterParsing()
{
  PhysicalObject::postProcessAfterParsing();
  std::vector<PhysicalObject*>::iterator pos;
  for(pos = physicalRepresentation.begin(); pos != physicalRepresentation.end(); ++pos)
  {
    (*pos)->setSurface(surface);
    //set collide bit
    (*pos)->setCollideBit(collideBit);
  }

  //physics
  if(mass == 0)
    mass = computeTotalMass();

  if(useTriangleMesh || useOBB || useABB)
  {
    prepareTrianglesForPhysics();
    if(useOBB)
    {
      createOBB();
    }
    else if(useABB)
    {
      createABB();
    }
  }
}

void ComplexShape::setBody(dBodyID body)
{
  PhysicalObject::setBody(body);
  std::vector<PhysicalObject*>::iterator pos;
  for(pos = physicalRepresentation.begin(); pos != physicalRepresentation.end(); ++pos)
  {
    (*pos)->setBody(body);
  }

  const dReal* bodyRot = dBodyGetRotation(body);
  Matrix3d bodyR;
  ODETools::convertMatrix(bodyRot, bodyR);
  initialBodyRotation = bodyR;
  initialRotation = rotation;
}

void ComplexShape::setPosition(const Vector3d& pos)
{
  Vector3d translation(pos);
  translation -= position;
  position = pos;
  //translate child objects
  ObjectList::const_iterator listPos;
  for(listPos = childNodes.begin(); listPos != childNodes.end(); ++listPos)
  {
    (*listPos)->translate(translation);
  }
  //translate physical representation
  std::vector<PhysicalObject*>::iterator iter;
  for(iter = physicalRepresentation.begin(); iter != physicalRepresentation.end(); ++iter)
  {
    (*iter)->translate(translation);    
  }
}

void ComplexShape::setRotation(const Matrix3d& rot)
{
  Matrix3d childRotation(rotation);
  childRotation.transpose();
  childRotation = rot * childRotation;

  rotation = rot;
  ObjectList::const_iterator listPos;
  //rotate child objects
  for(listPos = childNodes.begin(); listPos != childNodes.end(); ++listPos)
  {
    (*listPos)->rotate(childRotation, position);
  }

  //rotate physical representation
  std::vector<PhysicalObject*>::iterator iter;
  for(iter = physicalRepresentation.begin(); iter != physicalRepresentation.end(); ++iter)
  {
    (*iter)->rotate(childRotation, position);
  }
}

void ComplexShape::setCollideBit(bool value)
{
  PhysicalObject::setCollideBit(value);

  std::vector<PhysicalObject*>::iterator iter;
  for(iter = physicalRepresentation.begin(); iter != physicalRepresentation.end(); ++iter)
  {
    (*iter)->setCollideBit(value);
  }
}

void ComplexShape::setCollideBitfield()
{
  if(geometry)
    PhysicalObject::setCollideBitfield();

  std::vector<PhysicalObject*>::iterator iter;
  for(iter = physicalRepresentation.begin(); iter != physicalRepresentation.end(); ++iter)
  {
    (*iter)->setCollideBitfield();
  }
}

void ComplexShape::translateDnD(const Vector3d& translation)
{
  if(compoundPhysicalObject->isSimple())
  {
    if(transformedGeometry)
    {
      position+=translation;

      //body position
      Matrix3d initRot(initialRotation);
      initRot.transpose();
      Matrix3d posOffsetRot = rotation * initRot;
      Vector3d positionOffset = bodyPositionOffset;
      positionOffset.rotate(posOffsetRot);
      Vector3d bodyPos = position;
      bodyPos -= positionOffset;

      if(body)
      {
        dBodySetPosition(body, dReal(bodyPos.v[0]), dReal(bodyPos.v[1]), dReal(bodyPos.v[2]));
      }
      //translate physical representation
      std::vector<PhysicalObject*>::const_iterator physRepIter;
      for(physRepIter = physicalRepresentation.begin(); physRepIter != physicalRepresentation.end(); ++physRepIter)
      {
        (*physRepIter)->translate(translation);
      }
      //translate child objects
      ObjectList::const_iterator pos;
      for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
      {
        (*pos)->translateDnD(translation);
      }
    }
    else
    {
      position+=translation;
      if(geometry)
      {
        //body position
        Matrix3d initRot(initialRotation);
        initRot.transpose();
        Matrix3d posOffsetRot = rotation * initRot;
        Vector3d positionOffset = bodyPositionOffset;
        positionOffset.rotate(posOffsetRot);
        Vector3d bodyPos = position;
        bodyPos -= positionOffset;

        dGeomSetPosition(geometry, dReal(bodyPos.v[0]), dReal(bodyPos.v[1]), dReal(bodyPos.v[2]));
        if(body)
        {
          dBodySetPosition(body, dReal(bodyPos.v[0]), dReal(bodyPos.v[1]), dReal(bodyPos.v[2]));
        }
      }
      //translate physical representation
      std::vector<PhysicalObject*>::const_iterator physRepIter;
      for(physRepIter = physicalRepresentation.begin(); physRepIter != physicalRepresentation.end(); ++physRepIter)
      {
        (*physRepIter)->translate(translation);
      }
      //translate child objects
      ObjectList::const_iterator pos;
      for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
      {
        (*pos)->translateDnD(translation);
      }
    }
  }
  else
  {
    if(!compoundPhysicalObject->isSynchronized())
    {
      compoundPhysicalObject->translateDnD(translation, false);
    }
    SimObject::translateDnD(translation);

    std::vector<PhysicalObject*>::const_iterator physRepIter;
    for(physRepIter = physicalRepresentation.begin(); physRepIter != physicalRepresentation.end(); ++physRepIter)
    {
      (*physRepIter)->translate(translation);
    }
  }
}

void ComplexShape::rotateDnD(const Matrix3d& rot, const Vector3d& origin)
{
  if(this->compoundPhysicalObject->isSimple())
  {
    if(this->transformedGeometry)
    {
      //position
      Vector3d v = position - origin;
      v.rotate(rot);
      position = origin + v;
      //rotation
      Matrix3d dummy = rot;
      rotation = dummy * (rotation);

      //body position
      Matrix3d initRot(initialRotation);
      initRot.transpose();
      Matrix3d posOffsetRot = rotation * initRot;
      Vector3d positionOffset(bodyPositionOffset);
      positionOffset.rotate(posOffsetRot);
      Vector3d bodyPos(position);
      bodyPos -= positionOffset;
      //body rotation
      Matrix3d  bodyRot = posOffsetRot * initialBodyRotation;
      dMatrix3 rotationMatrix;
      ODETools::convertMatrix(bodyRot, rotationMatrix);  

      if(body)
      {
        dBodySetPosition(body, dReal(bodyPos.v[0]), dReal(bodyPos.v[1]), dReal(bodyPos.v[2]));
        dBodySetRotation(body, rotationMatrix);
      }
      //rotate physical representation
      std::vector<PhysicalObject*>::const_iterator physRepIter;
      for(physRepIter = physicalRepresentation.begin(); physRepIter != physicalRepresentation.end(); ++physRepIter)
      {
        (*physRepIter)->rotate(rot, origin);
      }
      //rotate child objects
      ObjectList::const_iterator pos;
      for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
      { 
        (*pos)->rotateDnD(rot, origin);
      }
    }
    else
    {
      if(geometry)
      {
        //position
        Vector3d v = position - origin;
        v.rotate(rot);
        position = origin + v;
        //rotation
        Matrix3d dummy = rot;
        rotation = dummy * (rotation);
        //body and geometry position
        Matrix3d initRot(initialRotation);
        initRot.transpose();
        Matrix3d posOffsetRot = rotation * initRot;
        Vector3d positionOffset = bodyPositionOffset;
        positionOffset.rotate(posOffsetRot);
        Vector3d bodyPos = position;
        bodyPos -= positionOffset;
        //body and geometry rotation
        Matrix3d  bodyRot = posOffsetRot * initialBodyRotation;
        dMatrix3 rotationMatrix;
        ODETools::convertMatrix(bodyRot, rotationMatrix);  

        dGeomSetPosition(geometry, dReal(bodyPos.v[0]), dReal(bodyPos.v[1]), dReal(bodyPos.v[2]));
        dGeomSetRotation(geometry, rotationMatrix);
        if(body)
        {
          dBodySetPosition(body, dReal(bodyPos.v[0]), dReal(bodyPos.v[1]), dReal(bodyPos.v[2]));
          dBodySetRotation(body, rotationMatrix);
        }
      }
      //rotate physical representation
      std::vector<PhysicalObject*>::const_iterator physRepIter;
      for(physRepIter = physicalRepresentation.begin(); physRepIter != physicalRepresentation.end(); ++physRepIter)
      {
        (*physRepIter)->rotate(rot, origin);
      }
      //rotate child objects
      ObjectList::const_iterator pos;
      for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
      { 
        (*pos)->rotateDnD(rot, origin);
      }

    }
  }
  else
  {
    SimObject::rotateDnD(rot, origin);
    if(!compoundPhysicalObject->isSynchronized())
      compoundPhysicalObject->rotateDnD(rot, origin, false);

    std::vector<PhysicalObject*>::const_iterator physRepIter;
    for(physRepIter = physicalRepresentation.begin(); physRepIter != physicalRepresentation.end(); ++physRepIter)
    {
      (*physRepIter)->rotate(rot, origin);
    }
  }
}

void ComplexShape::updateLocalBoundingBox()
{
  boundingBox.mini = position;
  boundingBox.maxi = position;
  std::list<GraphicsPrimitiveHandle>::const_iterator handle(graphicsPrimitives.begin());
  while(handle != graphicsPrimitives.end())
  {
    (graphicsManager->getGraphicsPrimitive(*(handle++)))->expandAxisAlignedBoundingBox(boundingBox, position, rotation);
  }
}

void ComplexShape::setCompoundPhysicalObject(CompoundPhysicalObject* compoundObject)
{
  compoundPhysicalObject = compoundObject;

  std::vector<PhysicalObject*>::const_iterator physRepIter;
  for(physRepIter = physicalRepresentation.begin(); physRepIter != physicalRepresentation.end(); ++physRepIter)
  {
    (*physRepIter)->setCompoundPhysicalObject(compoundObject);
  }
}


void ComplexShape::createConvexHull(TriangleMesh* convexHull)
{
  coordT *points;
  boolT isMalloc;             // True if qhull should free points in qh_freeqhull() or reallocation
  char flags[]= "qhull Tv";   // option flags for qhull, see qh_opt.htm
  FILE *errFile= stderr;      // error messages from qhull code
  int exitcode;               // 0 if no error from qhull
  facetT *facet;              // set by FORALLfacets
  int curlong, totlong;       // memory remaining after qh_memfreeshort

  vertexT* vertex, **vertexp;
  pointT* point;

  int dimOfPoints = 3;
  int numOfPoints = physVertexCount;

  points = new coordT[numOfPoints*dimOfPoints];

  //init points
  for(int i=0; i < numOfPoints; i++)
  {
    points[i*dimOfPoints] = physicsVertices[i][0];
    points[i*dimOfPoints+1] = physicsVertices[i][1];
    points[i*dimOfPoints+2] = physicsVertices[i][2];
  }

  isMalloc = false;
  //create convex hull
  exitcode= qh_new_qhull (dimOfPoints, numOfPoints, points, isMalloc, flags, NULL, errFile);
  if (!exitcode)
  {
    for(int i=0; i < qh num_points; i++)
    {
      point = qh_point(i);
      Vector3d tempVertex(point[0], point[1], point[2]);
      convexHull->vertices.push_back(tempVertex);
    }

    qh_triangulate();

    FORALLfacets
    {
      TriMeshFace tempTriangle;
      int i = 0;

      setT *tempVertices;
      tempVertices= qh_facet3vertex (facet);
      FOREACHvertex_(tempVertices)
      {
        int id = qh_pointid(vertex->point);
        tempTriangle.point[i] = id;
        i++;
      }
      qh_settempfree(&tempVertices);

      convexHull->triangles.push_back(tempTriangle);
      convexHull->numOfTriangles++;
    }
  }

  qh_freeqhull(!qh_ALL);
  qh_memfreeshort (&curlong, &totlong);
  if (curlong || totlong)
    fprintf (errFile, "qhull internal warning (main): did not free %d bytes of long memory (%d pieces)\n", totlong, curlong);

  delete[] points;
}


void ComplexShape::createOBB()
{
  TriangleMesh convexHull;
  createConvexHull(&convexHull);

  int numOfVertices = convexHull.vertices.size();

  Vector3d center;
  for(int j=0; j < numOfVertices; j++)
  {
    center += convexHull.vertices[j];
  }

  center /= (float)numOfVertices;


  double c00=0.0, c01=0.0, c02=0.0, c10=0.0, c11=0.0, c12=0.0, c20=0.0, c21=0.0, c22=0.0;
  for(unsigned int i=0; i < convexHull.vertices.size(); i++)
  {
    Vector3d centerToVertex = convexHull.vertices[i] - center;

    c00 += centerToVertex.v[0] * centerToVertex.v[0];
    c01 += centerToVertex.v[0] * centerToVertex.v[1];
    c02 += centerToVertex.v[0] * centerToVertex.v[2];
    c10 += centerToVertex.v[1] * centerToVertex.v[0];
    c11 += centerToVertex.v[1] * centerToVertex.v[1];
    c12 += centerToVertex.v[1] * centerToVertex.v[2];
    c20 += centerToVertex.v[2] * centerToVertex.v[0];
    c21 += centerToVertex.v[2] * centerToVertex.v[1];
    c22 += centerToVertex.v[2] * centerToVertex.v[2];
  }

  double scalingFactor = 1.0/convexHull.vertices.size();
  c00 *= scalingFactor;
  c01 *= scalingFactor;
  c02 *= scalingFactor;
  c10 *= scalingFactor;
  c11 *= scalingFactor;
  c12 *= scalingFactor;
  c20 *= scalingFactor;
  c21 *= scalingFactor;
  c22 *= scalingFactor;

  // eigen vectors
  double data[] = { c00, c01, c02,
    c10, c11, c12,
    c20, c21, c22 };
  gsl_matrix_view m = gsl_matrix_view_array(data, 3, 3);
  gsl_vector *eval = gsl_vector_alloc (3);
  gsl_matrix *evec = gsl_matrix_alloc (3, 3);
  gsl_eigen_symmv_workspace * w = gsl_eigen_symmv_alloc (3);
  gsl_eigen_symmv (&m.matrix, eval, evec, w);
  gsl_eigen_symmv_free(w);
  gsl_eigen_symmv_sort (eval, evec, GSL_EIGEN_SORT_ABS_ASC);

  Vector3d eigen[3];
  for (unsigned int i = 0; i < 3; i++)
  {
    /*double eval_i =*/ gsl_vector_get(eval, i);
    gsl_vector_view evec_i = gsl_matrix_column(evec, i);

    eigen[i].v[0] = gsl_vector_get(&evec_i.vector,0);
    eigen[i].v[1] = gsl_vector_get(&evec_i.vector,1);
    eigen[i].v[2] = gsl_vector_get(&evec_i.vector,2);
  }

  gsl_matrix_free(evec);
  gsl_vector_free(eval);

  Vector3d eigenv1 = eigen[0];
  Vector3d eigenv2 = eigen[1];
  Vector3d eigenv3 = eigen[2];

  // extensions along eigenvectors
  double ext1Min = convexHull.findMinProjectionOnVector(eigenv1);
  double ext1Max = convexHull.findMaxProjectionOnVector(eigenv1);
  double ext2Min = convexHull.findMinProjectionOnVector(eigenv2);
  double ext2Max = convexHull.findMaxProjectionOnVector(eigenv2);
  double ext3Min = convexHull.findMinProjectionOnVector(eigenv3);
  double ext3Max = convexHull.findMaxProjectionOnVector(eigenv3);

  double a1=0.0, a2=0.0, a3=0.0;
  double ldb=0.0, wdb=0.0, hdb=0.0;
  a1 = (ext1Min + ext1Max);
  ldb = (fabs(ext1Min) + fabs(ext1Max));
  wdb = (fabs(ext2Min) + fabs(ext2Max));
  hdb = (fabs(ext3Min) + fabs(ext3Max));
  if((ext1Min > 0.0 && ext1Max > 0.0) ||(ext1Min < 0.0 && ext1Max < 0.0))
  {
    ldb = fabs(ext1Max - ext1Min);
    a1 *= 0.5;
  }
  a2 = (ext2Min + ext2Max);
  if((ext2Min > 0.0 && ext2Max > 0.0) ||(ext2Min < 0.0 && ext2Max < 0.0))
  {
    wdb = fabs(ext2Max - ext2Min);
    a2 *= 0.5;
  }
  a3 = (ext3Min + ext3Max);
  if((ext3Min > 0.0 && ext3Max > 0.0) ||(ext3Min < 0.0 && ext3Max < 0.0))
  {
    hdb = fabs(ext3Max - ext3Min);
    a3 *= 0.5;
  }
  // center of obb
  Vector3d obbCenter = (eigenv1 * 0.5*(ext1Min + ext1Max)) + (eigenv2 * 0.5*(ext2Min + ext2Max)) + (eigenv3 * 0.5*(ext3Min + ext3Max));

  eigenv1.normalize();
  eigenv2.normalize();
  eigenv3.normalize();

  dMatrix3 mODE;
  Matrix3d mSim;
  dRFrom2Axes(mODE, dReal(eigenv3.v[0]), dReal(eigenv3.v[1]), dReal(eigenv3.v[2]), dReal(eigenv2.v[0]), dReal(eigenv2.v[1]), dReal(eigenv2.v[2]));

  mSim.col[0].v[0] = mODE[0];
  mSim.col[1].v[0] = mODE[1];
  mSim.col[2].v[0] = mODE[2];
  mSim.col[0].v[1] = mODE[4];
  mSim.col[1].v[1] = mODE[5];
  mSim.col[2].v[1] = mODE[6];
  mSim.col[0].v[2] = mODE[8];
  mSim.col[1].v[2] = mODE[9];
  mSim.col[2].v[2] = mODE[10];

  Box* pOBB = (Box*)physicalRepresentation.front();
  pOBB->setSurface(simulation->getDefaultSurface());
  pOBB->setDimensions(hdb, wdb, ldb);
  pOBB->setPosition(obbCenter);
  pOBB->setRotation(mSim);
  pOBB->createGraphics();
}

void ComplexShape::createABB()
{
  Box* pABB = (Box*)physicalRepresentation.front();
  pABB->setSurface(simulation->getDefaultSurface());
  expandAxisAlignedBoundingBox(boundingBox); // update this box
  Vector3d size = boundingBox.maxi - boundingBox.mini,
           center = (boundingBox.maxi + boundingBox.mini) / 2;
  pABB->setDimensions(size.v[0], size.v[1], size.v[2]);
  pABB->setPosition(center);
  pABB->createGraphics();
}

void ComplexShape::prepareTrianglesForPhysics()
{
  // prepare physics vertices
  int calcNumOfIndices(0);
  std::list<GraphicsPrimitiveHandle>::const_iterator handle;
  for(handle = graphicsPrimitives.begin(); handle != graphicsPrimitives.end(); ++handle)
  {
    const GraphicsPrimitiveDescription* primitive = graphicsManager->getGraphicsPrimitive(*handle);
    physVertexCount += primitive->numberOfVertices;
    calcNumOfIndices += ((primitive->numberOfVertices-2)*3);
  }

  physicsVertices = new dVector3[physVertexCount];
  physicsIndices = new dTriIndex[calcNumOfIndices];
  physicsNormals = new dVector3[calcNumOfIndices / 3];

  int counter(0);
  for(handle = graphicsPrimitives.begin(); handle != graphicsPrimitives.end(); ++handle)
  {
    const GraphicsPrimitiveDescription* primitive = graphicsManager->getGraphicsPrimitive(*handle);
    unsigned int i(0);
    while(i < primitive->numberOfVertices*3)
    {
      Vector3d tempV((dReal)primitive->vertices[i], (dReal)primitive->vertices[i+1],(dReal)primitive->vertices[i+2]);
      tempV.rotate(rotation);
      tempV += position + geometryOffset;
      i += 3;
      physicsVertices[counter][0] = (dReal)tempV.v[0];
      physicsVertices[counter][1] = (dReal)tempV.v[1];
      physicsVertices[counter][2] = (dReal)tempV.v[2];
      counter++;
    }

    //prepare physics indices
    if(primitive->type == POLYGON)
    {
      int startMarker = counter - primitive->numberOfVertices;
      for(unsigned int j=0; j < primitive->numberOfVertices-2; j++)
      {
        physicsIndices[physIndexCount++] = startMarker;
        physicsIndices[physIndexCount++] = startMarker+j+1;
        physicsIndices[physIndexCount++] = startMarker+j+2;
      }
      //normals
      for(unsigned int k=0; k < primitive->numberOfVertices-2; k++)
      {
        physicsNormals[physNormalsCount][0] = (dReal)primitive->normals[0];
        physicsNormals[physNormalsCount][1] = (dReal)primitive->normals[1];
        physicsNormals[physNormalsCount][2] = (dReal)primitive->normals[1];
        physNormalsCount++;
      }
    }
    else //TRIANGLE STRIP
    {
      int startMarker = counter - primitive->numberOfVertices;
      unsigned int j(0);
      while(j < (primitive->numberOfVertices - 2))
      {
        physicsIndices[physIndexCount++] = startMarker+j;
        physicsIndices[physIndexCount++] = startMarker+j+1;
        physicsIndices[physIndexCount++] = startMarker+j+2;
        j += 2;
        if(j <= (primitive->numberOfVertices - 2))
        {
          physicsIndices[physIndexCount++] = startMarker+j;
          physicsIndices[physIndexCount++] = startMarker+j-1;
          physicsIndices[physIndexCount++] = startMarker+j+1;
        }
      }
      //normals
      unsigned int k(0);
      while(k < ((primitive->numberOfVertices-2)*3))
      {
        physicsNormals[physNormalsCount][0] = (dReal)primitive->normals[k];
        physicsNormals[physNormalsCount][1] = (dReal)primitive->normals[k+1];
        physicsNormals[physNormalsCount][2] = (dReal)primitive->normals[k+2];
        physNormalsCount++;
        k+=3;
      }
    }
  }
}
