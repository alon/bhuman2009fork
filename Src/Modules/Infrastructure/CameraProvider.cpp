/** 
* @file CameraProvider.cpp
* This file declares a module that provides camera images.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#include "Platform/Camera.h"
#include "CameraProvider.h"
#include "Representations/Perception/JPEGImage.h"


PROCESS_WIDE_STORAGE CameraProvider* CameraProvider::theInstance = 0;

CameraProvider::CameraProvider()
#ifdef CAMERA_INCLUDED
  : camera(new Camera)
#endif
{
  theInstance = this;
}

CameraProvider::~CameraProvider()
{
#ifdef CAMERA_INCLUDED
  delete camera;
#endif
  theInstance = 0;
}

void CameraProvider::update(Image& image)
{
#ifdef CAMERA_INCLUDED
  if(camera)
  {
    image.setImage(const_cast<unsigned char*>(camera->getImage()));
    image.timeStamp = camera->getTimeStamp();
    camera->setSettings(theCameraSettings);
  }
#endif
  DEBUG_RESPONSE("representation:JPEGImage", OUTPUT(idJPEGImage, bin, JPEGImage(image)); );
}

void CameraProvider::update(FrameInfo& frameInfo)
{
  frameInfo.time = theImage.timeStamp;
}

void CameraProvider::update(CognitionFrameInfo& cognitionFrameInfo)
{
  cognitionFrameInfo.time = theImage.timeStamp;
}

bool CameraProvider::isNewImageAvailable()
{
#ifdef CAMERA_INCLUDED
  if(theInstance)
    return theInstance->camera->capturedNew();
  else 
#endif
    return true;
}

MAKE_MODULE(CameraProvider, Infrastructure)
