/**
 * @file Actuator.h
 * 
 * Definition of class Actuator
 *
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 */ 

#ifndef ACTUATOR_H_
#define ACTUATOR_H_

#include <Simulation/SimObject.h>

/**
* @class Actuator
*
* An abstract class representing an actuator
*/
class Actuator : public virtual SimObject
{
public:
  /** Returns a string describing the kind of object
  * @return The kind
  */
  virtual std::string getKind() const {return "actuator";}

  /** Sets the value of one actuatorport
  * @param value The value
  * @param port The port
  */
  virtual void setValue(double value, int port) {}

  /** Let the actuator do what it is supposed to do
   * @param initial Is this the initial step?
   */
  virtual void act(bool initial = false) {}
};

#endif //ACTUATOR_H_
