/**
 * @file DistanceSensor.h
 * 
 * Definition of class DistanceSensor
 *
 * @author <A href="mailto:Tim.Laue@dfki.de">Tim Laue</A>
 */ 

#ifndef DISTANCE_SENSOR_H_
#define DISTANCE_SENSOR_H_

#include "ImagingSensor.h"


template <typename T>
class ImagingBuffer
{
public:
  ImagingBuffer():pixels(0)
  {}

  ~ImagingBuffer()
  {
    reset();
  }

  void operator=(const ImagingBuffer<T>& other)
  {
    reset();
    openingAngleX = other.openingAngleX;
    openingAngleY = other.openingAngleY;
    resolutionX = other.resolutionX;
    resolutionY = other.resolutionY;
    pixels = new T[resolutionX*resolutionY];
  }

  void reset()
  {
    if(pixels)
    {
      delete[] pixels;
      pixels = 0;
    }
  }

  T* pixels;

  double modelViewMatrix[16];
  double projectionMatrix[16];
  int viewPort[4];
  double openingAngleX;
  double openingAngleY;
  unsigned int resolutionX;
  unsigned int resolutionY;
};


template <typename T>
class PixelLocation
{
public:
  /** A direct pointer to the pixel*/
  T* pixel;
  /** A pointer to the buffer containing the pixel*/
  ImagingBuffer<T>* buffer;
};


/**
* @class DistanceSensor
*
* A class representing a distance sensor (e.g. a laser scanner).
*/
class DistanceSensor : public ImagingSensor
{
protected:
  /** The computed sensor data*/
  SensorReading sensorReading;
  /** The standard rendering buffer*/
  ImagingBuffer<float> standardRenderingBuffer;
  /** A rendering buffer for spherical projection*/
  ImagingBuffer<float> leftRenderingBuffer;
  /** A rendering buffer for spherical projection*/
  ImagingBuffer<float> rightRenderingBuffer;
  /** The size of the sensor data array*/
  unsigned int sensorDataSize;
  /** A look-up table containing the addresses of all pixel values*/
  std::vector< PixelLocation<float> > pixelLUT;

  /** Scales the computed values to "real" distances*/
  void transformDataToDistances(ImagingBuffer<float>& buffer);

  /** Executes the rendering of the scene to a buffer*/
  void renderingInstructions(ImagingBuffer<float>& buffer);

public:
  /** Constructor*/
  DistanceSensor();

  /** Constructor
  * @param attributes The attributes of the distance sensor
  */
  DistanceSensor(const AttributeSet& attributes);

  /** Destructor */
  ~DistanceSensor();

  /** Returns a string describing the kind of object
  * @return The kind
  */
  virtual std::string getKind() const {return "distance-sensor";}
  
  /** Sets the resolution and initializes the sensor
  * @param x The new X-resolution
  * @param y The new Y-resolution
  */
  void setResolution(int x, int y);

  /** Adds the object to some internal lists
  * @param sensorPortList A list of all sensor ports in the scene
  * @param actuatorPortList A list of all actuator ports in the scene
  * @param actuatorList A list of all actuators in the scene
  */
  virtual void addToLists(std::vector<SensorPort>& sensorPortList,
                          std::vector<Actuatorport*>& actuatorPortList,
                          std::vector<Actuator*>& actuatorList);

  /** Adds descriptions of the object and its children to a list
  * @param objectDescriptionTree The list of descriptions
  * @param The depth of the object in the object tree
  */
  virtual void addToDescriptions(std::vector<ObjectDescription>& objectDescriptionTree,
    int depth);

  /** Clones the object and its child nodes
  * @return A pointer to a copy of the object
  */
  virtual SimObject* clone() const;

  /** Returns a reference to the result of the sensor data computation
   * @param localPortId The local sensor port
   * @return The reference
   */
  virtual SensorReading& getSensorReading(int localPortId);

  /** Draws the computed distances
  * @param visParams Parameters for Visualization
  */
  virtual void drawObject(const VisualizationParameterSet& visParams);

  /** Do internal computations after parsing.
  */
  virtual void postProcessAfterParsing();
};

#endif //DISTANCESENSOR_H_


