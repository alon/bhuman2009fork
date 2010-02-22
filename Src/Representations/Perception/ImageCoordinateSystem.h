/**
* @file ImageCoordinateSystem.h
* Declaration of a class that provides transformations on image coordinates.
* Parts of this class were copied from class ImageInfo.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
* @author <a href="mailto:oberlies@sim.tu-darmstadt.de">Tobias Oberlies</a>
*/

#ifndef __ImageCoordinateSystem_h_
#define __ImageCoordinateSystem_h_

#include "Representations/Infrastructure/CameraInfo.h"
#include "Tools/Math/Common.h"
#include "Tools/Math/Matrix2x2.h"

class CoordinateSystemProvider;
class CognitionLogDataProvider;

/**
* @class ImageCoordinateSystem
* A class that provides transformations on image coordinates.
*/
class ImageCoordinateSystem : public Streamable
{
private:
  void serialize(In* in, Out* out)
  {
    STREAM_REGISTER_BEGIN();
    STREAM(rotation);
    STREAM(invRotation);
    STREAM(origin);
    STREAM(offset);
    STREAM(a);
    STREAM(b);
    STREAM(cameraInfo);
    STREAM_REGISTER_FINISH();
    if(in)
    {
      offsetInt = Vector2<int>(int(offset.x * 1024 + 0.5), 
                               int(offset.y * 1024 + 0.5));
      aInt = int(a * 1024 + 0.5);
      bInt = int(b * 1024 + 0.5);
    }
  }

  /** 
  * The rotation from a horizon-aligned coordinate system to the image coordinate 
  * system. The horizon-aligned coordinate system is defined as follows:
  *  - the x-coordinate points parallel to the horizon to the right
  *  - the y-coordinate points perpendicular to the horizon downwards
  *  - the origin is the top left corner of the image, i.e. the same as the the origin
  *    of the image coordinate system. Thus the transformation from horizon-aligned to
  *    image coordinates only requires the rotation matrix. 
  * The direction of the horizon is c[0], the direction downwards is c[1].
  */
  Matrix2x2<double> rotation;
  Matrix2x2<double> invRotation; /**< The rotation from the image coordinates to the horizon-aligned coordinates. */
  Vector2<double> origin; /**< The origin of the horizon in image coordinates. */
  Vector2<double> offset; /**< The offset of the previous image to the current one. */
  double a, /**< Constant part of equation to motion distortion. */
         b; /**< Linear part of equation to motion distortion. */
  Vector2<int> offsetInt; /**< The offset of the previous image to the current one. */
  int aInt,
      bInt;
  CameraInfo cameraInfo; /**< Information required in some equations. */

  static int xTable[cameraResolutionWidth],
             yTable[cameraResolutionHeight],
             table[6144];

  static void initTables(const CameraInfo& cameraInfo)
  {
    for(int i = 0; i < cameraResolutionWidth; ++i)
      xTable[i] = int(::atan((cameraInfo.opticalCenter.x - i) / cameraInfo.focalLength) * 1024 + 0.5);
    for(int i = 0; i < cameraResolutionHeight; ++i)
      yTable[i] = int(::atan((i - cameraInfo.opticalCenter.y) / cameraInfo.focalLength) * 1024 + 0.5);
    for(int i = -3072; i < 3072; ++i)
      table[i + 3072] = int(::tan(i / 1024.0) * cameraInfo.focalLength + 0.5);
  }

public:
  /**
  * Converts image coordintates into coordinates in the horizon-aligned coordinate system.
  * @param imageCoords The point in image coordinates.
  * @return The point in horizon-aligned coordinates.
  */
  Vector2<double> toHorizonAligned(const Vector2<int>& imageCoords) const
  {
    return invRotation * Vector2<double>(imageCoords.x, imageCoords.y);
  }
  Vector2<double> toHorizonAligned(const Vector2<>& imageCoords) const
  {
    return invRotation * imageCoords;
  }

  /**
  * Converts coordinates in the horizon-aligned coordinate system into image coordinates.
  * No clipping is done.
  * @param horizonAlignedCoords The point in horizon-aligned coordinates.
  * @return The point in image coordinates.
  */
  Vector2<int> fromHorizonAligned(const Vector2<double>& horizonAlignedCoords) const
  {
    Vector2<double> result = rotation * horizonAlignedCoords;
    return Vector2<int>(int(result.x), int(result.y));
  }

