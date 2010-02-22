/**
 * @file DistanceSensor.cpp
 * 
 * Implementation of class DistanceSensor
 *
 * @author <A href="mailto:Tim.Laue@dfki.de">Tim Laue</A>
 */

#include "DistanceSensor.h"
#include "../Platform/OffscreenRenderer.h"
#include "../OpenGL/GLHelper.h"
#include "../Simulation/Simulation.h"


DistanceSensor::DistanceSensor():ImagingSensor()
{
  sensorReading.data.doubleArray = 0;
  visualizationColor = Vector3d(0.0,1.0,0.0);
}

DistanceSensor::DistanceSensor(const AttributeSet& attributes):ImagingSensor(attributes)
{
  name = ParserUtilities::getValueFor(attributes, "name");
  visualizationColor = Vector3d(0.0,1.0,0.0);
  sensorReading.data.doubleArray = 0;
  sensorReading.minValue = 0;
  sensorReading.maxValue = 100;
  sensorReading.unitForDisplay = "m";
  sensorReading.sensorType = SensorReading::doubleArraySensor;
  sensorReading.portName = "distance-data";
}

DistanceSensor::~DistanceSensor()
{
  if(sensorReading.data.doubleArray)
    delete [] sensorReading.data.doubleArray;
}

void DistanceSensor::addToLists(std::vector<SensorPort>& sensorPortList,
                        std::vector<Actuatorport*>& actuatorportList,
                        std::vector<Actuator*>& actuatorList) 
{
  sensorReading.fullSensorName = fullName;
  sensorReading.minValue = minRange;
  sensorReading.maxValue = maxRange;
  sensorPortList.push_back(SensorPort(fullName+".distance-data",0,this));
}

void DistanceSensor::addToDescriptions(std::vector<ObjectDescription>& objectDescriptionTree,
                               int depth) 
{
  Sensor::addToDescriptions(objectDescriptionTree, depth);  
  //Add Sensorport:
  ObjectDescription sensorportDesc;
  sensorportDesc.name = "distance-data";
  sensorportDesc.fullName = fullName + ".distance-data";
  sensorportDesc.depth = depth + 1;
  sensorportDesc.type = OBJECT_TYPE_SENSORPORT;
  objectDescriptionTree.push_back(sensorportDesc);
}

void DistanceSensor::setResolution(int x, int y)
{
  if(sensorReading.data.doubleArray)
  {
    delete[] sensorReading.data.doubleArray;
    standardRenderingBuffer.reset();
  }
  resolutionX = x;
  resolutionY = y;
  standardRenderingBuffer.resolutionX = x;
  standardRenderingBuffer.resolutionY = y;
  sensorReading.dimensions.push_back(x);
  if(y > 1)
    sensorReading.dimensions.push_back(y);
  sensorDataSize = x*y;
  sensorReading.data.doubleArray = new double[sensorDataSize];
}

void DistanceSensor::renderingInstructions(ImagingBuffer<float>& buffer)
{
  // set viewport
  glViewport(0, 0, buffer.resolutionX, buffer.resolutionY);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(Functions::toDeg(angleY), tan(angleX/2) / tan(angleY/2), minRange, maxRange);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // clear the viewport
  glClearDepth(1.0);
  Surface* backgroundSurface = simulation->getBackgroundSurface();
  glClearColor(backgroundSurface->color[0], backgroundSurface->color[1],
               backgroundSurface->color[2], backgroundSurface->color[3]);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // set the visual parameters
  VisualizationParameterSet visParams;
  visParams.surfaceStyle = VisualizationParameterSet::FLAT_SHADING;
  visParams.drawForSensor = true;
  simulation->enableStateDL(visParams.surfaceStyle);

  // set the camera
  Vector3d forwardVec(1.0,0.0,0.0);
  Vector3d upVec(0.0,0.0,1.0);
  forwardVec.rotate(rotation);
  upVec.rotate(rotation);
  gluLookAt(position.v[0], position.v[1], position.v[2], 
            position.v[0] + forwardVec.v[0], position.v[1] + forwardVec.v[1], 
            position.v[2] + forwardVec.v[2],
            upVec.v[0], upVec.v[1], upVec.v[2]);

  glGetDoublev(GL_MODELVIEW_MATRIX, buffer.modelViewMatrix);
  glGetDoublev(GL_PROJECTION_MATRIX, buffer.projectionMatrix);
  glGetIntegerv(GL_VIEWPORT, buffer.viewPort);
  
  rootNode->draw(visParams);
  glFlush();
  simulation->disableStateDL(visParams.surfaceStyle);
}

SensorReading& DistanceSensor::getSensorReading(int localPortId)
{ 
  if(!osRenderer)
    osRenderer = simulation->getOffscreenRenderer();
  int simulationStep(simulation->getSimulationStep());
  if(simulationStep>lastComputationStep)
  {
    OffscreenRenderer::Content offContent = OffscreenRenderer::DEPTH;
    osRenderer->prepareRendering(standardRenderingBuffer.resolutionX, 
                                 standardRenderingBuffer.resolutionY);
    renderingInstructions(standardRenderingBuffer);
    osRenderer->finishRendering(standardRenderingBuffer.pixels,
                                standardRenderingBuffer.resolutionX, 
                                standardRenderingBuffer.resolutionY, 
                                offContent);
    transformDataToDistances(standardRenderingBuffer);
    lastComputationStep = simulationStep;
  }
  return sensorReading;
}

