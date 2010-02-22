/**
* @file Tools/ImageProcessing/BresenhamLineScan.cpp
* 
* Utility class which performs the Bresenham algorithm for line scanning
*
* @author <a href="mailto:timlaue@tzi.de">Tim Laue</a>
* @author <a href="mailto:walter.nistico@uni-dortmund.de">Walter Nistico</a>
* @author <a href="mailto:oberlies@sim.tu-darmstadt.de">Tobias Oberlies</a> (revised constructors and commenting) 
*/

#include "BresenhamLineScan.h"

void BresenhamLineScan::setup(const Vector2<int>& diff)
{
	int dx = diff.x;
	int dy = diff.y;
	int incX = ((dx>0) ? 1:-1);
	int incY = ((dy>0) ? 1:-1);
	int absDx(abs(dx));
	int absDy(abs(dy));
	alongX = (absDy < absDx);
	if(alongX)
	{
		baseError = -absDx;
		delta = 2*absDy;
		standardOffset.x = incX;
		standardOffset.y = 0;
		correctionOffset.x = 0;
		correctionOffset.y = incY;
		numberOfPixels = absDx;
	}
	else
	{
		baseError = -absDy;
		delta = 2*absDx;
		standardOffset.x = 0;
		standardOffset.y = incY;
		correctionOffset.x = incX;
		correctionOffset.y = 0;
		numberOfPixels = absDy;
	}
	resetError = 2*baseError;
	error = baseError;				//added, for lines != 45 degrees
}

BresenhamLineScan::BresenhamLineScan(const Vector2<int>& start, const Vector2<int>& end)
{
	setup(end-start);
}

BresenhamLineScan::BresenhamLineScan(const Vector2<double>& direction)
{
	setup(Vector2<int>(static_cast<int>(direction.x*1024.0),
					   static_cast<int>(direction.y*1024.0)));
}

BresenhamLineScan::BresenhamLineScan(const double& direction)
{
	setup(Vector2<int>(static_cast<int>(cos(direction)*1024.0), 
					   static_cast<int>(sin(direction)*1024.0)));
}

BresenhamLineScan::BresenhamLineScan(const Vector2<int>& start, const Vector2<double>& direction, const CameraInfo& cameraInfo)
{
	// Create a line through the point start with the given direction
	Geometry::Line line(Vector2<double>(static_cast<double>(start.x), static_cast<double>(start.y)),
					    direction);
	// Intersect line with image border
	Vector2<int> intersection1, intersection2;
	Geometry::getIntersectionPointsOfLineAndRectangle(Vector2<int>(0,0), Vector2<int>(cameraInfo.resolutionWidth-1,cameraInfo.resolutionHeight-1),
													  line, intersection1, intersection2);
	// Select the intersection point that is on the ray defined by start and direction
	Vector2<double> delta(intersection1.x - start.x, intersection1.y - start.y);
	if (delta*direction > 0)
		setup(intersection1-start);
	else if (delta*direction < 0)
        setup(intersection2-start);
	else
	{
		// Start point is on the edge, so compute difference to other intersection point
		delta.x = intersection2.x - start.x;
		delta.y = intersection2.y - start.y;
		if (delta*direction > 0)
			setup(intersection2-start);
		else
			setup(intersection1-start);
	}
}

BresenhamLineScan::BresenhamLineScan(const Vector2<int>& start, const double& direction, const CameraInfo& cameraInfo)
{
	const Vector2<int> frameUpperLeft(0,0);
	const Vector2<int> frameLowerRight(cameraInfo.resolutionWidth-1, cameraInfo.resolutionHeight-1);
	Geometry::Line scanLine(start, Vector2<double>(cos(direction), sin(direction)));
	Vector2<int> lineStart, lineEnd;
	Geometry::getIntersectionPointsOfLineAndRectangle(
                                      frameUpperLeft,
                                      frameLowerRight,
                                      scanLine, lineStart, lineEnd);
	Vector2<double> delta((lineEnd - start).x, (lineEnd - start).y);
	Vector2<double> directionVector(cos(direction), sin(direction));
	if (delta*directionVector >= 0)
		setup(lineEnd-start);
	else
		setup(lineStart-start); 
}
