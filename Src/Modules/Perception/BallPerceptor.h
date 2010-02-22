/**
* @file BallPerceptor.h
* This file declares a module that provides the ball percept based on the
* biggest orange segment found by the PointsPerceptor. It is based on the
* BallSpecialist in GT2005.
* @author <A href="mailto:juengel@informatik.hu-berlin.de">Matthias Juengel</A>
* @author Max Risler
* @author <A href="mailto:brunn@sim.tu-darmstadt.de">Ronnie Brunn</A>
* @author <A href="mailto:mkunz@sim.tu-darmstadt.de">Michael Kunz</A>
* @author <a href="mailto:walter.nistico@uni-dortmund.de">Walter Nistico</a>
* @author <a href="mailto:judy@informatik.uni-bremen.de">Judith Müller</a>
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
* @author <a href="mailto:ingsie@informatik.uni-bremen.de">Ingo Sieverdingbeck</a>
* @author <a href="mailto:jworch@informatik.uni-bremen.de">Jan-Hendrik Worch</a>
*/

#ifndef __BallPerceptor_h_
#define __BallPerceptor_h_

#include "Tools/Module/Module.h"
#include "Representations/Configuration/ColorTable64.h"
#include "Representations/Configuration/FieldDimensions.h"
#include "Representations/Infrastructure/Image.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/ImageCoordinateSystem.h"
#include "Representations/Perception/BallSpots.h"
#include "Representations/Perception/ExtendedBallPercepts.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Perception/BodyContour.h"
#include "Tools/ImageProcessing/ColorModelConversions.h"

MODULE(BallPerceptor)
  REQUIRES(ColorTable64)
  REQUIRES(FieldDimensions)
  REQUIRES(Image)
  REQUIRES(CameraMatrix)
  REQUIRES(ImageCoordinateSystem)
  REQUIRES(BallSpots)
  REQUIRES(BodyContour)
  REQUIRES(RobotPose)
  PROVIDES_WITH_MODIFY_AND_OUTPUT_AND_DRAW(BallPercept)
  PROVIDES_WITH_MODIFY_AND_OUTPUT_AND_DRAW(ExtendedBallPercepts)
END_MODULE

class BallPerceptor : public BallPerceptorBase
{
  class Parameters : public Streamable
  {
  private:
    void serialize(In* in, Out* out)
    {
      STREAM_REGISTER_BEGIN();
      STREAM(stopColorCounter);
      STREAM(maxScanPoints);
      STREAM(evalBallCenterOrangeMinPercent);
      STREAM(evalBallCenterNoneMaxPercent);
      STREAM(evalBallCenterYellowMaxPercent);
      STREAM(evalBallCenterOtherMaxPercent);
      STREAM(evalBallPartOrangeMinPercent);
      STREAM(evalBallPartGreenMaxPercent);
      STREAM(evalOuterOrangeMaxPercent);
      STREAM(minValidityFirstRun);
      STREAM(minValiditySecondRun);
      STREAM(distanceResultCenterOutOfImage);
      STREAM(quantifierDurabilityOfBallPoints);
      STREAM(quantifierEccentricity);
      STREAM(quantifierDistance);
      STREAM(quantifierInner);
      STREAM(quantifierMiddle);
      STREAM(quantifierOuter);
      STREAM_REGISTER_FINISH();
    }
  public:
    int stopColorCounter;
    int maxScanPoints;
    double evalBallCenterOrangeMinPercent;
    double evalBallCenterNoneMaxPercent;
    double evalBallCenterYellowMaxPercent;
    double evalBallCenterOtherMaxPercent;
    double evalBallPartOrangeMinPercent;
    double evalBallPartGreenMaxPercent;
    double evalOuterOrangeMaxPercent;
    double minValidityFirstRun;
    double minValiditySecondRun;
    double distanceResultCenterOutOfImage;
    double quantifierDurabilityOfBallPoints;
    double quantifierEccentricity;
    double quantifierDistance;
    double quantifierInner;
    double quantifierMiddle;
    double quantifierOuter;
  }; 
   /** Parameters():
    maxBallRadius(50),
    evalBallCenterOrangeMinPercent(0.80),
    evalBallCenterNoneMaxPercent(0.20),
    evalBallCenterYellowMaxPercent(0.05),
    evalBallCenterOtherMaxPercent(0.10),
    evalBallPartOrangeMinPercent(0.30),
    evalBallPartGreenMaxPercent(0.05),
    evalOuterOrangeMaxPercent(0.05),
    minValidityFirstRun(0.6)
    minValiditySecondRun(0.8);
    distanceResultCenterOutOfImage(0.7);
    quantifierDurabilityOfBallPoints(1.);
    quantifierEccentricity(1.);
    quantifierDistance(1.);
    quantifierInner(1.);
    quantifierMiddle(1.);
    quantifierOuter(1.);
    {}
  } parameters;*/

