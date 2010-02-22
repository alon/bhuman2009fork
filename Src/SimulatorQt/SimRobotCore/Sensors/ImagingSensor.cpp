/**
 * @file ImagingSensor.cpp
 * 
 * Implementation of abstract class ImagingSensor
 *
 * @author <A href="mailto:Tim.Laue@dfki.de">Tim Laue</A>
 */

#include "../Platform/OffscreenRenderer.h"
#include "../Simulation/Simulation.h"
#include "ImagingSensor.h"


ImagingSensor::ImagingSensor():resolutionX(0),resolutionY(0),
                               angleX(0), angleY(0), sensorIs1D(false), aspect(1.0),
                               minRange(0.01), maxRange(100),
                               exposureTime(0.05), fbo_reg(-1),
                               projection(PERSPECTIVE_PROJECTION),
                               osRenderer(0),
                               visualizationColor(0.0,0.0,1.0)
{
}

ImagingSensor::ImagingSensor(const AttributeSet& attributes):
                               resolutionX(0), resolutionY(0),
                               angleX(0), angleY(0), sensorIs1D(false), aspect(1.0),
                               minRange(0.01), maxRange(100),
                               projection(PERSPECTIVE_PROJECTION),
                               osRenderer(0),
                               visualizationColor(0.0,0.0,1.0)
{
  name = ParserUtilities::getValueFor(attributes, "name");
}

void ImagingSensor::drawObject(const VisualizationParameterSet& visParams)
{
  if(visParams.visualizeSensors && !visParams.drawForSensor)
  {
    const double dist(simulation->getStandardLength());
    const double yRot(angleY/2.0);
    const double zRot(angleX/2.0);
    Vector3d camPos = position;
    //Compute four points: Upper left, Upper right, Lower left and lower right
    Vector3d ptUL(dist,0.0,0.0);
    ptUL.rotateY(yRot);
    Vector3d ptUR(ptUL);
    ptUL.rotateZ(zRot);
    ptUR.rotateZ(-zRot);
    Vector3d ptLL(dist,0.0,0.0);
    ptLL.rotateY(-yRot);
    Vector3d ptLR(ptLL);
    ptLL.rotateZ(zRot);
    ptLR.rotateZ(-zRot);
    //Transform points in scene
    ptUL.rotate(rotation);
    ptUL+=camPos;
    ptUR.rotate(rotation);
    ptUR+=camPos;
    ptLL.rotate(rotation);
    ptLL+=camPos;
    ptLR.rotate(rotation);
    ptLR+=camPos;
    //Draw camera opening
    glColor3d(visualizationColor.v[0],
              visualizationColor.v[1],
              visualizationColor.v[2]);
    glBegin(GL_LINE_STRIP);
      glVertex3d (ptUR.v[0], ptUR.v[1], ptUR.v[2]);
      glVertex3d (camPos.v[0], camPos.v[1], camPos.v[2]);
      glVertex3d (ptUL.v[0], ptUL.v[1], ptUL.v[2]);
    glEnd();
    glBegin(GL_LINE_STRIP);
      glVertex3d (ptLR.v[0], ptLR.v[1], ptLR.v[2]);
      glVertex3d (camPos.v[0], camPos.v[1], camPos.v[2]);
      glVertex3d (ptLL.v[0], ptLL.v[1], ptLL.v[2]);
    glEnd();
    glBegin(GL_LINE_LOOP);
      glVertex3d (ptUL.v[0], ptUL.v[1], ptUL.v[2]);
      glVertex3d (ptUR.v[0], ptUR.v[1], ptUR.v[2]);
      glVertex3d (ptLR.v[0], ptLR.v[1], ptLR.v[2]);
      glVertex3d (ptLL.v[0], ptLL.v[1], ptLL.v[2]);
    glEnd();
  }
}

bool ImagingSensor::parseAttributeElements(const std::string& name, 
                                           const AttributeSet& attributes,
                                           int line, int column, 
                                           ErrorManager* errorManager)
{
  if(SimObject::parseAttributeElements(name,attributes,line,column, errorManager))
  {
    return true;
  }
  else if(name == "Resolution")
  {
    int resX(0), resY(0);
    ParserUtilities::parseXYIntPair(attributes, resX, resY);
    if(resY == 0)
    {
      resY = 1;
      sensorIs1D = true;
    }
    setResolution(resX, resY);
  }
  else if(name == "Exposure")
  {
    for(unsigned int i=0; i< attributes.size(); i++)
    {
      if(attributes[i].attribute == "time")
      {
        exposureTime = ParserUtilities::toDouble(attributes[i].value);
      }
    }
  }
  else if(name == "OpeningAngles")
  {
    ParserUtilities::parseXYDoublePair(attributes, angleX, angleY);
    angleX = Functions::toRad(angleX);
    angleY = Functions::toRad(angleY);
    if(angleY == 0)
    {
      angleY = Functions::toRad(0.0001);
      sensorIs1D = true;
    }
    aspect = tan(angleX) / tan(angleY);
  }
  else if(name == "Range")
  {
    ParserUtilities::parseXYDoublePair(attributes, minRange, maxRange);
  }
  else if(name == "SphericalProjection")
  {
    projection = SPHERICAL_PROJECTION;
  }
  else if(name == "PerspectiveProjection")
  {
    projection = PERSPECTIVE_PROJECTION;
  }
  else
  {
    return false;
  }
  return true;
}

void ImagingSensor::copyStandardMembers(const ImagingSensor* other)
{
  SimObject::copyStandardMembers(other);
  resolutionX = other->resolutionX;
  resolutionY = other->resolutionY;
  angleX = other->angleX;
  angleY = other->angleY;
  sensorIs1D = other->sensorIs1D;
  aspect = other->aspect;
  minRange = other->minRange;
  maxRange = other->maxRange;
  exposureTime = other->exposureTime;
  projection = other->projection;
  fbo_reg = other->fbo_reg;
}
