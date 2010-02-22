/** 
* @file SpecialActions.cpp
* This file implements a module that creates the motions of special actions.
* @author <A href="mailto:dueffert@informatik.hu-berlin.de">Uwe Düffert</A>
* @author Martin Lötzsch
* @author Max Risler
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
*/

#include "SpecialActions.h"
#include "Platform/GTAssert.h"
#include "Tools/Streams/InStreams.h"

PROCESS_WIDE_STORAGE SpecialActions* SpecialActions::theInstance = 0;

void SpecialActions::MotionNetData::load(In& stream)
{
  for(int i = 0; i < SpecialActionRequest::numOfSpecialActions; ++i)
    stream >> label_extern_start[i];
  label_extern_start[SpecialActionRequest::numOfSpecialActions] = 0;

  int numberOfNodes;
  stream >> numberOfNodes;

  if(nodeArray)
    delete[] nodeArray;
  
  nodeArray = new MotionNetNode[numberOfNodes];
  
  for(int i = 0; i < numberOfNodes; ++i)
  {
    short s;
    stream >> s;
    
    switch (s)
    {
    case 2:
      nodeArray[i].d[0] = (short) MotionNetNode::typeTransition;
      stream >> nodeArray[i].d[1] >> nodeArray[i].d[JointData::numOfJoints + 3];
      break;
    case 1:
      nodeArray[i].d[0] = (short) MotionNetNode::typeConditionalTransition;
      stream >> nodeArray[i].d[1] >> nodeArray[i].d[2] >> nodeArray[i].d[JointData::numOfJoints + 3];
      break;
    case 4:
      nodeArray[i].d[0] = (short) MotionNetNode::typeHardness;
      for(int j = 1; j < JointData::numOfJoints + 3; j++)
        stream >> nodeArray[i].d[j];
      break;
    case 3:
      nodeArray[i].d[0] = (short) MotionNetNode::typeData;
      for(int j = 1; j < JointData::numOfJoints + 1; ++j)
      {
        stream >> nodeArray[i].d[j];
        if (nodeArray[i].d[j] != JointData::off    &&
            nodeArray[i].d[j] != JointData::ignore )        
          nodeArray[i].d[j] = fromDegrees(nodeArray[i].d[j]);
      }
      for(int k = JointData::numOfJoints + 1; k < JointData::numOfJoints + 4; ++k)
        stream >> nodeArray[i].d[k];
      break;
    }
  }
}

SpecialActions::SpecialActions() :
wasEndOfSpecialAction(false),
//hardnessInterpolationStart(0),
hardnessInterpolationCounter(0),
hardnessInterpolationLength(0),
wasActive(false),
dataRepetitionCounter(0),
lastSpecialAction(SpecialActionRequest::numOfSpecialActions),
mirror(false)
{
  theInstance = this;

  InConfigFile file("specialActions.dat");
  if(!file.exists() || file.eof()) 
  {
    OUTPUT(idText, text, "SpecialActions : Error, 'specialActions.dat' not found.");
  }
  else
    motionNetData.load(file);

  // create an uninitialised motion request to set startup motion
  currentNode = motionNetData.label_extern_start[SpecialActionRequest().specialAction];
}

void SpecialActions::init()
{
  // read entries from file
  InConfigFile odometryFile("odometry.cfg");
  if(!odometryFile.exists() || odometryFile.eof()) 
  {
    OUTPUT(idText, text, "SpecialActions : Error, 'odometry.cfg' not found.");
  }
  else 
  {
    while(!odometryFile.eof()) 
    {
      std::string fileEntry;
      odometryFile >> fileEntry;
      if(fileEntry != "")
      {
        SpecialActionRequest::SpecialActionID i = SpecialActionRequest::getSpecialActionFromName(fileEntry.c_str());
        if(i < SpecialActionRequest::numOfSpecialActions)
        {
          int t;
          double tmp;
          odometryFile >> t;
          switch(t) 
          {
          case 0:
            // no odometry
            infoTable[i].type = SpecialActionInfo::none;
            break;
          case 1:
            // once
            infoTable[i].type = SpecialActionInfo::once;
            odometryFile >> infoTable[i].odometryOffset.translation.x;
            odometryFile >> infoTable[i].odometryOffset.translation.y;
            odometryFile >> tmp;
            infoTable[i].odometryOffset.fromAngle(tmp);
            break;
          case 2:
            // homogeneous
            infoTable[i].type = SpecialActionInfo::homogeneous;
            odometryFile >> infoTable[i].odometryOffset.translation.x;
            odometryFile >> infoTable[i].odometryOffset.translation.y;
            odometryFile >> tmp;
            infoTable[i].odometryOffset.fromAngle(tmp);
            // convert from mm/seconds to mm/tick
            double motionCycleTime = theRobotDimensions.motionCycleTime;
            infoTable[i].odometryOffset.translation.x *= motionCycleTime;
            infoTable[i].odometryOffset.translation.y *= motionCycleTime;
            // convert from rad/seconds to rad/tick
            infoTable[i].odometryOffset.rotation *= motionCycleTime;
            break;
          }
          odometryFile >> t;
          infoTable[i].isMotionStable = (t!=0);
        }
        else
        {
          OUTPUT(idText, text, "SpecialActions : Error, invalid odometry entry for :");
          OUTPUT(idText, text, fileEntry);
        }
      }
    }
  }
}

