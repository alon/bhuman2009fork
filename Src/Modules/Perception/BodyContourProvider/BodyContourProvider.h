/**
* @file BodyContourProvider.h
* This file declares a module that provides the contour of the robot's body in the image.
* The contour can be used to exclude the robot's body from image processing.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#ifndef __BodyContourProvider_h_
#define __BodyContourProvider_h_

#include "Tools/Module/Module.h"
#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/ImageCoordinateSystem.h"
#include "Representations/Sensing/RobotModel.h"
#include "Representations/Perception/BodyContour.h"

MODULE(BodyContourProvider)
  REQUIRES(CameraInfo)
  REQUIRES(ImageCoordinateSystem)
  REQUIRES(RobotCameraMatrix)
  REQUIRES(RobotModel)
  PROVIDES_WITH_MODIFY_AND_DRAW(BodyContour)
END_MODULE

/**
* @class BodyContourProvider
* A module that provides the contour of the robot's body in the image.
* The contour can be used to exclude the robot's body from image processing.
*/
class BodyContourProvider: public BodyContourProviderBase
{
private:
  /**
  * The method makes the object streamable.
  * @param in The stream from which the object is read (if in != 0).
  * @param out The stream to which the object is written (if out != 0).
  */
  class Parameters : public Streamable
  {
  private:
    void serialize(In* in, Out* out)
    {
      STREAM_REGISTER_BEGIN();
      STREAM_VECTOR(torso);
      STREAM_VECTOR(upperArm);
      STREAM_VECTOR(lowerArm);
      STREAM_VECTOR(upperLeg1);
      STREAM_VECTOR(upperLeg2);
      STREAM_VECTOR(foot);
      STREAM_REGISTER_FINISH();
    }

  public:
    std::vector<Vector3<double> > torso, /**< The contour of the torso. */
                                  upperArm, /**< The contour of the left upper arm. */
                                  lowerArm, /**< The contour of the left lower arm. */
                                  upperLeg1, /**< The contour of the left upper leg (part 1). */
                                  upperLeg2, /**< The contour of the left upper leg (part 2). */
                                  foot; /**< The contour of the left foot. */
  };

  Parameters parameters; /**< The parameters of this module. */

  void update(BodyContour& bodyContour);

  /**
  * The method loads the contour for a specific body part from the file bodyContour.cfg.
  * @param section The name of the element, i.e. the section in the configuration file.
  * @param contour The vector in which the data read is stored.
  */
  void load(const std::string& section, std::vector<Vector3<double> >& contour);

  /**
  * The method add a 3-D contour to the 2-D image contour.
  * @param origin The origin the 3-D coordinates will be interpreted relatively to.
  * @param c The 3-D contour.
  * @param sign The sign for y coordinates. Passing -1 allows to use contours of left
  *             body parts for right body parts.
  * @param bodyContour The 2-D contour in image coordinates the 3-D contour is added to.
  */
  void add(const Pose3D& origin, const std::vector<Vector3<double> >& c, double sign, BodyContour& bodyContour);

public:
  /** Default constructor. */
  BodyContourProvider();
};

#endif// __BodyContourProvider_h_
