/*
* @file ControllerQt/RobotConsole.h
*
* Declaration of RobotConsole.
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef __RobotConsole_H__
#define __RobotConsole_H__

#include <SimRobotCore/Controller/Controller.h>
#include <SimRobotCore/Controller/DirectView.h>
#include "Tools/Process.h"
#include <fstream>
#include "Platform/hash_map.h"
#include <list>
#include "Representations/Infrastructure/SensorData.h"
#include "Representations/Infrastructure/JointData.h"
#include "Representations/Infrastructure/LEDRequest.h"
#include "Representations/Infrastructure/RobotHealth.h"
#include "Representations/BehaviorControl/BehaviorData.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Perception/GoalPercept.h"
#include "Representations/Perception/LinePercept.h"
#include "Representations/MotionControl/MotionRequest.h"
#include "Representations/MotionControl/OdometryData.h"
#include "Representations/Configuration/ColorTable64.h"
#include "LogPlayer.h"
#include "Tools/Debugging/DebugImages.h"
#include "Tools/Debugging/DebugDrawings3D.h"
#include "Visualization/DebugDrawing.h"
#include "Visualization/DebugDrawing3D.h"
#include "Representations/ColorTableHandler.h"
#include "Representations/ModuleInfo.h"
#include "Representations/TimeInfo.h"
#include "Representations/XabslInfo.h"

#include "Representations/Configuration/JointCalibration.h"
#include "Representations/Configuration/RobotDimensions.h"

class ConsoleRoboCupCtrl;

/**
* @class RobotConsole
*
* A process that simulates the Aperios RobotConsole.
*/
class RobotConsole : public Process
{
private:
  class Printer : public MessageHandler
  {
  private:
    RobotConsole& console;

  public:
    Printer(RobotConsole& console) : console(console) {}

    bool handleMessage(InMessage& message)
    {
      ASSERT(message.getMessageID() == idDebugDataResponse);
      std::string name,
                  type;
      long flags;
      message.bin >> name >> flags >> type;
      console.printData(message.bin, type.c_str());
      return true;
    }
  };

  /**
  * Writes a Message to a file. Used to save representations as files,
  * i.e. save calibration values set in the simulator
  */
  class Writer : public MessageHandler
  {
  private:
    std::string filename;
    RobotConsole& console;

  public:
    Writer(std::string filename, RobotConsole& console) : filename(filename), console(console) {}

