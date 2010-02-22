/**
 * @file Controller/View.h
 * 
 * Definition of class View
 *
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 */ 

#ifndef SIMROBOTVIEW_H_
#define SIMROBOTVIEW_H_

#include <string>

#include "../Sensors/Sensor.h"

/**
 * @class View
 * 
 * A class for visualizing extra information
 * (Not implemented yet)
 */ 
class View : public Sensor
{
private:
  /** Fake sensor data, but contains sensor type */
  SensorReading viewSensorReading;

public:
  /**
  * All possible mouse events.
  */
  enum MouseEvent
  {
    leftButtonDown,
    leftButtonDrag,
    leftButtonUp,
    mouseMove,
  };

  /**
  * The keys that can be pressed during a mouse event.
  * @attention Each key requires a separate bit.
  */
  enum Key
  {
    none,
    control = 1,
    shift = 2
  };

  /** Updates the current view*/
  virtual void update() {};

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
  void addToDescriptions(std::vector<ObjectDescription>& objectDescriptionTree,
                         int depth);

  /** Returns a string describing the kind of object
  * @return The kind
  */
  virtual std::string getKind() const {return "view";}

  /** Returns a reference to the fake sensor reading
   * @param localPortId The local sensor port
   * @return The reference
   */
  virtual SensorReading& getSensorReading(int localPortId)
  {
    viewSensorReading.sensorType = SensorReading::viewSensor;
    return viewSensorReading;
  }

  /* Needs the view to be repainted?
  * @return Yes or no? 
  */
  //virtual bool needsRepaint() const {return true;}

  /* 
  * Uses the view OpenGL for displaying information?
  * @return Yes or no? 
  */
  //virtual bool isOpenGLView() const {return false;}

  /**
  * The function handles a mouse event.
  * @param event The event.
  * @param x The x coordinate of the cursor when the event occured.
  * @param y The y coordinate of the cursor when the event occured.
  * @param keys The set of keys pressed when the event occured.
  * @return Does the view need to be repainted?
  */
  virtual bool handleMouseEvent(MouseEvent event, int x, int y, int keys) {return false;}

  /**
  * The function is called to return a string that contains the configuration of the view.
  * @return A text string containing all information to restore the internal state of the view.
  */
  virtual std::string getConfiguration() const {return "";}

  /**
  * The function is called to restore the configuration of the view.
  * @param A text string containing all information to restore the internal state of the view.
  */
  virtual void setConfiguration(const std::string& configuration) {}
};

#endif //SIMROBOTVIEW_H_