  Parameters parameters;
  bool ballPointsAtImageBorder;
  double calculatedBallRadius;
  void update(BallPercept& ballPercept);
  
  void update(ExtendedBallPercepts& extendedBallPercepts);

  class BallPoint : public Vector2<int>
  {
  public:
    BallPoint() : greenIsClose(true), yellowIsClose(false), hardEdge(true), atBorder(false){};
    bool greenIsClose;
    bool yellowIsClose;
    bool hardEdge;
    bool atBorder;

    BallPoint& operator=(const Vector2<int>& other)
    {
      x = other.x;
      y = other.y;
      return *this;
    }
  };

  class BallPointList
  {
  public:
    BallPointList() : number(0) {};

    enum {maxNumberOfPoints = 400}; /**< The maximum number of ball points. */
    BallPoint ballPoints[maxNumberOfPoints]; /**< The ball points. */
    int number; /**< The actual number of ball points. */

    void add(const BallPoint& ballPoint);

    BallPoint& operator[] (int i) { return ballPoints[i]; }
    const BallPoint& operator[] (int i) const { return ballPoints[i]; }
  };

  /** 
  * Searches for the ball in the image, starting from the specified point. 
  * @param ballSpot The image coordinates of a point inside the potential ball.
  * @param extendedBallPercept The resulting ball percept.
  */
  void searchBall(const BallSpot& ballSpot, ExtendedBallPercept& extendedBallPercept);
  
  /**
  * Returns the Similarity of the given color to orange.
  */
  unsigned char getSimilarityToOrange (unsigned char y, unsigned char u, unsigned char v) const
  {
    unsigned char r, g, b;
    ColorModelConversions::fromYCbCrToRGB(y, u, v, r, g, b);
    using namespace std;
    return (unsigned char) min(max(r - 2*b - 3*max((g-r/2),0) - (3*max((r/2-g),0))/2 ,0),255);
  }

  /** Scan for the ball starting at a given trigger point */
  void scanForBallPoints(
   const Vector2<int>& ballStart,
   BallPointList& ballPoints,
   int& countAmbiguous,
   int& countOrange,
   int& maxOrangePerLine,
   int& countPixel
  );

  /** Finds the end of the ball */
  bool findEndOfBall(
    const BallPoint& start,
    const Vector2<int>& step,
    BallPoint& destination,
    int& countAmbiguous,
    int& countOrange,
    int& maxOrangePerLine,
    int& countPixel
  );

  /**
  * The function tries to fit the ball position by using the Levenberg-Marquardt
  * algorithm. The function fails if less than 3 points are available.
  * @param ballPoints Some points on the border of a potential ball
  * @param center The center resulting from the computation
  * @param radius The radius resulting from the computation
  * @return true if ball could be fitted into the given points
  */
  bool computeBallInImageLevenbergMarquardt(
    const BallPointList& ballPoints,
    Vector2<int>& center,
    double& radius
  );

  /*
  * Check if a list of points is inside a given circle
  */
  bool checkIfPointsAreInsideBall(const BallPointList& ballPoints, Vector2<int>& center, double radius, ExtendedBallPercept& extendedBallPercept);

  /*
  * Calculate the deviation of the points from the circle. (Only needed if checkIfPointsAreInsideBall(...) wasn't called first
  */
  void calculateDeviationOfBallPoints(const BallPointList& ballPoints, Vector2<int>& center, double radius, ExtendedBallPercept& extendedBallPercept);

  /**
  * The function checks whether a ball percept is plausible and will add it if so.
  * @param extendedBallPercept The resulting ball percept.
  */
  void checkCurrentBall(ExtendedBallPercept& extendedBallPercept);  

  void checkSquares(ExtendedBallPercept& extendedBallPercept);

  void checkBallDistance(ExtendedBallPercept& extendedBallPercept);

  /** Adds a seen ball to the percept, specifing its center and radius in image coords */
  void computeOffsetToBall
  (
    const Vector2<int>& centerInPixel,
    const Vector2<double>& centerAsAngles,
    double radiusInPixel,
    double radiusAsAngle,
    const Vector3<double>& translationOfCamera,
    bool isCameraMatrixValid,
    ExtendedBallPercept& extendedBallPercept
  );

  /**
   * The function calculates the factor durabilityOfBallPoints that will be needed later to calculate a reliability for the percept.
   */
  void considerBallPoints(const BallPointList &ballPoints, ExtendedBallPercept& extendedBallPercept);
  
  void evaluateColorClasses(ExtendedBallPercept& extendedBallPercept);

  void calculateValidityFirstRun(ExtendedBallPercept& extendedBallPercept);
  void calculateValiditySecondRun(ExtendedBallPercept& extendedBallPercept);

  public:
  /**
  * Default constructor.
  */
    BallPerceptor();

};

#endif// __BallPerceptor_h_