    bool handleMessage(InMessage& message)
    {
      ASSERT(message.getMessageID() == idDebugDataResponse);
      
      std::string name,
        type;
      long flags;
      
      message.bin >> name >> flags >> type;

      std::ofstream outFile;
      outFile.open(filename.c_str());
      
      // Configfile specific startvalues
      if(name == "representation:WalkingParameters" ||
         name == "representation:ExpWalkingParameters3" ||
         name == "representation:sideParameters" ||
         name == "representation:rotationParameters" ||
         name == "representation:backParameters" ||
         name == "representation:expParameters")
        outFile << "[default]" << std::endl;

      console.myPrintData(message.bin, type.c_str(),outFile);

      outFile.close();

      return true;
    }
  };

public:
  DECLARE_SYNC; /**< Make this object synchronizable. */
  ColorTableHandler colorTableHandler; /**< The color table handler. */
  
protected:
  ConsoleRoboCupCtrl* ctrl; /** A pointer to the controller object. */
  std::string robotName; /**< The name of the robot. */
  SystemCall::Mode mode; /**< Defines mode in which this process runs. */
  bool printMessages, /**< Decides whether to output text messages in the console window. */
       handleMessages, /**< Decides whether messages are handled or not. */
       calculateImage, /**< Decides whether images are calculated by the simulator. */
    //logCycle, /**< Decides whether log files are repeated when their end is reached. */
    //logOnce, /**< A flag that reflects whether a log files was already replayed once. */
       logAcknowledged, /**< The flag is true whenever log data sent to the robot code was processed. */
       destructed; /**< A flag stating that this object has already been destructed. */
  std::string logFile; /**< The name of the log file replayed. */
  LogPlayer logPlayer; /**< The log player to record and replay log files. */
  MessageQueue& debugIn, /**< The incoming debug queue. */
              & debugOut; /**< The outgoing debug queue. */
  StreamHandler streamHandler; /**< Local stream handler. Note: Process::streamHandler may be accessed unsynchronized in different thread, so don't use it here. */
  DrawingManager drawingManager;
  DrawingManager3D drawingManager3D;
  DebugRequestTable debugRequestTable;
  const char* pollingFor; /**< The information the console is waiting for. */
  JointData jointRequest; /**< The joint angles request received from the robot code. */
  LEDRequest ledRequest; /**< The led state request received from the robot code. */
  enum MoveOp {noMove, movePosition, moveBoth, moveBallPosition} moveOp; /**< The move operation to perform. */
  Vector3d movePos, /**< The position the robot is moved to. */
           moveRot; /**< The rotation the robot is moved to. */
  RobotPose robotPose; /**< Robot pose from team communication. */
  BallModel ballModel; /**< Ball model from team communication. */
  GoalPercept goalPercept; /**< Goal percept from team communication. */
  LinePercept linePercept; /**< Line percept from team communication. */
  BehaviorData behaviorData; /**< Behavior data from team communication. */
  OdometryData odometryData; /**< Odometry data from logfile. */
  RobotHealth robotHealth; /**< Robot Health from team communication. */
  MotionRequest motionRequest; /**< Motion Request from team communication. */
  unsigned robotPoseReceived, /**< When was the robot pose received from team communication. */
           ballModelReceived, /**< When was the ball model received from team communication. */
           goalPerceptReceived, /**< When was the goal percept received from team communication. */
           linePerceptReceived, /**< When was the line percept received from team communication. */
           robotHealthReceived, /**< When was the robot health received from team communication. */
           motionRequestReceived; /**< When was the motion request received from team communication. */
  SensorData sensorData; /**< The most current sensor data sent. */
  FilteredSensorData filteredSensorData; /**< The received filtered sensor data. */

  JointCalibration jointCalibration; /**< The joint calibration received from the robot code. */
  RobotDimensions robotDimensions; /**< The robotDimensions received from the robot code. */

public:

  class ImagePtr
  {
  public:
    Image* image;
    char processIdentifier;
    ImagePtr() : image(0), processIdentifier(0) {}
    ~ImagePtr() {reset();}
    void reset() {if(image) delete image; image = 0;}
  };
  typedef stdext::hash_map<std::string, ImagePtr> Images; /**< The type of the map of images. */
  Images images; /** All debug images (created on demand). */

  typedef stdext::hash_map<std::string, DebugDrawing> Drawings;
  Drawings imageDrawings, /**< Buffers for image drawings from the debug queue. */
           fieldDrawings; /**< Buffers for field drawings from the debug queue. */
  typedef stdext::hash_map<std::string, DebugDrawing3D> Drawings3D;
  Drawings3D drawings3D; /**< Buffers for 3d drawings from the debug queue. */

  class Plot
  {
  public:
    std::list<double> points;
    unsigned timeStamp;

    Plot() : timeStamp(0) {}
  };

  typedef stdext::hash_map<std::string, Plot> Plots;
  Plots plots; /**< Buffers for plots from the debug queue. */

  class Layer
  {
  public:
    std::string layer;
    std::string description;
    ColorRGBA color;

    bool operator==(const Layer& other) const {return layer == other.layer;}
  };

  typedef stdext::hash_map<std::string, std::list<Layer> > PlotViews;
  PlotViews plotViews; /**< The map of all plot views. */

  typedef stdext::hash_map<std::string, std::list<std::string> > Views;
  Views fieldViews, /**< The map of all field views. */
        imageViews, /**< The map of all image views. */
        imageViews3D; /**< The map of all 3-D image views. */
  ModuleInfo moduleInfo; /**< The current state of all solution requests. */

private:
  typedef std::pair<std::string, MessageQueue*> DebugDataInfoPair; /**< The type of the information on a debug data entry. */
  typedef stdext::hash_map<std::string, DebugDataInfoPair> DebugDataInfos; /**< The type of the map debug data. */
  DebugDataInfos debugDataInfos; /** All debug data information. */

