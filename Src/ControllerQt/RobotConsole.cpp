/** 
* @file ControllerQt/RobotConsole.cpp
*
* Implementation of RobotConsole.
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#include <algorithm>
#include <fstream>

#define USE_GETMESSAGEIDNAME
#include "../Tools/Settings.h"
#include "../Tools/ColorIndex.h"
#include "../Tools/ImageProcessing/ColorModelConversions.h"
#include "../Tools/Debugging/QueueFillRequest.h"
#include "../URC/MofCompiler.h"
#include "RobotConsole.h"
#include "ConsoleRoboCupCtrl.h"
#include "Views/ImageView.h"
#include "Views/FieldView.h"
#include "Views/XabslView.h"
#include "Views/TimeView.h"
#include "Views/SensorView.h"
#include "Views/ModuleView.h"
#include "Views/ColorTableView.h"
#include "Views/ColorSpaceView.h"
#include "Views/PlotView.h"
#include "Views/JointView.h"
#include "Platform/itoa.h"
#include "../Representations/Perception/JPEGImage.h"
#ifdef LINUX
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <fcntl.h>
#  include <errno.h>
#  include <linux/joystick.h>
#endif

#define PREREQUISITE(p) pollingFor = #p; if(!poll(p)) return false;

RobotConsole::RobotConsole(MessageQueue& in, MessageQueue& out)
: Process(in, out),
  printMessages(true),
  handleMessages(true),
  calculateImage(true),
  logAcknowledged(true),
  destructed(false),
  logPlayer(out),
  debugIn(in), debugOut(out),
  pollingFor(0),
  moveOp(noMove),
  robotPoseReceived(0),
  ballModelReceived(0),
  goalPerceptReceived(0),
  robotHealthReceived(0),
  motionRequestReceived(0),
  xabslError(false),
  logMessages(0),
  background(0.5f, 0.5f, 0.5f),
  colorTableSendDelay(1000),
  updateCompletion(false),
  directMode(false),
  logImagesAsJPEGs(false),
  joystickTrace(false),
  joystickLastTime(0),
  joystickID(-1),
  processIdentifier(0),
  maxPlotSize(0)
{
  // this is a hack: call global functions to get parameters
  ctrl = (ConsoleRoboCupCtrl*) RoboCupCtrl::getController();
  joystickID = ctrl->getNextJoystickID();
#ifdef LINUX
  joystickID = -1;
#endif
  robotName = RoboCupCtrl::getRobotFullName();
  lastColorTableSent = colorTableHandler.timeStamp;
  for(int i = 0; i < numOfMessageIDs; ++i)
  {
    waitingFor[i] = 0;
    polled[i] = false;
  }
  for(int i = 0; i < 6; ++i)
  {
    motionCommandIndices[i] = 0;
    motionCommandMaxSpeeds[i] = 0;
    motionCommandThresholds[i] = 0;
  }
  logPlayer.setSize(1 << 30); // max. 1 GB
}

RobotConsole::~RobotConsole()
{
  SYNC;
  streamHandler.clear();
  xabslInfo.reset();
  if(logMessages)
    delete [] logMessages;
  for(DebugDataInfos::iterator i = debugDataInfos.begin(); i != debugDataInfos.end(); ++i)
    delete i->second.second;
  destructed = true;
}

void RobotConsole::init()
{
  if(mode != SystemCall::teamRobot && mode != SystemCall::groundTruth)
  {
    initJoystick();
    poll(idColorTable64);
  }
}

void RobotConsole::addViews()
{
  ctrl->addView(new TimeView(*this, timeInfo), 
                robotName.substr(robotName.rfind('.') + 1) + ".timing");
  if(mode != SystemCall::groundTruth)  
  {
    ctrl->addView(new SensorView(*this, sensorData, filteredSensorData), 
                    robotName.substr(robotName.rfind('.') + 1) + ".sensorData");
    if(mode != SystemCall::teamRobot)
    {
      ctrl->addView(new JointView(*this, jointData, sensorData, jointRequest), 
                    robotName.substr(robotName.rfind('.') + 1) + ".jointData");
      ctrl->addView(new XabslView(*this, xabslInfo), 
                    robotName.substr(robotName.rfind('.') + 1) + ".xabsl");
      ctrl->addView(new ModuleView(*this, 'c', ""), 
                    robotName.substr(robotName.rfind('.') + 1) + ".modules.cognition.all");
      ctrl->addView(new ModuleView(*this, 'c', "Behavior Control"), 
                    robotName.substr(robotName.rfind('.') + 1) + ".modules.cognition.behaviorControl");
      ctrl->addView(new ModuleView(*this, 'c', "Infrastructure"), 
                    robotName.substr(robotName.rfind('.') + 1) + ".modules.cognition.infrastructure");
      ctrl->addView(new ModuleView(*this, 'c', "Modeling"), 
                    robotName.substr(robotName.rfind('.') + 1) + ".modules.cognition.modeling");
      ctrl->addView(new ModuleView(*this, 'c', "Perception"), 
                    robotName.substr(robotName.rfind('.') + 1) + ".modules.cognition.perception");
      ctrl->addView(new ModuleView(*this, 'm', ""), 
                    robotName.substr(robotName.rfind('.') + 1) + ".modules.motion.all");
      ctrl->addView(new ModuleView(*this, 'm', "Infrastructure"), 
                    robotName.substr(robotName.rfind('.') + 1) + ".modules.motion.infrastructure");
      ctrl->addView(new ModuleView(*this, 'm', "Motion Control"), 
                    robotName.substr(robotName.rfind('.') + 1) + ".modules.motion.motionControl");      
      ctrl->addView(new ModuleView(*this, 'm', "Sensing"), 
                    robotName.substr(robotName.rfind('.') + 1) + ".modules.motion.sensing");
      ctrl->addView(new ColorTableView(*this, colorTableHandler, background), 
                    robotName.substr(robotName.rfind('.') + 1) + ".colorSpace.colorTable");
    }
  }
}

void RobotConsole::addColorSpaceViews(const std::string& id, const std::string& name, bool user)
{
  for(int cm = 0; cm < ColorSpaceView::numberOfColorModels - (user ? 0 : 1); ++cm)
    if(ColorSpaceView::ColorModel(cm) == ColorSpaceView::user)
      for(int channel = 1; channel < 4; ++channel)
        ctrl->addView(new ColorSpaceView(*this, id, ColorSpaceView::YCbCr, 
                                         channel + 3, background), 
                      robotName.substr(robotName.rfind('.') + 1) + ".colorSpace." + name + "." + 
                      ColorSpaceView::getColorModelName(ColorSpaceView::ColorModel(cm)) + "." + 
                      ColorSpaceView::getChannelNameForColorModel(ColorSpaceView::ColorModel(cm), channel));
    else
      for(int channel = 0; channel < 4; ++channel)
        ctrl->addView(new ColorSpaceView(*this, id, ColorSpaceView::ColorModel(cm), 
                          channel + (cm == ColorSpaceView::YCbCr  && channel ? 1 : 0), background), 
                      robotName.substr(robotName.rfind('.') + 1) + ".colorSpace." + name + "." + 
                      ColorSpaceView::getColorModelName(ColorSpaceView::ColorModel(cm)) + "." + 
                      ColorSpaceView::getChannelNameForColorModel(ColorSpaceView::ColorModel(cm), channel));
}

bool RobotConsole::handleMessage(InMessage& message)
{
  if(!handleMessages)
    return true;
  
  // Only one thread can access *this now.
  SYNC;
  if(destructed) // if object is already destructed, abort here
    return true; // avoid further processing of this message

  if(message.getMessageID() < numOfDataMessageIDs)
  {
    if(logImagesAsJPEGs && message.getMessageID() == idImage)
    {
      Image image;
      message.bin >> image;
      MessageQueue queue;
      queue.out.bin << JPEGImage(image);
      queue.out.finishMessage(idJPEGImage);
      logPlayer.handleMessage(queue.in);
    }
    else
      logPlayer.handleMessage(message);
    message.resetReadPosition();
  }

  switch(message.getMessageID())
  {
    case idText:
    {
      std::string buffer(message.text.readAll());
      if(printMessages)
        ctrl->printLn(buffer);
      if(logMessages)
        *logMessages << buffer << std::endl;
      return true;
    }
    case idConsole:
    {
      std::string buffer(message.text.readAll());
      ctrl->executeConsoleCommand(buffer, this);
      return true;
    }
    case idOdometryData:
      message.bin >> odometryData;
      return true;
    case idImage:
      if(!incompleteImages["raw image"].image)
        incompleteImages["raw image"].image = new Image;
      message.bin >> *incompleteImages["raw image"].image;
      return true;
    case idJPEGImage:
    {
      if(!incompleteImages["raw image"].image)
        incompleteImages["raw image"].image = new Image;
      JPEGImage jpi;
      message.bin >> jpi;
      jpi.toImage(*incompleteImages["raw image"].image);
      return true;
    }
    case idDebugImage:
    {
      std::string id;
      message.bin >> id;
      if(!incompleteImages[id].image)
        incompleteImages[id].image = new Image;
      message.bin >> *incompleteImages[id].image;
      incompleteImages[id].image->timeStamp = SystemCall::getCurrentSystemTime();
      break;
    }
    case idDebugJPEGImage:
    {
      std::string id;
      message.bin >> id;
      if(!incompleteImages[id].image)
        incompleteImages[id].image = new Image;
      JPEGImage jpi;
      message.bin >> jpi;
      jpi.toImage(*incompleteImages[id].image);
      incompleteImages[id].image->timeStamp = SystemCall::getCurrentSystemTime();
      break;
    }
    case idDebugColorClassImage:
    {
      std::string id;
      message.bin >> id;
      if(!incompleteImages[id].image)
        incompleteImages[id].image = new Image;
      ColorClassImage c;
      message.bin >> c;
      c.convertToImage(*incompleteImages[id].image);
      incompleteImages[id].image->timeStamp = SystemCall::getCurrentSystemTime();
      break;
    }
    case idDebugGrayScaleImage:
    {
      std::string id;
      message.bin >> id;
      if(!incompleteImages[id].image)
        incompleteImages[id].image = new Image;
      Image& i = *incompleteImages[id].image;
      GrayScaleImage g;
      message.bin >> g;
      i.cameraInfo = g.cameraInfo;
      for(int y = 0; y < g.cameraInfo.resolutionHeight; ++y)
        for(int x = 0; x < g.cameraInfo.resolutionWidth; ++x)
        if (g.image[y][x] < ColorIndex::numOfColors)
        {
          unsigned char chR, chG, chB;
          ColorIndex::toRGB(static_cast<ColorIndex::Color>(g.image[y][x]), chR, chG, chB);
          ColorModelConversions::fromRGBToYCbCr(chR, chG, chB, 
                                                i.image[y][x].y, 
                                                i.image[y][x].cb, 
                                                i.image[y][x].cr);
        }
        else
        {
          i.image[y][x].y = g.image[y][x] << 1;
          i.image[y][x].cb = i.image[y][x].cr = 128;
        }
      i.timeStamp = SystemCall::getCurrentSystemTime();
      break;
    }
    case idSensorData:
    {
      message.bin >> sensorData;
      return true;
    }
    case idFilteredSensorData:
    {
      message.bin >> filteredSensorData;
      return true;
    }
    case idJointData:
    {
      message.bin >> jointData;
      return true;
    }
    case idJointRequest:
    {
      message.bin >> jointRequest;
      if(Global::getSettings().model == Settings::nao)
        jointRequest.angles[JointData::legRight0] = jointRequest.angles[JointData::legLeft0];
      return true;
    }
    case idLEDRequest:
    {
      message.bin >> ledRequest;
      return true;
    }
    case idDebugDrawing:
    {
      if(waitingFor[idDrawingManager]) // drawing manager not up-to-date
        return true; // skip drawing, may not be known anyway

      char shapeType, 
           id;
      message.bin >> shapeType >> id;
      const char* name = drawingManager.getDrawingName(id); // const char* is required here
      std::string type = drawingManager.getDrawingType(name);
 
      if(type == "drawingOnImage")
        incompleteImageDrawings[name].addShapeFromQueue(message, (::Drawings::ShapeType)shapeType, '?');
      else if(type == "drawingOnField")
        incompleteFieldDrawings[name].addShapeFromQueue(message, (::Drawings::ShapeType)shapeType, '?');
      return true;
    }
    case idDebugDrawing3D:
    {
      if(!waitingFor[idDrawingManager3D])
      {
        char shapeType, 
          id;
        message.bin >> shapeType >> id;
        incompleteDrawings3D[drawingManager3D.getDrawingName(id)].addShapeFromQueue(message, (::Drawings3D::ShapeType)shapeType, processIdentifier);
      }
      return true;
    }
    case idPlot:
    {
      std::string id;
      double value;
      message.bin >> id >> value;
      Plot& plot = plots[ctrl->translate(id)];
      plot.points.push_back(value);
      while(plot.points.size() > maxPlotSize)
        plot.points.pop_front();
      plot.timeStamp = SystemCall::getCurrentSystemTime();
      return true;
    }
    case idProcessBegin:
    {
      message.bin >> processIdentifier;
      drawingManager.setProcess(processIdentifier);
      drawingManager3D.setProcess(processIdentifier);
      return true;
    }
    case idProcessFinished:
    {
      char c;
      message.bin >> c;
      ASSERT(processIdentifier == c);
      // Delete all images received earlier from the current process
      for(Images::iterator i = images.begin(), next; i != images.end(); i = next)
      {
        next = i;
        ++next;
        if(i->second.processIdentifier == processIdentifier)
          images.erase(i);
      }

      // Add all images received now from the current process
      for(Images::iterator i = incompleteImages.begin(); i != incompleteImages.end(); ++i)
      {
        images[i->first].image = i->second.image;
        images[i->first].processIdentifier = processIdentifier;
        i->second.image = 0;
      }
      incompleteImages.clear();

      // Delete all image drawings received earlier from the current process
      for(Drawings::iterator i = imageDrawings.begin(), next; i != imageDrawings.end(); i = next)
      {
        next = i;
        ++next;
        if(i->second.processIdentifier == processIdentifier)
          imageDrawings.erase(i);
      }

      // Add all image drawings received now from the current process
      for(Drawings::const_iterator i = incompleteImageDrawings.begin(); i != incompleteImageDrawings.end(); ++i)
      {
        imageDrawings[i->first] = i->second;
        imageDrawings[i->first].processIdentifier = processIdentifier;
      }
      incompleteImageDrawings.clear();

      // Delete all field drawings received earlier from the current process
      for(Drawings::iterator i = fieldDrawings.begin(), next; i != fieldDrawings.end(); i = next)
      {
        next = i;
        ++next;
        if(i->second.processIdentifier == processIdentifier)
          fieldDrawings.erase(i);
      }

      // Add all field drawings received now from the current process
      for(Drawings::const_iterator i = incompleteFieldDrawings.begin(); i != incompleteFieldDrawings.end(); ++i)
      {
        fieldDrawings[i->first] = i->second;
        fieldDrawings[i->first].processIdentifier = processIdentifier;
      }
      incompleteFieldDrawings.clear();

      // 3D Drawings
       if(!waitingFor[idDrawingManager3D])
          for(Drawings3D::iterator i = incompleteDrawings3D.begin(); i != incompleteDrawings3D.end();)
          {
            Drawings3D::iterator j = i++;
            if(j->second.processIdentifier == processIdentifier)
            {
              bool drawn = drawings3D[j->first].drawn;
              j->second.robotConsole = this;
              drawings3D[j->first] = j->second;
              drawings3D[j->first].drawn = drawn;
              if(!drawn)
              {
                std::string type = drawingManager3D.getDrawing3DType(j->first.c_str());
                if(type != "unknown")
                {
                  std::vector<std::string> parts;
                  parts.push_back(robotName);
                  if(type == "field")
                    parts[0] = "RoboCup";
                  else if(type != "robot")
                    parts.push_back(type);
                  ctrl->registerDrawingByPartOfName(parts, &(drawings3D[j->first]));
                  drawings3D[j->first].drawn = true;
                }
              }
              incompleteDrawings3D.erase(j);
            }
          }

      return true;
    }
    case idXabslDebugSymbols:
      {
        SYNC_WITH(*ctrl);
        std::string id;
        message.bin >> id;
        message.resetReadPosition();
        if(xabslInfo.handleMessage(message))
        {
          ctrl->setXabslInfo(xabslInfo);
          updateCompletion = true;
          --waitingFor[idXabslDebugMessage];
          return true;
        }
        else
          return false;
      }
    case idXabslDebugMessage:
      {
        SYNC_WITH(*ctrl);
        std::string id;
        message.bin >> id;
        message.resetReadPosition();
        return xabslInfo.handleMessage(message);
      }
    case idStopwatch:
      return timeInfo.handleMessage(message);
    case idDebugResponse:
      {
        SYNC_WITH(*ctrl);
        std::string description;
        int enable;
        message.text >> description >> enable;
        if(description != "pollingFinished")
          debugRequestTable.addRequest(DebugRequest(description, enable != 0, true));
        else if(--waitingFor[idDebugResponse] <= 0)
        {
          ctrl->setDebugRequestTable(debugRequestTable);
          updateCompletion = true;
        }
        return true;
      }
    case idModuleTable:
      {
        SYNC_WITH(*ctrl);
        moduleInfo.handleMessage(message, processIdentifier);
        if(--waitingFor[idModuleTable] <= 0)
        {
          ctrl->setModuleInfo(moduleInfo);
          updateCompletion = true;
        }
        return true;
      }
    case idDrawingManager:
      {
        SYNC_WITH(*ctrl);
        message.bin >> drawingManager;
        if(--waitingFor[idDrawingManager] <= 0)
        {
          ctrl->setDrawingManager(drawingManager);
          updateCompletion = true;
        }
        return true;
      }
      case idDrawingManager3D:
    {
      SYNC_WITH(*ctrl);
      message.bin >> drawingManager3D;
      if(--waitingFor[idDrawingManager3D] <= 0)
      {
        ctrl->setDrawingManager3D(drawingManager3D);
        updateCompletion = true;
      }
      return true;
    }
    case idColorTable64:
      colorTableHandler.handleMessage(message);
      lastColorTableSent = colorTableHandler.timeStamp;
      waitingFor[idColorTable64] = 0;
      return true;
    case idStreamSpecification:
      {
        SYNC_WITH(*ctrl);
        message.bin >> streamHandler;
        --waitingFor[idStreamSpecification];
        return true;
      }
    case idDebugDataResponse:
      {
        SYNC_WITH(*ctrl);
        std::string name,
                    type;
        long flags;
        message.bin >> name >> flags >> type;
        if(debugDataInfos.find(name) == debugDataInfos.end())
          debugDataInfos[name] = DebugDataInfoPair(type, new MessageQueue);
        debugDataInfos[name].second->clear();
        message >> *debugDataInfos[name].second;
        const char* t = streamHandler.getString(type);
        if(streamHandler.basicTypeSpecification.find(t) == streamHandler.basicTypeSpecification.end() &&
           streamHandler.specification.find(t) == streamHandler.specification.end() &&
           streamHandler.enumSpecification.find(t) == streamHandler.enumSpecification.end())
          polled[idStreamSpecification] = false;
        waitingFor[idDebugDataResponse] = 0;
        return true;
      }
    case idLogResponse:
      logAcknowledged = true;
      return true;
    case idTeamMateRobotPose:
      message.bin >> robotPose;
      robotPoseReceived = SystemCall::getCurrentSystemTime();
      return true;
    case idTeamMateBallModel:
      message.bin >> ballModel;
      if(ballModel.timeWhenLastSeen)
        ballModel.timeWhenLastSeen = ctrl->ntp.getTeamMateTimeInOwnTime(ballModel.timeWhenLastSeen, ((TeamRobot*) this)->getNumber());
      ballModelReceived = SystemCall::getCurrentSystemTime();
      return true;
    case idTeamMateGoalPercept:
      message.bin >> goalPercept;
      if(goalPercept.timeWhenOppGoalLastSeen)
        goalPercept.timeWhenOppGoalLastSeen = ctrl->ntp.getTeamMateTimeInOwnTime(goalPercept.timeWhenOppGoalLastSeen, ((TeamRobot*) this)->getNumber());
      goalPerceptReceived = SystemCall::getCurrentSystemTime();
      return true;
    case idLinePercept:
      message.bin >> linePercept;
      linePerceptReceived = SystemCall::getCurrentSystemTime();
      return true;
    case idTeamMateBehaviorData:
      message.bin >> behaviorData;
      return true;
    case idRobotHealth:
      message.bin >> robotHealth;
      robotHealthReceived = SystemCall::getCurrentSystemTime();
      return true;
    case idMotionRequest:
      message.bin >> motionRequest;
      motionRequestReceived = SystemCall::getCurrentSystemTime();
      return true;
    case idRobotname:
    { 
      std::string buffer;
      message.bin >> buffer;
      Global::getSettings().robot = buffer;
      return true;
    }
    case idRobotDimensions:
      {
        message.bin >> robotDimensions;
        return true;
      }
    case idJointCalibration:
      {
        message.bin >> jointCalibration;
        return true;
      }
    default:
      return mode == SystemCall::groundTruth || mode == SystemCall::teamRobot
             ? Process::handleMessage(message) : false;
  }
  return false;
}

void RobotConsole::update()
{
  setGlobals(); // this is called in GUI thread -> set globals for this process
  handleJoystick();

  if(lastColorTableSent < colorTableHandler.timeStamp && 
     SystemCall::getTimeSince(lastColorTableSent) >= colorTableSendDelay)
  {
    {
      SYNC;
      colorTableHandler.send(debugOut.out);
    }
    lastColorTableSent = SystemCall::getCurrentSystemTime();
    triggerProcesses();
  }

  while(!lines.empty())
  {
    std::list<std::string> temp = lines;
    lines.clear();
    if(handleConsoleLine(temp.front()))
    {
      temp.pop_front();
      lines.splice(lines.end(), temp);
    }
    else
    {
      lines = temp;
      break;
    }
  }

  pollForDirectMode();

  if(updateCompletion)
  {
    SYNC;
    ctrl->updateCommandCompletion();
    updateCompletion = false;
  }
}

void RobotConsole::handleConsole(const std::string& line)
{
  setGlobals(); // this is called in GUI thread -> set globals for this process
  lines.push_back(line);
  while(!lines.empty())
  {
    std::list<std::string> temp = lines;
    lines.clear();
    if(handleConsoleLine(temp.front()))
    {
      temp.pop_front();
      lines.splice(lines.end(), temp);
    }
    else
    {
      lines = temp;
      break;
    }
  }

  pollForDirectMode();
}

void RobotConsole::triggerProcesses()
{
  if(mode == SystemCall::logfileReplay)
  {
    SYNC;
    debugOut.out.bin << 'c';
    debugOut.out.finishMessage(idProcessBegin);
    debugOut.out.bin << 'c';
    debugOut.out.finishMessage(idProcessFinished);
    debugOut.out.bin << 'm';
    debugOut.out.finishMessage(idProcessBegin);
    debugOut.out.bin << 'm';
    debugOut.out.finishMessage(idProcessFinished);
  }
}

bool RobotConsole::poll(MessageID id)
{
  if(waitingFor[id] > 0)
  {
    // When in replay log file mode, force replay while polling to keep Cognition running
    triggerProcesses();
    return false;
  }
  else if(polled[id])
    return true;
  else
  {
    polled[id] = true;
    switch(id)
    {
    case idDebugResponse:
      {
        SYNC;
        debugOut.out.bin << DebugRequest("poll");
        debugOut.out.finishMessage(idDebugRequest);
        if(mode == SystemCall::groundTruth || mode == SystemCall::teamRobot)
          waitingFor[id] = 1; // Ground truth module should answer
        else
          waitingFor[id] = 3; // Cognition + Motion + Debug will answer
        break;
      }
    case idModuleTable:
      {
        SYNC;
        moduleInfo.clear();
        debugOut.out.bin << DebugRequest("automated requests:ModuleTable", true, true);
        debugOut.out.finishMessage(idDebugRequest);
        waitingFor[id] = 2;  // Cognition + Motion will answer
        break;
      }
    case idStreamSpecification:
      {
        SYNC;
        streamHandler.clear();
        debugOut.out.bin << DebugRequest("automated requests:StreamSpecification", true, true);
        debugOut.out.finishMessage(idDebugRequest);
        if(mode == SystemCall::groundTruth || mode == SystemCall::teamRobot)
          waitingFor[id] = 1; // Ground truth or team robot module should answer
        else
          waitingFor[id] = 2;  // Cognition + Motion will answer
        break;
      }
    case idDrawingManager:
      {
        SYNC;
        drawingManager.clear();
        debugOut.out.bin << DebugRequest("automated requests:DrawingManager", true, true);
        debugOut.out.finishMessage(idDebugRequest);
        if(mode == SystemCall::groundTruth || mode == SystemCall::teamRobot)
          waitingFor[id] = 1; // Ground truth or team robot module should answer
        else
          waitingFor[id] = 2;  // Cognition + Motion will answer
        break;
      }
    case idDrawingManager3D:
      {
        SYNC;
        drawingManager3D.clear();
        debugOut.out.bin << DebugRequest("automated requests:DrawingManager3D", true, true);
        debugOut.out.finishMessage(idDebugRequest);
        if(mode == SystemCall::groundTruth || mode == SystemCall::teamRobot)
          waitingFor[id] = 1; // Ground truth or team robot module should answer
        else
          waitingFor[id] = 2;  // Cognition + Motion will answer
        break;
      }
    case idColorTable64:
      {
        SYNC;
        debugOut.out.bin << DebugRequest("representation:ColorTable64", true, true);
        debugOut.out.finishMessage(idDebugRequest);
        waitingFor[id] = 1;  // Cognition will answer
        break;
      }
    default:
      break;
    }
    return false;
  }
}

void RobotConsole::pollForDirectMode()
{
  if(directMode)
  {
    poll(idDebugResponse);
    poll(idDrawingManager);
    poll(idDrawingManager3D);
    if(mode != SystemCall::groundTruth)
      poll(idModuleTable);
  }
}

bool RobotConsole::handleConsoleLine(const std::string& line)
{
  InConfigMemory stream(line.c_str(), line.size());
  std::string command;
  stream >> command;
  bool result = false;
  if(command == "") // comment
    result = true;
  else if(command == "endOfStartScript")
  {
    directMode = true;
    result = true;
  }
  else if(command == "cls")
  {
    ctrl->printLn("_cls");
    result = true;
  }
  else if(command == "dr")
  {
    PREREQUISITE(idDebugResponse);
    PREREQUISITE(idDrawingManager3D);
    result = debugRequest(stream);
  }
  else if(command == "echo")
  {
    ctrl->echo(stream);
    result = true;
  }
  else if(command == "get")
  {
    PREREQUISITE(idDebugResponse);
    result = get(stream, true);
  }
  else if(command == "_get") // get, part 2
  {
    PREREQUISITE(idDebugDataResponse);
    PREREQUISITE(idStreamSpecification);
    result = get(stream, false);
  }
  else if(command == "jc")
    result = joystickCommand(stream);
  else if(command == "js")
    result = joystickSpeeds(stream);
  else if(command == "log")
    result = log(stream);
  else if(command == "msg")
  {
    if(mode != SystemCall::teamRobot && mode != SystemCall::groundTruth)
      PREREQUISITE(idColorTable64); // receive color table before processing "msg disable"
    result = msg(stream);
  }
  else if(command == "poll")
    result = repoll(stream);
  else if(command == "set")
  {
    PREREQUISITE(idDebugResponse);
    result = set(stream, true);
  }
  else if(command == "_set") // set, part 2
  {
    PREREQUISITE(idDebugDataResponse);
    PREREQUISITE(idStreamSpecification);
    result = set(stream, false);
  }
  else if(command == "vf" && mode != SystemCall::groundTruth)
  {
    PREREQUISITE(idDebugResponse);
    PREREQUISITE(idDrawingManager);
    result = viewField(stream);
  }
  else if(command == "vfd" && mode != SystemCall::groundTruth)
  {
    PREREQUISITE(idDebugResponse);
    PREREQUISITE(idDrawingManager);
    result = viewDrawing(stream, fieldViews, "drawingOnField");
  }
  else if(command == "vid" && mode != SystemCall::teamRobot)
  {
    PREREQUISITE(idDebugResponse);
    PREREQUISITE(idDrawingManager);;
    result = viewDrawing(stream, imageViews, "drawingOnImage");
  }
  else if(command == "vi" && mode != SystemCall::teamRobot)
  {
    PREREQUISITE(idDebugResponse);
    PREREQUISITE(idDrawingManager);
    result = viewImage(stream);
  }
  else if(mode == SystemCall::groundTruth || mode == SystemCall::teamRobot) // if in ground truth mode, stop here
    result = false;
  else if(command == "bc")
    result = backgroundColor(stream);
  else if(command == "ci")
    result = calcImage(stream);
  else if(command == "ct")
  {
    PREREQUISITE(idColorTable64);
    result = colorTable(stream);
  }
  else if(command == "mof")
    result = sendMof(stream);
  else if(command == "mr")
  {
    PREREQUISITE(idModuleTable);
    result = moduleRequest(stream);
  }
  else if(command == "mv")
    result = moveRobot(stream);
  else if(command == "mvb")
    result = moveBall(stream);
  else if(command == "qfr")
    result = queueFillRequest(stream);
  else if(command == "v3")
  {
    PREREQUISITE(idDebugResponse);
    PREREQUISITE(idDrawingManager);
    result = view3D(stream);
  }
  else if(command == "vp")
  {
    PREREQUISITE(idDebugResponse);
    result = viewPlot(stream);
  }
  else if(command == "vpd")
  {
    PREREQUISITE(idDebugResponse);
    result = viewPlotDrawing(stream);
  }
  else if(command == "xbb")
  {
    PREREQUISITE(idXabslDebugMessage);
    result = xabslBasicBehavior(stream);
  }
  else if(command == "xis")
  {
    PREREQUISITE(idXabslDebugMessage);
    result = xabslInputSymbol(stream);
  }
  else if(command == "xo")
  {
    PREREQUISITE(idXabslDebugMessage);
    result = xabslOption(stream);
  }
  else if(command == "xos")
  {
    PREREQUISITE(idXabslDebugMessage);
    result = xabslOutputSymbol(stream);
  }
  else if(command == "xsb")
    result = xabslSendBehavior(stream);
  else if (command == "save")
  {
    PREREQUISITE(idDebugResponse);
    result = saveRequest(stream, true);
  }
  else if (command == "_save")
  { 
    PREREQUISITE(idDebugDataResponse);
    PREREQUISITE(idStreamSpecification);
    result = saveRequest(stream, false);
  }

  pollingFor = 0;
  if(!result)
  {
    if(directMode)
    {
      ctrl->printLn("Syntax Error");
    }
    else
    {
      ctrl->printLn((std::string("Syntax Error: ") + line).c_str());
    }
  }
  return true;
}

bool RobotConsole::msg(InConfigMemory& stream)
{
  std::string state;
  stream >> state;
  if(state == "off")
  {
    printMessages = false;
    return true;
  }
  else if(state == "on")
  {
    printMessages = true;
    return true;
  }
  else if(state == "log")
  {
    if(logMessages)
      delete logMessages;
    stream >> state;
    std::string name(state);
    if(name.size() == 0)
   
      return false;
    else 
    {
      if((int) name.rfind('.') <= (int) name.find_last_of("\\/"))
        name = name + ".txt";
      char buf[FILENAME_MAX];
      if(name[0] != '/' && name[0] != '\\' && name[0] != '.' && (name[0] == 0 || name[1] != ':'))
        sprintf(buf, "%s/Config/Logs/", File::getGTDir());
      else
        buf[0] = 0;
      ASSERT(strlen(buf) + strlen(name.c_str()) < FILENAME_MAX);
      strcat(buf, name.c_str());
      logMessages = new std::fstream(buf, std::ios_base::out);
      return true;
    }
  }
  else if(state == "disable")
  {
    handleMessages = false;
    return true;
  }
  else if(state == "enable")
  {
    handleMessages = true;
    return true;
  }
  return false;
}

bool RobotConsole::backgroundColor(InConfigMemory& stream)
{
  stream >> background.x >> background.y >> background.z;
  background *= 0.01F;
  return true;
}

bool RobotConsole::calcImage(InConfigMemory& stream)
{
  std::string state;
  stream >> state;
  if(state == "off")
  {
    calculateImage = false;
    return true;
  }
  else if(state == "on")
  {
    calculateImage = true;
    return true;
  }
  return false;
}

bool RobotConsole::colorTable(InConfigMemory& stream)
{
  std::string command;
  stream >> command;
  if(command == "off")
  {
    SYNC;
    colorTableHandler.active = false;
    return true;
  }
  else if(command == "on")
  {
    SYNC;
    colorTableHandler.active = true;
    return true;
  }
  else if(command == "smart")
  {
    stream >> command;
    SYNC;
    if(command == "off")
      colorTableHandler.smart = false;
    else
      colorTableHandler.smart = true;
    return true;
  }
  else if(command == "undo")
  {
    SYNC;
    colorTableHandler.undo();
  }
  else if(command == "load")
  {
    stream >> command;
    SYNC;
    if(!colorTableHandler.load(command))
      return false;
  }
  else if(command == "save")
  {
    stream >> command;
    if(command != "")
    {
      SYNC;
      colorTableHandler.save(command);
      return true;
    }
    else
      return false;
  }
  else if(command == "send")
  {
    stream >> command;
    if(command == "")
    {
      {
        SYNC;
        colorTableHandler.send(debugOut.out);
      }
      triggerProcesses();
      return true;
    }
    else if(command == "off")
    {
      colorTableSendDelay = 0xffffffff;
      triggerProcesses();
      return true;
    }
    else
      colorTableSendDelay = atoi(command.c_str());
  }
  else if(command == "sendAndWrite")
  {
    stream >> command;
    if(command == "")
    {
      {
        SYNC;
        colorTableHandler.sendAndWrite(debugOut.out);
      }
      triggerProcesses();
      return true;
    }
  }
  else if(command == "clear")
  {
    stream >> command;
    if(command == "")
      colorTableHandler.clear();
    else
    {
      int color;
      for(color = ColorClasses::none; color < ColorClasses::numOfColors; ++color)
        if(ctrl->translate(ColorClasses::getColorName((ColorClasses::Color) color)) == command)
        {
          SYNC;
          colorTableHandler.clear((ColorClasses::Color) color);
          break;
        }
      if(color == ColorClasses::numOfColors)
        return false;
    }
  }
  else if(command == "mask")
  {
    stream >> command;
    int color;
    for(color = ColorClasses::none; color < ColorClasses::numOfColors; ++color)
      if(ctrl->translate(ColorClasses::getColorName((ColorClasses::Color) color)) == command)
      {
        SYNC;
        colorTableHandler.maskColorClass = (ColorClasses::Color) color;
        break;
      }
    if(color == ColorClasses::numOfColors)
      return false;
  }
  else if(command == "shrink")
  {
    stream >> command;
    if(command == "")
    {
      SYNC;
      colorTableHandler.shrink();
    }
    else
    {
      int color;
      for(color = ColorClasses::none; color < ColorClasses::numOfColors; ++color)
        if(ctrl->translate(ColorClasses::getColorName((ColorClasses::Color) color)) == command)
        {
          SYNC;
          colorTableHandler.shrink((ColorClasses::Color) color);
          break;
        }
      if(color == ColorClasses::numOfColors)
        return false;
    }
  }
  else if(command == "grow")
  {
    stream >> command;
    if(command == "")
    {
      SYNC;
      colorTableHandler.grow();
    }
    else
    {
      int color;
      for(color = ColorClasses::none; color < ColorClasses::numOfColors; ++color)
        if(ctrl->translate(ColorClasses::getColorName((ColorClasses::Color) color)) == command)
        {
          SYNC;
          colorTableHandler.grow((ColorClasses::Color) color);
          break;
        }
      if(color == ColorClasses::numOfColors)
        return false;
    }
  }
  else if(command == "imageRadius")
  {
    SYNC;
    stream >> colorTableHandler.imageRadius;
    return true;
  }
  else if(command == "colorSpaceRadius")
  {
    SYNC;
    stream >> colorTableHandler.colorSpaceRadius;
    return true;
  }
  else
  {
    int color;
    for( color = ColorClasses::none; color < ColorClasses::numOfColors; ++color)
        if(ctrl->translate(ColorClasses::getColorName((ColorClasses::Color) color)) == command)
      {
        stream >> command;
        SYNC;
        if(command == "" || command == "replace")
          colorTableHandler.replace = true;
        else if(command == "add")
          colorTableHandler.replace = false;
        else
          return false;
        colorTableHandler.selectedColorClass = (ColorClasses::Color) color;
        return true;
      }
    if(color == ColorClasses::numOfColors)
      return false;
  }

  if(SystemCall::getTimeSince(lastColorTableSent) >= colorTableSendDelay)
  {
    {
      SYNC;
      colorTableHandler.send(debugOut.out);
    }
    lastColorTableSent = SystemCall::getCurrentSystemTime();
    triggerProcesses();
  }
  return true;
}

bool RobotConsole::debugRequest(InConfigMemory& stream)
{
  std::string debugRequestString, 
              state;
  stream >> debugRequestString >> state;
  
  if(debugRequestString == "?")
  {
    for(int i = 0; i < debugRequestTable.currentNumberOfDebugRequests; ++i)
      ctrl->list(ctrl->translate(debugRequestTable.debugRequests[i].description).c_str(), state);
    ctrl->printLn("");
    return true;
  }
  else
  {
    if(debugRequestString == "off")
    {
      SYNC;
      debugOut.out.bin << DebugRequest("disableAll");
      debugOut.out.finishMessage(idDebugRequest);
      return true;
    }
    else
      for(int i = 0; i < debugRequestTable.currentNumberOfDebugRequests; ++i)
        if(ctrl->translate(debugRequestTable.debugRequests[i].description) == debugRequestString)
        {
          DebugRequest& d = debugRequestTable.debugRequests[i];
          if(state == "off")
          {
            d.enable = false;
            d.once = false;
          }
          else if(state == "on" || state == "") 
          {
            d.enable = true;
            d.once = false;
          }
          else if(state == "once")
          {
            d.enable = true;
            d.once = true;
          }
          else
            return false;

          SYNC;
          debugOut.out.bin << d;
          debugOut.out.finishMessage(idDebugRequest);
          if(d.enable && d.description == "automated requests:xabsl:debugSymbols")
          {
            waitingFor[idXabslDebugMessage] = 1;
            polled[idXabslDebugMessage] = false;
          }
          return true;
        }
  }
  return false;
}

class OldSensorData : public SensorData
{
private:
  virtual void serialize(In* in, Out* out)
  {  
    STREAM_REGISTER_BEGIN();
    float data[numOfSensors + 2];
    STREAM_ARRAY(data);
    for(int i = 0, j = 0; i < numOfSensors; ++i, ++j)
    {
      if(j == batteryLevel)
        j += 2;
      this->data[i] = data[j];
    }
    STREAM_ARRAY(currents);
    STREAM_ARRAY(temperatures);
    STREAM_ENUM(usSensorType, numOfUsSensorTypes, SensorData::getUsSensorTypeName);
    STREAM(timeStamp);
    STREAM_REGISTER_FINISH();
  }
};

class Converter : public MessageHandler
{
private:
  MessageQueue& target;

  bool handleMessage(InMessage& message)
  {
    switch(message.getMessageID())
    {
    case idSensorData:
      {
        OldSensorData os;
        message.bin >> os;
        SensorData s(os);
        target.out.bin << s;
        target.out.finishMessage(idSensorData);
        return true;
      }
    default:
      message >> target;
      return true;
    }
  }

public:
  Converter(MessageQueue& target) : target(target) {}
};

bool RobotConsole::log(InConfigMemory& stream)
{
  std::string command;
  stream >> command;
  SYNC;
  if(command == "start")
  {
    if(logFile != "")
      logPlayer.play();
    else if(logPlayer.state != LogPlayer::recording)
      logPlayer.recordStart();
    return true;
  }
  else if(command == "stop")
  {
    if (logPlayer.state == LogPlayer::recording)
      logPlayer.recordStop();
    else
      logPlayer.stop();
    return true;
  }
  else if(command == "clear")
  {
    logPlayer.init();
    return true;
  }
  else if(command == "save")
  {
    std::string name;
    stream >> name;
    if(name.size() == 0)
      return false;
    else 
    {
      if((int) name.rfind('.') <= (int) name.find_last_of("\\/"))
        name = name + ".log";
      if(name[0] != '/' && name[0] != '\\' && (name.size() < 2 || name[1] != ':'))
        name = std::string("Logs\\") + name;
      return logPlayer.save(name.c_str());
    }
  }
  else if(command == "saveImages")
  {
    stream >> command;
    std::string name(command);
    bool raw = false;
    if(name == "raw")
    {
      raw = true;
      stream >> name;
    }
    if(name.size() == 0)
      name = "Images\\image";
    return logPlayer.saveImages(raw, name.c_str());
  }
  else if(command == "keep" || command == "remove")
  {
    std::string buf;
    stream >> buf;
    std::vector<MessageID> messageIDs;
    while(buf != "")
    {
      int i;
      for(i = 1; i < numOfMessageIDs; ++i)
        if(buf == getMessageIDName(MessageID(i)))
        {
          messageIDs.push_back(MessageID(i));
          break;
        }
      if(i == numOfMessageIDs)
        return false;
      stream >> buf;
    }
    if(messageIDs.size())
    {
      messageIDs.push_back(undefined);
      if(command == "keep")
        logPlayer.keep(&messageIDs[0]);
      else
        logPlayer.remove(&messageIDs[0]);
      return true;
    }
  }
  else if(command == "full")
  {
    logImagesAsJPEGs = false;
    return true;
  }
  else if(command == "jpeg")
  {
    logImagesAsJPEGs = true;
    return true;
  }
  else if(command == "?")
  {
    int frequency[numOfMessageIDs];
    logPlayer.statistics(frequency);
    char buf[20];
    for(int i = 0; i < numOfDataMessageIDs; ++i)
      if(frequency[i])
        ctrl->printLn(std::string(itoa_s(frequency[i], buf, sizeof(buf), 10)) + "\t" + getMessageIDName(MessageID(i)));
    ctrl->printLn(std::string(itoa_s(logPlayer.getNumberOfMessages(), buf, sizeof(buf), 10)) + "\ttotal");
    return true;
  }
  else if(logFile != "")
  {
    if(command == "load")
    {
      std::string name;
      stream >> name;
      if(name.size() == 0)
        return false;
      else 
      {
        if((int) name.rfind('.') <= (int) name.find_last_of("\\/"))
          name = name + ".log";
        if(name[0] != '/' && name[0] != '\\' && (name.size() < 2 || name[1] != ':'))
          name = std::string("Logs\\") + name;
        logFile = name;
        LogPlayer::LogPlayerState state = logPlayer.state;
        bool result = logPlayer.open(name.c_str());
        if(result && state == LogPlayer::playing)
          logPlayer.play();
        return result;
      }
    }
    else if(command == "cycle")
    {
      logPlayer.setLoop(true);
      return true;
    }
    else if(command == "once")
    {
      logPlayer.setLoop(false);
      return true;
    }
    else if(command == "pause")
    {
      logPlayer.pause();
      return true;
    }
    else if(command == "forward")
    {
      logPlayer.stepForward();
      return true;
    }
    else if(command == "backward")
    {
      logPlayer.stepBackward();
      return true;
    }
    else if(command == "repeat")
    {
      logPlayer.stepRepeat();
      return true;
    }
    else if(command == "goto")
    {
      LogPlayer::LogPlayerState state = logPlayer.state;
      int frame;
      stream >> frame;
      logPlayer.gotoFrame(std::max<>(std::min<>(frame - 1, logPlayer.numberOfFrames - 1), 0));
      if(state == LogPlayer::playing)
        logPlayer.play();
      return true;
    }
    else if(command == "convert")
    {
      MessageQueue temp;
      Converter converter(temp);
      logPlayer.stop();
      logPlayer.handleAllMessages(converter);
      logPlayer.clear();
      temp.moveAllMessages(logPlayer);
      return true;
    }
  }
  return false;
}

bool RobotConsole::get(InConfigMemory& stream, bool first)
{
  std::string request,
              option;
  stream >> request >> option;
  if(request == "?")
  {
    for(int i = 0; i < debugRequestTable.currentNumberOfDebugRequests; ++i)
      if(debugRequestTable.debugRequests[i].description.substr(0, 11) == "debug data:")
        ctrl->list(ctrl->translate(debugRequestTable.debugRequests[i].description.substr(11)), option);
    ctrl->printLn("");
    return true;
  }
  else
    for(int i = 0; i < debugRequestTable.currentNumberOfDebugRequests; ++i)
      if(std::string("debugData:") + request == ctrl->translate(debugRequestTable.debugRequests[i].description))
      {
        if(first)
        { // request up-to-date data
          {
            SYNC;
            debugOut.out.bin << DebugRequest(debugRequestTable.debugRequests[i].description, true, true);
            debugOut.out.finishMessage(idDebugRequest);
          }
          waitingFor[idDebugDataResponse] = 1;
          polled[idDebugDataResponse] = true; // no automatic repolling
          handleConsole(std::string("_get ") + request + " " + option);
          return true;
        }
        else
        {
          DebugDataInfos::const_iterator j = debugDataInfos.find(debugRequestTable.debugRequests[i].description.substr(11));
          ASSERT(j != debugDataInfos.end());
          if(option == "?")
          {
            printType(j->second.first.c_str());
            ctrl->printLn("");
            return true;
          }
          else if(option == "")
          {
            ctrl->print(std::string("set ") + request + " ");
            Printer printer(*this);
            j->second.second->handleAllMessages(printer);
            ctrl->printLn("");
            return true;
          }
        }
        break;
      }
  return false;
}

bool RobotConsole::set(InConfigMemory& stream, bool first)
{
  std::string request,
              option;
  stream >> request;
  bool eof = stream.eof();
  stream >> option;
  if(request == "?")
  {
    for(int i = 0; i < debugRequestTable.currentNumberOfDebugRequests; ++i)
      if(debugRequestTable.debugRequests[i].description.substr(0, 11) == "debug data:")
        ctrl->list(ctrl->translate(debugRequestTable.debugRequests[i].description.substr(11)), option);
    ctrl->printLn("");
    return true;
  }
  else
    for(int i = 0; i < debugRequestTable.currentNumberOfDebugRequests; ++i)
      if(std::string("debugData:") + request == ctrl->translate(debugRequestTable.debugRequests[i].description))
      {
        if(option == "unchanged")
        {
          SYNC;
          debugOut.out.bin << debugRequestTable.debugRequests[i].description.substr(11) << char(0);
          debugOut.out.finishMessage(idDebugDataChangeRequest);
          return true;
        }
        else
        {
          std::string line = "";
          std::string next(option);
          while(!eof)
          {
            std::string n;
            bool containsSpaces = next.empty() || next[0] == '"' || next.find_first_of(" \n\r\t") != std::string::npos;
            if(containsSpaces)
              n += '"';
            for(unsigned i = 0; i < next.size(); ++i)
              if(next[i] == '"' && containsSpaces)
                n += "\\\"";
              else if(next[i] == '\n')
                n += "\\n";
              else if(next[i] == '\r')
                n += "\\r";
              else if(next[i] == '\t')
                n += "\\t";
              else if(next[i] == '\\')
                n += "\\\\";
              else
                n += next[i];
            if(containsSpaces)
              n += '"';
            next = n;
            if(next[next.size() - 1] == ';') // insert spaces before semicolons
              next.insert(next.size() - 1, " ");
            line += " ";
            line += next;
            eof = stream.eof();
            if(!eof)
              stream >> next;
          }
          DebugDataInfos::const_iterator j = debugDataInfos.find(debugRequestTable.debugRequests[i].description.substr(11));
          if(j == debugDataInfos.end())
          { // request type specification
            {
              SYNC;
              debugOut.out.bin << DebugRequest(debugRequestTable.debugRequests[i].description, true, true);
              debugOut.out.finishMessage(idDebugRequest);
            }
            waitingFor[idDebugDataResponse] = 1;
            polled[idDebugDataResponse] = true; // no automatic repolling
            handleConsole(std::string("_set ") + request + " " + line);
            return true;
          }
          else
          {
            if(option == "?")
            {
              printType(j->second.first.c_str());
              ctrl->printLn("");
              return true;
            }
            else
            {
              SYNC;
              debugOut.out.bin << debugRequestTable.debugRequests[i].description.substr(11) << char(1);
              InConfigMemory stream(line.c_str(), line.size());
              if(parseData(stream, j->second.first.c_str()))
              {
                debugOut.out.finishMessage(idDebugDataChangeRequest);
                return true;
              }
              else
              {
                debugOut.out.cancelMessage();
                return false;
              }
            }
          }
        }
        break;
      }
  return false;
}

void RobotConsole::printType(const char* type, const char* field)
{
  if(type[strlen(type) - 1] == ']')
  {
    int index = strlen(type);
    while(type[index - 1] != '[')
      --index;
    int index2 = type[index - 2] == ' ' ? index - 2 : index - 1;
    printType(std::string(type).substr(0, index2).c_str(), (std::string(field) + (type + index - 1)).c_str());
  }
  else if(type[strlen(type) - 1] == '*')
    printType(std::string(type).substr(0, strlen(type) - (strlen(type) > 1 && type[strlen(type) - 2] == ' ' ? 2 : 1)).c_str(), 
              (std::string(field) + "[]").c_str());
  else
  {
    std::string typeName = type;
    if(typeName.size() > 6 && typeName.substr(typeName.size() - 6) == " const")
      typeName = typeName.substr(0, typeName.size() - 6);
    const char* t = streamHandler.getString(typeName.c_str());
    StreamHandler::Specification::const_iterator i = streamHandler.specification.find(t);
    StreamHandler::BasicTypeSpecification::const_iterator b = streamHandler.basicTypeSpecification.find(t);
    StreamHandler::EnumSpecification::const_iterator e = streamHandler.enumSpecification.find(t);
    if(i != streamHandler.specification.end())
    {
      ctrl->print("{ ");
      for(std::vector<StreamHandler::TypeNamePair>::const_iterator j = i->second.begin(); j != i->second.end(); ++j)
      {
        printType(j->second, j->first.c_str());
        ctrl->print("; ");
      }
      if(*field)
        ctrl->print(std::string("} ") + field);
      else
        ctrl->print("}");
    }
    else if(b != streamHandler.basicTypeSpecification.end())
      ctrl->print(std::string(t) + (*field ? " " : "") + field);
    else if(e != streamHandler.enumSpecification.end())
      ctrl->print(std::string(t) + (*field ? " " : "") + field);
    else
      ctrl->print(" UNKNOWN");
  }
}

void RobotConsole::printData(In& stream, const char* type, const char* field)
{
  char buf[1000];
  if(type[strlen(type) - 1] == ']')
  {
    int index = strlen(type);
    while(type[index - 1] != '[')
      --index;
    int index2 = type[index - 2] == ' ' ? index - 2 : index - 1;
    int size = atoi(type + index);
    ctrl->print(std::string(field) + (type + index - 1) + " = { ");
    for(int i = 0; i < size; ++i)
    {
      printData(stream, std::string(type).substr(0, index2).c_str());
      ctrl->print(" ");
    }
    ctrl->print("}");
  }
  else if(type[strlen(type) - 1] == '*')
  {
    int size;
    stream >> size;
    sprintf(buf, "%d", size);
    ctrl->print(std::string(field) + "[" + buf + "] = { ");
    for(int i = 0; i < size; ++i)
    {
      printData(stream, std::string(type).substr(0, strlen(type) - (strlen(type) > 1 && type[strlen(type) - 2] == ' ' ? 2 : 1)).c_str());
      ctrl->print(" ");
    }
    ctrl->print("}");
  }
  else
  {
    if(*field)
      ctrl->print(std::string(field) + " = ");
    std::string typeName = type;
    if(typeName.size() > 6 && typeName.substr(typeName.size() - 6) == " const")
      typeName = typeName.substr(0, typeName.size() - 6);
    const char* t = streamHandler.getString(typeName.c_str());
    StreamHandler::Specification::const_iterator i = streamHandler.specification.find(t);
    StreamHandler::BasicTypeSpecification::const_iterator b = streamHandler.basicTypeSpecification.find(t);
    StreamHandler::EnumSpecification::const_iterator e = streamHandler.enumSpecification.find(t);
    if(i != streamHandler.specification.end())
    {
      ctrl->print("{ ");
      for(std::vector<StreamHandler::TypeNamePair>::const_iterator j = i->second.begin(); j != i->second.end(); ++j)
      {
        printData(stream, j->second, j->first.c_str());
        ctrl->print("; ");
      }
      ctrl->print("}");
    }
    else if(b != streamHandler.basicTypeSpecification.end())
    {
      if(!strcmp("double", t))
      {
        double d;
        stream >> d;
        sprintf(buf, "%lg", d);
      }
      else if(!strcmp("bool", t))
      {
        bool b;
        stream >> b;
        strcpy(buf, b ? "true" : "false");
      }
      else if(!strcmp("float", t))
      {
        float f;
        stream >> f;
        sprintf(buf, "%g", f);
      }
      else if(!strcmp("int", t) || !strcmp("long", t))
      {
        int i;
        stream >> i;
        sprintf(buf, "%d", i);
      }
      else if(!strcmp("unsigned", t) || !strcmp("unsigned int", t) || !strcmp("unsigned long", t))
      {
        unsigned int u;
        stream >> u;
        sprintf(buf, "%u", u);
      }
      else if(!strcmp("short", t))
      {
        short s;
        stream >> s;
        sprintf(buf, "%d", s);
      }
      else if(!strcmp("unsigned short", t))
      {
        unsigned short u;
        stream >> u;
        sprintf(buf, "%u", u);
      }
      else if(!strcmp("char", t))
      {
        char c;
        stream >> c;
        sprintf(buf, "%d", c);
      }
      else if(!strcmp("unsigned char", t))
      {
        unsigned char u;
        stream >> u;
        sprintf(buf, "%u", u);
      }
      else if(!strcmp("class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >", t) ||
              !strcmp("std::string", t))
      {
        std::string s,
                    s2;
        stream >> s;
        int j = 0;
        bool containsSpaces = s.empty() || s[0] == '"' || s.find_first_of(" \n\r\t") != std::string::npos;
        if(containsSpaces)
          buf[j++] = '"';
        for(unsigned int i = 0; i < s.size() && j < int(sizeof(buf) - 4); ++i)
          if(s[i] == '"' && containsSpaces)
          {
            buf[j++] = '\\';
            buf[j++] = '"';
          }
          else if(s[i] == '\n')
          {
            buf[j++] = '\\';
            buf[j++] = 'n';
          }
          else if(s[i] == '\r')
          {
            buf[j++] = '\\';
            buf[j++] = 'r';
          }
          else if(s[i] == '\t')
          {
            buf[j++] = '\\';
            buf[j++] = 't';
          }
          else if(s[i] == '\\')
          {
            buf[j++] = '\\';
            buf[j++] = '\\';
          }
          else
            buf[j++] = s[i];
        if(containsSpaces)
          buf[j++] = '"';
        buf[j] = 0;
      }
      ctrl->print(buf);
    }
    else if(e != streamHandler.enumSpecification.end())
    {
      unsigned i;
      stream >> i;
      if(i < e->second.size())
        ctrl->print(e->second[i]);
      else
      {
        sprintf(buf, "%d", i);
        ctrl->print(buf);
      }
    }
    else
      ctrl->print(" UNKNOWN");
  }
}

bool RobotConsole::parseData(In& stream, const char* type, const char* field)
{
  std::string buf;

  // strings are allowed to be empty, anything else is not
  if(stream.eof() && 
     strcmp(type, "class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >") &&
     strcmp(type, "std::string"))
    return false;
  else if(type[strlen(type) - 1] == ']')
  {
    stream >> buf;
    if(buf.substr(0, strlen(field) + 1) != std::string(field) + "[")
      return false;

    int size = atoi(buf.substr(strlen(field) + 1).c_str()),
        index = strlen(type);
    while(type[index - 1] != '[')
      --index;
    int index2 = type[index - 2] == ' ' ? index - 2 : index - 1;
    if(atoi(type + index) != size)
      return false;

    stream >> buf;
    if(buf != "=")
      return false;

    stream >> buf;
    if(buf != "{")
      return false;

    for(int i = 0; i < size; ++i)
      if(!parseData(stream, std::string(type).substr(0, index2).c_str()))
        return false;

    stream >> buf;
    if(buf != "}")
      return false;
  }
  else if(type[0] == 'A' && strchr(type, '_'))
  {
    stream >> buf;
    if(buf.substr(0, strlen(field) + 1) != std::string(field) + "[")
      return false;

    int size = atoi(buf.substr(strlen(field) + 1).c_str()),
        index = strchr(type, '_') - type;
    if(atoi(type + 1) != size)
      return false;

    stream >> buf;
    if(buf != "=")
      return false;

    stream >> buf;
    if(buf != "{")
      return false;

    for(int i = 0; i < size; ++i)
      if(!parseData(stream, std::string(type).substr(index + 1).c_str()))
        return false;

    stream >> buf;
    if(buf != "}")
      return false;
  }
  else if(type[strlen(type) - 1] == '*')
  {
    stream >> buf;
    if(buf.substr(0, strlen(field) + 1) != std::string(field) + "[")
      return false;

    int size = atoi(buf.substr(strlen(field) + 1).c_str());
    debugOut.out.bin << size;

    stream >> buf;
    if(buf != "=")
      return false;

    stream >> buf;
    if(buf != "{")
      return false;

    for(int i = 0; i < size; ++i)
      if(!parseData(stream, std::string(type).substr(0, strlen(type) - (strlen(type) > 1 && type[strlen(type) - 2] == ' ' ? 2 : 1)).c_str()))
        return false;

    stream >> buf;
    if(buf != "}")
      return false;
  }
  else
  {
    if(*field)
    {
      stream >> buf;
      if(buf != field)
        return false;

      stream >> buf;
      if(buf != "=")
        return false;
    }

    std::string typeName = type;
    if(typeName.size() > 6 && typeName.substr(typeName.size() - 6) == " const")
      typeName = typeName.substr(0, typeName.size() - 6);
    const char* t = streamHandler.getString(typeName.c_str());
    StreamHandler::Specification::const_iterator i = streamHandler.specification.find(t);
    StreamHandler::BasicTypeSpecification::const_iterator b = streamHandler.basicTypeSpecification.find(t);
    StreamHandler::EnumSpecification::const_iterator e = streamHandler.enumSpecification.find(t);
    if(i != streamHandler.specification.end())
    {
      stream >> buf;
      if(buf != "{")
        return false;

      for(std::vector<StreamHandler::TypeNamePair>::const_iterator j = i->second.begin(); j != i->second.end(); ++j)
      {
        if(!parseData(stream, j->second, j->first.c_str()))
          return false;
        stream >> buf;
        if(buf != ";")
          return false;
      }

      stream >> buf;
      if(buf != "}")
        return false;
    }
    else if(b != streamHandler.basicTypeSpecification.end())
    {
      if(!strcmp("double", t))
      {
        double d;
        stream >> d;
        debugOut.out.bin << d;
      }
      else if(!strcmp("bool", t))
      {
        stream >> buf;
        debugOut.out.bin << (buf == "true");
      }
      else if(!strcmp("float", t))
      {
        float f;
        stream >> f;
        debugOut.out.bin << f;
      }
      else if(!strcmp("int", t) || !strcmp("long", t))
      {
        int i;
        stream >> i;
        debugOut.out.bin << i;
      }
      else if(!strcmp("unsigned", t) || !strcmp("unsigned int", t) || !strcmp("unsigned long", t))
      {
        unsigned int u;
        stream >> u;
        debugOut.out.bin << u;
      }
      else if(!strcmp("short", t))
      {
        short s;
        stream >> s;
        debugOut.out.bin << s;
      }
      else if(!strcmp("unsigned short", t))
      {
        unsigned short u;
        stream >> u;
        debugOut.out.bin << u;
      }
      else if(!strcmp("char", t))
      {
        char c;
        stream >> c;
        debugOut.out.bin << c;
      }
      else if(!strcmp("unsigned char", t))
      {
        unsigned char u;
        stream >> u;
        debugOut.out.bin << u;
      }
      else if(!strcmp("class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >", t) ||
              !strcmp(type, "std::string"))
      {
        std::string s;
        stream >> s;
        debugOut.out.bin << s;
      }
    }
    else if(e != streamHandler.enumSpecification.end())
    {
      stream >> buf;
      unsigned i;
      for(i = 0; i < e->second.size(); ++i)
        if(buf == e->second[i])
        {
          debugOut.out.bin << i;
          break;
        }
      if(i == e->second.size())
      {
        i = atoi(buf.c_str());
        if(i)
          debugOut.out.bin << i;
        else
          return false;
      }
    }
    else
      return false;
  }
  return true;
}

bool RobotConsole::sendMof(InConfigMemory& stream)
{
  std::string option;
  stream >> option;
  if(option != "" && option != "bredo")
    return false;

  char buffer[200000];
  bool success = option == "bredo" ? convertMofs(buffer, sizeof(buffer)) : compileMofs(buffer, sizeof(buffer));

  char* p = buffer;
  while(*p)
  {
    char* p2 = strchr(p, '\n');
    *p2 = 0;
    ctrl->printLn(p);
    p = p2 + 1;
  }

  if(success && option != "bredo")
  {
    ctrl->printLn("SpecialActions compiled");
    InBinaryFile file("specialActions.dat");
    if(file.exists())
    {
      memset(buffer, 0, sizeof(buffer));
      file.read(buffer, sizeof(buffer));
      SYNC;
      debugOut.out.bin.write(buffer, strlen(buffer));
      debugOut.out.finishMessage(idMotionNet);
    }
    else
      ctrl->printLn("Error: specialActions.dat not found.");
  }
  return true;
}

bool RobotConsole::repoll(InConfigMemory& stream)
{
  polled[idDebugResponse] = polled[idDrawingManager] = polled[idDrawingManager3D] = false;
  return true;
}

bool RobotConsole::queueFillRequest(InConfigMemory& stream)
{
  std::string request;
  stream >> request;
  QueueFillRequest qfr;
  if(request == "queue")
  {
    qfr.behavior = QueueFillRequest::sendImmediately;
    qfr.filter = QueueFillRequest::sendEverything;
    qfr.target = QueueFillRequest::sendViaNetwork;
  }
  else if(request == "replace")
  {
    qfr.behavior = QueueFillRequest::sendImmediately;
    qfr.filter = QueueFillRequest::latestOnly;
    qfr.target = QueueFillRequest::sendViaNetwork;
  }
  else if(request == "reject")
  {
    qfr.behavior = QueueFillRequest::discardAll;
  }
  else if(request == "collect")
  {
    qfr.behavior = QueueFillRequest::sendAfter;
    qfr.filter = QueueFillRequest::sendEverything;
    qfr.target = QueueFillRequest::sendViaNetwork;

    stream >> qfr.timingMilliseconds;
    qfr.timingMilliseconds *= 1000;
    if(!qfr.timingMilliseconds)
      return false;
  }
  else if(request == "save")
  {
    qfr.filter = QueueFillRequest::sendEverything;
    qfr.target = QueueFillRequest::writeToStick;

    stream >> qfr.timingMilliseconds;
    qfr.timingMilliseconds *= 1000;
    if(!qfr.timingMilliseconds)
      qfr.behavior = QueueFillRequest::sendImmediately;
    else
      qfr.behavior = QueueFillRequest::sendAfter;
  }
  else
    return false;
  SYNC;
  debugOut.out.bin << qfr;
  debugOut.out.finishMessage(idQueueFillRequest);
  return true;
}

bool RobotConsole::moduleRequest(InConfigMemory& stream)
{
  std::string representation, 
              module,
              pattern;
  stream >> representation >> module >> pattern;
  if(representation == "modules")
  {
    for(std::list<ModuleInfo::Module>::const_iterator i = moduleInfo.modules.begin(); i != moduleInfo.modules.end(); ++i)
      if(i->name != "default")
      {
        std::string text = i->name + " (" + (i->processIdentifier == 'c' ? "Cognition" : "Motion") + ", " + i->category + "): ";
        for(std::vector<std::string>::const_iterator j = i->requirements.begin(); j != i->requirements.end(); ++j)
          text += *j + " ";
        text += "-> ";
        for(std::vector<std::string>::const_iterator j = i->representations.begin(); j != i->representations.end(); ++j)
        {
          std::list<ModuleInfo::Provider>::const_iterator k;
          for(k = moduleInfo.providers.begin(); k != moduleInfo.providers.end() && k->representation != *j; ++k)
            ;
          text += *j + (k != moduleInfo.providers.end() && k->selected == i->name ? "* " : " ");
        }
        ctrl->list(text, module, true);
      }
    return true;
  }
  else if(representation == "?")
  {
    for(std::list<ModuleInfo::Provider>::const_iterator i = moduleInfo.providers.begin(); i != moduleInfo.providers.end(); ++i)
    {
      std::string text = i->representation + " (" + (i->processIdentifier == 'c' ? "Cognition" : "Motion") + "): ";
      for(std::vector<std::string>::const_iterator j = i->modules.begin(); j != i->modules.end(); ++j)
        if(*j == i->selected || *j != "default")
          text += *j + (*j == i->selected ? "* " : " ");
      ctrl->list(text, module, true);
    }
    return true;
  }
  else if(representation == "save")
  {
    OutTextFile stream(Global::getSettings().expandLocationFilename("modules.cfg"));
    std::string error = moduleInfo.sendRequest(stream);
    if(error != "")
      ctrl->printLn(error);
    return true;
  }
  else
  {
    bool found = false;
    for(std::list<ModuleInfo::Provider>::iterator i = moduleInfo.providers.begin(); i != moduleInfo.providers.end(); ++i)
      if(i->representation == representation)
      {
        if(module == "?")
        {
          for(std::vector<std::string>::const_iterator j = i->modules.begin(); j != i->modules.end(); ++j)
            if(*j == i->selected || *j != "default")
              ctrl->list(*j + (*j == i->selected ? "*" : ""), pattern);
          found = true;
        }
        else
        {
          if(module == "off")
          {
            i->selected = "";
            found = true;
          }
          else
          {
            if(std::find(i->modules.begin(), i->modules.end(), module) != i->modules.end())
            {
              found = true;
              i->selected = module;
            }
          }
        }
      }
    if(found)
    {
      if(module == "?")
        ctrl->printLn("");
      else
      {
        SYNC;
        moduleInfo.timeStamp = SystemCall::getCurrentSystemTime();
        debugOut.out.bin << moduleInfo.timeStamp;
        std::string error = moduleInfo.sendRequest(debugOut.out.bin);
        if(error == "")
          debugOut.out.finishMessage(idModuleRequest);
        else
        {
          ctrl->printLn(error);
          debugOut.out.cancelMessage();
          return true; // Error message already displayed
        }
        polled[idDebugResponse] = polled[idDrawingManager] = polled[idDrawingManager3D] = false;
      }
      return true;
    }
  }
  return false;
}

bool RobotConsole::moveRobot(InConfigMemory& stream)
{
  SYNC;
  stream >> movePos.v[0] >> movePos.v[1] >> movePos.v[2];
  if(stream.eof())
    moveOp = movePosition;
  else
  {
    stream >> moveRot.v[0] >> moveRot.v[1] >> moveRot.v[2];
    moveOp = moveBoth;
  }
  return true;
}

bool RobotConsole::moveBall(InConfigMemory& stream)
{
  SYNC;
  stream >> movePos.v[0] >> movePos.v[1] >> movePos.v[2];
  moveOp = moveBallPosition;
  return true;
}

bool RobotConsole::xabslSendBehavior(InConfigMemory& stream)
{
  InBinaryFile file(std::string("Xabsl/") + xabslInfo.id + "-ic.dat");
  if(!file.exists())
    return false;
  char s[500000];
  memset(s, 0, sizeof(s));
  file.read(s, sizeof(s) - 1);
  xabslInfo.forceReload();
  waitingFor[idXabslDebugMessage] = 1;
  polled[idXabslDebugMessage] = false;
  SYNC;
  debugOut.out.bin << xabslInfo.id;
  debugOut.out.bin.write(s, strlen(s));
  debugOut.out.finishMessage(idXabslIntermediateCode);
  return true;
}

bool RobotConsole::xabslInputSymbol(InConfigMemory& stream)
{
  SYNC;
  std::string buffer;
  stream >> buffer;
  if(buffer == "?")
  {
    stream >> buffer;
    for(std::list<XabslInfo::InputSymbol>::const_iterator i = xabslInfo.inputSymbols.begin(); i != xabslInfo.inputSymbols.end(); ++i)
      if(i->decimalParameters.size() == 0 &&
         i->booleanParameters.size() == 0 &&
         i->enumeratedParameters.size() == 0)
        ctrl->list(i->name, buffer);
    ctrl->printLn("");
    return true;
  }
  else
    for(std::list<XabslInfo::InputSymbol>::iterator i = xabslInfo.inputSymbols.begin(); i != xabslInfo.inputSymbols.end(); ++i)
      if(i->decimalParameters.size() == 0 &&
         i->booleanParameters.size() == 0 &&
         i->enumeratedParameters.size() == 0 &&
         i->name == buffer)
      {
        stream >> buffer;
        if(buffer == "off" || buffer == "on" || buffer == "")
        {
          i->show = buffer != "off";
          debugOut.out.bin << xabslInfo;
          debugOut.out.finishMessage(idXabslDebugRequest);
          return true;
        }
      }
  return false;
}

bool RobotConsole::xabslOutputSymbol(InConfigMemory& stream)
{
  SYNC;
  std::string buffer;
  stream >> buffer;
  if(buffer == "?")
  {
    stream >> buffer;
    for(std::list<XabslInfo::OutputSymbol>::const_iterator i = xabslInfo.outputSymbols.begin(); i != xabslInfo.outputSymbols.end(); ++i)
      ctrl->list(i->name, buffer);
    ctrl->printLn("");
    return true;
  }
  else
    for(std::list<XabslInfo::OutputSymbol>::iterator i = xabslInfo.outputSymbols.begin(); i != xabslInfo.outputSymbols.end(); ++i)
      if(i->name == buffer)
      {
        stream >> buffer;
        if(buffer == "off" || buffer == "on" || buffer == "")
        {
          i->show = buffer != "off";
          debugOut.out.bin << xabslInfo;
          debugOut.out.finishMessage(idXabslDebugRequest);
          return true;
        }
        else if(buffer == "?")
        {
          stream >> buffer;

          if(i->type == XabslInfo::boolean)
          {
            ctrl->list("true", buffer);
            ctrl->list("false", buffer);
            ctrl->printLn("");
          }
          else if(i->type == XabslInfo::enumerated)
          {
            for(std::list<std::string>::const_iterator j = i->enumeration->elements.begin(); j != i->enumeration->elements.begin(); ++j)
              if(i->name + "." == j->substr(i->name.length() + 1))
                ctrl->list(j->substr(i->name.length() + 1), buffer);
              else
                ctrl->list(*j, buffer);
            ctrl->printLn("");
          }
          return true;
        }
        else if(buffer == "unchanged")
        {
          i->set = false;
          debugOut.out.bin << xabslInfo;
          debugOut.out.finishMessage(idXabslDebugRequest);
          return true;
        }
        else
        {
          if (i->type == XabslInfo::enumerated)
          {
            for(std::list<std::string>::const_iterator j = i->enumeration->elements.begin(); j != i->enumeration->elements.end(); ++j)
              if(*j == buffer || *j == i->enumeration->name + "." + buffer)
              {
                i->set = true;
                i->selectedAlternative = j;
                debugOut.out.bin << xabslInfo;
                debugOut.out.finishMessage(idXabslDebugRequest);
                return true;
              }
          }
          else if (i->type == XabslInfo::boolean)
          {
            if (buffer == "true" || buffer == "false")
            {
              i->set = true;
              i->booleanValue = buffer == "true";
              debugOut.out.bin << xabslInfo;
              debugOut.out.finishMessage(idXabslDebugRequest);
              return true;
            }
          }
          else
          {
            i->set = true;
            i->decimalValue = atof(buffer.c_str());
            debugOut.out.bin << xabslInfo;
            debugOut.out.finishMessage(idXabslDebugRequest);
            return true;
          }
        }
      }
  return false;
}

bool RobotConsole::xabslOption(InConfigMemory& stream)
{
  SYNC;
  std::string buffer;
  stream >> buffer;
  if(buffer == "?")
  {
    stream >> buffer;
    for(std::list<XabslInfo::Option>::const_iterator i = xabslInfo.options.begin(); i != xabslInfo.options.end(); ++i)
      ctrl->list(i->name, buffer);
    ctrl->printLn("");
    return true;
  }
  else if(buffer == "unchanged")
  {
    xabslInfo.selectedOption = 0;
    xabslInfo.selectedBasicBehavior = 0;
    debugOut.out.bin << xabslInfo;
    debugOut.out.finishMessage(idXabslDebugRequest);
    return true;
  }
  else
    for(std::list<XabslInfo::Option>::iterator i = xabslInfo.options.begin(); i != xabslInfo.options.end(); ++i)
      if(i->name == buffer)
      {
        xabslInfo.selectedOption = &*i;
        xabslInfo.selectedBasicBehavior = 0;
        for(unsigned j = 0; j < i->decimalParameters.size(); ++j)
          stream >> xabslInfo.decimalParameters[j];
        for(unsigned j = 0; j < i->booleanParameters.size(); ++j)
        {
          stream >> buffer;
          xabslInfo.booleanParameters[j] = buffer == "true";
        }
        for(unsigned j = 0; j < i->enumeratedParameters.size(); ++j)
        {
          stream >> buffer;
          xabslInfo.enumeratedParameters[j] = buffer;
        }
        debugOut.out.bin << xabslInfo;
        debugOut.out.finishMessage(idXabslDebugRequest);
        return true;
      }
  return false;
}

bool RobotConsole::xabslBasicBehavior(InConfigMemory& stream)
{
  SYNC;
  std::string buffer;
  stream >> buffer;
  if(buffer == "?")
  {
    stream >> buffer;
    for(std::list<XabslInfo::BasicBehavior>::const_iterator i = xabslInfo.basicBehaviors.begin(); i != xabslInfo.basicBehaviors.end(); ++i)
      ctrl->list(i->name, buffer);
    ctrl->printLn("");
    return true;
  }
  else if(buffer == "unchanged")
  {
    xabslInfo.selectedOption = 0;
    xabslInfo.selectedBasicBehavior = 0;
    debugOut.out.bin << xabslInfo;
    debugOut.out.finishMessage(idXabslDebugRequest);
    return true;
  }
  else
    for(std::list<XabslInfo::BasicBehavior>::iterator i = xabslInfo.basicBehaviors.begin(); i != xabslInfo.basicBehaviors.end(); ++i)
    {
      if(i->name == buffer)
      {
        xabslInfo.selectedOption = 0;
        xabslInfo.selectedBasicBehavior = &*i;
        for(int j = 0; j < 10; ++j)
          stream >> xabslInfo.decimalParameters[j];
        debugOut.out.bin << xabslInfo;
        debugOut.out.finishMessage(idXabslDebugRequest);
        return true;
      }
    }
  return false;
}

bool RobotConsole::view3D(InConfigMemory& stream)
{
  std::string buffer;
  stream >> buffer;
  if(buffer == "?")
  {
    stream >> buffer;
    ctrl->list("image", buffer);
    for(int i = 0; i < debugRequestTable.currentNumberOfDebugRequests; ++i)
      if(debugRequestTable.debugRequests[i].description.substr(0, 13) == "debug images:")
        ctrl->list(ctrl->translate(debugRequestTable.debugRequests[i].description.substr(13)), buffer);
    ctrl->printLn("");
    return true;
  }
  else
  {
    std::string buffer2;
    bool jpeg = false;
    for(;;)
    {
      stream >> buffer2;
      if(!jpeg && buffer2 == "jpeg")
        jpeg = true;
      else
        break;
    }

    if(buffer == "image")
    {
      std::string name = buffer2 != "" ? buffer2 : std::string("image");
      if(imageViews3D.find(name) != imageViews3D.end())
      {
        ctrl->printLn("View already exists. Specify a (different) name.");
        return true;
      }
      imageViews3D[name];
      addColorSpaceViews("raw image", name, false);
      if(jpeg)
        handleConsole("dr representation:JPEGImage on");
      else
        handleConsole("dr representation:Image on");
      return true;
    }
    else if(!jpeg)
      for(int i = 0; i < debugRequestTable.currentNumberOfDebugRequests; ++i)
        if(debugRequestTable.debugRequests[i].description.substr(0, 13) == "debug images:" &&
          ctrl->translate(debugRequestTable.debugRequests[i].description.substr(13)) == buffer)
        {
          std::string name = buffer2 != "" ? buffer2 : std::string(buffer);
          if(imageViews3D.find(name) != imageViews3D.end())
          {
            ctrl->printLn("View already exists. Specify a (different) name.");
            return true;
          }
          addColorSpaceViews(debugRequestTable.debugRequests[i].description.substr(13), name, true);
          handleConsole(std::string("dr ") + ctrl->translate(debugRequestTable.debugRequests[i].description) + " on");
          return true;
        }
  }
  return false;
}

bool RobotConsole::viewField(InConfigMemory& stream)
{
  std::string name;
  stream >> name;
  if(fieldViews.find(name) != fieldViews.end())
    ctrl->printLn("View already exists. Specify a different name.");
  else
  {
    fieldViews[name];
    ctrl->setFieldViews(fieldViews);
    ctrl->updateCommandCompletion();
    ctrl->addView(new FieldView(*this, name), 
                  robotName.substr(robotName.rfind('.') + 1) + ".field." + name);
  }
  return true;
}


bool RobotConsole::viewDrawing(InConfigMemory& stream, RobotConsole::Views& views, const char* type)
{
  std::string buffer;
  stream >> buffer;
  if(buffer == "?")
  {
    stream >> buffer;
    for(Views::const_iterator i = views.begin(); i != views.end(); ++i)
      ctrl->list(i->first, buffer);
    ctrl->printLn("");
    return true;
  }
  else
    for(Views::const_iterator i = views.begin(); i != views.end(); ++i)
      if(i->first == buffer)
      {
        stream >> buffer;
        if(buffer == "?")
        {
          stream >> buffer;
          for(DrawingManager::DrawingNameIdTable::const_iterator j = drawingManager.drawingNameIdTable.begin();
              j != drawingManager.drawingNameIdTable.end(); ++j)
            if(!strcmp(drawingManager.getDrawingType(j->first), type))
              ctrl->list(ctrl->translate(j->first), buffer);
          ctrl->printLn("");
          return true;
        }
        else
        {
          for(DrawingManager::DrawingNameIdTable::const_iterator j = drawingManager.drawingNameIdTable.begin();
              j != drawingManager.drawingNameIdTable.end(); ++j)
            if(ctrl->translate(j->first) == buffer && !strcmp(drawingManager.getDrawingType(j->first), type))
            {
              std::string buffer2;
              stream >> buffer2;
              if(buffer2 == "on" || buffer2 == "")
              {
                views[i->first].remove(j->first);
                views[i->first].push_back(j->first);
                handleConsole(std::string("dr debugDrawing:") + buffer + " on");
                return true;
              }
              else if(buffer2 == "off")
              {
                views[i->first].remove(j->first);
                handleConsole(std::string("dr debugDrawing:") + buffer + " off");
                return true;
              }
              else
                return false;
            }
        }
      }
  return false;
}

bool RobotConsole::viewImage(InConfigMemory& stream)
{
  std::string buffer;
  stream >> buffer;
  if(buffer == "?")
  {
    stream >> buffer;
    if(mode != SystemCall::groundTruth)
      ctrl->list("none", buffer);
    ctrl->list("image", buffer);
    for(int i = 0; i < debugRequestTable.currentNumberOfDebugRequests; ++i)
      if(debugRequestTable.debugRequests[i].description.substr(0, 13) == "debug images:")
        ctrl->list(ctrl->translate(debugRequestTable.debugRequests[i].description.substr(13)), buffer);
    ctrl->printLn("");
    return true;
  }
  else if(buffer == "none")
  {
    stream >> buffer;
    std::string name = buffer != "" ? buffer : "none";
    if(imageViews.find(name) != imageViews.end())
    {
      ctrl->printLn("View already exists. Specify a (different) name.");
      return true;
    }
    imageViews[name];
    ctrl->setImageViews(imageViews);
    ctrl->updateCommandCompletion();
    ctrl->addView(new ImageView(*this, "none", name, false), 
                  robotName.substr(robotName.rfind('.') + 1) + ".image." + name);
    return true;
  }
  else 
  {
    std::string buffer2;
    bool jpeg = false, 
         segmented = false;
    for(;;)
    {
      stream >> buffer2;
      if(!jpeg && buffer2 == "jpeg")
        jpeg = true;
      else if(!segmented && buffer2 == "segmented")
        segmented = true;
      else
        break;
    }

    if(buffer == "image")
    {
      std::string name = buffer2 != "" ? buffer2 : std::string("image") + (segmented ? "Segmented" : "");
      if(imageViews.find(name) != imageViews.end())
      {
        ctrl->printLn("View already exists. Specify a (different) name.");
        return true;
      }
      imageViews[name];
      ctrl->setImageViews(imageViews);
      ctrl->updateCommandCompletion();
      ctrl->addView(new ImageView(*this, "raw image", name, segmented), 
        robotName.substr(robotName.rfind('.') + 1) + ".image." + name);
      if(jpeg)
        handleConsole("dr representation:JPEGImage on");
      else
        handleConsole("dr representation:Image on");

      return true;
    }
    else if(!jpeg)
      for(int i = 0; i < debugRequestTable.currentNumberOfDebugRequests; ++i)
        if(debugRequestTable.debugRequests[i].description.substr(0, 13) == "debug images:" &&
          ctrl->translate(debugRequestTable.debugRequests[i].description.substr(13)) == buffer)
        {
          std::string name = buffer2 != "" ? buffer2 : std::string(buffer) + (segmented ? "Segmented" : "");
          if(imageViews.find(name) != imageViews.end())
          {
            ctrl->printLn("View already exists. Specify a (different) name.");
            return true;
          }
          imageViews[name];
          ctrl->setImageViews(imageViews);
          ctrl->updateCommandCompletion();
          ctrl->addView(new ImageView(*this, debugRequestTable.debugRequests[i].description.substr(13), name, segmented), 
                        robotName.substr(robotName.rfind('.') + 1) + ".image." + name);
          handleConsole(std::string("dr ") + ctrl->translate(debugRequestTable.debugRequests[i].description) + " on");
          return true;
        }
  }
  return false;
}

bool RobotConsole::viewPlot(InConfigMemory& stream)
{
  std::string name;
  unsigned plotSize;
  double minValue;
  double maxValue;
  std::string yUnit;
  std::string xUnit;
  double xScale;
  stream >> name >> plotSize >> minValue >> maxValue >> yUnit >> xUnit >> xScale;
  if(plotSize < 2 || minValue >= maxValue)
    return false;
  if(xScale == 0.)
    xScale = 1.;

  if(plotViews.find(name) != plotViews.end())
  {
    ctrl->printLn("View already exists. Specify a (different) name.");
    return true;
  }
  if(plotSize > maxPlotSize)
    maxPlotSize = plotSize;
  plotViews[name];
  ctrl->setPlotViews(plotViews);
  ctrl->updateCommandCompletion();
  ctrl->addView(new PlotView(*this, name, plotSize, minValue, maxValue, yUnit, xUnit, xScale), 
                robotName.substr(robotName.rfind('.') + 1) + ".plot." + name);
  return true;
}

bool RobotConsole::viewPlotDrawing(InConfigMemory& stream)
{
  std::string buffer;
  stream >> buffer;
  if(buffer == "?")
  {
    stream >> buffer;
    for(PlotViews::const_iterator i = plotViews.begin(); i != plotViews.end(); ++i)
      ctrl->list(i->first.c_str(), buffer);
    ctrl->printLn("");
    return true;
  }
  else
    for(PlotViews::const_iterator i = plotViews.begin(); i != plotViews.end(); ++i)
      if(i->first == buffer)
      {
        stream >> buffer;
        if(buffer == "?")
        {
          stream >> buffer;
          for(int j = 0; j < debugRequestTable.currentNumberOfDebugRequests; ++j)
            if(debugRequestTable.debugRequests[j].description.substr(0,5) == "plot:")
              ctrl->list(ctrl->translate(debugRequestTable.debugRequests[j].description.substr(5)).c_str(), buffer);
          ctrl->printLn("");
          return true;
        }
        else
        {
          for(int j = 0; j < debugRequestTable.currentNumberOfDebugRequests; ++j)
            if(ctrl->translate(debugRequestTable.debugRequests[j].description) == std::string("plot:") + buffer)
            {
              Layer layer;
              layer.layer = buffer;
              stream >> buffer;
              if(buffer == "?")
              {
                stream >> buffer;
                for(int color = 1; color < ColorClasses::numOfColors; ++color)
                  ctrl->list(ctrl->translate(ColorClasses::getColorName((ColorClasses::Color) color)).c_str(), buffer);
                ctrl->printLn("");
                return true;
              }
              if(buffer == "off")
              {
                {
                  SYNC;
                  plotViews[i->first].remove(layer);
                }
                handleConsole(std::string("dr plot:") + layer.layer + " off");
                return true;
              }
              else
              {
                int color;
                for(color = 1; color < ColorClasses::numOfColors; ++color)
                  if(ctrl->translate(ColorClasses::getColorName((ColorClasses::Color) color)) == buffer)
                  {
                    layer.color = ColorClasses::Color(color);
                    break;
                  }
                if(color >= ColorClasses::numOfColors)
                {
                  int c = 0;
                  sscanf(buffer.c_str(), "%x", &c);
                  layer.color = ColorRGBA((c >> 16) & 0xff, (c >> 8) & 0xff, c & 0xff);
                }
                stream >> layer.description;
                if(layer.description.empty())
                {
                  const char* name = strrchr(layer.layer.c_str(), ':');
                  if(name)
                    layer.description = name + 1;
                  else
                    layer.description = layer.layer;
                }
                {
                  SYNC;
                  plotViews[i->first].remove(layer);
                  plotViews[i->first].push_back(layer);
                }
                handleConsole(std::string("dr plot:") + layer.layer + " on");
                return true;
              }
              return false;
            }
        }
      }
  return false;
}

bool RobotConsole::initJoystick()
{
  for (int i = 0; i<joystickButtonNumber; ++i)
    joystickButtonValue[i] = false;
  for (int i = 0; i<joystickAxisNumber; ++i)
    joystickAxisValue[i] = 0;

#ifdef LINUX
  joystickID = -1;
  for (int devnum = 0; devnum<32; ++devnum)
  {
    //construct device name
    char devname[16];
    snprintf(devname, 16, "/dev/input/js%i", devnum);

    //try to open device
    joystickID = open(devname, O_RDONLY|O_NONBLOCK);
    if (-1 == joystickID)
    {
      //no joy. try next devicename.
      continue;
    }
    else
    {
      //found joy :-)
      return true;
    }
  }
#endif
  return false;
}


bool RobotConsole::joystickExecCommand(const std::string& cmd)
{
  if (cmd == "")
    return false;

  ctrl->executeConsoleCommand(cmd, this);
  if(joystickTrace)
    ctrl->printLn(cmd);

  return true;
}

void RobotConsole::handleJoystick()
{
//return if no joystick was found
  if (-1 == joystickID)
    return;

#ifdef LINUX
  bool changedAxis(false);

  //read multiple events from event buffer
  struct js_event e;
  ssize_t r;
  while ( (r = read(joystickID, &e, sizeof(struct js_event))) > 0)
  {
    //button press or release event
    if ((e.type & JS_EVENT_BUTTON) && (e.number < joystickButtonNumber))
    {
      joystickButtonValue[e.number] = e.value;
      if (e.value)
        joystickExecCommand(joystickButtonPressCommand[e.number]);
      else
        joystickExecCommand(joystickButtonReleaseCommand[e.number]);
    }
    //axis position changed
    else if ( (e.type & JS_EVENT_AXIS) && (e.number < joystickAxisNumber))
    {
      joystickAxisValue[e.number] = e.value;
      changedAxis = true;
    }
  }

  //check for errors. EAGAIN is caused by O_NONBLOCK and is not an error.
  if (errno!=EAGAIN)
  {
    perror("joystick: read failed");
    return;
  }

  //execute axis commands after at least 100ms
  if( motionCommand != ""
      && lines.empty()
      && SystemCall::getTimeSince(joystickLastTime) >= 100 ) 
  {
    ASSERT(joystickAxisNumber == 6);
    double speeds[joystickAxisNumber]; 
    for(int i = 0; i < joystickAxisNumber; ++i)
    {
      const double threshold  = motionCommandThresholds[i];
      const double d = (double)-joystickAxisValue[i] / 32767.0;
      double speed;
      if(d < -threshold)
        speed = (d + threshold) / (1 - threshold);
      else if(d > threshold)
        speed = (d - threshold) / (1 - threshold);
      else
        speed = 0;
      speeds[i] = speed * motionCommandMaxSpeeds[i];
    }
    char buffer[300];
    snprintf(buffer, 300, motionCommand.c_str(),
             speeds[motionCommandIndices[0]], 
             speeds[motionCommandIndices[1]],
             speeds[motionCommandIndices[2]],
             speeds[motionCommandIndices[3]],
             speeds[motionCommandIndices[4]],
             speeds[motionCommandIndices[5]]);
    if (joystickLastMotionCommand != buffer)
    {
      joystickLastMotionCommand = buffer;
      joystickExecCommand(buffer);

    }
    joystickLastTime = SystemCall::getCurrentSystemTime();
  }
#endif


#ifdef _WIN32
  JOYINFOEX joy;
  joy.dwSize = sizeof(joy);
  joy.dwFlags = JOY_RETURNALL;
  int r = joyGetPosEx(joystickID, &joy);
  if (r != JOYERR_NOERROR)
    return;

  //check button state
  bool buttonCommandExecuted(false);
  bool buttonEvent(false);
  for (int i = 0; i<32; ++i)
  {
    const bool val = (((1<<i) & joy.dwButtons) != 0);
    if (val != joystickButtonValue[i])
    {
      buttonEvent = true;
      joystickButtonValue[i] = val;
      
      if (val) //button press event
      {
        buttonCommandExecuted |= joystickExecCommand(joystickButtonPressCommand[i]);
      }
      else //button release event
      {
        buttonCommandExecuted |= joystickExecCommand(joystickButtonReleaseCommand[i]);
      }
    }
  }

  //coolie hat
  JOYCAPS joyCaps;
  if(joyGetDevCaps(joystickID, &joyCaps, sizeof(joyCaps)) == 0)
  {
    if (joyCaps.wCaps != 0xffff)
    {
      int button = 32 + joyCaps.wCaps / 4500; //32-40
      buttonCommandExecuted |= joystickExecCommand(joystickButtonPressCommand[button]);
    }
  }

  // walk and move head only if no button command is 
  if (buttonCommandExecuted)
    return;

  unsigned int timeNow = SystemCall::getCurrentSystemTime();
  if(motionCommand != ""
     && lines.empty()
     && (timeNow - joystickLastTime >= 100))  // don't generate too many commands
  {
    double speeds[6]; 
    for(int i = 0; i < 6; ++i)
      speeds[i] = convert(*(&joy.dwXpos + i), motionCommandThresholds[i]) * motionCommandMaxSpeeds[i];
    char buffer[1024];
    sprintf(buffer, motionCommand.c_str(),
            speeds[motionCommandIndices[0]], 
            speeds[motionCommandIndices[1]],
            speeds[motionCommandIndices[2]],
            speeds[motionCommandIndices[3]],
            speeds[motionCommandIndices[4]],
            speeds[motionCommandIndices[5]]);
    if (joystickLastMotionCommand != buffer)
    {
      joystickExecCommand(buffer);
      joystickLastMotionCommand = buffer;
    }
    joystickLastTime = timeNow;
  }
#endif
}

double RobotConsole::convert(unsigned value, double threshold) const
{
  double d = (32768 - int(value)) / 32767.0;
  if(d < -threshold)
    return (d + threshold) / (1 - threshold);
  else if(d > threshold)
    return (d - threshold) / (1 - threshold);
  else
    return 0;
}

bool RobotConsole::joystickCommand(InConfigMemory& stream)
{
  std::string command;
  stream >> command;
  if(command == "show")
  {
    joystickTrace = true;
    return true;
  }
  else if(command == "hide")
  {
    joystickTrace = false;
    return true;
  }
  else if(command == "press")
  {
    int number;
    stream >> number;

    if (number >0 && number <= joystickButtonNumber)
    {
      //rest of line into one string (std::stringstream-like .str() would be nice :-/):
      bool first(true);
      std::string line;
      while(!stream.eof())
      {
        std::string text;
        stream >> text;
        if(first)
          first = false;
        else
          line += " ";
        line += text;
      }
      joystickButtonPressCommand[number-1] = line;
      return true;
    }
    return false;
  }
  else if(command == "release")
  {
    int number;
    stream >> number;
    if (number > 0 && number <= joystickButtonNumber)
    {
      //rest of line into one string (std::stringstream-like .str() would be nice :-/):
      bool first(true);
      std::string line;
      while(!stream.eof())
      {
        std::string text;
        stream >> text;
        if(first)
          first = false;
        else
          line += " ";
        line += text;
      }

      joystickButtonReleaseCommand[number-1] = line;
      return true;
    }
    return false;
  }
  std::string line = "";
  bool first = true;
  while(!stream.eof())
  {
    std::string text;
    stream >> text;
    if(first)
      first = false;
    else
      line += " ";
    line += text;
  }
  if(command == "motion")
  {
    SYNC;
    motionCommand = line;
    int pos = motionCommand.find("$");
    for(int i = 0; i < 6; ++i)
      motionCommandIndices[i] = 0;
    int i = 0;
    while(i < 6 && pos != -1)
    {
      int id = motionCommand[pos + 1] - '1';
      if(id >= 0 && id < 6)
      {
        motionCommandIndices[i++] = id;
        motionCommand.replace(pos, 2, "%lf");
        pos = motionCommand.find("$");
      }
      else
        return false;
    }
  }
  else
  {
    int id = atoi(command.c_str());
    if(id > 0 && id <= 40)
      joystickButtonPressCommand[--id] = line;
    else
      return false;
  }
  return true;
}

bool RobotConsole::joystickSpeeds(InConfigMemory& stream)
{
  int id;
  stream >> id;
  if(id > 0 && id <= 6)
  {
    stream >> motionCommandMaxSpeeds[id - 1] >> motionCommandThresholds[id - 1];
    return true;
  }
  else
    return false;
}

bool RobotConsole::saveRequest(InConfigMemory& stream, bool first)
{
  std::string buffer;
  std::string path;
  stream >> buffer;
  stream >> path;

  for(int i = 0; i < debugRequestTable.currentNumberOfDebugRequests; ++i)
    if(std::string("debugData:") + buffer == ctrl->translate(debugRequestTable.debugRequests[i].description))
    {
      if(first) // request current Values
      {
        SYNC;
        debugOut.out.bin << DebugRequest(debugRequestTable.debugRequests[i].description, true, true);
        debugOut.out.finishMessage(idDebugRequest);

        waitingFor[idDebugDataResponse] = 1;
        polled[idDebugDataResponse] = true; // no automatic repolling
        handleConsole(std::string("_save ") + buffer + " " + path);
        return true;
      }
      else
      {         
        DebugDataInfos::const_iterator j = debugDataInfos.find(debugRequestTable.debugRequests[i].description.substr(11));
        ASSERT(j != debugDataInfos.end());

        if(path == "") // no path specified, use default location
        {         
          std::string filename = getPathForRepresentation(debugRequestTable.debugRequests[i].description.substr(11));
          if(filename != "")
          {
            Writer writer(filename, *this);
            j->second.second->handleAllMessages(writer);
          }
          else
            ctrl->printLn("Error getting filename for " + debugRequestTable.debugRequests[i].description.substr(11) + ". Representation can not be saved.");
        }
        else  // path specified
        {
          if(path.find_first_of("/") == 0)
            path = path.substr(1);
          Writer writer("../" + path, *this);
          j->second.second->handleAllMessages(writer);
        }
      }
    }
  return true;
}

void RobotConsole::handleKeyEvent(int key, bool pressed)
{
  std::string* joystickButtonCommand(pressed ? joystickButtonPressCommand : joystickButtonReleaseCommand);
  if(joystickButtonCommand[key] != "")
    ctrl->executeConsoleCommand(joystickButtonCommand[key], this);
}

void RobotConsole::myPrintData(In& stream, const char* type, std::ofstream& file, const char* field)
{
  char buf[1000];
  if(*field)
    file << std::endl << "# " << std::string(field) << std::endl;

  if(type[strlen(type) - 1] == ']')
  {
    int index = strlen(type);
    while(type[index - 1] != '[')
      --index;
    int index2 = type[index - 2] == ' ' ? index - 2 : index - 1;
    int size = atoi(type + index);
    for(int i = 0; i < size; ++i)
    {
      myPrintData(stream, std::string(type).substr(0, index2).c_str(), file);
      file << " ";
    }
  }
  else if(type[strlen(type) - 1] == '*')
  {
    int size;
    stream >> size;
    for(int i = 0; i < size; ++i)
    {
      myPrintData(stream, std::string(type).substr(0, strlen(type) - 2).c_str(), file);
      file << " ";
    }
    file << " ";
  }
  else
  {
    const char* t = streamHandler.getString(type);
    StreamHandler::Specification::const_iterator i = streamHandler.specification.find(t);
    StreamHandler::BasicTypeSpecification::const_iterator b = streamHandler.basicTypeSpecification.find(t);
    StreamHandler::EnumSpecification::const_iterator e = streamHandler.enumSpecification.find(t);
    if(i != streamHandler.specification.end())
    {
      for(std::vector<StreamHandler::TypeNamePair>::const_iterator j = i->second.begin(); j != i->second.end(); ++j)
      {        
        myPrintData(stream, j->second, file, j->first.c_str());
        file << " ";
      }
      file << std::endl;
    }
    else if(b != streamHandler.basicTypeSpecification.end())
    {
      if(!strcmp("double", t))
      {
        double d;
        stream >> d;
        sprintf(buf, "%lg", d);
      }
      else if(!strcmp("bool", t))
      {
        bool b;
        stream >> b;
        strcpy(buf, b ? "true" : "false");
      }
      else if(!strcmp("float", t))
      {
        float f;
        stream >> f;
        sprintf(buf, "%g", f);
      }
      else if(!strcmp("int", t) || !strcmp("long", t))
      {
        int i;
        stream >> i;
        sprintf(buf, "%d", i);
      }
      else if(!strcmp("unsigned", t) || !strcmp("unsigned int", t) || !strcmp("unsigned long", t))
      {
        unsigned int u;
        stream >> u;
        sprintf(buf, "%u", u);
      }
      else if(!strcmp("short", t))
      {
        short s;
        stream >> s;
        sprintf(buf, "%d", s);
      }
      else if(!strcmp("unsigned short", t))
      {
        unsigned short u;
        stream >> u;
        sprintf(buf, "%u", u);
      }
      else if(!strcmp("char", t))
      {
        char c;
        stream >> c;
        sprintf(buf, "%d", c);
      }
      else if(!strcmp("unsigned char", t))
      {
        unsigned char u;
        stream >> u;
        sprintf(buf, "%u", u);
      }
      else if(!strcmp("class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >", t) ||
        !strcmp("std::string", t))
      {
        std::string s,
          s2;
        stream >> s;
        int j = 0;
        bool containsSpaces = s.empty() || s[0] == '"' || s.find_first_of(" \n\r\t") != std::string::npos;
        if(containsSpaces)
          buf[j++] = '"';
        for(unsigned i = 0; i < s.size() && j < int(sizeof(buf) - 4); ++i)
          if(s[i] == '"' && containsSpaces)
          {
            buf[j++] = '\\';
            buf[j++] = '"';
          }
          else if(s[i] == '\n')
          {
            buf[j++] = '\\';
            buf[j++] = 'n';
          }
          else if(s[i] == '\r')
          {
            buf[j++] = '\\';
            buf[j++] = 'r';
          }
          else if(s[i] == '\t')
          {
            buf[j++] = '\\';
            buf[j++] = 't';
          }
          else if(s[i] == '\\')
          {
            buf[j++] = '\\';
            buf[j++] = '\\';
          }
          else
            buf[j++] = s[i];
          if(containsSpaces)
            buf[j++] = '"';
          buf[j] = 0;
      }
      file << buf;
    }
    else if(e != streamHandler.enumSpecification.end())
    {
      unsigned i;
      stream >> i;
      if(i < e->second.size())
      {
        // write comment
        file << "# ";
        for(unsigned int j = 0; j < e->second.size(); j++)
          file << j << " = " << e->second[j] << " ";
        file << std::endl << i;
      }
      else
      {
        sprintf(buf, "%d", i);
        file << buf;
      }
    }
    else
      file << " UNKNOWN";
  }
}


std::string RobotConsole::getPathForRepresentation( std::string representation )
{
  std::string filename = "";

  // Get correct filename
  if(representation == "representation:JointCalibration")
    filename = "jointCalibration.cfg";
  else if(representation == "representation:WalkingParameters")
    filename = "walking.cfg";
  else if(representation == "representation:ExpWalkingParameters3")
    filename = "expWalking3.cfg";
  else if(representation == "representation:SensorCalibration")
    filename = "sensorCalibration.cfg";
  else if(representation == "representation:CameraCalibration")
    filename = "cameraCalibration.cfg";

  // Check where the file has to be placed. Search order: 
  // /Robots/[robotname]
  // /Locations/[locationname]
  // /. (the config directory, default return value)
  std::string filepath = "";
  std::fstream fin;
  
  filepath = "../" + Global::getSettings().expandRobotFilename(filename);
  fin.open(filepath.c_str(), std::ios::in);
  if(fin.is_open())
  {
    fin.close();
    return filepath;
  }

  filepath = "../" + Global::getSettings().expandLocationFilename(filename);
  fin.open(filepath.c_str(), std::ios::in);
  if(fin.is_open())
  {
    fin.close();
    return filepath;
  }

  // if file is not anywhere, return config directory as default directory
  filepath = "../" + filename;
  fin.open(filepath.c_str(), std::ios::in);
  if(fin.is_open())
  {
    fin.close();
    return filepath;
  }

  return "";
}

