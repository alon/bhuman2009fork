/**
 * @file CameraMatrix.cpp
 *
 * Implementation of class CameraMatrix.
 */

#include "CameraMatrix.h"
#include "Tools/Boundary.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Math/Geometry.h"

void CameraMatrix::draw()
{
  DECLARE_DEBUG_DRAWING("representation:CameraMatrix:Image", "drawingOnImage"); // Shows the robot coordinate system
  DECLARE_DEBUG_DRAWING("representation:CameraMatrix:Field", "drawingOnField"); // Shows the robot coordinate system

  COMPLEX_DRAWING("representation:CameraMatrix:Field",
  {
    CameraInfo cameraInfo; // HACK!
    Vector2<int> pointOnField[6];
    // calculate the projection of the four image corners to the ground
    Geometry::calculatePointOnField(0, 0, *this, cameraInfo, pointOnField[0]);
    Geometry::calculatePointOnField(cameraInfo.resolutionWidth, 0, *this, cameraInfo, pointOnField[1]);
    Geometry::calculatePointOnField(cameraInfo.resolutionWidth, cameraInfo.resolutionHeight, *this, cameraInfo, pointOnField[2]);
    Geometry::calculatePointOnField(0, cameraInfo.resolutionHeight, *this, cameraInfo, pointOnField[3]);

    // calculate a line 15 pixels below the horizon in the image
    Geometry::Line horizon = Geometry::calculateHorizon(*this, cameraInfo);
    Geometry::Line lineBelowHorizon;
    Vector2<double> vertLineDirection(-horizon.direction.y, horizon.direction.x);
    lineBelowHorizon.direction = horizon.direction;
    lineBelowHorizon.base = horizon.base;
    lineBelowHorizon.base += vertLineDirection * 15.0;

    // calculate the projection to the ground of the intersection points of the line parallel to the horizon and the image borders
    Vector2<int> beginPoint;
    Vector2<int> endPoint;
    if(Geometry::getIntersectionPointsOfLineAndRectangle(
       Vector2<int>(0,0), Vector2<int>(cameraInfo.resolutionWidth-1, cameraInfo.resolutionHeight-1), lineBelowHorizon, beginPoint, endPoint))
    {
      Geometry::calculatePointOnField(beginPoint.x,beginPoint.y, *this, cameraInfo, pointOnField[4]);
      Geometry::calculatePointOnField(endPoint.x, endPoint.y, *this, cameraInfo, pointOnField[5]);
      LINE("representation:CameraMatrix:Field", pointOnField[4].x, pointOnField[4].y, pointOnField[5].x, pointOnField[5].y, 30, Drawings::ps_solid, ColorClasses::yellow);
    }

    // determine the boundary of all the points that were projected to the ground
    Boundary<int> boundary(-10000, +10000);
    if(pointOnField[0].x !=0 || pointOnField[0].y != 0) {boundary.add(pointOnField[0]); CIRCLE("representation:CameraMatrix:Field", pointOnField[0].x, pointOnField[0].y, 100, 50, Drawings::ps_solid, ColorClasses::white, Drawings::bs_null, ColorClasses::white); }
    if(pointOnField[1].x !=0 || pointOnField[1].y != 0) {boundary.add(pointOnField[1]); CIRCLE("representation:CameraMatrix:Field", pointOnField[1].x, pointOnField[1].y, 100, 50, Drawings::ps_solid, ColorClasses::white, Drawings::bs_null, ColorClasses::white); }
    if(pointOnField[2].x !=0 || pointOnField[2].y != 0) {boundary.add(pointOnField[2]); CIRCLE("representation:CameraMatrix:Field", pointOnField[2].x, pointOnField[2].y, 100, 50, Drawings::ps_solid, ColorClasses::white, Drawings::bs_null, ColorClasses::white); }
    if(pointOnField[3].x !=0 || pointOnField[3].y != 0) {boundary.add(pointOnField[3]); CIRCLE("representation:CameraMatrix:Field", pointOnField[3].x, pointOnField[3].y, 100, 50, Drawings::ps_solid, ColorClasses::white, Drawings::bs_null, ColorClasses::white); }
    if(pointOnField[4].x !=0 || pointOnField[4].y != 0) {boundary.add(pointOnField[4]); CIRCLE("representation:CameraMatrix:Field", pointOnField[4].x, pointOnField[4].y, 100, 50, Drawings::ps_solid, ColorClasses::yellow, Drawings::bs_null, ColorClasses::yellow); }
    if(pointOnField[5].x !=0 || pointOnField[5].y != 0) {boundary.add(pointOnField[5]); CIRCLE("representation:CameraMatrix:Field", pointOnField[5].x, pointOnField[5].y, 100, 50, Drawings::ps_solid, ColorClasses::yellow, Drawings::bs_null, ColorClasses::yellow); }

    LINE("representation:CameraMatrix:Field", boundary.x.min, boundary.y.min, boundary.x.max, boundary.y.min, 30, Drawings::ps_solid, ColorRGBA(255,0,0));
    LINE("representation:CameraMatrix:Field", boundary.x.max, boundary.y.min, boundary.x.max, boundary.y.max, 30, Drawings::ps_solid, ColorClasses::yellow);
    LINE("representation:CameraMatrix:Field", boundary.x.max, boundary.y.max, boundary.x.min, boundary.y.max, 30, Drawings::ps_solid, ColorClasses::blue);
    LINE("representation:CameraMatrix:Field", boundary.x.min, boundary.y.max, boundary.x.min, boundary.y.min, 30, Drawings::ps_solid, ColorClasses::white);

    // fill the bounding rectangle with coordinate system lines (and reproject it to the image)
    int spacing = 100;
    for(int xx = boundary.x.min - boundary.x.min%spacing + spacing ; xx <= boundary.x.max; xx+=spacing)
    {
      LINE("representation:CameraMatrix:Field", xx, boundary.y.min, xx, boundary.y.max, 5, Drawings::ps_solid, ColorClasses::white);
    }
    for(int yy = boundary.y.min - boundary.y.min%spacing + spacing ; yy <= boundary.y.max; yy+=spacing)
    {
      LINE("representation:CameraMatrix:Field", boundary.x.min, yy, boundary.x.max, yy, 5, Drawings::ps_solid, ColorClasses::white);
    }
  }
  );// end complex drawing

  COMPLEX_DRAWING("representation:CameraMatrix:Image",
    CameraInfo cameraInfo; // HACK!
    Vector2<int> pointOnField[6];
    // calculate the projection of the four image corners to the ground
    Geometry::calculatePointOnField(0, 0, *this, cameraInfo, pointOnField[0]);
    Geometry::calculatePointOnField(cameraInfo.resolutionWidth, 0, *this, cameraInfo, pointOnField[1]);
    Geometry::calculatePointOnField(cameraInfo.resolutionWidth, cameraInfo.resolutionHeight, *this, cameraInfo, pointOnField[2]);
    Geometry::calculatePointOnField(0, cameraInfo.resolutionHeight, *this, cameraInfo, pointOnField[3]);

    // calculate a line 15 pixels below the horizon in the image
    Geometry::Line horizon = Geometry::calculateHorizon(*this, cameraInfo);
    Geometry::Line lineBelowHorizon;
    Vector2<double> vertLineDirection(-horizon.direction.y, horizon.direction.x);
    lineBelowHorizon.direction = horizon.direction;
    lineBelowHorizon.base = horizon.base;
    lineBelowHorizon.base += vertLineDirection * 15.0;

    // calculate the projection to the ground of the intersection points of the line parallel to the horizon and the image borders
    Vector2<int> beginPoint;
    Vector2<int> endPoint;
    if(Geometry::getIntersectionPointsOfLineAndRectangle(
       Vector2<int>(0,0), Vector2<int>(cameraInfo.resolutionWidth-1, cameraInfo.resolutionHeight-1), lineBelowHorizon, beginPoint, endPoint))
    {
      LINE("representation:CameraMatrix:Image", beginPoint.x, beginPoint.y, endPoint.x, endPoint.y, 3, Drawings::ps_dash, ColorClasses::white);
      Geometry::calculatePointOnField(beginPoint.x,beginPoint.y, *this, cameraInfo, pointOnField[4]);
      Geometry::calculatePointOnField(endPoint.x, endPoint.y, *this, cameraInfo, pointOnField[5]);
    }

    // determine the boundary of all the points that were projected to the ground
    Boundary<int> boundary(-10000, +10000);
    if(pointOnField[0].x !=0 || pointOnField[0].y != 0) {boundary.add(pointOnField[0]); }
    if(pointOnField[1].x !=0 || pointOnField[1].y != 0) {boundary.add(pointOnField[1]); }
    if(pointOnField[2].x !=0 || pointOnField[2].y != 0) {boundary.add(pointOnField[2]); }
    if(pointOnField[3].x !=0 || pointOnField[3].y != 0) {boundary.add(pointOnField[3]); }
    if(pointOnField[4].x !=0 || pointOnField[4].y != 0) {boundary.add(pointOnField[4]); }
    if(pointOnField[5].x !=0 || pointOnField[5].y != 0) {boundary.add(pointOnField[5]); }

    // fill the bounding rectangle with coordinate system lines (and reproject it to the image)
    int spacing = 100;
    for(int xx = boundary.x.min - boundary.x.min%spacing + spacing ; xx <= boundary.x.max; xx+=spacing)
    {
      Geometry::calculatePointInImage(Vector3<double>(xx, boundary.y.min, 0), *this, cameraInfo, beginPoint); 
      Geometry::calculatePointInImage(Vector3<double>(xx, boundary.y.max, 0), *this, cameraInfo, endPoint); 
      int lineWidth = 0;
      if(xx == 0) lineWidth = 3;
      LINE("representation:CameraMatrix:Image", 
        beginPoint.x, beginPoint.y,
        endPoint.x, endPoint.y,
        lineWidth, Drawings::ps_solid, ColorClasses::white);
    }
    for(int yy = boundary.y.min - boundary.y.min%spacing + spacing ; yy <= boundary.y.max; yy+=spacing)
    {
      Geometry::calculatePointInImage(Vector3<double>(boundary.x.min, yy, 0), *this, cameraInfo, beginPoint); 
      Geometry::calculatePointInImage(Vector3<double>(boundary.x.max, yy, 0), *this, cameraInfo, endPoint); 
      int lineWidth = 0;
      if(yy == 0) lineWidth = 3;
      LINE("representation:CameraMatrix:Image", 
        beginPoint.x, beginPoint.y,
        endPoint.x, endPoint.y,
        lineWidth, Drawings::ps_solid, ColorClasses::white);
    }
  );// end complex drawing
}