void DistanceSensor::transformDataToDistances(ImagingBuffer<float>& buffer)
{
  Vector3d objectPos;
  GLdouble unProjectMatrix[16];
  GLHelper::getGLH()->prepareMatrixForUnProject(buffer.modelViewMatrix, 
                                         buffer.projectionMatrix, 
                                         unProjectMatrix);
  double* sensorData = const_cast<double*>(sensorReading.data.doubleArray);
  if(projection == PERSPECTIVE_PROJECTION)
  {
    for(unsigned int i=0; i<sensorDataSize; i++)
    {
      double winX((double)(i%buffer.resolutionX));
      double winY((double)(i/buffer.resolutionX));
      double winZ(buffer.pixels[i]);
      GLHelper::getGLH()->unProject(winX, winY, winZ, unProjectMatrix, buffer.viewPort, &objectPos.v[0], &objectPos.v[1], &objectPos.v[2]);
      sensorData[i] = (objectPos-position).getLength();
    }
  }
  else //projection == SPHERICAL_PROJECTION
  {
    for(unsigned int i=0; i<sensorDataSize; i++)
    {
      double winX(pixelLUT[i].pixel - &pixelLUT[i].buffer->pixels[0]);
      double winY((double)(0));
      double winZ(*(pixelLUT[i].pixel));
      GLHelper::getGLH()->unProject(winX, winY, winZ, unProjectMatrix, buffer.viewPort, &objectPos.v[0], &objectPos.v[1], &objectPos.v[2]);
      sensorData[sensorDataSize-1-i] = (objectPos-position).getLength();
      if(sensorData[sensorDataSize-1-i] > maxRange)
        sensorData[sensorDataSize-1-i] = maxRange;
    }
  }
}

SimObject* DistanceSensor::clone() const
{
  DistanceSensor* newDistanceSensor = new DistanceSensor();
  newDistanceSensor->copyStandardMembers(this);
  newDistanceSensor->sensorDataSize = sensorDataSize;
  newDistanceSensor->sensorReading = sensorReading;
  newDistanceSensor->sensorReading.data.doubleArray = new double[sensorDataSize];
  newDistanceSensor->standardRenderingBuffer = standardRenderingBuffer;
  std::list<SimObject*>::const_iterator pos;
  for(pos = childNodes.begin(); pos != childNodes.end(); ++pos)
  {
    SimObject* childNode = (*pos)->clone();
    newDistanceSensor->addChildNode(childNode, false);
  }
  SimObject* newObject = newDistanceSensor;
  return newObject;
}

void DistanceSensor::drawObject(const VisualizationParameterSet& visParams)
{
  if(visParams.visualizeSensors && !visParams.drawForSensor)
  {
    glColor3d(visualizationColor.v[0],
      visualizationColor.v[1],
      visualizationColor.v[2]);
    Vector3d camPos(position);
    if(projection == SPHERICAL_PROJECTION)
    {
      glBegin(GL_LINES);
        for(unsigned int i=0; i< sensorDataSize; i++)
        {
          Vector3d distanceVec(sensorReading.data.doubleArray[i],0,0);
          double maxAngle(angleX);
          double zRotation(maxAngle/(double)resolutionX);
          zRotation *= (double)i;
          zRotation -= maxAngle/2.0;
          distanceVec.rotateZ(zRotation);
          distanceVec.rotate(rotation);
          distanceVec+=position;
          glVertex3d (camPos.v[0], camPos.v[1], camPos.v[2]);
          glVertex3d (distanceVec.v[0], distanceVec.v[1], distanceVec.v[2]);
        }
      glEnd();
    }
    else
    {
      Vector3d distanceVec(sensorReading.data.doubleArray[0],0,0);
      distanceVec.rotate(rotation);
      distanceVec+=position;
      //Draw distance vector
      glBegin(GL_LINES);
        glVertex3d (camPos.v[0], camPos.v[1], camPos.v[2]);
        glVertex3d (distanceVec.v[0], distanceVec.v[1], distanceVec.v[2]);
      glEnd();
    }
  }
}

void DistanceSensor::postProcessAfterParsing()
{
  if(projection == SPHERICAL_PROJECTION)
  {
    pixelLUT.resize(sensorDataSize);
    if(angleX <= Functions::toRad(120)) //All pixels will be in the front buffer
    {
      //Compute new resolution of rendering buffer
      double maxAngle(angleX/2.0);
      double minPixelWidth(tan(maxAngle/((double)resolutionX/2.0)));
      double totalWidth(tan(maxAngle));
      double newXRes(totalWidth/minPixelWidth);
      standardRenderingBuffer.resolutionX = (unsigned int)(ceil(newXRes))*2;
      standardRenderingBuffer.reset();
      standardRenderingBuffer.pixels = new float[standardRenderingBuffer.resolutionX];
      //Compute values for LUT (sensor data -> rendering buffer)
      double firstAngle(-maxAngle);
      double step(maxAngle/((double)resolutionX/2.0));
      double currentAngle(firstAngle);
      double gToPixelFactor(newXRes/tan(maxAngle));
      for(unsigned int i=0; i<(unsigned int)resolutionX; i++)
      {
        pixelLUT[i].buffer = &standardRenderingBuffer;
        double g(tan(currentAngle));
        g *= gToPixelFactor;
        int gPixel((int)g + (int)standardRenderingBuffer.resolutionX/2);
        pixelLUT[i].pixel = &(standardRenderingBuffer.pixels[gPixel]);
        currentAngle += step;
      }
    }
    else
    {
      assert(false); //Not implemented yet :-(
    }
  }
  else //projection == PERSPECTIVE_PROJECTION
  {
    standardRenderingBuffer.reset();
    standardRenderingBuffer.pixels = new float[sensorDataSize];
  }
}
