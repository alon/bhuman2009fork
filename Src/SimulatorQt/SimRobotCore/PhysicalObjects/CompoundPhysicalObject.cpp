/**
 * @file CompoundPhysicalObject.cpp
 * 
 * Implementation of class CompoundPhysicalObject
 *
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 */

#include <PhysicalObjects/CompoundPhysicalObject.h>

#include <PhysicalObjects/PhysicalObject.h>
#include <PhysicalObjects/ComplexShape.h>
#include <Simulation/Simulation.h>
#include <Tools/ODETools.h>

CompoundPhysicalObject::CompoundPhysicalObject() : totalMass(0.0), simple(false), movableID(-1), synchronized(true)
{
}


void CompoundPhysicalObject::computeTotalMass()
{
  if(!objects.empty())
  {
    std::vector<SimObject*>::const_iterator pos;

    for(pos = objects.begin(); pos != objects.end(); ++pos)
    {
      PhysicalObject* physObj = (*pos)->castToPhysicalObject();
      if(physObj)
      {
        if(physObj->getKind() == "complex-shape")
          totalMass += ((ComplexShape*)physObj)->computeTotalMass();
        else
          totalMass += physObj->getMass();
      }
    }
  }
}

void CompoundPhysicalObject::computeCenterOfMass()
{
  if(!objects.empty())
  {
    std::vector<SimObject*>::const_iterator pos;

    for(pos = objects.begin(); pos != objects.end(); ++pos)
    {
      PhysicalObject* physObj = (*pos)->castToPhysicalObject();
      if(physObj)
      {
        if(physObj->getKind() == "complex-shape")
        {
          std::vector<PhysicalObject*>* physRepr = ((ComplexShape*)physObj)->getPhysicalRepresentations();
          std::vector<PhysicalObject*>::const_iterator physReprIter;
          for(physReprIter = physRepr->begin(); physReprIter != physRepr->end(); ++physReprIter)
          {
            PhysicalObject* physReprObj = *physReprIter;
            centerOfMass += (physReprObj->getPosition() * physReprObj->getMass());
          }
        }
        else
          centerOfMass += (physObj->getPosition() * physObj->getMass());
      }
    }

    computeTotalMass();
    centerOfMass /= totalMass;
  }
}


