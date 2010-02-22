/**
* @file ImageCoordinateSystem.cpp
* Implementation of a class that provides transformations on image coordinates.
* Parts of this class were copied from class ImageInfo.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
* @author <a href="mailto:oberlies@sim.tu-darmstadt.de">Tobias Oberlies</a>
*/

#include "ImageCoordinateSystem.h"

int ImageCoordinateSystem::xTable[cameraResolutionWidth],
    ImageCoordinateSystem::yTable[cameraResolutionHeight],
    ImageCoordinateSystem::table[6144];
