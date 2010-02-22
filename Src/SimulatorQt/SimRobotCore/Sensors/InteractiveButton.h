/**
 * @file InteractiveButton.h
 * 
 * Definition of class InteractiveButton
 *
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 */ 

#ifndef INTERACTIVE_BUTTON_H_
#define INTERACTIVE_BUTTON_H_

#include "Sensor.h"


/**
* @class InteractiveButton
*
* A class representing a button which can be
* clicked by the user in any scene view.
*/
class InteractiveButton : public Sensor
{
protected:
  /** The current value of the button (true/false)*/
  bool value;
  /** Flag: Button keeps activated after click, if true*/
  bool snapIn;

public:
  /** Constructor */
  InteractiveButton();

  /** Constructor
  * @param attributes The attributes of the interactive button
  */
  InteractiveButton(const AttributeSet& attributes);

  /** Returns a string describing the kind of object
  * @return The kind
  */
  virtual std::string getKind() const {return "interactiveButton";}
  
  /** Returns the current value of the button
  * @param value The value
  * @param portId The id of the sensorport
  */
  virtual void computeValue(bool& value, int portId);
  
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
  
  /** Checks if the object is movable or part of a movable object or
  *   part of an interactive button
  *   (needed for interaction)
  * @param object A pointer to this object will be copied to object
  * @param testClickable Flag: Also test for interactive buttons, if true
  * @return true, if the object is part of a movable object or an interactive button
  */
  virtual bool isMovableOrClickable(SimObject*& object, 
                                    bool testClickable);

  /** Sets the value of the snapIn flag
  * @param snapIn The new value of snapIn 
  */
  void setSnapIn(bool snapIn) {this->snapIn = snapIn;}

  /** Informs the button about being clicked*/
  void hasBeenClicked();

  /** Informs the button about being released*/
  void hasBeenReleased();

  /** 
   * Returns a pointer to the interactive button. This is used to prevent dynamic downcasts from sensor 
   * objects to interactive button objects   
   * @ return A pointer to the interactive button   */
  InteractiveButton* castToInteractiveButton(){return this;};

};

#endif //INTERACTIVE_BUTTON_H_
