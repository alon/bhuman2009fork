/**
 * @file ImagingSensor.h
 * 
 * Definition of abstract class ImagingSensor
 *
 * @author <A href="mailto:Tim.Laue@dfki.de">Tim Laue</A>
 */ 

#ifndef IMAGING_SENSOR_H_
#define IMAGING_SENSOR_H_

#include "Sensor.h"

class OffscreenRenderer;


/**
* @class ImagingSensor
*
* An abstract class representing a sensor
* which generates a multidimensional "image"
* of the environment (e.g. a camera or a laser range finder).
*/
class ImagingSensor : public virtual Sensor
{
protected:
  /** The image width*/
  int resolutionX;
  /** The image height*/
  int resolutionY;
  /** The horizontal opening angle*/
  double angleX;
  /** The vertical opening angle*/
  double angleY;
  /** Flag for special handling of 1D data */
  bool sensorIs1D;
  /** The image aspect */
  double aspect;
  /** The minimum range*/
  double minRange;
  /** The maximum range*/
  double maxRange;
  /** The time in seconds the sensor needs to capture a single image */
  double exposureTime;
  /** the frame buffer object registration identifier */
  int fbo_reg;
  /** The kind of projection*/
  enum Projection {SPHERICAL_PROJECTION, PERSPECTIVE_PROJECTION} projection;
  /** A pointer to an offscreen renderer*/
  OffscreenRenderer* osRenderer;
  /** The color, which is used to draw the sensor*/
  Vector3d visualizationColor;

  /** Draws the camera opening
  * @param visParams Parameters for Visualization
  */
  virtual void drawObject(const VisualizationParameterSet& visParams);
  
public:
  /** Constructor*/
  ImagingSensor();

  /** Constructor
  * @param attributes The attributes of the sensor
  */
  ImagingSensor(const AttributeSet& attributes);

  /** Returns a string describing the kind of object
  * @return The kind
  */
  virtual std::string getKind() const {return "imaging-sensor";}
  
  /** Sets the resolution and initializes the image
  * @param x The new image width
  * @param y The new image height
  */
  virtual void setResolution(int x, int y) {};

  /**
  * Parses the attributes of a simulated object which are
  * defined in seperate elements
  * @param name The name of the attribute element
  * @param attributes The attributes of the element
  * @param line The line in the scene description
  * @param column The column in the scene description
  * @param errorManager An object managing errors
  * @return true, if any element has been parsed
  */
  virtual bool parseAttributeElements(const std::string& name, 
                                      const AttributeSet& attributes,
                                      int line, int column, 
                                      ErrorManager* errorManager);

  /** Copies the members of another imaging sensor object
  * @param other The other object
  */
  void copyStandardMembers(const ImagingSensor* other);
};

#endif //IMAGING_SENSOR_H_