  Images incompleteImages; /** Buffers images of this frame (created on demand). */
  Drawings incompleteImageDrawings, /**< Buffers incomplete image drawings from the debug queue. */
           incompleteFieldDrawings; /**< Buffers incomplete field drawings from the debug queue. */
  Drawings3D incompleteDrawings3D; /**< Buffers incomplete 3d drawings from the debug queue. */

  XabslInfo xabslInfo; /**< Xabsl symbols. */
  bool xabslError; /**< A flag stating that the loaded behavior is different from the one on the robot. */
  std::fstream* logMessages; /** The file messages from the robot are written to. */
  TimeInfo timeInfo; /**< Information about the timing of modules. */
  JointData jointData; /**< The most current set of joint angles sent. */
  Vector3<float> background; /**< The background color of all 3-D views. */
  unsigned lastColorTableSent; /**< The last time the color table was sent. */
  int colorTableSendDelay; /**< The number of ms between sending the color table. */
  std::list<std::string> lines; /**< Console lines buffered because the process is currently waiting. */
  int waitingFor[numOfMessageIDs]; /**< Each entry states for how many information packages the process waits. */
  bool polled[numOfMessageIDs]; /**< Each entry states whether certain information is up-to-date (if not waiting for). */
  bool updateCompletion, /**< Determines whether the tab-completion table has to be updated. */
       directMode, /**< Console is in direct mode, not replaying a script. */
       logImagesAsJPEGs; /**< Compress images before they are stored in a log file. */

  //Joystick
  static const int joystickAxisNumber = 6; /**< Max number of supported axis. */
  static const int joystickButtonNumber = 40; /**< Max number of supported buttons. */
  bool joystickTrace; /**< Display joystick commands when executed. */
  int motionCommandIndices[joystickAxisNumber]; /**< Indices for the sequence of $x, $y, and $r in motionCommand. */
  double motionCommandMaxSpeeds[joystickAxisNumber], /**< The maximum speeds in dimensions x, y, and rotation. */
         motionCommandThresholds[joystickAxisNumber]; /**< The thresholds below which joystick commands are suppressed. */
  unsigned joystickLastTime; /**< The last time when joystick commands were handled. */
  int joystickID; /**< The id of the joystick used by this robot. */
  std::string motionCommand; /**< The pattern for the motion command. */
  std::string joystickLastMotionCommand; /**< The last joystick commands calculated. */
  bool joystickExecCommand(const std::string&); /**< Exec command and optionally output trace to console. */
  signed short joystickAxisValue[joystickAxisNumber]; /**< The axis value memory. Updated by events in handleJoystick(). */
  bool joystickButtonValue[joystickButtonNumber]; /**< The button state memory. Updated by events in handleJoystick(). */
  int joystickCoolieValue;
  std::string joystickButtonPressCommand[joystickButtonNumber];
  std::string joystickButtonReleaseCommand[joystickButtonNumber];

  char processIdentifier; /** The process from which messages are currently read. */
  unsigned maxPlotSize; /**< The maximum number of data points to remember for plots. */
  bool bikeView; /**Indicator if there is already a BikeView, we need it just once */

public:
  /**
  * Constructor.
  */
  RobotConsole(MessageQueue& in,MessageQueue& out);
  
  /**
  * Destructor.
  */
  ~RobotConsole();

  /**
   * That function is called once before the first main(). It can be used 
   * for things that can't be done in the constructor.
   */
  virtual void init();

  /**
  * The function adds all views.
  */
  void addViews();

  /** 
  * The function is called for every incoming debug message.
  */
  virtual bool handleMessage(InMessage& message);
  
  /**
  * The function is called when a console command has been entered.
  * @param line A string containing the console command.
  */
  void handleConsole(const std::string& line);
  
  /**
  * The method is called when Shift+Ctrl+letter was pressed.
  * @param key A: 0 ... Z: 25.
  * @param pressed Whether the key was pressed or released.
  */
  void handleKeyEvent(int key, bool pressed);

  /**
  * The function must be called to exchange data with SimRobot.
  * It sends the motor commands to SimRobot and acquires new sensor data.
  */
  virtual void update();