void RobotCameraMatrix::draw()
{
  DECLARE_DEBUG_DRAWING("representation:RobotCameraMatrix:Image", "drawingOnImage"); // Shows the robot coordinate system
  DECLARE_DEBUG_DRAWING("representation:RobotCameraMatrix:Field", "drawingOnField"); // Shows the robot coordinate system

  COMPLEX_DRAWING("representation:RobotCameraMatrix:Field",
  {
    CameraInfo cameraInfo; // HACK!
    Vector2<int> pointOnField[6];
    // calculate the projection of the four image corners to the ground
    Geometry::calculatePointOnField(0, 0, *this, cameraInfo, pointOnField[0]);
    Geometry::calculatePointOnField(cameraInfo.resolutionWidth, 0, *this, cameraInfo, pointOnField[1]);
    Geometry::calculatePointOnField(cameraInfo.resolutionWidth, cameraInfo.resolutionHeight, *this, cameraInfo, pointOnField[2]);
    Geometry::calculatePointOnField(0, cameraInfo.resolutionHeight, *this, cameraInfo, pointOnField[3]);

    // calculate a line 15 pixels below the horizon in the image
    Geometry::Line horizon = Geometry::calculateHorizon(*this, cameraInfo);
    Geometry::Line lineBelowHorizon;
    Vector2<double> vertLineDirection(-horizon.direction.y, horizon.direction.x);
    lineBelowHorizon.direction = horizon.direction;
    lineBelowHorizon.base = horizon.base;
    lineBelowHorizon.base += vertLineDirection * 15.0;

    // calculate the projection to the ground of the intersection points of the line parallel to the horizon and the image borders
    Vector2<int> beginPoint;
    Vector2<int> endPoint;
    if(Geometry::getIntersectionPointsOfLineAndRectangle(
       Vector2<int>(0,0), Vector2<int>(cameraInfo.resolutionWidth-1, cameraInfo.resolutionHeight-1), lineBelowHorizon, beginPoint, endPoint))
    {
      Geometry::calculatePointOnField(beginPoint.x,beginPoint.y, *this, cameraInfo, pointOnField[4]);
      Geometry::calculatePointOnField(endPoint.x, endPoint.y, *this, cameraInfo, pointOnField[5]);
      LINE("representation:CameraMatrix:Field", pointOnField[4].x, pointOnField[4].y, pointOnField[5].x, pointOnField[5].y, 30, Drawings::ps_solid, ColorClasses::yellow);
    }

    // determine the boundary of all the points that were projected to the ground
    Boundary<int> boundary(-10000, +10000);
    if(pointOnField[0].x !=0 || pointOnField[0].y != 0) {boundary.add(pointOnField[0]); CIRCLE("representation:RobotCameraMatrix:Field", pointOnField[0].x, pointOnField[0].y, 100, 50, Drawings::ps_solid, ColorClasses::white, Drawings::bs_null, ColorClasses::white); }
    if(pointOnField[1].x !=0 || pointOnField[1].y != 0) {boundary.add(pointOnField[1]); CIRCLE("representation:RobotCameraMatrix:Field", pointOnField[1].x, pointOnField[1].y, 100, 50, Drawings::ps_solid, ColorClasses::white, Drawings::bs_null, ColorClasses::white); }
    if(pointOnField[2].x !=0 || pointOnField[2].y != 0) {boundary.add(pointOnField[2]); CIRCLE("representation:RobotCameraMatrix:Field", pointOnField[2].x, pointOnField[2].y, 100, 50, Drawings::ps_solid, ColorClasses::white, Drawings::bs_null, ColorClasses::white); }
    if(pointOnField[3].x !=0 || pointOnField[3].y != 0) {boundary.add(pointOnField[3]); CIRCLE("representation:RobotCameraMatrix:Field", pointOnField[3].x, pointOnField[3].y, 100, 50, Drawings::ps_solid, ColorClasses::white, Drawings::bs_null, ColorClasses::white); }
    if(pointOnField[4].x !=0 || pointOnField[4].y != 0) {boundary.add(pointOnField[4]); CIRCLE("representation:RobotCameraMatrix:Field", pointOnField[4].x, pointOnField[4].y, 100, 50, Drawings::ps_solid, ColorClasses::yellow, Drawings::bs_null, ColorClasses::yellow); }
    if(pointOnField[5].x !=0 || pointOnField[5].y != 0) {boundary.add(pointOnField[5]); CIRCLE("representation:RobotCameraMatrix:Field", pointOnField[5].x, pointOnField[5].y, 100, 50, Drawings::ps_solid, ColorClasses::yellow, Drawings::bs_null, ColorClasses::yellow); }

    LINE("representation:RobotCameraMatrix:Field", boundary.x.min, boundary.y.min, boundary.x.max, boundary.y.min, 30, Drawings::ps_solid, ColorRGBA(255,0,0));
    LINE("representation:RobotCameraMatrix:Field", boundary.x.max, boundary.y.min, boundary.x.max, boundary.y.max, 30, Drawings::ps_solid, ColorClasses::yellow);
    LINE("representation:RobotCameraMatrix:Field", boundary.x.max, boundary.y.max, boundary.x.min, boundary.y.max, 30, Drawings::ps_solid, ColorClasses::blue);
    LINE("representation:RobotCameraMatrix:Field", boundary.x.min, boundary.y.max, boundary.x.min, boundary.y.min, 30, Drawings::ps_solid, ColorClasses::white);

    // fill the bounding rectangle with coordinate system lines (and reproject it to the image)
    int spacing = 100;
    for(int xx = boundary.x.min - boundary.x.min%spacing + spacing ; xx <= boundary.x.max; xx+=spacing)
    {
      LINE("representation:RobotCameraMatrix:Field", xx, boundary.y.min, xx, boundary.y.max, 5, Drawings::ps_solid, ColorClasses::white);
    }
    for(int yy = boundary.y.min - boundary.y.min%spacing + spacing ; yy <= boundary.y.max; yy+=spacing)
    {
      LINE("representation:RobotCameraMatrix:Field", boundary.x.min, yy, boundary.x.max, yy, 5, Drawings::ps_solid, ColorClasses::white);
    }
  }
  );// end complex drawing

  COMPLEX_DRAWING("representation:RobotCameraMatrix:Image",
  {
    CameraInfo cameraInfo; // HACK!
    Vector2<int> pointOnField[6];
    // calculate the projection of the four image corners to the ground
    Geometry::calculatePointOnField(0, 0, *this, cameraInfo, pointOnField[0]);
    Geometry::calculatePointOnField(cameraInfo.resolutionWidth, 0, *this, cameraInfo, pointOnField[1]);
    Geometry::calculatePointOnField(cameraInfo.resolutionWidth, cameraInfo.resolutionHeight, *this, cameraInfo, pointOnField[2]);
    Geometry::calculatePointOnField(0, cameraInfo.resolutionHeight, *this, cameraInfo, pointOnField[3]);

    // calculate a line 15 pixels below the horizon in the image
    Geometry::Line horizon = Geometry::calculateHorizon(*this, cameraInfo);
    Geometry::Line lineBelowHorizon;
    Vector2<double> vertLineDirection(-horizon.direction.y, horizon.direction.x);
    lineBelowHorizon.direction = horizon.direction;
    lineBelowHorizon.base = horizon.base;
    lineBelowHorizon.base += vertLineDirection * 15.0;

    // calculate the projection to the ground of the intersection points of the line parallel to the horizon and the image borders
    Vector2<int> beginPoint;
    Vector2<int> endPoint;
    if(Geometry::getIntersectionPointsOfLineAndRectangle(
       Vector2<int>(0,0), Vector2<int>(cameraInfo.resolutionWidth-1, cameraInfo.resolutionHeight-1), lineBelowHorizon, beginPoint, endPoint))
    {
      LINE("representation:RobotCameraMatrix:Image", beginPoint.x, beginPoint.y, endPoint.x, endPoint.y, 3, Drawings::ps_dash, ColorClasses::white);
      Geometry::calculatePointOnField(beginPoint.x,beginPoint.y, *this, cameraInfo, pointOnField[4]);
      Geometry::calculatePointOnField(endPoint.x, endPoint.y, *this, cameraInfo, pointOnField[5]);
    }

    // determine the boundary of all the points that were projected to the ground
    Boundary<int> boundary(-10000, +10000);
    if(pointOnField[0].x !=0 || pointOnField[0].y != 0) {boundary.add(pointOnField[0]); }
    if(pointOnField[1].x !=0 || pointOnField[1].y != 0) {boundary.add(pointOnField[1]); }
    if(pointOnField[2].x !=0 || pointOnField[2].y != 0) {boundary.add(pointOnField[2]); }
    if(pointOnField[3].x !=0 || pointOnField[3].y != 0) {boundary.add(pointOnField[3]); }
    if(pointOnField[4].x !=0 || pointOnField[4].y != 0) {boundary.add(pointOnField[4]); }
    if(pointOnField[5].x !=0 || pointOnField[5].y != 0) {boundary.add(pointOnField[5]); }

    // fill the bounding rectangle with coordinate system lines (and reproject it to the image)
    int spacing = 100;
    for(int xx = boundary.x.min - boundary.x.min%spacing + spacing ; xx <= boundary.x.max; xx+=spacing)
    {
      Geometry::calculatePointInImage(Vector3<double>(xx, boundary.y.min, 0), *this, CameraInfo(), beginPoint); 
      Geometry::calculatePointInImage(Vector3<double>(xx, boundary.y.max, 0), *this, CameraInfo(), endPoint); 
      int lineWidth = 0;
      if(xx == 0) lineWidth = 3;
      LINE("representation:RobotCameraMatrix:Image", 
        beginPoint.x, beginPoint.y,
        endPoint.x, endPoint.y,
        lineWidth, Drawings::ps_solid, ColorClasses::yellow);
    }
    for(int yy = boundary.y.min - boundary.y.min%spacing + spacing ; yy <= boundary.y.max; yy+=spacing)
    {
      Geometry::calculatePointInImage(Vector3<double>(boundary.x.min, yy, 0), *this, CameraInfo(), beginPoint); 
      Geometry::calculatePointInImage(Vector3<double>(boundary.x.max, yy, 0), *this, CameraInfo(), endPoint); 
      int lineWidth = 0;
      if(yy == 0) lineWidth = 3;
      LINE("representation:RobotCameraMatrix:Image", 
        beginPoint.x, beginPoint.y,
        endPoint.x, endPoint.y,
        lineWidth, Drawings::ps_solid, ColorClasses::yellow);
    }
  }
  );// end complex drawing
}
