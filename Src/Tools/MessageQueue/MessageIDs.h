/**
* @file MessageIDs.h
*
* Declaration of ids for debug messages.
* 
* @author Martin Lötzsch
*/

#ifndef __MessageIDs_h_
#define __MessageIDs_h_

/** 
* IDs for debug messages
*
* To distinguish debug messages, they all have an id.
*/
enum MessageID
{
  undefined = 0,
  idProcessBegin,
  idProcessFinished,
    
  // data (ids should remain constant over code changes, so old log files will still work)
  idImage,
  idJPEGImage,
  idJointData,
  idSensorData,
  idKeyStates,
  idOdometryData,
  idFrameInfo,
  idFilteredJointData,
  idLinePercept,
  idGoalPercept,
  idBallPercept,
  idGroundTruthBallModel,
  idGroundTruthRobotPose,
  idExtendedBallPercepts,
  idCameraMatrix,
  idImageCoordinateSystem,
  idBoardInfo,
  idRobotPose,
  idBallModel,
  idFilteredSensorData,
  // insert new data ids here
  numOfDataMessageIDs, /**< everything below this does not belong into log files */
  
  // ids used in team communication
  idReceiveTimeStamp = numOfDataMessageIDs,
  idSendTimeStamp,
  idRobot,
  idNTPRequest,
  idNTPResponse,
  idReleaseOptions,
  idStopwatch,
  idTeamMateBallModel,
  idTeamMateRobotPose,
  idTeamMateBehaviorData,
  idRobotHealth,
  idMotionRequest,
  idTeamMateGoalPercept,
  // insert new team comm ids here

  // infrastructure
  idText,
  idDebugRequest,
  idDebugResponse,
  idDebugDataResponse,
  idDebugDataChangeRequest,
  idStreamSpecification,
  idModuleTable,
  idModuleRequest,
  idQueueFillRequest,
  idLogResponse,
  idDrawingManager,
  idDrawingManager3D,
  idDebugImage,
  idDebugJPEGImage,
  idDebugGrayScaleImage,
  idDebugColorClassImage,
  idDebugDrawing,
  idDebugDrawing3D,
  idColorTable64,
  idWriteColorTable64,
  idXabslDebugRequest,
  idXabslDebugMessage,
  idXabslDebugSymbols,
  idXabslIntermediateCode,
  idMotionNet,
  idJointRequest,
  idLEDRequest,
  idPlot,
  idConsole,
  idRobotname,
  idRobotDimensions,
  idJointCalibration,

  numOfMessageIDs /**< the number of message ids */
};

#ifdef USE_GETMESSAGEIDNAME
/** returns names for message ids */
static const char* getMessageIDName(MessageID id)
{
  switch (id)
  {
  case idProcessBegin: return "ProcessBegin";
  case idProcessFinished: return "ProcessFinished";

  case undefined: return "undefined";
  case idImage: return "Image";
  case idJPEGImage: return "JPEGImage";
  case idJointData: return "JointData";
  case idSensorData: return "SensorData";
  case idKeyStates: return "KeyStates";
  case idOdometryData: return "OdometryData";
  case idFrameInfo: return "FrameInfo";
  case idFilteredJointData: return "FilteredJointData";
  case idLinePercept: return "LinePercept";
  case idGoalPercept: return "GoalPercept";
  case idBallPercept: return "BallPercept";
  case idGroundTruthBallModel: return "GroundTruthBallModel";
  case idGroundTruthRobotPose: return "GroundTruthRobotPose";
  case idExtendedBallPercepts: return "ExtendedBallPercepts";
  case idCameraMatrix: return "CameraMatrix";
  case idImageCoordinateSystem: return "ImageCoordinateSystem";
  case idBoardInfo: return "BoardInfo";
  case idRobotPose: return "RobotPose";
  case idBallModel: return "BallModel";
  case idFilteredSensorData: return "FilteredSensorData";

  case idReceiveTimeStamp: return "ReceiveTimeStamp";
  case idSendTimeStamp: return "SendTimeStamp";
  case idRobot: return "Robot";
  case idNTPRequest: return "NTPRequest";
  case idNTPResponse: return "NTPResponse";
  case idReleaseOptions: return "ReleaseOptions";
  case idStopwatch: return "Stopwatch";
  case idTeamMateBallModel: return "TeamMateBallModel";
  case idTeamMateRobotPose: return "TeamMateRobotPose";
  case idTeamMateBehaviorData: return "TeamMateBehaviorData";
  case idRobotHealth: return "RobotHealth";
  case idMotionRequest: return "MotionRequest";
  case idTeamMateGoalPercept: return "TeamMateGoalPercept";

  case idText: return "Text";
  case idDebugRequest: return "DebugRequest";
  case idDebugResponse: return "DebugResponse";
  case idDebugDataResponse: return "DebugDataResponse";
  case idDebugDataChangeRequest: return "DebugDataChangeRequest";
  case idStreamSpecification: return "StreamSpecification";
  case idModuleTable: return "ModuleTable";
  case idModuleRequest: return "ModuleRequest";
  case idQueueFillRequest: return "QueueFillRequest";
  case idLogResponse: return "LogResponse";
  case idDrawingManager: return "DrawingManager";
  case idDrawingManager3D: return "DrawingManager3D";
  case idDebugImage: return "DebugImage"; 
  case idDebugJPEGImage: return "idDebugJPEGImage";
  case idDebugGrayScaleImage: return "DebugGrayScaleImage";
  case idDebugColorClassImage: return "DebugColorClassImage"; 
  case idDebugDrawing: return "DebugDrawing";
  case idDebugDrawing3D: return "DebugDrawing3D";
  case idColorTable64: return "ColorTable64";
  case idWriteColorTable64: return "WriteColorTable64";
  case idXabslDebugRequest: return "XabslDebugRequest";
  case idXabslDebugMessage: return "XabslDebugMessage";
  case idXabslIntermediateCode: return "XabslIntermediateCode";
  case idMotionNet: return "MotionNet";
  case idJointRequest: return "JointRequest";
  case idLEDRequest: return "LEDRequest";
  case idPlot: return "Plot";
  case idConsole: return "Console";
  case idRobotname: return "Robotname";
  case idRobotDimensions: return "RobotDimensions";
  case idJointCalibration: return "JointCalibration";

  default: return "unknown";
  }
}
#endif

#endif //__MessageIDs_h_
