/**
* @file FallDownStateDetector.h
*
* This file declares a module that provides information about the current state of the robot's body.
*
* @author <a href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</a>
*/

#ifndef __FallDownStateDetector_h_
#define __FallDownStateDetector_h_

#include "Representations/Infrastructure/SensorData.h"
#include "Representations/Modeling/FallDownState.h"
#include "Tools/Module/Module.h"
#include "Tools/RingBufferWithSum.h"


MODULE(FallDownStateDetector)
  REQUIRES(FilteredSensorData)
  PROVIDES_WITH_MODIFY(FallDownState)
END_MODULE


/**
* @class FallDownStateDetector
*
* A module for computing the current body state from sensor data
*/
class FallDownStateDetector: public FallDownStateDetectorBase
{
private:
  /** Executes this module
  * @param fallDownState The data structure that is filled by this module
  */
  void update(FallDownState& fallDownState);

  /** Checks, if the sensor data is valid
  * @param avgX average acceleration in x direction
  * @param avgY average acceleration in y direction
  * @param avgZ average acceleration in z direction
  * @return true, if data is valid
  */
  bool sensorAvailable(double avgX, double avgY, double avgZ) const;

  /** Indices for buffers of sensor data */
	enum BufferEntries{accX = 0, accY, accZ, numOfBuffers};

	/** Buffers for averaging sensor data */
	RingBufferWithSum<double,5> buffers[numOfBuffers];

};

#endif// __FallDownStateDetector_h_