void CompoundPhysicalObject::createPhysics()
{
  setRootObjects();

  if(objects.size() == 1)
  {
    simple = true;
    PhysicalObject* temp = objects[0]->castToPhysicalObject();
    if(temp)
    {
      temp->setCompoundPhysicalObject(this);
      temp->createPhysics();
      body = *(temp->getBody());
      temp->determineRollingFrictionFlag();
    }
    return;
  }
  else
  {
    unsigned int i=0;

    dMass singleMass;
    dMass compoundMass;

    //compute body position (->center of mass)
    computeCenterOfMass();
    assert(totalMass != 0);

    //create body
    body = dBodyCreate(*(simulation->getPhysicalWorld()));

    //set body position
    dBodySetPosition(body, dReal(centerOfMass.v[0]), dReal(centerOfMass.v[1]), dReal(centerOfMass.v[2]));


    for(i=0; i<objects.size(); i++)
    {
      PhysicalObject* physObj = objects[i]->castToPhysicalObject();
      if(physObj != NULL)
      {
        //complex shape
        if(physObj->getKind() == "complex-shape")
        {
          ComplexShape* complexShape = (ComplexShape*)physObj;
          std::vector<PhysicalObject*>* physRepr = complexShape->getPhysicalRepresentations();
          std::vector<PhysicalObject*>::const_iterator physReprIter;
          for(physReprIter = physRepr->begin(); physReprIter != physRepr->end(); ++physReprIter)
          {
            PhysicalObject* physReprObj = *physReprIter;
            //create geometry transform
            dGeomID transform = dCreateGeomTransform(*(simulation->getMovableSpace()));
            dGeomTransformSetCleanup(transform,1);
            //set collision response to attached geometry, not to transform object
            dGeomTransformSetInfo(transform, 1);
            //create geometry
            physReprObj->createGeometry(0);
            //compute mass properties
            physReprObj->computeMassProperties(singleMass);
            //attach geometry to geometry transform
            dGeomTransformSetGeom(transform, *(physReprObj->getGeometry()));

            //set transformed geometry flag for physical object
            physReprObj->setTransformedGeometry(true);

            //compute geometry offset relativ to the center of mass
            Vector3d offset = physReprObj->getPosition() - centerOfMass;
            //set geometry offset for transform object
            dGeomSetPosition(*(physReprObj->getGeometry()), dReal(offset.v[0]), dReal(offset.v[1]), dReal(offset.v[2]));
            //rotate geometry
            /*
            dMatrix3 rotationMatrix;
            ODETools::convertMatrix(physReprObj->getRotation(), rotationMatrix);
            dGeomSetRotation(*(physReprObj->getGeometry()), rotationMatrix);
            */
            dMatrix3 rotationMatrix;
            dQuaternion rotationQuat;
            dGeomGetQuaternion(*(physReprObj->getGeometry()), rotationQuat);
            dQtoR(rotationQuat, rotationMatrix);
            dGeomSetRotation(*(physReprObj->getGeometry()), rotationMatrix);
        
            //rotate mass
            dMatrix3 geomRotationMatrix;
            dQuaternion geomRotationQuat;
            dGeomGetQuaternion(*(physReprObj->getGeometry()), geomRotationQuat);
            dQtoR(geomRotationQuat, geomRotationMatrix);
            dMassRotate(&singleMass, geomRotationMatrix);

            //translate mass 
            dMassTranslate(&singleMass, dReal(offset.v[0]), dReal(offset.v[1]), dReal(offset.v[2]));

            //add mass to compound mass
            if (singleMass.mass >0)
              dMassAdd(&compoundMass, &singleMass);

            //store geomTransforms in vector of geometry
            geometry.push_back(transform);
          }
          physObj->setTransformedGeometry(true);
          complexShape->setCollideBitfield();
          complexShape->setBodyPositionOffset(complexShape->getPosition() - centerOfMass);
          complexShape->setBodyRotationOffset(complexShape->getRotation());
          if(physRepr->size() == 1)
          {
            physObj->setGeometry(*(physRepr->front()->getGeometry()));
          }
        }
        else
        {
           //create geometry transform
          dGeomID transform = dCreateGeomTransform(*(simulation->getMovableSpace()));
          dGeomTransformSetCleanup(transform,1);
          //set collision response to attached geometry, not to transform object
          dGeomTransformSetInfo(transform, 1);
          //create geometry
          physObj->createGeometry(0);
          //compute mass properties
          physObj->computeMassProperties(singleMass);
          //attach geometry to geometry transform
          dGeomTransformSetGeom(transform, *(physObj->getGeometry()));

          //set transformed geometry flag for physical object
          physObj->setTransformedGeometry(true);

          //compute geometry offset relativ to the center of mass
          Vector3d offset = physObj->getPosition() - centerOfMass;
          //set geometry offset for transform object
          dGeomSetPosition(*(physObj->getGeometry()), dReal(offset.v[0]), dReal(offset.v[1]), dReal(offset.v[2]));
          //rotate geometry
          /*
          dMatrix3 rotationMatrix;
          ODETools::convertMatrix(physObj->getRotation(), rotationMatrix);
          dGeomSetRotation(*(physObj->getGeometry()), rotationMatrix);
          */
          dMatrix3 rotationMatrix;
          dQuaternion rotationQuat;
          dGeomGetQuaternion(*(physObj->getGeometry()), rotationQuat);
          dQtoR(rotationQuat, rotationMatrix);
          dGeomSetRotation(*(physObj->getGeometry()), rotationMatrix);

        
          //translate mass 
          dMassTranslate(&singleMass, dReal(offset.v[0]), dReal(offset.v[1]), dReal(offset.v[2]));
          //rotatet mass
          dMatrix3 geomRotationMatrix;
          dQuaternion geomRotationQuat;
          dGeomGetQuaternion(*(physObj->getGeometry()), geomRotationQuat);
          dQtoR(geomRotationQuat, geomRotationMatrix);
          dMassRotate(&singleMass, geomRotationMatrix);

          //add mass to compound mass
          if (singleMass.mass >0)
            dMassAdd(&compoundMass, &singleMass);

          //store geomTransforms in vector of geometry
          geometry.push_back(transform);
        }
      }
    }

    dBodySetPosition(body, dReal(centerOfMass.v[0] + compoundMass.c[0]), dReal(centerOfMass.v[1] + compoundMass.c[1]), dReal(centerOfMass.v[2] + compoundMass.c[2]));

    for(i=0; i < geometry.size(); i++)
    {
      dGeomID g = dGeomTransformGetGeom(geometry[i]);
      const dReal* p = dGeomGetPosition(g);
      dGeomSetPosition(g, dReal(p[0] - compoundMass.c[0]), dReal(p[1] - compoundMass.c[1]), dReal(p[2] - compoundMass.c[2]));
      dGeomSetBody(geometry[i], body);
    }

    //set body to objects
    for(i=0; i<objects.size(); i++)
    {
      PhysicalObject* physObj = objects[i]->castToPhysicalObject();
      if(physObj)
      {
        physObj->setBody(body);
        physObj->setCompoundPhysicalObject(this);
        if(physObj->getKind() == "complex-shape")
        {
          ComplexShape* complexShape = (ComplexShape*)physObj;
          complexShape->setBodyPositionOffset(complexShape->getBodyPositionOffset() - Vector3d(compoundMass.c[0], compoundMass.c[1], compoundMass.c[2]));
        }
     }
    }
    compoundMass.c[0] = compoundMass.c[1] = compoundMass.c[2] = 0;
    dBodySetMass(body, &compoundMass);
  }
}

void CompoundPhysicalObject::translate(const Vector3d& translation)
{
  std::vector<SimObject*>::const_iterator iter;
  for(iter=rootObjects.begin(); iter != rootObjects.end(); ++iter)
  {
    (*iter)->translate(translation);
  }
}

