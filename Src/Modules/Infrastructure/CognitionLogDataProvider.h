/** 
* @file CognitionLogDataProvider.h
* This file declares a module that provides data replayed from a log file.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef __CognitionLogDataProvider_h_
#define __CognitionLogDataProvider_h_

#include "Tools/Module/Module.h"
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Perception/LinePercept.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Perception/GoalPercept.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/ImageCoordinateSystem.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/BallModel.h"
#include "Tools/MessageQueue/InMessage.h"
#include "Tools/Debugging/DebugImages.h"

MODULE(CognitionLogDataProvider)
  PROVIDES_WITH_OUTPUT(Image)
  PROVIDES(CameraInfo)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(FrameInfo)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(LinePercept)
  PROVIDES_WITH_MODIFY_AND_OUTPUT_AND_DRAW(BallPercept)
  PROVIDES_WITH_MODIFY_AND_OUTPUT_AND_DRAW(GoalPercept)
  PROVIDES_WITH_MODIFY_AND_OUTPUT_AND_DRAW(RobotPose)
  PROVIDES_WITH_MODIFY_AND_OUTPUT_AND_DRAW(BallModel)
  PROVIDES_WITH_MODIFY_AND_OUTPUT_AND_DRAW(GroundTruthRobotPose)
  PROVIDES_WITH_MODIFY_AND_OUTPUT_AND_DRAW(GroundTruthBallModel)
  PROVIDES_WITH_MODIFY_AND_OUTPUT_AND_DRAW(CameraMatrix)
  REQUIRES(Image)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(ImageCoordinateSystem)
END_MODULE

class CognitionLogDataProvider : public CognitionLogDataProviderBase
{
private:
  PROCESS_WIDE_STORAGE_STATIC CognitionLogDataProvider* theInstance; /**< Points to the only instance of this class in this process or is 0 if there is none. */
  bool frameDataComplete; /**< Were all messages of the current frame received? */
  Streamable* representationBuffer[numOfDataMessageIDs]; /**< The array of all logable representations. */

  DECLARE_DEBUG_IMAGE(corrected);
  
  void update(Image& image);
  void update(CameraInfo& cameraInfo) {}
  void update(FrameInfo& frameInfo);
  void update(LinePercept& linePercept);
  void update(GoalPercept& goalPercept);
  void update(BallPercept& ballPercept);
  void update(CameraMatrix& cameraMatrix);
  void update(ImageCoordinateSystem& imageCoordinateSystem);
  void update(BallModel& ballModel);
  void update(RobotPose& robotPose);
  void update(GroundTruthBallModel& groundTruthBallModel);
  void update(GroundTruthRobotPose& groundTruthRobotPose);

  /** 
  * The method is called for every incoming debug message by handleMessage.
  * @param message An interface to read the message from the queue.
  * @return Was the message handled?
  */
  bool handleMessage2(InMessage& message);

public:
  /**
  * Default constructor.
  */
  CognitionLogDataProvider();

  /**
  * Destructor.
  */
  ~CognitionLogDataProvider();

  /** 
  * The method is called for every incoming debug message.
  * @param message An interface to read the message from the queue.
  * @return Was the message handled?
  */
  static bool handleMessage(InMessage& message);

  /** 
  * The method returns whether idProcessFinished was received.
  * @return Were all messages of the current frame received?
  */
  static bool isFrameDataComplete();
};

#endif // __CognitionLogDataProvider_h_