  /**
  * The method returns whether the console is polling for some data from the robot.
  * @return Currently waiting?
  */
  bool isPolling() const {return !lines.empty();}

private:
  /**
  * Poll information of a certain kind if it needs updated.
  * @param id The information required.
  * @return The information requested is already up-to-date.
  */
  bool poll(MessageID id);

  /**
  * The function polls all outdated information required for and only in direct mode.
  */
  void pollForDirectMode();

  /**
  * The method triggers the processes to keep them busy or to receive updated data.
  */
  void triggerProcesses();

  void addColorSpaceViews(const std::string& id, const std::string& name, bool user);

  /**
  * The function is called when a console command has been entered.
  * @param line A string containing the console command.
  * @return Was the command processed? Otherwise, it has to be processed later.
  */
  bool handleConsoleLine(const std::string& line);
  
  /**
  * The function prints an unfolded type to the console.
  * @param type The type to print.
  * @param field The field with that type.
  */
  void printType(const char* type, const char* field = "");

  /**
  * The function prints a MODIFY data set to the console.
  * @param stream The stream that contains the data.
  * @param type The type to print.
  * @param field The field with that type.
  */
  void printData(In& stream, const char* type, const char* field = "");

  void myPrintData(In& stream, const char* type, std::ofstream& file, const char* field = "");

  /**
  * The function parses a MODIFY data set from the console.
  * @param stream The stream that contains the console text.
  * @param type The type to of the data.
  * @param field The field with that type.
  */
  bool parseData(In& stream, const char* type, const char* field = "");


  /**
  * The function initialises the joystick.
  */
  bool initJoystick();

  /**
  * The function handles the joystick.
  */
  void handleJoystick();

  /**
  * The function calculates a command from the buttons pressed.
  * @param buttons A bit set containing the state of all buttons.
  * @param coolie The state of the coolie hat.
  * @return The command generated or "" if no button was pressed.
  */
  std::string calcButtonCommand(unsigned buttons, unsigned coolie) const;

  /**
  * The function converts a value from integer to [-1 .. 1].
  * @param value A value in the range [0 .. 32767].
  * @param threshold All results [-threshold .. threshold] will be set to 0.
  * @return The value in the range [-1 .. 1].
  */
  double convert(unsigned value, double threshold) const;

  /**
  * The function returns the path and filename for a given representation
  * @param representation A string naming a representation
  * @return A string to the filename to the requested file
  */
  std::string getPathForRepresentation(std::string representation);

  //!@name Handler for different console commands
  //!@{
  bool msg(InConfigMemory&);
  bool backgroundColor(InConfigMemory& stream);
  bool calcImage(InConfigMemory&);
  bool colorTable(InConfigMemory& stream);
  bool debugRequest(InConfigMemory&);
  bool gameControl(InConfigMemory& stream);
  bool joystickCommand(InConfigMemory& stream);
  bool joystickSpeeds(InConfigMemory& stream);
  bool log(InConfigMemory& stream);
  bool get(InConfigMemory& stream, bool first);
  bool set(InConfigMemory& stream, bool first);
  bool penalizeRobot(InConfigMemory& stream);
  bool saveRequest(InConfigMemory& stream, bool first);
  bool sendMof(InConfigMemory& stream);
  bool repoll(InConfigMemory& stream);
  bool queueFillRequest(InConfigMemory& stream);
  bool moduleRequest(InConfigMemory&);
  bool moveRobot(InConfigMemory&);
  bool moveBall(InConfigMemory&);
  bool view3D(InConfigMemory& stream);
  bool viewField(InConfigMemory& stream);
  bool viewBike();
  bool viewDrawing(InConfigMemory& stream, RobotConsole::Views& views, const char* type);
  bool viewImage(InConfigMemory& stream);
  bool viewPlot(InConfigMemory& stream);
  bool viewPlotDrawing(InConfigMemory& stream);
  bool xabslSendBehavior(InConfigMemory& stream);
  bool xabslInputSymbol(InConfigMemory& stream);
  bool xabslOutputSymbol(InConfigMemory& stream);
  bool xabslOption(InConfigMemory& stream);
  bool xabslBasicBehavior(InConfigMemory& stream);
  //!@}
};

#endif // __RobotConsole_H__
