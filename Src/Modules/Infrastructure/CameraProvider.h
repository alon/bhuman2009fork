/** 
* @file CameraProvider.h
* This file declares a module that provides camera images.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef __CameraProvider_h_
#define __CameraProvider_h_

#include "Tools/Module/Module.h"
#include "Representations/Configuration/CameraSettings.h"
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/FrameInfo.h"

class Camera;

MODULE(CameraProvider)
  REQUIRES(CameraSettings)
  REQUIRES(Image)
  PROVIDES_WITH_OUTPUT(Image)
  PROVIDES(CameraInfo)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(FrameInfo)
  PROVIDES_WITH_MODIFY(CognitionFrameInfo)
END_MODULE

class CameraProvider : public CameraProviderBase
{
private:
  PROCESS_WIDE_STORAGE_STATIC CameraProvider* theInstance; /**< Points to the only instance of this class in this process or is 0 if there is none. */

  Camera* camera;

  void update(Image& image);
  void update(CameraInfo& cameraInfo) {} // nothing to do here
  void update(FrameInfo& frameInfo);
  void update(CognitionFrameInfo& cognitionFrameInfo);

public:
  /**
  * Default constructor.
  */
  CameraProvider();

  /**
  * Destructor.
  */
  ~CameraProvider();

  /** 
  * The method returns whether idProcessFinished was received.
  * @return Were all messages of the current frame received?
  */
  static bool isNewImageAvailable();
};

#endif // __CameraProvider_h_