  /**
  * Converts image coordintates into coordinates in the horizon-based coordinate system,
  * i.e. a system of coordinates, in which the origin of the horizon is mapped to (0, 0).
  * @param imageCoords The point in image coordinates.
  * @return The point in horizon-based coordinates.
  */
  Vector2<double> toHorizonBased(const Vector2<int>& imageCoords) const
  {
    return invRotation * (Vector2<double>(imageCoords.x, imageCoords.y) - origin);
  }
  Vector2<double> toHorizonBased(const Vector2<double>& imageCoords) const
  {
    return invRotation * (imageCoords - origin);
  }

  /**
  * Converts coordinates in the horizon-based coordinate system into image coordinates.
  * No clipping is done.
  * @param horizonBasedCoords The point in horizon-based coordinates.
  * @return The point in image coordinates.
  */
  Vector2<int> fromHorizonBased(const Vector2<double>& horizonBasedCoords) const
  {
    Vector2<double> result = rotation * horizonBasedCoords;
    return Vector2<int>(int(result.x + origin.x), int(result.y + origin.y));
  }

  /**
  * Corrects image coordinates so that the distortion resulting from the rolling 
  * shutter is compensated.
  * No clipping is done.
  * @param imageCoords The point in image coordinates.
  * @return The corrected point.
  */
  Vector2<double> toCorrected(const Vector2<int>& imageCoords) const
  {
    double factor = a + imageCoords.y * b;
    return Vector2<double>(cameraInfo.opticalCenter.x - tan(atan((cameraInfo.opticalCenter.x - imageCoords.x) / cameraInfo.focalLength) - factor * offset.x) * cameraInfo.focalLength,
                           cameraInfo.opticalCenter.y + tan(atan((imageCoords.y - cameraInfo.opticalCenter.y) / cameraInfo.focalLength) - factor * offset.y) * cameraInfo.focalLength);
  }

  Vector2<double> fromCorrectedApprox(const Vector2<int>& coords) const
  {
    double factor = a + cameraInfo.resolutionHeight/2 * b;
    Vector2<double> v(factor * offset.x - atan((coords.x - cameraInfo.opticalCenter.x) / cameraInfo.focalLength),
                      factor * offset.y + atan((coords.y - cameraInfo.opticalCenter.y) / cameraInfo.focalLength));
    const static double EPSILON = 0.1;
    if(v.x < -pi_2 + EPSILON)
      v.x = -pi_2 + EPSILON;
    else if(v.x > pi_2 - EPSILON)
      v.x = pi_2 - EPSILON;

    if(v.y < -pi_2 + EPSILON)
      v.y = -pi_2 + EPSILON;
    else if(v.y > pi_2 - EPSILON)
      v.y = pi_2 - EPSILON;

    return Vector2<double>(cameraInfo.opticalCenter.x - tan(v.x) * cameraInfo.focalLength,
                           cameraInfo.opticalCenter.y + tan(v.y) * cameraInfo.focalLength);
  }
  /**
  * Corrects image coordinates so that the distortion resulting from the rolling 
  * shutter is compensated.
  * No clipping is done.
  * @param x The x coordinate of the point in image coordinates.
  * @param y The y coordinate of the point in image coordinates.
  * @return The corrected point relative to the image center with negated signs.
  */
  Vector2<int> toCorrectedCenteredNeg(int x, int y) const
  {
    int factor = aInt + y * bInt;
    x = xTable[x] - ((factor * offsetInt.x) >> 10);
    y = yTable[y] - ((factor * offsetInt.y) >> 10);
    if(x < -3072)
      x = -3072;
    else if(x > 3071)
      x = 3071;
    if(y < -3072)
      y = -3072;
    else if(y > 3071)
      y = 3071;
    return Vector2<int>(table[x + 3072], -table[y + 3072]);
  }

  friend class CoordinateSystemProvider; /**< Only that class can write the private members. */
  friend class CognitionLogDataProvider; /**< Only that class can write the private members. */
};

#endif //__ImageCoordinateSystem_h_
