/**
* @file GoalPerceptor.h
*
* Simple GoalLocator implementation.
*
* @author Andrik Rieskamp
*/

#ifndef Goal_Locator_Nao_h_
#define Goal_Locator_Nao_h_

#include "Tools/Module/Module.h"
#include "Tools/RingBuffer.h"
#include "Modules/Perception/PointExplorer.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/TeamInfo.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/RegionPercept.h"
#include "Representations/Perception/GoalPercept.h"
#include "Representations/MotionControl/OdometryData.h"
#include "Representations/Infrastructure/Image.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Configuration/ColorTable64.h"
#include "Representations/Configuration/FieldDimensions.h"
#include "Representations/Perception/ImageCoordinateSystem.h"
#include "Tools/RingBufferWithSum.h"
#include "Tools/Math/Common.h"
#include "Tools/Math/Geometry.h"

MODULE(GoalPerceptor)
  REQUIRES(Image)
  REQUIRES(RegionPercept)
  REQUIRES(CameraMatrix)
  REQUIRES(OdometryData)
  REQUIRES(FieldDimensions)
  REQUIRES(GroundTruthRobotPose)
  USES(RobotPose)
  REQUIRES(ColorTable64)
  REQUIRES(FrameInfo)
  REQUIRES(OwnTeamInfo) 
  REQUIRES(ImageCoordinateSystem)
  PROVIDES_WITH_MODIFY_AND_OUTPUT_AND_DRAW(GoalPercept)
END_MODULE


/**
* @class GoalPerceptor
*
* A GoalLocator using no probabilistics at all
*/
class GoalPerceptor : public GoalPerceptorBase
{
public:
  
  class SobelPixel
  {
   public:
    // Signed response of the sobel X and Y filter
    short sobelX, sobelY;
    
    //! 0 Pixel
    SobelPixel () :sobelX(0), sobelY(0){}        

    //! Default constructor
    SobelPixel (short sobelX, short sobelY) :sobelX(sobelX), sobelY(sobelY) {}      
  };

  /** Constructor */
  GoalPerceptor();

  /** Executes the GoalPerceptor module 
  * @param goalPercept The goal representation which is updated by this module
  */
  void update(GoalPercept& goalPercept);

private:  

  class Parameters : public Streamable
  {
  private:
    void serialize(In* in, Out* out)
    {
      STREAM_REGISTER_BEGIN();
      STREAM(PoleAccSurroundingSize);
      STREAM(PoleAccMinDistance);
      STREAM(PoleAccThreshold);
      STREAM(crossbarAngleCount);
      STREAM(crossbarDistanceScale);
      STREAM(crossbarAccThreshold);
      STREAM(pixelsToTreatPoleAsNear);
      STREAM(gridStepSize);
      STREAM(allowedDistanceNoise);
      STREAM_REGISTER_FINISH();
    }

  public:
    int PoleAccSurroundingSize;
    int PoleAccMinDistance;
    int PoleAccThreshold;
    int crossbarAngleCount;
    int crossbarDistanceScale;
    int crossbarAccThreshold;
    int pixelsToTreatPoleAsNear;
    int gridStepSize;
    double allowedDistanceNoise;
  };

  class PolePercept
  {
  public:
    ColorClasses::Color color;
    enum PoleSide {LEFT=0, RIGHT, UNDEFINED} poleSide;
    GoalPost::DistanceType distanceType;
    int xImage;
    int xImageTop;
    int xImageBottom;
    int yImageTop;
    int yImageBottom;
    int widthLeft;
    int widthRight;
    int width;
    bool footVisible;
    bool headVisible;
    bool discarded;
    bool discarded2;
    Vector2<double> onField;
    unsigned timeWhenLastSeen;
    bool seenInThisFrame;
    int accumulatorMatchSize;

    PolePercept() : 
      poleSide(UNDEFINED),
      distanceType(GoalPost::BEARING_BASED),
      xImage(0),
      xImageTop(0),
      xImageBottom(0),
      yImageTop(0),
      yImageBottom(0),
      widthLeft(0),
      widthRight(0),
      width(0),
      footVisible(true),
      headVisible(true),
      discarded(false),
      discarded2(false),
      timeWhenLastSeen(0), 
      seenInThisFrame(false),
      accumulatorMatchSize(0)
      {};
  };

  Parameters parameters; /**< The parameters of this module. */
  PointExplorer pointExplorer;

  /** prepares execution, initializes some values*/
  void preExecution(GoalPercept& goalPercept);

  /** motion update step*/
  void motionUpdate(GoalPercept& goalPercept);

  /** Sets the member values of the GoalPercept */
  void detectGoal(ColorClasses::Color goalColor, PolePercept polePercepts[]);

  void updateRepresentation(GoalPercept& goalPercept, PolePercept oppPercepts[], PolePercept ownPercepts[]);

  void createGoalPercept(GoalPercept& goalPercept, int index, PolePercept percept);

  void createUnknownGoalPercept(GoalPercept& goalPercept, int index, PolePercept percept);
  
  ColorClasses::Color convertColorAt(int x, int y);

  int calculateExpectedPixelWidth(Vector2<int> posImg);

  /** Sets some values after everything else of this module was executed */
  void postExecution();

  /** Draws some stuff*/
  void draw();

  /** Init function, called after construction of all currently active modules*/
  void init();

  //short colorAtPixel(int y, int x);

  void pointToField(int x, int y, Vector2<double>& p) const;

  unsigned lastExecution;                                    /** time stamp*/
  Pose2D lastOdometry;                                       /** save last odometry state*/
  Pose2D odometryOffset;                                     /** odometry offset since last execution*/
  ColorClasses::Color oppColor;                              /** color for drawing*/
  ColorClasses::Color ownColor;                              /** color for drawing*/
  enum {MAX_NUMBER_OF_PERCEPTS = 2}; /**< constant */
  PolePercept ownPolePercepts[MAX_NUMBER_OF_PERCEPTS]; /**< the perceived posts*/
  PolePercept oppPolePercepts[MAX_NUMBER_OF_PERCEPTS]; /**< the perceived posts*/

  int rotCos;
  int rotSin;
  int tiltCos2;
  int tiltSin2;
  int tiltCos;
  int tiltSin;
  int cameraHeight;
  Pose2D origin;

};

#endif
