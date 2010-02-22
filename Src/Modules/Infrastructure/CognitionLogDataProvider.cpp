/** 
* @file CognitionLogDataProvider.cpp
* This file implements a module that provides data replayed from a log file.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#include "CognitionLogDataProvider.h"
#include "Representations/Perception/JPEGImage.h"
#include "Tools/Team.h"
#include "Tools/Debugging/DebugDrawings3D.h"

PROCESS_WIDE_STORAGE CognitionLogDataProvider* CognitionLogDataProvider::theInstance = 0;

#define UPDATE2(representation, cmd) \
void CognitionLogDataProvider::update(representation& _##representation) \
{ \
  ASSERT(id##representation < numOfDataMessageIDs);     \
  if(representationBuffer[id##representation]) \
    _##representation = *((representation*)representationBuffer[id##representation]); \
  cmd \
}

#define UPDATE(representation) UPDATE2(representation, )

#define ALLOC(representation) \
  ASSERT(id##representation < numOfDataMessageIDs);     \
  if(!representationBuffer[id##representation]) \
    representationBuffer[id##representation] = new representation;

#define HANDLE2(representation, cmd) \
  case id##representation:\
  { \
    ALLOC(representation) \
    message.bin >> *((representation*)representationBuffer[id##representation]); \
    cmd \
    return true; \
  }

#define HANDLE(representation) HANDLE2(representation, )

#define ASSIGN(target, source) \
  ALLOC(target) \
  (target&) *representationBuffer[id##target] = (target&) *representationBuffer[id##source];

CognitionLogDataProvider::CognitionLogDataProvider() :
frameDataComplete(false)
{
  theInstance = this;
  for(int i = 0; i < numOfDataMessageIDs; i++)
    representationBuffer[i] = 0;
  ImageCoordinateSystem::initTables(CameraInfo());
}

CognitionLogDataProvider::~CognitionLogDataProvider()
{
  for(int i = 0; i < numOfDataMessageIDs; i++)
    if(representationBuffer[i])
      delete representationBuffer[i];
  theInstance = 0;
}

UPDATE2(BallModel,
  TEAM_OUTPUT(idTeamMateBallModel, bin, _BallModel);
)
UPDATE(BallPercept)
UPDATE(CameraMatrix)
UPDATE(FrameInfo)
UPDATE(GoalPercept)
UPDATE(GroundTruthBallModel)
UPDATE(GroundTruthRobotPose)
UPDATE2(Image, 
  DECLARE_DEBUG_DRAWING3D("representation:Image", "camera");
  const double distance = 300;
  IMAGE3D("representation:Image", distance, 0, 0, 0, 0, 0, 
          distance * _Image.cameraInfo.resolutionWidth / _Image.cameraInfo.focalLength,
          distance * _Image.cameraInfo.resolutionHeight / _Image.cameraInfo.focalLength,
          _Image);
  DEBUG_RESPONSE("representation:JPEGImage", OUTPUT(idJPEGImage, bin, JPEGImage(_Image)); );
)
UPDATE2(ImageCoordinateSystem, 
  DECLARE_DEBUG_DRAWING("loggedHorizon", "drawingOnImage"); // displays the horizon
  ARROW("loggedHorizon",
       _ImageCoordinateSystem.origin.x,
       _ImageCoordinateSystem.origin.y,
       _ImageCoordinateSystem.origin.x + _ImageCoordinateSystem.rotation.c[0].x * 50,
       _ImageCoordinateSystem.origin.y + _ImageCoordinateSystem.rotation.c[0].y * 50,
       1, Drawings::ps_solid, ColorRGBA(255,0,0));
  ARROW("loggedHorizon",
       _ImageCoordinateSystem.origin.x,
       _ImageCoordinateSystem.origin.y,
       _ImageCoordinateSystem.origin.x + _ImageCoordinateSystem.rotation.c[1].x * 50,
       _ImageCoordinateSystem.origin.y + _ImageCoordinateSystem.rotation.c[1].y * 50,
       1, Drawings::ps_solid, ColorRGBA(255,0,0));
  GENERATE_DEBUG_IMAGE(corrected,
    Image* i = (Image*) representationBuffer[idImage];
    if(i)
    {
      INIT_DEBUG_IMAGE_BLACK(corrected);
      int yDest = -_ImageCoordinateSystem.toCorrectedCenteredNeg(0, 0).y;
      for(int ySrc = 0; ySrc < i->cameraInfo.resolutionHeight; ++ySrc)
        for(int yDest2 = -_ImageCoordinateSystem.toCorrectedCenteredNeg(0, ySrc).y; yDest <= yDest2; ++yDest)
        {
          int xDest = -_ImageCoordinateSystem.toCorrectedCenteredNeg(0, ySrc).x;
          for(int xSrc = 0; xSrc < i->cameraInfo.resolutionWidth; ++xSrc)
          {
            for(int xDest2 = -_ImageCoordinateSystem.toCorrectedCenteredNeg(xSrc, ySrc).x; xDest <= xDest2; ++xDest)
            {
              DEBUG_IMAGE_SET_PIXEL_YUV(corrected, xDest + int(i->cameraInfo.opticalCenter.x + 0.5), 
                                                   yDest + int(i->cameraInfo.opticalCenter.y + 0.5),
                                                   i->image[ySrc][xSrc].y, 
                                                   i->image[ySrc][xSrc].cb,
                                                   i->image[ySrc][xSrc].cr);
            }
          }
        }
      SEND_DEBUG_IMAGE(corrected);
    }
  );
)
UPDATE(LinePercept)
UPDATE2(RobotPose, TEAM_OUTPUT(idTeamMateRobotPose, bin, _RobotPose); )


bool CognitionLogDataProvider::handleMessage(InMessage& message)
{
  return theInstance && theInstance->handleMessage2(message);
}

bool CognitionLogDataProvider::isFrameDataComplete()
{
  if(!theInstance)
    return true;
  else if(theInstance->frameDataComplete)
  {
    OUTPUT(idLogResponse, bin, '\0');
    theInstance->frameDataComplete = false;
    return true;
  }
  else
    return false;
}

bool CognitionLogDataProvider::handleMessage2(InMessage& message)
{
  switch(message.getMessageID())
  {
  HANDLE(BallModel)
  HANDLE(BallPercept)
  HANDLE(CameraMatrix)
  HANDLE(FrameInfo)
  HANDLE(GoalPercept)
  HANDLE2(GroundTruthRobotPose, ASSIGN(RobotPose, GroundTruthRobotPose))
  HANDLE2(GroundTruthBallModel, ASSIGN(BallModel, GroundTruthBallModel))
  HANDLE2(Image,
    ALLOC(FrameInfo)
    ((FrameInfo&) *representationBuffer[idFrameInfo]).time = ((Image&) *representationBuffer[idImage]).timeStamp;
  )
  HANDLE(ImageCoordinateSystem)
  HANDLE(LinePercept)
  HANDLE(RobotPose)

  case idProcessFinished:
    frameDataComplete = true;
    return true;

  case idJPEGImage:
    ALLOC(Image)
    {
      JPEGImage jpegImage;
      message.bin >> jpegImage;
      jpegImage.toImage((Image&) *representationBuffer[idImage]);
    }
    ALLOC(FrameInfo)
    ((FrameInfo&) *representationBuffer[idFrameInfo]).time = ((Image&) *representationBuffer[idImage]).timeStamp;
    return true;

  default:
    return false; 
  }
}

MAKE_MODULE(CognitionLogDataProvider, Infrastructure)
