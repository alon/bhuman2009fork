/**
* @file Modules/Infrastructure/SoundControl.h
* The file declares a module that plays back sounds.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef __SoundControl_H__
#define __SoundControl_H__

#include "Tools/Module/Module.h"
#include "Representations/Infrastructure/SoundRequest.h"

MODULE(SoundControl)
  REQUIRES(SoundRequest)
  PROVIDES(SoundOutput)
END_MODULE

/**
* @class SoundControl
* A module that plays back sounds.
*/
class SoundControl : public SoundControlBase
{
private:
  SoundRequest::Sound lastSound; /**< The last sound played back. */

  void update(SoundOutput& soundOutput);

public:
  /**
  * Constructor.
  */
  SoundControl() : lastSound(SoundRequest::none) {}
};

#endif  //__SoundControl_h_