bool SpecialActions::getNextData(const SpecialActionRequest& specialActionRequest, 
                                 SpecialActionsOutput& specialActionsOutput)
{
  while((MotionNetNode::NodeType)short(motionNetData.nodeArray[currentNode].d[0]) != MotionNetNode::typeData)
  {
    switch ((MotionNetNode::NodeType)short(motionNetData.nodeArray[currentNode].d[0]))
    {
    case MotionNetNode::typeHardness:
      lastHardnessRequest = specialActionsOutput.jointHardness;//currentHardnessRequest;
      motionNetData.nodeArray[currentNode].toHardnessRequest(currentHardnessRequest, hardnessInterpolationLength);
      hardnessInterpolationCounter = hardnessInterpolationLength;
      currentNode++;
      break;
    case MotionNetNode::typeConditionalTransition:
      if(motionNetData.nodeArray[currentNode].d[2] != (short) specialActionRequest.specialAction)
      {
        currentNode++;
        break;
      }
      //no break here: if condition is true, continue with transition!
    case MotionNetNode::typeTransition:
      // follow transition
      if (currentNode == 0) //we come from extern
        currentNode = motionNetData.label_extern_start[(short) specialActionRequest.specialAction];
      else
        currentNode = short(motionNetData.nodeArray[currentNode].d[1]);
      mirror = specialActionRequest.mirror;
      // leave if transition to external motion
      if (currentNode == 0)
        return false;
      break;
    case MotionNetNode::typeData:
      break;
    }
  }

  motionNetData.nodeArray[currentNode].toJointRequest(currentRequest, dataRepetitionLength, interpolationMode, deShakeMode);
  dataRepetitionCounter = dataRepetitionLength;

  specialActionsOutput.executedSpecialAction.specialAction = motionNetData.nodeArray[currentNode++].getSpecialActionID();
  specialActionsOutput.executedSpecialAction.mirror = mirror;
  specialActionsOutput.isMotionStable = infoTable[specialActionsOutput.executedSpecialAction.specialAction].isMotionStable;

  //get currently executed special action from motion net traversal:
  if(specialActionsOutput.executedSpecialAction.specialAction != lastSpecialAction)
  {
    currentInfo = infoTable[specialActionsOutput.executedSpecialAction.specialAction];
    lastSpecialAction = specialActionsOutput.executedSpecialAction.specialAction;
  }

  return true;
}

