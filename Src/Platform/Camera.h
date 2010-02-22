/**
* \file Platform/Camera.h
* Inclusion of platform dependend camera interface.
* \author Colin Graf
*/

#ifndef CAMERA_INCLUDED

#ifdef TARGET_ROBOT

#ifdef LINUX
#include "linux/NaoCamera.h"
class Camera : public NaoCamera {};
#define CAMERA_INCLUDED
#endif

#endif

#endif

