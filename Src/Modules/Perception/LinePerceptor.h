/**
* @file LinePerceptor.h
* @author jeff
*/

#ifndef __LinePerceptor_h_
#define __LinePerceptor_h_

#include "Tools/Module/Module.h"
#include "Representations/Perception/LineSpots.h"
#include "Representations/Perception/LinePercept.h"
#include "Representations/Perception/ImageCoordinateSystem.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Configuration/FieldDimensions.h"

MODULE(LinePerceptor)
  REQUIRES(CameraMatrix)
  REQUIRES(CameraInfo)
  REQUIRES(ImageCoordinateSystem)
  REQUIRES(LineSpots)
  REQUIRES(FieldDimensions)
  REQUIRES(FrameInfo)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(LinePercept)
  REQUIRES(LinePercept)
END_MODULE

/**
 * @class LinePerceptor
 * This class creates lines and intersection from the lineSpots. It takes
 * the nonLineSpots to form "BanSectors" to filter out lines seen in robots.
 */
class LinePerceptor : public LinePerceptorBase
{
  /**
   * @class Parameters
   * The parameters for the LinePerceptor.
   */
  class Parameters : public Streamable
  {
  private:
    void serialize(In* in, Out* out)
    {
      STREAM_REGISTER_BEGIN();
      STREAM(minWidthRatio);
      STREAM(maxAlphaDiff);
      STREAM(maxDDiff);
      STREAM(minSupporters);
      STREAM(minLineSingleRegionLength);
      STREAM(minLineStartLength);
      STREAM(maxLineUniteDist);
      STREAM(maxLineUniteAlphaDiff);
      STREAM(maxLineSingleSegDist);
      STREAM(maxLineSingleSegDist2);
      STREAM(maxLineDistance);
      STREAM(minHardcover);
      STREAM(minTToEnd);
      STREAM(maxTFromEnd);
      STREAM(minIntersectionAlphaDiff);
      STREAM(minIntersectionLength);
      STREAM(circleBiggerThanSpecified);
      STREAM(maxMidLineToCircleDist);
      STREAM(minMidLineLength);
      STREAM(maxLineCircleDist);
      STREAM_REGISTER_FINISH();
    }
  public:
    double minWidthRatio; /**< The minimum height/width ratio a spot needs to be taken into account */
    double maxAlphaDiff, /**< The maximum difference in direction (Hess norm form) for spots belonging to the same line */
           maxDDiff; /**< The maximum difference in distance (Hess norm form) for spots belonging to the same line */
    int minLineSingleRegionLength; /**< The minimum size of a region to form a line wihtout other the support of other spots */
    int minLineStartLength; /**< The minimum size of a region to be able to look for supporters to forma line */
    int maxLineUniteDist; /**< The maximum distance (Hess norm form) for two lines to be merge together */
    double maxLineUniteAlphaDiff; /**< The maximum difference in direction (Hess norm form) for two lines to be merged together */
    int maxLineSingleSegDist; /**< The maximum distance of the start and end point of a single segment to be added to a line */
    int maxLineSingleSegDist2; /**< The maximum disatnce of the start / end point to the start / end point of a line to be added to it */
    int maxLineDistance; /**< The maximum distance a line segment can be away from the camera to be taken into account */
    double minHardcover; /**< The minimum hardcover ratio (length/covered by segments) of a line */
    int minSupporters; /**< The minimum of supporters a segments needs to become a line */
    int minTToEnd; /**< The minimum distance a intersection needs to the start / end of a line to be a T or X intersection */
    int maxTFromEnd; /**< The maximum distance a intersection can have to the start / end of a line to be a T and not a X */
    double minIntersectionAlphaDiff; /**< The minimum difference in direction two lines need to intersect */
    int minIntersectionLength; /**< The minimum length of a line to be able to intersect another one */
    int circleBiggerThanSpecified; /**< hrhrhr, this is a parameter to (fine)tune the center circle detection. This can improve the center circle detection if the cameraMatrix is not B-Human like aaeeh perfectly calibrated :-p */
    int maxMidLineToCircleDist; /**< The maximum distance a line may have to the center of the center circle to be detected as the middle line */
    int minMidLineLength; /**< The minimum length of a line to be accepted a middle line */
    int maxLineCircleDist; /**< If a line is closer to the center circle than this, it will be deleted */
  }; 

  /**
   * @class NonLineParameters
   * Parameters to filter out lineSpots which are in BanSectors.
   */
  class NonLineParameters : public Streamable
  {
    private:
      void serialize(In* in, Out* out)
      {
        STREAM_REGISTER_BEGIN();
        STREAM(minLineLength);
        STREAM(maxAlphaDiff);
        STREAM(minWidthRatio);
        STREAM_REGISTER_FINISH();
      }
    public:
      int minLineLength; /**< The minimum length of a spot to be filtered out */
      double maxAlphaDiff; /**< The maximum direction offset of a spot from upright to be filtered out */
      double minWidthRatio; /**< The minimum height/width ratio of a spot to be filtered out */
  };

  /**
   * @class CircleParameters
   * Parameters for the center circle detection.
   */
  class CircleParameters : public Streamable
  {

