/**
 * @file Actuatorport.h
 * 
 * Definition of class Actuatorport
 *
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 */ 

#ifndef ACTUATORPORT_H_
#define ACTUATORPORT_H_

#include <string>
#include <Actuators/Actuator.h>


/**
* @class Actuatorport
*
* A class representing an actuatorport
*/
class Actuatorport
{
private:
  /** The name of the port*/
  std::string name;
  /** A pointer to the actuator*/
  Actuator* actuator;
  /** The id of the port*/
  int portId;
  /** The minimum value to be set*/
  double minValue;
  /** The maximum value to be set*/
  double maxValue;

public:
  /** Constructor
  * @param name The name of the actuatorport
  * @param actuator A pointer to the actuator
  * @param portId The id of the port
  * @param minValue The minimum value to be set
  * @param maxValue The maximum value to be set
  */
  Actuatorport(const std::string& name, Actuator* actuator, int portId,
               double minValue, double maxValue);

  /** Returns the name
  * @return The name
  */
  std::string getName() const {return name;}

  /** Sets the value of the actuatorport
  * @param value The value
  */
  void setValue(double value);

  /** Returns the minimum value to be set
  * @return The value
  */ 
  double getMinValue() const {return minValue;}

  /** Returns the maximum value to be set
  * @return The value
  */ 
  double getMaxValue() const {return maxValue;}
};

#endif //ACTUATORPORT_H_