void SpecialActions::calculateJointRequest(JointRequest& jointRequest)
{
  double ratio, f, t;

  //joint angles
  if (interpolationMode) 
  {
    ratio = dataRepetitionCounter / (double) dataRepetitionLength;
    for(int i = 0; i < JointData::numOfJoints; ++i)
    {
      f = lastRequest.angles[i];
      if(!mirror)
        t = currentRequest.angles[i];
      else
        t = currentRequest.mirrorAngle((JointData::Joint)i);
      // if fromAngle is off or ignore use JointData for further calculation
      if(f == JointData::off || f == JointData::ignore)
        f = theFilteredJointData.angles[i];

      // if toAngle is off or ignore -> turn joint off/ignore
      if(t == JointData::off || t == JointData::ignore)
        jointRequest.angles[i] = t;
      //interpolate
      else
        jointRequest.angles[i] = (double) (t + (f - t) * ratio);
    }
  }
  else
  {
    if(!mirror)
      jointRequest = currentRequest;
    else
      jointRequest.mirror(currentRequest);
  }

  //hardness stuff
  if(hardnessInterpolationCounter <= 0)
  {
    if(!mirror)
      jointRequest.jointHardness = currentHardnessRequest;
    else
      jointRequest.jointHardness.mirror(currentHardnessRequest);
  }
  else
  {
    ratio = ((double)hardnessInterpolationCounter) / hardnessInterpolationLength;
    for(int i = 0; i < JointData::numOfJoints; i++)
    {
      f = lastHardnessRequest.getHardness(i);
      if(!mirror)
        t = currentHardnessRequest.getHardness(i);
      else
        t = currentHardnessRequest.mirror((JointData::Joint)i);
      jointRequest.jointHardness.hardness[i] = int(t + (f - t) * ratio);
    }
  }
}

void SpecialActions::update(SpecialActionsOutput& specialActionsOutput)
{
  double speedFactor = 1.0;
  MODIFY("parameters:SpecialActions:speedFactor", speedFactor);
  if(theMotionSelection.specialActionMode != MotionSelection::deactive)
  {
    specialActionsOutput.isLeavingPossible = true;
    if(dataRepetitionCounter <= 0)
    {
      if(!wasActive)
      {
        //entered from external motion
        currentNode = 0;
        for(int i = 0; i < JointData::numOfJoints; ++i)
          lastRequest.angles[i] = theFilteredJointData.angles[i];
        lastSpecialAction = SpecialActionRequest::numOfSpecialActions;
      }

      // this is need when a special actions gets executed directly after another without
      // switching to a different motion for interpolating the hardness
      if(wasEndOfSpecialAction)
      {
        specialActionsOutput.jointHardness.resetToDefault();
        if(!mirror)
          lastHardnessRequest = currentHardnessRequest;
        else
          lastHardnessRequest.mirror(currentHardnessRequest);
        currentHardnessRequest.resetToDefault();
      }
      wasEndOfSpecialAction = false;
      // search next data, leave on transition to external motion
      if(!getNextData(theMotionSelection.specialActionRequest, specialActionsOutput))
      {
        wasActive = true;
        wasEndOfSpecialAction = true;
        specialActionsOutput.odometryOffset = Pose2D();
        return;
      }
    }
    else
    {
      dataRepetitionCounter -= int(theRobotDimensions.motionCycleTime * 1000 * speedFactor);
      hardnessInterpolationCounter -= int(theRobotDimensions.motionCycleTime * 1000 * speedFactor);
    }

    //set current joint values
    calculateJointRequest(specialActionsOutput);

    //odometry update
    if (currentInfo.type == SpecialActionInfo::homogeneous || currentInfo.type == SpecialActionInfo::once)
      if (mirror)
        specialActionsOutput.odometryOffset = Pose2D(-currentInfo.odometryOffset.rotation, currentInfo.odometryOffset.translation.x, -currentInfo.odometryOffset.translation.y);
      else
        specialActionsOutput.odometryOffset = currentInfo.odometryOffset;
    else
      specialActionsOutput.odometryOffset = Pose2D();
    if (currentInfo.type == SpecialActionInfo::once)
      currentInfo.type = SpecialActionInfo::none;

    //store value if current data line finished
    if(dataRepetitionCounter <= 0)
    {
      if(!mirror)
        lastRequest = currentRequest;
      else 
        lastRequest.mirror(currentRequest);
    }
    specialActionsOutput.isLeavingPossible = false;
    if(deShakeMode)
      for(int i = JointData::armLeft0; i <= JointData::armRight3; ++i)
        if(randomDouble() < 0.25)
          specialActionsOutput.angles[i] = JointData::off;
  }
  wasActive = theMotionSelection.specialActionMode == MotionSelection::active;
}

bool SpecialActions::handleMessage(InMessage& message)
{
  return theInstance && theInstance->handleMessage2(message);
}

bool SpecialActions::handleMessage2(InMessage& message)
{
  if(message.getMessageID() == idMotionNet)
  {
    motionNetData.load(message.config);
    wasActive = false;
    dataRepetitionCounter = 0;
    return true;
  }
  else
    return false;
}

MAKE_MODULE(SpecialActions, Motion Control)