  private:
    void serialize(In* in, Out* out)
    {
      STREAM_REGISTER_BEGIN();
      STREAM(maxNgbhDist);
      STREAM(maxRadiusError);
      STREAM(minSegmentLength);
      STREAM(minSegmentImgLength);
      STREAM(minSupporters);
      STREAM(maxSupporterDist);
      STREAM(maxSupporterDist2);
      STREAM_REGISTER_FINISH();
    }
  public:
    int maxNgbhDist; /**< The maximum distance of two linesegments to create a circleSpot */ 
    int maxRadiusError; /**< The maximum error in radius a intersection from two linesegments may have to create a circleSpot */
    int minSegmentLength; /**< The minimum length of a lineSegment to be taken into account for the center circle */
    int minSegmentImgLength; /**< The minimum length in image coordinates of a linesegment to be taken into acount for the center circle */
    int minSupporters; /**< The minimum number of supporters for the center circle */
    int maxSupporterDist; /**< The maximum distance of two circleSpots to support each other */
    int maxSupporterDist2; /**< The maximum distance of two circleSpots to support each other in the second round */
  };

  /**
   * @class BanSectorParameters
   * Parameters for the creation of the BanSectors.
   */
  class BanSectorParameters : public Streamable
  {
    private:
      void serialize(In* in, Out* out)
      {
        STREAM_REGISTER_BEGIN();
        STREAM(angleStepSize);
        STREAM(minSectorCounter);
        STREAM(maxLineAngleDiff);
        STREAM_REGISTER_FINISH();
      }
    public:
      double angleStepSize; /**< A angle added to the left and right of a sector (so a sector is bigger than the detected nonLineSpots) */
      int minSectorCounter; /**< The minimum number of spots in a sector to be a BanSector */
      double maxLineAngleDiff; /**< The maximum distance (angle) a line may have from the sector to be filtered out */
  };

  /**
   * @class BanSector
   * A class to hold a BanSector.
   */
  class BanSector
  {
    public:
      int start; /**< Distance the Sector starts */
      int end; /**<Distance the sector ends */
      double alphaLeft; /**< left angle of the sector */
      double alphaRight; /**< right angle of the sector */
      int counter; /**< Number of nonLineSpots in this sector */
  };

  Parameters parameters; /**< Parameters for this module */
  CircleParameters circleParams; /**< Parameters for center circle detection */
  NonLineParameters nonLineParams; /**< Parameters for filtering out lines near robots */
  BanSectorParameters banSectorParams; /**< Parameters for the creation of ban sectors */

  std::list<LinePercept::LineSegment> lineSegs; /**< All the lineSegments */
  std::list<BanSector> banSectors; /**< The ban sectors, where no vertical, long spots are accepted */

  /** update the LinePercept */
  void update(LinePercept& linePercept);

  /** create BanSectors */
  void createBanSectors();

  /** 
   * creates the linesegments from the lineSpots, it creates the BanSector and filters out lines which meet
   * the banSector filter criterions
   * @param singleSegs a reference to the singleSegs list in the LinePercept
   * */
  void createLineSegments(std::list<LinePercept::LineSegment>& singleSegs);

  /** 
   * creates the lines from the singleSegments 
   * @param lines a reference to the lines list in the LinePercept
   * @param singleSegs a reference to the singleSegs list in the LinePercept
   * */
  void createLines(std::list<LinePercept::Line>& lines, std::list<LinePercept::LineSegment>& singleSegs);
  
  /** 
   * analyzes the lines, merges and deletes some if neccessary and creates intersections 
   * @param lines a reference to the lines list in the LinePercept
   * @param intersections a reference to the intersections vector in the LinePercept
   * @param circle a reference to the circle in the LinePercept
   * @param singleSegs a reference to the singleSegs list in the LinePercept
   * */
  void analyzeLines(std::list<LinePercept::Line>& lines, std::vector<LinePercept::Intersection>& intersections, LinePercept::CircleSpot& circle, std::list<LinePercept::LineSegment>& singleSegs);

  /**
   * analyze the singleSegments and try to find the center circle 
   * @param singleSegs a reference to the singleSegs list in the LinePercept
   * @param circle a reference to the circle in the LinePercept
   * @param lines a reference to the lines list in the LinePercept
   * */
  void analyzeSingleSegments(std::list<LinePercept::LineSegment>& singleSegs, LinePercept::CircleSpot& circle, std::list<LinePercept::Line>& lines);

  /** 
   * Determines the start and end point of a line. If updateLine == true the d and alpha values of the line 
   * (Hess norm form) are recalculated.
   * @param line the line to determine the start and end point of
   * @param first returns the start/end point of the line
   * @param last returns the start/end point of the line
   * @param updateLine recalculate d and alpha of line (Hess norm form)?
   * */
  void getFirstAndLastOfLine(LinePercept::Line& line, Vector2<int>& first, Vector2<int>& last, bool updateLine = true);
  
  public:
  /*
   * Default constructor
   */
  LinePerceptor();
};

#endif// __LinePerceptor_h_
