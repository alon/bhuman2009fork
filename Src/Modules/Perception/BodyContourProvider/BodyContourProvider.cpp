/**
* @file BodyContourProvider.h
* This file implements a module that provides the contour of the robot's body in the image.
* The contour can be used to exclude the robot's body from image processing.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#include "BodyContourProvider.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Debugging/DebugDrawings3D.h"
#include "Tools/Math/Geometry.h"
#include <map>

BodyContourProvider::BodyContourProvider()
{
  load("torso", parameters.torso);
  load("upperArm", parameters.upperArm);
  load("lowerArm", parameters.lowerArm);
  load("upperLeg1", parameters.upperLeg1);
  load("upperLeg2", parameters.upperLeg2);
  load("foot", parameters.foot);
}

void BodyContourProvider::load(const std::string& section, std::vector<Vector3<double> >& contour)
{
  InConfigFile stream("bodyContour.cfg", section);
  ASSERT(stream.exists());
  while(!stream.eof())
  {
    Vector3<double> v;
    stream >> v;
    contour.push_back(v);
  }
}

void BodyContourProvider::update(BodyContour& bodyContour)
{
  DECLARE_DEBUG_DRAWING3D("module:BodyContourProvider:contour", "origin");

  MODIFY("parameters:BodyContourProvider", parameters);

  bodyContour.lines.clear();

  add(Pose3D(), parameters.torso, 1, bodyContour);
  add(theRobotModel.limbs[RobotModel::upperArmLeft], parameters.upperArm, 1, bodyContour);
  add(theRobotModel.limbs[RobotModel::upperArmRight], parameters.upperArm, -1, bodyContour);
  add(theRobotModel.limbs[RobotModel::lowerArmLeft], parameters.lowerArm, 1, bodyContour);
  add(theRobotModel.limbs[RobotModel::lowerArmRight], parameters.lowerArm, -1, bodyContour);
  add(theRobotModel.limbs[RobotModel::upperLegLeft], parameters.upperLeg1, 1, bodyContour);
  add(theRobotModel.limbs[RobotModel::upperLegRight], parameters.upperLeg1, -1, bodyContour);
  add(theRobotModel.limbs[RobotModel::upperLegLeft], parameters.upperLeg2, 1, bodyContour);
  add(theRobotModel.limbs[RobotModel::upperLegRight], parameters.upperLeg2, -1, bodyContour);
  add(theRobotModel.limbs[RobotModel::footLeft], parameters.foot, 1, bodyContour);
  add(theRobotModel.limbs[RobotModel::footRight], parameters.foot, -1, bodyContour);
}

void BodyContourProvider::add(const Pose3D& origin, const std::vector<Vector3<double> >& c, double sign, 
                              BodyContour& bodyContour)
{
  Vector2<int> q1,
               q2;
  Vector3<double> p1 = origin * Vector3<double>(c[0].x, c[0].y * sign, c[0].z);
  bool valid1 = Geometry::calculatePointInImage(p1, theRobotCameraMatrix, theCameraInfo, q1);
  if(valid1)
  {
    Vector2<double> v = theImageCoordinateSystem.fromCorrectedApprox(q1);
    q1 = Vector2<int>(int(floor(v.x)), int(floor(v.y)));
  }

  for(unsigned i = 1; i < c.size(); ++i)
  {
    Vector3<double> p2 = origin * Vector3<double>(c[i].x, c[i].y * sign, c[i].z);
    bool valid2 = Geometry::calculatePointInImage(p2, theRobotCameraMatrix, theCameraInfo, q2);
    if(valid2)
    {
      Vector2<double> v = theImageCoordinateSystem.fromCorrectedApprox(q2);
      q2 = Vector2<int>(int(floor(v.x)), int(floor(v.y)));
    }

    if(valid1 && valid2 && 
       (q1.y < theCameraInfo.resolutionHeight || q2.y < theCameraInfo.resolutionHeight) &&
       (q1.x >= 0 || q2.x >= 0) &&
       (q1.x < theCameraInfo.resolutionWidth || q2.x < theCameraInfo.resolutionWidth))
      bodyContour.lines.push_back(BodyContour::Line(q1, q2));

    q1 = q2;
    valid1 = valid2;

    COMPLEX_DRAWING3D("module:BodyContourProvider:contour",
      LINE3D("module:BodyContourProvider:contour", p1.x, p1.y, p1.z, p2.x, p2.y, p2.z, 1, ColorRGBA(255,0,0));
      p1 = p2;
    );
  }
}

MAKE_MODULE(BodyContourProvider, Perception)
