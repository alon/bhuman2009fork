/**
* @file Representations/Infrastructure/JointData.h
*
* This file declares a class to represent the joint angles sent to the robot.
*
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
*/

#ifndef __JointData_H__
#define __JointData_H__

#include <string.h>
#include "Tools/Streams/Streamable.h"
#include "Tools/Streams/InStreams.h"
#include "Tools/Math/Common.h"
#include "Tools/Debugging/Debugging.h"
#include "Platform/GTAssert.h"

/**
* @class JointData
* A class to represent the joint angles sent to the robot.
*/
class JointData : public Streamable
{
protected:
  virtual void serialize(In* in, Out* out)
  {  
    STREAM_REGISTER_BEGIN();
    STREAM_ARRAY(angles);
    STREAM(timeStamp);
    STREAM_REGISTER_FINISH();
  }

public:
  enum Joint
  {
    headPan,
    headTilt,
    armLeft0,
    armLeft1,
    armLeft2,
    armLeft3,
    armRight0,
    armRight1,
    armRight2,
    armRight3,
    legLeft0,
    legLeft1,
    legLeft2,
    legLeft3,
    legLeft4,
    legLeft5,
    legRight0,
    legRight1,
    legRight2,
    legRight3,
    legRight4,
    legRight5,
    numOfJoints
  };


  // If you change those values be sure to change them in MofCompiler.cpp too. (Line ~280)
  enum 
  {
    off    = 1000, /**< Special angle for switching off a joint. */
    ignore = 2000  /**< Special angle for not overwriting the previous setting. */
  };
  double angles[numOfJoints]; /**< The angles of all joints. */
  unsigned timeStamp; /**< The time when these angles were received. */

  /**
  * Default constructor.
  * Switches off all joints.
  */
  JointData() : timeStamp(0)
  {
    for(int i = 0; i < numOfJoints; ++i)
      angles[i] = off;
  }

  /**
  * The method returns the angle of the mirror (left/right) of the given joint.
  * @param joint The joint the mirror of which is returned.
  * @return The angle of the mirrored joint.
  */
  double mirror(Joint joint) const
  {
    switch (joint)
    {
      // don't mirror an invalid join value (!)
      case headPan: return angles[headPan] == off || angles[headPan] == ignore ? angles[headPan]: -angles[headPan];
      case armLeft0: return angles[armRight0]; 
      case armLeft1: return angles[armRight1]; 
      case armLeft2: return angles[armRight2]; 
      case armLeft3: return angles[armRight3]; 
      case armRight0: return angles[armLeft0]; 
      case armRight1: return angles[armLeft1]; 
      case armRight2: return angles[armLeft2]; 
      case armRight3: return angles[armLeft3]; 
      case legLeft0: return angles[legRight0]; 
      case legLeft1: return angles[legRight1]; 
      case legLeft2: return angles[legRight2]; 
      case legLeft3: return angles[legRight3]; 
      case legLeft4: return angles[legRight4]; 
      case legLeft5: return angles[legRight5]; 
      case legRight0: return angles[legLeft0]; 
      case legRight1: return angles[legLeft1]; 
      case legRight2: return angles[legLeft2]; 
      case legRight3: return angles[legLeft3]; 
      case legRight4: return angles[legLeft4]; 
      case legRight5: return angles[legLeft5];
      default: return angles[joint];
    }
  }

  /**
  * The method initializes the joint angles as a mirror of a set of other joint angles.
  * @param other The set of joint angles that are mirrored.
  */
  virtual void mirror(const JointData& other)
  {
    for(int i = 0; i < numOfJoints; ++i)
      angles[i] = other.mirror((Joint) i);
    timeStamp = other.timeStamp;
  }

  /**
  * The function returns the name of a joint.
  * @param joint The joint the name of which will be returned.
  * @return The name.
  */
  static const char* getJointName(Joint joint)
  {
    switch(joint)
    {
    case headPan: return "headPan";
    case headTilt: return "headTilt";
    case armLeft0: return "armLeft0";
    case armLeft1: return "armLeft1";
    case armLeft2: return "armLeft2";
    case armLeft3: return "armLeft3";
    case armRight0: return "armRight0";
    case armRight1: return "armRight1";
    case armRight2: return "armRight2";
    case armRight3: return "armRight3";
    case legLeft0: return "legLeft0";
    case legLeft1: return "legLeft1";
    case legLeft2: return "legLeft2";
    case legLeft3: return "legLeft3";
    case legLeft4: return "legLeft4";
    case legLeft5: return "legLeft5";
    case legRight0: return "legRight0";
    case legRight1: return "legRight1";
    case legRight2: return "legRight2";
    case legRight3: return "legRight3";
    case legRight4: return "legRight4";
    case legRight5: return "legRight5";
    default:
      return "unknown";
    }
  }
};

