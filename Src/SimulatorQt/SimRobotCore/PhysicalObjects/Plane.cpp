/**
 * @file Plane.cpp
 *
 * Implementation of class Plane
 *
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 */

#include <Tools/Surface.h>
#include <Tools/ODETools.h>
#include <Simulation/Simulation.h>
#include "Plane.h"


Plane::Plane() : a(0.0), b(0.0), c(0.0), d(0.0), surface(0), invisible(false), materialIndex(-1), geometry(0)
{
}

Plane::Plane(const AttributeSet& attributes) : a(0.0), b(0.0), c(0.0), d(0.0), surface(0), invisible(false), materialIndex(-1), geometry(0)
{
  for(unsigned int i=0; i<attributes.size(); i++)
  {
    std::string attributeName(attributes[i].attribute);
    if(attributeName == "name")
    {
      name = attributes[i].value;
    }
    else if(attributeName == "a")
    {
      a = ParserUtilities::toDouble(attributes[i].value);
    }
    else if(attributeName == "b")
    {
      b = ParserUtilities::toDouble(attributes[i].value);
    }
    else if(attributeName == "c")
    {
      c = ParserUtilities::toDouble(attributes[i].value);
    }
    else if(attributeName == "d")
    {
      d = ParserUtilities::toDouble(attributes[i].value);
    }
  }
  if(isPlaneEquationValid())
    precomputeDrawingParameter();
}

void Plane::parseAppearance(const AttributeSet& attributes,
                            int line, int column, 
                            ErrorManager* errorManager,
                            const Simulation& simulation)
{
  for(unsigned int i=0; i< attributes.size(); i++)
  {
    if(attributes[i].attribute == "ref")
    {
      Surface* surface = simulation.getSurface(attributes[i].value);
      if(surface)
      {
        this->surface = surface;
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

void Plane::parseMaterial(const AttributeSet& attributes,
                          int line, int column, 
                          ErrorManager* errorManager,
                          const Simulation& simulation)
{
  for(unsigned int i=0; i < attributes.size(); i++)
  {
    if(attributes[i].attribute == "name")
    {
      int matIndex = simulation.getMaterialIndex(attributes[i].value);
      if(matIndex != -1)
      {
        materialIndex = matIndex;
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

void Plane::drawObject(const VisualizationParameterSet& visParams)
{
  if(!invisible)
  {
    glColor4fv(surface->color);

    //Draw plane:
    glPushMatrix();
      glBegin(GL_QUADS);
        glNormal3d(drawingNormal.v[0], drawingNormal.v[1], drawingNormal.v[2]);
        glVertex3d(drawingPoint1.v[0], drawingPoint1.v[1], drawingPoint1.v[2]);
        glVertex3d(drawingPoint2.v[0], drawingPoint2.v[1], drawingPoint2.v[2]);
        glVertex3d(drawingPoint3.v[0], drawingPoint3.v[1], drawingPoint3.v[2]);
        glVertex3d(drawingPoint4.v[0], drawingPoint4.v[1], drawingPoint4.v[2]);
      glEnd();
    glPopMatrix();
  }
}

void Plane::quickDrawObject()
{
  if(!invisible)
  {
    glPushMatrix();
      glBegin(GL_QUADS);
        glNormal3d(drawingNormal.v[0], drawingNormal.v[1], drawingNormal.v[2]);
        glVertex3d(drawingPoint1.v[0], drawingPoint1.v[1], drawingPoint1.v[2]);
        glVertex3d(drawingPoint2.v[0], drawingPoint2.v[1], drawingPoint2.v[2]);
        glVertex3d(drawingPoint3.v[0], drawingPoint3.v[1], drawingPoint3.v[2]);
        glVertex3d(drawingPoint4.v[0], drawingPoint4.v[1], drawingPoint4.v[2]);
      glEnd();
    glPopMatrix();
  }
}

void Plane::createPhysics(const dSpaceID& collisionSpace)
{
  geometry = dCreatePlane(collisionSpace, dReal(a),dReal(b),dReal(c),dReal(d));
  //set user data pointer of ODE geometry object to this plane object
  dGeomSetData(geometry, this);
}

Vector3d Plane::projectPointOnPlane(const Vector3d &point)
{
  Vector3d normal (a,b,c);
  double normalLength = normal.getLength();
  normal.normalize();

  double distancePointToPlane = ((a*point.v[0] + b*point.v[1] + c*point.v[2] - d) / normalLength);

  Vector3d projectedPoint = normal * distancePointToPlane;
  projectedPoint = point - projectedPoint;

  return projectedPoint;
}

void Plane::precomputeDrawingParameter()
{
  Vector3d normal (a,b,c);

  //origin
  Vector3d p0 = projectPointOnPlane(Vector3d(0.0,0.0,0.0));

  //point1
  Vector3d p1 = projectPointOnPlane(Vector3d(1.0,1.0,1.0));
  drawingPoint1 = p1 - p0;
  drawingPoint1.normalize();
  drawingPoint1 *= DRAWING_EXTENSION;
  drawingPoint1 = p0 + drawingPoint1;

  //point2
  Vector3d p2 = projectPointOnPlane(Vector3d(-1.0,1.0,1.0));
  drawingPoint2 = p2 - p0;
  drawingPoint2.normalize();
  drawingPoint2 *= DRAWING_EXTENSION;
  drawingPoint2 = p0 + drawingPoint2;

  //point3
  Vector3d p3 = projectPointOnPlane(Vector3d(-1.0,-1.0,1.0));
  drawingPoint3 = p3 - p0;
  drawingPoint3.normalize();
  drawingPoint3 *= DRAWING_EXTENSION;
  drawingPoint3 = p0 + drawingPoint3;

  //point4
  Vector3d p4 = projectPointOnPlane(Vector3d(1.0,-1.0,1.0));
  drawingPoint4 = p4 - p0;
  drawingPoint4.normalize();
  drawingPoint4 *= DRAWING_EXTENSION;
  drawingPoint4 = p0 + drawingPoint4;

  normal.normalize();
  drawingNormal = normal;
}

bool Plane::isPlaneEquationValid()
{
  return(!(a==0.0 && b==0.0 && c==0.0));
}