void CompoundPhysicalObject::rotate(const Matrix3d& rotation, const Vector3d& origin)
{
  std::vector<SimObject*>::const_iterator iter;
  for(iter=rootObjects.begin(); iter != rootObjects.end(); ++iter)
  {
    (*iter)->rotate(rotation, origin);
  }
}

void CompoundPhysicalObject::rotateAroundAxis(double angle, Vector3d axis)
{
  Matrix3d m;
  axis.normalize();
  m.setRotationAroundAxis(axis, angle);
  const dReal* bodyPos = dBodyGetPosition(body);
  Vector3d bodyP(bodyPos[0], bodyPos[1], bodyPos[2]);
  rotate(m, bodyP);
}


void CompoundPhysicalObject::invertColors()
{
  std::vector<SimObject*>::const_iterator pos;
  for(pos = rootObjects.begin(); pos != rootObjects.end(); ++pos)
  {
    (*pos)->invertColors();
  }
}

void CompoundPhysicalObject::setRootObjects()
{
  std::vector<SimObject*>::const_iterator pos;
  for(pos = objects.begin(); pos != objects.end(); ++pos)
  {
    if((*pos)->getParentNode() == parent)
      rootObjects.push_back(*pos);
  }

}

Vector3d CompoundPhysicalObject::getPosition() const
{
  const dReal* bodyPos = dBodyGetPosition(body);
  return Vector3d(bodyPos[0], bodyPos[1], bodyPos[2]);
}

void CompoundPhysicalObject::disablePhysics(bool disableGeometry)
{
  std::vector<SimObject*>::const_iterator pos;
  PhysicalObject* physObj = NULL;
  for(pos = objects.begin(); pos != objects.end(); ++pos)
  {
    physObj = (*pos)->castToPhysicalObject();
    if(physObj)
      physObj->disablePhysics(disableGeometry);
  }
  if(disableGeometry)
  {
    std::vector<dGeomID>::const_iterator geomTransIter;
    for(geomTransIter = geometry.begin(); geomTransIter != geometry.end(); ++geomTransIter)
    {
      if(dGeomIsEnabled(dGeomID(*geomTransIter)))
        dGeomDisable(*geomTransIter);
    }
  }

}

void CompoundPhysicalObject::enablePhysics()
{
  std::vector<SimObject*>::const_iterator pos;
  PhysicalObject* physObj = NULL;
  for(pos = objects.begin(); pos != objects.end(); ++pos)
  {
    physObj = (*pos)->castToPhysicalObject();
    if(physObj)
      physObj->enablePhysics();
  }
  std::vector<dGeomID>::const_iterator geomTransIter;
  for(geomTransIter = geometry.begin(); geomTransIter != geometry.end(); ++geomTransIter)
  {
    if(!dGeomIsEnabled(*geomTransIter))
      dGeomEnable(*geomTransIter);
  }

}

void CompoundPhysicalObject::translateDnD(const Vector3d& translation, bool incChilds)
{
  if(!synchronized)
  {
    const dReal* bodyPos = dBodyGetPosition(body);
    Vector3d bodyP(bodyPos[0]+translation.v[0], bodyPos[1]+translation.v[1], bodyPos[2]+translation.v[2]);

    dBodySetPosition(body, dReal(bodyP.v[0]), dReal(bodyP.v[1]),dReal(bodyP.v[2]));
    synchronized = true;

    if(incChilds)
    {
      std::vector<SimObject*>::const_iterator iter;
      for(iter=rootObjects.begin(); iter != rootObjects.end(); ++iter)
      {
        (*iter)->translateDnD(translation);
      }
    }
  }
}

void CompoundPhysicalObject::rotateDnD(const Matrix3d& rot, const Vector3d& origin, bool incChilds)
{
  if(!synchronized)
  {
    const dReal* bodyPos = dBodyGetPosition(body);
    Vector3d bodyP(bodyPos[0], bodyPos[1], bodyPos[2]);
    Vector3d v = bodyP - origin;
    v.rotate(rot);

    Vector3d pos = origin + v;
    dBodySetPosition(body, dReal(pos.v[0]), dReal(pos.v[1]), dReal(pos.v[2]));

    Matrix3d dummy = rot;
    const dReal* bodyRot = dBodyGetRotation(body);
    Matrix3d temp;
    ODETools::convertMatrix(bodyRot, temp);
    dummy *= temp;
    dMatrix3 bodyR;
    ODETools::convertMatrix(dummy, bodyR);
    dBodySetRotation(body, bodyR);

    synchronized = true;

    if(incChilds)
    {
      std::vector<SimObject*>::const_iterator iter;
      for(iter=rootObjects.begin(); iter != rootObjects.end(); ++iter)
      {
        (*iter)->rotateDnD(rot, origin);
      }
    }
  }
}

void CompoundPhysicalObject::rotateAroundAxisDnD(double angle, Vector3d axis, bool incChilds)
{
  Matrix3d m;
  axis.normalize();
  m.setRotationAroundAxis(axis, angle);
  const dReal* bodyPos = dBodyGetPosition(body);
  Vector3d bodyP(bodyPos[0], bodyPos[1], bodyPos[2]);
  rotateDnD(m, bodyP, incChilds);
}