/**
* @class JointDataDeg
* A class that wraps joint data to be transmitted in degrees.
*/
class JointDataDeg : public JointData
{
private:
  virtual void serialize(In* in, Out* out)
  {  
    STREAM_REGISTER_BEGIN();
    if(jointData)
    {
      ASSERT(out);
      for(int i = 0; i < JointData::numOfJoints; ++i)
        angles[i] = jointData->angles[i] == JointData::off ? JointData::off
                    : floor(toDegrees(jointData->angles[i]) * 10.0 + 0.5) / 10.0;
      timeStamp = jointData->timeStamp;

      STREAM_ARRAY(angles);
      STREAM(timeStamp);
    }
    else
    {
      STREAM_BASE(JointData);
    }
    STREAM_REGISTER_FINISH();
  }

  JointData* jointData; /**< The joint data that is wrapped. */

public:
  /**
  * Default constructor.
  */
  JointDataDeg() : jointData(0) {}

  /**
  * Constructor.
  * @param jointData The joint data that is wrapped.
  */
  JointDataDeg(JointData& jointData) : jointData(&jointData) {}

  /**
  * Assignment operator.
  */
  JointDataDeg& operator=(const JointDataDeg& other)
  {
    if(jointData)
      for(int i = 0; i < JointData::numOfJoints; ++i)
        jointData->angles[i] = other.angles[i] == JointData::off ? JointData::off
                              : fromDegrees(other.angles[i]);
    else
      *((JointData*) this) = other;
    return *this;
  }
};

/**
 * @class HardnessData
 * This class represents the joint hardness in a jointRequest.
 * It loads the default hardness values from jointHardness.cfg.
 */
class HardnessData : public Streamable
{
public:
  int hardness[JointData::numOfJoints]; /**< the custom hardness for each joint */
  int defaultHardness[JointData::numOfJoints]; /**< the default hardness for each joint */

  enum Hardness
  {
    useDefault = -1,
  };
  
  /**
   * Default Constructor
   */
  HardnessData()
  {
    resetToDefault();
    
    InConfigFile stream("jointHardness.cfg");
    if(stream.exists())
    {
      for(int i = 0; i < JointData::numOfJoints ; i++)
        stream >> defaultHardness[i];
    }
    else
    {
      OUTPUT(idText, text, "JointRequest.hardness : Could not find jointHardness.cfg, using a default value of 90 for all joints!");
      for(int i = 0; i < JointData::numOfJoints ; i++)
        defaultHardness[i] = 90;
      ASSERT(false);
    }
  }

  /**
  * The method returns the OUTPUT( getHardness(joint)!!!) hardness of the mirror (left/right) of the given joint.
  * @param joint The joint the mirror of which is returned.
  * @return The output hardness of the mirrored joint.
  */
  int mirror(const JointData::Joint joint) const
  {
    switch (joint)
    {
      // don't mirror an invalid join value (!)
      case JointData::headPan: return getHardness(JointData::headPan);
      case JointData::armLeft0: return getHardness(JointData::armRight0); 
      case JointData::armLeft1: return getHardness(JointData::armRight1); 
      case JointData::armLeft2: return getHardness(JointData::armRight2); 
      case JointData::armLeft3: return getHardness(JointData::armRight3); 
      case JointData::armRight0: return getHardness(JointData::armLeft0); 
      case JointData::armRight1: return getHardness(JointData::armLeft1); 
      case JointData::armRight2: return getHardness(JointData::armLeft2); 
      case JointData::armRight3: return getHardness(JointData::armLeft3); 
      case JointData::legLeft0: return getHardness(JointData::legRight0); 
      case JointData::legLeft1: return getHardness(JointData::legRight1); 
      case JointData::legLeft2: return getHardness(JointData::legRight2); 
      case JointData::legLeft3: return getHardness(JointData::legRight3); 
      case JointData::legLeft4: return getHardness(JointData::legRight4); 
      case JointData::legLeft5: return getHardness(JointData::legRight5); 
      case JointData::legRight0: return getHardness(JointData::legLeft0); 
      case JointData::legRight1: return getHardness(JointData::legLeft1); 
      case JointData::legRight2: return getHardness(JointData::legLeft2); 
      case JointData::legRight3: return getHardness(JointData::legLeft3); 
      case JointData::legRight4: return getHardness(JointData::legLeft4); 
      case JointData::legRight5: return getHardness(JointData::legLeft5);
      default: return getHardness(joint);
    }
  }

   /**
  * The method returns the internal hardness of the mirror (left/right) of the given joint.
  * @param joint The joint the mirror of which is returned.
  * @return The internal hardness of the mirrored joint.
  */
  int mirrorInternal(const JointData::Joint joint) const
  {
    switch (joint)
    {
      // don't mirror an invalid join value (!)
      case JointData::headPan: return hardness[JointData::headPan];
      case JointData::armLeft0: return hardness[JointData::armRight0]; 
      case JointData::armLeft1: return hardness[JointData::armRight1]; 
      case JointData::armLeft2: return hardness[JointData::armRight2]; 
      case JointData::armLeft3: return hardness[JointData::armRight3]; 
      case JointData::armRight0: return hardness[JointData::armLeft0]; 
      case JointData::armRight1: return hardness[JointData::armLeft1]; 
      case JointData::armRight2: return hardness[JointData::armLeft2]; 
      case JointData::armRight3: return hardness[JointData::armLeft3]; 
      case JointData::legLeft0: return hardness[JointData::legRight0]; 
      case JointData::legLeft1: return hardness[JointData::legRight1]; 
      case JointData::legLeft2: return hardness[JointData::legRight2]; 
      case JointData::legLeft3: return hardness[JointData::legRight3]; 
      case JointData::legLeft4: return hardness[JointData::legRight4]; 
      case JointData::legLeft5: return hardness[JointData::legRight5]; 
      case JointData::legRight0: return hardness[JointData::legLeft0]; 
      case JointData::legRight1: return hardness[JointData::legLeft1]; 
      case JointData::legRight2: return hardness[JointData::legLeft2]; 
      case JointData::legRight3: return hardness[JointData::legLeft3]; 
      case JointData::legRight4: return hardness[JointData::legLeft4]; 
      case JointData::legRight5: return hardness[JointData::legLeft5];
      default: return hardness[joint];
    }
  }
  
  /**
   * initializes this instance with the mirrored values of other
   * @param other the HardnessData to be mirrored
   */
  void mirror(const HardnessData& other)
  {
    for(int i = 0; i < JointData::numOfJoints; i++)
      hardness[i] = other.mirrorInternal((JointData::Joint)i);
  }

  /**
   * This function returns the "final" hardness for a joint.
   * @param i joint id
   * @return the hardness of joint i
   */
  inline int getHardness(unsigned i) const
  {
    return hardness[i] != useDefault ? hardness[i] : defaultHardness[i];
  }

  /**
   * This function resets the hardness for all joints to the default value.
   */
  inline void resetToDefault()
  {
    for(int i = 0; i < JointData::numOfJoints; i++)
      hardness[i] = useDefault;
  }
private:
  virtual void serialize(In* in, Out * out)
  {
    STREAM_REGISTER_BEGIN();
      STREAM_ARRAY(hardness);
      STREAM_ARRAY(defaultHardness);
    STREAM_REGISTER_FINISH();
  }
};

/**
 * @class JointRequest
 */
class JointRequest : public JointData
{
  public:
    HardnessData jointHardness; /**< the hardness for all joints*/

  /**
   * Initializes this instance with the mirrored data from a other JointRequest
   * @param other the JointRequest to be mirrored
   */
  void mirror(const JointRequest& other)
  {
    JointData::mirror(other);
    jointHardness.mirror(other.jointHardness);
  }
  
  /**
   * Returns the mirrored angle of joint
   * @param joint the joint to be mirrored
   */
  double mirrorAngle(const JointData::Joint joint)
  {
    return JointData::mirror(joint);
  }

  /**
   * Returns the mirrored hardness of joint
   * @param joint the joint to be mirrored
   */
  int mirrorHardness(const JointData::Joint joint)
  {
    return jointHardness.mirror(joint);
  }
protected:
  virtual void serialize(In* in, Out* out)
  {  
    STREAM_REGISTER_BEGIN();
    STREAM_BASE(JointData);
    STREAM(jointHardness);
    STREAM_REGISTER_FINISH();
  }
};
class FilteredJointData : public JointData {};
class UnstableJointRequest : public JointRequest {};

#endif // __JointData_H__
