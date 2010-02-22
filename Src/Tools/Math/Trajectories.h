/**
* @file Trajectories.h
* Contains class 
*
* @author <a href="mailto:sancho@informatik.uni-bremen.de">Dario Sancho-Pradel</a>
*/
//------------------------------------------------------------------------------
#ifndef __TRAJECTORIES_H__
#define __TRAJECTORIES_H__
//------------------------------------------------------------------------------

#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"
#include "Tools/Math/Common.h"

 class Trajectory1D
  {
  public:
    /**
    * The available shapes of trajectories.
    */
    enum Shape
    {
      constant,
      rectangle,
      sineRectangle,
      sine,
      constAccel,
      sineSqrt,
      doubleSine,
      doubleSinePositive,
      halfSine,
      flatSine,
      airSine,
      doubleParabola,
      sqrRectangle,
      expRectangle,
      cubicRectangle,
      numberOfShapes
    };

    /**
    * The available scale modes for trajectories.
    */
    enum ScaleMode
    {
      noScale,
      sign,
      linear,
      absLinear,
      posLinear,
      numberOfScaleModes
    };

    /**
    * The method returns a name for each shape.
    * @param shape The shape.
    * @return The name of the shape.
    */
    static const char* getShapeName(Shape shape);

    /**
    * The method returns a name for each scale mode.
    * @param scaleMode The scale mode.
    * @return The name of the scale mode.
    */
    static const char* getScaleModeName(ScaleMode scaleMode);

  private:
    double origin; /**< The origin of the trajectory. */
    ScaleMode scaleMode; /**< How to scale the resulting amplitude. */
    double scale, /**< Scales the resulting amplitude or defines a constant amplitude. */
           phaseShift; /**< The phase shift of this trajectory in the range of [0..1[. */

  protected:
    /**
    * The method calculates the amplitude in the range [-1..1].
    * @param phase The phase in the trajectory in the range of [0..1[.
    * @return The amplitude.
    */
    virtual double getAmplitude(double phase) const = 0;

  public:
    /**
    * Constructor.
    * @param origin The origin of the trajectory.
    * @param scaleMode How to scale the resulting amplitude.
    * @param scale Scales the resulting amplitude or defines a constant amplitude.
    * @param phaseShift The phase shift of this trajectory in the range of [0..1[.
    */
    Trajectory1D(double origin, ScaleMode scaleMode, double scale, double phaseShift)
      : origin(origin),
        scaleMode(scaleMode),
        scale(scale),
        phaseShift(fmod(phaseShift, 1.0)) {}

    /**
    * Virtual destructor, so objects of derived classes will be freed correctly.
    */
    virtual ~Trajectory1D() {}

    /**
    * The method calculates the position in the trajectory for a certain point in the phase.
    * @param phase The phase in the trajectory in the range of [0..1[.
    * @param size The amplitude of the trajectory in radians (will be scaled).
    * @return The angle.
    */
    double getPosition(double phase, double size) const
    {
      phase += phaseShift;
      if(phase >= 1.0)
        phase -= 1.0;
      using std::max;
      return origin + scale * (scaleMode == noScale 
                               ? 1 : scaleMode == linear 
                               ? size : scaleMode == absLinear 
                               ? fabs(size) : scaleMode == posLinear
                               ? max(size, 0.0) : size > 0 ? 1 : -1) * getAmplitude(phase);
    }

    /**
    * The method creates a new 3-D trajectory.
    * @param shape The shape of the trajectory.
    * @param origin The origin of the trajectory.
    * @param scaleMode How to scale the resulting amplitude.
    * @param scale Scales the resulting amplitude or defines a constant amplitude.
    * @param phaseShift The phase shift of the trajectory in the range of [0..1[.
    * @param phases Phase ratios for ground time, up time, and air time for each step.
    *               Only required for shape rectangle.
    * @return The new trajectory. It is dynamically allocated and has to be freed by the caller.
    */
    static Trajectory1D* create(Shape shape, double origin, ScaleMode scaleMode, 
                                double scale, double phaseShift, const double* phases);
  };

  /**
  * A rectangular-shaped 1-D trajectory.
  */
  class Rectangle1D : public Trajectory1D
  {
  private:
    double startFrontStop, /**< Point in phase, when lifting the foot. */
           startBackward, /**< Point in phase, when swing forward the foot. */
           startBackStop, /**< Point in phase, when lowering the foot. */
           startForward; /**< Point in phase, when touching the ground. */

    /**
    * The method calculates the amplitude in the range [-1..1].
    * @param phase The phase in the trajectory in the range of [0..1[.
    * @return The amplitude.
    */
    virtual double getAmplitude(double phase) const
    {
      if(phase < startFrontStop)
        return phase / startFrontStop;
      else if(phase < startBackward)
        return 1.0;
      else if(phase < startBackStop)
        return 1.0 - 2.0 * (phase - startBackward) / (startBackStop - startBackward);
      else if(phase < startForward)
        return -1.0;
      else
        return (phase - 1) / startFrontStop;
    }

  public:
    /**
    * Constructor.
    * @param origin The origin of the trajectory.
    * @param scaleMode How to scale the resulting amplitude.
    * @param scale Scales the resulting amplitude.
    * @param phaseShift The phase shift of this trajectory in the range of [0..1[.
    * @param phases Phase ratios for forward time, stop front time, and backward time.
    */
    Rectangle1D(double origin, ScaleMode scaleMode, double scale, double phaseShift, const double* phases)
      : Trajectory1D(origin, scaleMode, scale, phaseShift),
        startFrontStop(phases[0] / 2),
        startBackward(startFrontStop + phases[1]),
        startBackStop(startBackward + phases[2]),
        startForward(1 - phases[0] / 2) {}
  };

  /**
  * A rectangular-shaped 1-D trajectory, where the interpolated amplitude is sinusoid.
  */
  class SineRectangle1D : public Trajectory1D
  {
  private:
    double startFrontStop, /**< Point in phase, when lifting the foot. */
           startBackward, /**< Point in phase, when swing forward the foot. */
           startBackStop, /**< Point in phase, when lowering the foot. */
           startForward; /**< Point in phase, when touching the ground. */

    /**
    * The method calculates the amplitude in the range [-1..1].
    * @param phase The phase in the trajectory in the range of [0..1[.
    * @return The amplitude.
    */
    virtual double getAmplitude(double phase) const
    {
      if(phase < startFrontStop)
        return sin(phase / startFrontStop * pi_2);
      else if(phase < startBackward)
        return 1.0;
      else if(phase < startBackStop)
        return cos((phase - startBackward) / (startBackStop - startBackward) * pi);
      else if(phase < startForward)
        return -1.0;
      else
        return sin((phase - 1) / startFrontStop * pi_2);
    }

  public:
    /**
    * Constructor.
    * @param origin The origin of the trajectory.
    * @param scaleMode How to scale the resulting amplitude.
    * @param scale Scales the resulting amplitude.
    * @param phaseShift The phase shift of this trajectory in the range of [0..1[.
    * @param phases Phase ratios for forward time, stop front time, and backward time.
    */
    SineRectangle1D(double origin, ScaleMode scaleMode, double scale, double phaseShift, const double* phases)
      : Trajectory1D(origin, scaleMode, scale, phaseShift),
        startFrontStop(phases[0] / 2),
        startBackward(startFrontStop + phases[1]),
        startBackStop(startBackward + phases[2]),
        startForward(1 - phases[0] / 2) {}
  };

  /**
  * A rectangular-shaped 1-D trajectory, where the interpolated amplitude is cubic.
  */
  class PowRectangle1D : public Trajectory1D
  {
  private:
    double startFrontStop, /**< Point in phase, when lifting the foot. */
           startBackward, /**< Point in phase, when swing forward the foot. */
           startBackStop, /**< Point in phase, when lowering the foot. */
           startForward,
           exponent; /**< Point in phase, when touching the ground. */

    /**
    * The method calculates the amplitude in the range [-1..1].
    * @param phase The phase in the trajectory in the range of [0..1[.
    * @return The amplitude.
    */
    virtual double getAmplitude(double phase) const
    {
      if(phase < startFrontStop)
        return -pow(1 - phase / startFrontStop, exponent) + 1;
      else if(phase < startBackward)
        return 1.0;
      else if(phase < startBackStop)
      {
        double subPhase = (phase - startBackward) / (startBackStop - startBackward);
        if(subPhase < 0.5)
          return -pow(subPhase / 0.5, exponent) + 1;
        else
          return pow(1 - (subPhase - 0.5) / 0.5, exponent) - 1;
      }
      else if(phase < startForward)
        return -1.0;
      else
        return pow((phase - 1 + startFrontStop) / startFrontStop, exponent) - 1;
    }

  public:
    /**
    * Constructor.
    * @param origin The origin of the trajectory.
    * @param scaleMode How to scale the resulting amplitude.
    * @param scale Scales the resulting amplitude.
    * @param phaseShift The phase shift of this trajectory in the range of [0..1[.
    * @param phases Phase ratios for forward time, stop front time, and backward time.
    * @param exp the exponent
    */
    PowRectangle1D(double origin, ScaleMode scaleMode, double scale, double phaseShift, const double* phases, double exp)
      : Trajectory1D(origin, scaleMode, scale, phaseShift),
        startFrontStop(phases[0] / 2),
        startBackward(startFrontStop + phases[1]),
        startBackStop(startBackward + phases[2]),
        startForward(1 - phases[0] / 2),
        exponent(exp) {}
  };

  /**
  * A constant 1-D value.
  */
  class Constant1D : public Trajectory1D
  {
  private:
    /**
    * The method calculates the amplitude in the range [-1..1].
    * @param phase The phase in the trajectory in the range of [0..1[.
    * @return The amplitude.
    */
    virtual double getAmplitude(double phase) const
    {
      return 0;
    }

  public:
    /**
    * Constructor.
    * @param origin The origin of the trajectory.
    */
    Constant1D(double origin)
      : Trajectory1D(origin, noScale, 0, 0) {}
  };

  /**
  * A sine-shaped 1-D trajectory.
  */
  class Sine1D : public Trajectory1D
  {
  private:
    /**
    * The method calculates the amplitude in the range [-1..1].
    * @param phase The phase in the trajectory in the range of [0..1[.
    * @return The amplitude.
    */
    virtual double getAmplitude(double phase) const
    {
      return sin(phase * pi2);
    }

  public:
    /**
    * Constructor.
    * @param origin The origin of the trajectory.
    * @param scaleMode How to scale the resulting amplitude.
    * @param scale Scales the resulting amplitude.
    * @param phaseShift The phase shift of this trajectory in the range of [0..1[.
    */
    Sine1D(double origin, ScaleMode scaleMode, double scale, double phaseShift)
      : Trajectory1D(origin, scaleMode, scale, phaseShift) {}
  };

  /**
  * A 1-D trajectory consisting of two parabolas (one opened upwards and one downwards)
  */
  class DoubleParabola1D : public Trajectory1D
  {
  private:
    /**
    * The method calculates the amplitude in the range [-1..1].
    * @param phase The phase in the trajectory in the range of [0..1[.
    * @return The amplitude.
    */
    virtual double getAmplitude(double phase) const
    {
      return phase < 0.5 ? -16.0 * sqr(phase - 0.25) + 1.0 : 16.0 * sqr(phase - 0.75) - 1.0;
    }

  public:
    /**
    * Constructor.
    * @param origin The origin of the trajectory.
    * @param scaleMode How to scale the resulting amplitude.
    * @param scale Scales the resulting amplitude.
    * @param phaseShift The phase shift of this trajectory in the range of [0..1[.
    */
    DoubleParabola1D(double origin, ScaleMode scaleMode, double scale, double phaseShift)
      : Trajectory1D(origin, scaleMode, scale, phaseShift) {}
  };

  /**
  * A 1-D trajectory where are parabolas before and after ground phase.
  */
  class ConstAccel1D : public Trajectory1D
  {
  private:
    double startUp, /**< Point in phase, when lifting the foot. */
           startAir, /**< Point in phase, when swing forward the foot. */
           startDown, /**< Point in phase, when lowering the foot. */
           startGround, /**< Point in phase, when touching the ground. */
           firstPhase, /**< Length of the first phase. */
           secondPhase, /**< Length of the first phase. */
           thirdPhase, /**< Length of the first phase. */
           fourthPhase, /**< Length of the first phase. */
           velGround, /**< velocity of the ground phase in 1/sec. */
           velAir, /**< velocity of the air phase in 1/sec. */
           decel, /**< negative acceleration of the phase between ground and air in 1/sec². */
           accel; /**< positive acceleration of the phase between air and ground in 1/sec². */

    /**
    * The method calculates the amplitude in the range [-1..1].
    * @param phase The phase in the trajectory in the range of [0..1[.
    * @return The amplitude.
    */
    virtual double getAmplitude(double phase) const
    {
      if(phase < startUp)
      {
        return phase / startUp;
      }
      else if(phase < startAir)
      {
        double timeInPhase = (phase - startUp);
        return 1.0 + velGround * timeInPhase + 0.5 * decel * timeInPhase * timeInPhase;
      }
      else if(phase < startDown)
      {
        double timeInPhase = (phase - startAir);
        return 1.0 + velGround * secondPhase + 0.5 * decel * secondPhase * secondPhase + velAir * timeInPhase;
      }
      else if(phase < startGround)
      {
        double timeInPhase = (phase - startDown);
        return 1.0 + velGround * secondPhase + 0.5 * decel * secondPhase * secondPhase + velAir * (thirdPhase + timeInPhase) + 0.5 * accel * timeInPhase * timeInPhase;
      }
      else
      {
        double timeInPhase = (phase - startGround);
        return 1.0 + velGround * (secondPhase + timeInPhase) + 0.5 * decel * secondPhase * secondPhase + velAir * (thirdPhase + fourthPhase) + 0.5 * accel * fourthPhase * fourthPhase;
      }
    }

  public:
    /**
    * Constructor.
    * @param origin The origin of the trajectory.
    * @param scaleMode How to scale the resulting amplitude.
    * @param scale Scales the resulting amplitude.
    * @param phaseShift The phase shift of this trajectory in the range of [0..1[.
    * @param phases Phase ratios for forward time, stop front time, and backward time.
    */
    ConstAccel1D(double origin, ScaleMode scaleMode, double scale, double phaseShift, const double* phases)
      : Trajectory1D(origin, scaleMode, scale, phaseShift),
        startUp(phases[0] / 2),
        startAir(startUp + phases[1]),
        startDown(startAir + phases[2]),
        startGround(1 - phases[0] / 2),
        firstPhase(phases[0]),
        secondPhase(phases[1]),
        thirdPhase(phases[2]),
        fourthPhase(1 - phases[0] - phases[1] - phases[2]),
        velGround(1 / startUp),
        velAir(-velGround * (firstPhase + 0.5 * secondPhase + 0.5 * fourthPhase) / (0.5 * secondPhase + thirdPhase + 0.5 * fourthPhase)),
        decel((velAir - velGround) / secondPhase),
        accel((velGround - velAir) / fourthPhase) {}

  };

  /**
  * A sine-shaped 1-D trajectory.
  */
  class SineSqrt1D : public Trajectory1D
  {
  private:
    /**
    * The method calculates the amplitude in the range [-1..1].
    * @param phase The phase in the trajectory in the range of [0..1[.
    * @return The amplitude.
    */
    virtual double getAmplitude(double phase) const
    {
      double s = sin(phase * pi2);
      return sgn(s) * sqrt(fabs(s));
    }

  public:
    /**
    * Constructor.
    * @param origin The origin of the trajectory.
    * @param scaleMode How to scale the resulting amplitude.
    * @param scale Scales the resulting amplitude.
    * @param phaseShift The phase shift of this trajectory in the range of [0..1[.
    */
    SineSqrt1D(double origin, ScaleMode scaleMode, double scale, double phaseShift)
      : Trajectory1D(origin, scaleMode, scale, phaseShift) {}
  };

  /**
  * A half-sine-shaped 1-D trajectory: max(sin(x), 0);
  */
  class HalfSine1D : public Trajectory1D
  {
  private:
    /**
    * The method calculates the amplitude in the range [-1..1].
    * @param phase The phase in the trajectory in the range of [0..1[.
    * @return The amplitude.
    */
    virtual double getAmplitude(double phase) const
    {
      using namespace std;
      return max(sin(phase * pi2), 0.0);
    }

  public:
    /**
    * Constructor.
    * @param origin The origin of the trajectory.
    * @param scaleMode How to scale the resulting amplitude.
    * @param scale Scales the resulting amplitude.
    * @param phaseShift The phase shift of this trajectory in the range of [0..1[.
    */
    HalfSine1D(double origin, ScaleMode scaleMode, double scale, double phaseShift)
      : Trajectory1D(origin, scaleMode, scale, phaseShift) {}
  };

  /**
  * A sine-shaped 1-D trajectory with double frequency.
  */
  class DoubleSine1D : public Trajectory1D
  {
  private:
    /**
    * The method calculates the amplitude in the range [-1..1].
    * @param phase The phase in the trajectory in the range of [0..1[.
    * @return The amplitude.
    */
    virtual double getAmplitude(double phase) const
    {
      return fabs(sin(phase * pi2 * 2)) * (phase < 0.5 ? 1 : -1);
    }

  public:
    /**
    * Constructor.
    * @param origin The origin of the trajectory.
    * @param scaleMode Scale the resulting amplitude instead of using a constant anplitude.
    * @param scale Scales the resulting amplitude.
    * @param phaseShift The phase shift of this trajectory in the range of [0..1[.
    */
    DoubleSine1D(double origin, ScaleMode scaleMode, double scale, double phaseShift)
      : Trajectory1D(origin, scaleMode, scale, phaseShift) {}
  };

  /**
  * A sine-shaped 1-D trajectory with double frequency scaled to range [0..1].
  */
  class DoubleSinePositive1D : public Trajectory1D
  {
  private:
    /**
    * The method calculates the amplitude in the range [-1..1].
    * @param phase The phase in the trajectory in the range of [0..1[.
    * @return The amplitude.
    */
    virtual double getAmplitude(double phase) const
    {
      return 0.5 + sin(phase * pi2 * 2) / 2;
    }

  public:
    /**
    * Constructor.
    * @param origin The origin of the trajectory.
    * @param scaleMode Scale the resulting amplitude instead of using a constant anplitude.
    * @param scale Scales the resulting amplitude.
    * @param phaseShift The phase shift of this trajectory in the range of [0..1[.
    */
    DoubleSinePositive1D(double origin, ScaleMode scaleMode, double scale, double phaseShift)
      : Trajectory1D(origin, scaleMode, scale, phaseShift) {}
  };

  /**
  * A sine-like 1-D trajectory with f'(0) = 0 for x = 0, 0.25, 0.5, 0.75.
  */
  class FlatSine1D : public Trajectory1D
  {
  private:
    /**
    * The method calculates the amplitude in the range [-1..1].
    * @param phase The phase in the trajectory in the range of [0..1[.
    * @return The amplitude.
    */
    virtual double getAmplitude(double phase) const
    {
      return (1 - cos(phase * pi2 * 2)) * 0.5 * (phase < 0.5 ? 1 : -1);
    }

  public:
    /**
    * Constructor.
    * @param origin The origin of the trajectory.
    * @param scaleMode How to scale the resulting amplitude.
    * @param scale Scales the resulting amplitude.
    * @param phaseShift The phase shift of this trajectory in the range of [0..1[.
    */
    FlatSine1D(double origin, ScaleMode scaleMode, double scale, double phaseShift)
      : Trajectory1D(origin, scaleMode, scale, phaseShift) {}
  };

  /**
  * A sine like trajectory which is 0 is the phase is groundphase and oterhwise a sine.
  */
  class AirSine1D : public Trajectory1D
  {
  private:
    double startUp, /**< Point in phase, when lifting the foot. */
           startAir, /**< Point in phase, when swing forward the foot. */
           startDown, /**< Point in phase, when lowering the foot. */
           startGround; /**< Point in phase, when touching the ground. */

    /**
    * The method calculates the amplitude in the range [-1..1].
    * @param phase The phase in the trajectory in the range of [0..1[.
    * @return The amplitude.
    */
    virtual double getAmplitude(double phase) const
    {
      if(phase < startUp)
        return 0;
      else if(phase > startGround)
        return 0;
      else
        return sin(((phase-startUp) / (startGround-startUp))*pi);
    }

  public:
    /**
    * Constructor.
    * @param origin The origin of the trajectory.
    * @param scaleMode How to scale the resulting amplitude.
    * @param scale Scales the resulting amplitude.
    * @param phaseShift The phase shift of this trajectory in the range of [0..1[.
    * @param phases Phase ratios for forward time, stop front time, and backward time.
    */
    AirSine1D(double origin, ScaleMode scaleMode, double scale, double phaseShift, const double* phases)
      : Trajectory1D(origin, scaleMode, scale, phaseShift),
        startUp(phases[0] / 2),
        startAir(startUp + phases[1]),
        startDown(startAir + phases[2]),
        startGround(1 - phases[0] / 2) {}
  };

  /**
  * The base class for all 3-D trajectories.
  */
  class Trajectory3D
  {
  private:
    Vector3<double> origin; /**< The origin of the trajectory. */
    double xTiltForward, /**< x-component of the tilt of the trajectory when walking forward. */
           zTiltForward, /**< z-component of the tilt of the trajectory when walking forward. */
           xTiltBackward, /**< x-component of the tilt of the trajectory when walking backward. */
           zTiltBackward, /**< z-component of the tilt of the trajectory when walking backward. */
           yTiltSideways, /**< x-component of the tilt of the trajectory when walking sideways. */
           zTiltSideways, /**< z-component of the tilt of the trajectory when walking sideways. */
           phaseShift; /**< The phase shift of this trajectory in the range of [0..1[. */

  protected:
    /**
    * The method calculates an untilted offset in the trajectory around origin (0, 0, 0).
    * It has to be overridden in derived classes.
    * @param phase The phase in the trajectory in the range of [0..1[.
    * @param xSize Half of the size of the trajectory in x-direction in mm.
    * @param ySize Half of the size of the trajectory in y-direction in mm.

    * @return The offset.
    */
    virtual Vector3<double> getOffset(double phase, double xSize, double ySize) const = 0;

  public:
    /**
    * The available shapes of trajectories.
    */
    enum Shape
    {
      rectangle,
      trapezoid,
      constAccel,
      ellipse,
      halfEllipse,
      inverseHalfEllipse,
      numberOfShapes
    };

    /**
    * The method returns a name for each shape.
    * @param shape The shape.
    * @return The name of the shape.
    */
    static const char* getShapeName(Shape shape);

    /**
    * Constructor.
    * @param origin The origin of the trajectory.
    * @param tiltForward The tilt of the trajectory when moving forward.
    * @param tiltBackward The tilt of the trajectory when moving backward.
    * @param tiltSideways The tilt of the trajectory when moving sideways.
    * @param phaseShift The phase shift of this trajectory in the range of [0..1[.
    */
    Trajectory3D(const Vector3<double>& origin, 
                 double tiltForward, double tiltBackward, double tiltSideways, 
                 double phaseShift)
      : origin(origin),
        xTiltForward(cos(-tiltForward)),
        zTiltForward(tan(-tiltForward)),
        xTiltBackward(cos(-tiltBackward)),
        zTiltBackward(tan(-tiltBackward)),
        yTiltSideways(cos(-tiltSideways)),
        zTiltSideways(tan(-tiltSideways)),
        phaseShift(phaseShift) {}

    /**
    * Virtual destructor, so objects of derived classes will be freed correctly.
    */
    virtual ~Trajectory3D() {}

    /**
    * The method calculates the position in the trajectory for a certain point in the phase.
    * @param phase The phase in the trajectory in the range of [0..1[.
    * @param xSize Half of the size of the trajectory in x-direction in mm.
    * @param ySize Half of the size of the trajectory in y-direction in mm.
    * @param yOffset Additional offset in y-direction in mm. Will always be added
    *                in the direction away from 0.
    * @return The position.
    */
    Vector3<double> getPosition(double phase, double xSize, double ySize, double yOffset) const
    {
      phase += phaseShift;
      if(phase >= 1.0)
        phase -= 1.0;
      Vector3<double> offset = getOffset(phase, xSize, ySize);
      double f = xSize || ySize ? fabs(xSize) / (fabs(xSize) + fabs(ySize)) : 1.0,
             xt = (xSize < 0 ? zTiltBackward : zTiltForward) * f,
             yt = (ySize < 0 ? -zTiltSideways : zTiltSideways) * (1 - f);

      return origin + Vector3<double>(offset.x, 
                                      offset.y + (origin.y < 0 ? -yOffset : yOffset),
                                      offset.x * xt + offset.y * yt + offset.z);
    }

    /**
    * The method creates a new 3-D trajectory.
    * @param shape The shape of the trajectory.
    * @param origin The origin of the trajectory.
    * @param tiltForward The tilt of the trajectory when moving forward.
    * @param tiltBackward The tilt of the trajectory when moving backward.
    * @param tiltSideways The tilt of the trajectory when moving sideways.
    * @param phaseShift The phase shift of the trajectory in the range of [0..1[.
    * @param height The height of a step in mm.
    * @param phases Phase ratios for ground time, up time, and air time for each step.
    * @return The new trajectory. It is dynamically allocated and has to be freed by the caller.
    */
    static Trajectory3D* create(Shape shape, const Vector3<double>& origin, 
                                double tiltForward, double tiltBackward, double tiltSideways, 
                                double phaseShift, double height, const double* phases);
  };

  /**
  * A rectangular-shaped 3-D trajectory.
  */
  class Rectangle3D : public Trajectory3D
  {
  private:
    double zSize, /**< The height of the trajectory. */
           startUp, /**< Point in phase, when lifting the foot. */
           startAir, /**< Point in phase, when swing forward the foot. */
           startDown, /**< Point in phase, when lowering the foot. */
           startGround; /**< Point in phase, when touching the ground. */

    /**
    * The method calculates an untilted offset in the trajectory around origin (0, 0, 0).
    * @param phase The phase in the trajectory in the range of [0..1[.
    * @param xSize Half of the size of the trajectory in x-direction in mm.
    * @param ySize Half of the size of the trajectory in y-direction in mm.

    * @return The offset.
    */
    virtual Vector3<double> getOffset(double phase, double xSize, double ySize) const
    {
      Vector2<double> scale;
      if(phase < startUp)
        scale = Vector2<double>(phase / startUp, 0);
      else if(phase < startAir)
        scale = Vector2<double>(1.0, (phase - startUp) / (startAir - startUp));
      else if(phase < startDown)
        scale = Vector2<double>(1.0 - 2.0 * (phase - startAir) / (startDown - startAir), 1.0);
      else if(phase < startGround)
        scale = Vector2<double>(-1.0, 1.0 - (phase - startDown) / (startGround - startDown));
      else
        scale = Vector2<double>((phase - 1) / startUp, 0);
      return Vector3<double>(-xSize * scale.x, -ySize * scale.x, zSize * scale.y);
    }

  public:
    /**
    * Constructor.
    * @param origin The origin of the trajectory.
    * @param tiltForward The tilt of the trajectory when moving forward.
    * @param tiltBackward The tilt of the trajectory when moving backward.
    * @param tiltSideways The tilt of the trajectory when moving sideways.
    * @param phaseShift The phase shift of this trajectory in the range of [0..1[.
    * @param zSize The height of a step in mm.
    * @param phases Phase ratios for ground time, up time, and air time for each step.
    */
    Rectangle3D(const Vector3<double>& origin, 
                double tiltForward, double tiltBackward, double tiltSideways, 
                double phaseShift, double zSize, const double* phases)
      : Trajectory3D(origin, tiltForward, tiltBackward, tiltSideways, phaseShift),
        zSize(zSize),
        startUp(phases[0] / 2),
        startAir(startUp + phases[1]),
        startDown(startAir + phases[2]),
        startGround(1 - phases[0] / 2) {}
  };

  /**
  * A 3-D trajectory, where the foot is accelerated constantly (in x- and y-direction) before and after the ground time
  */
  class ConstAccel3D : public Trajectory3D
  {
  private:
    double zSize, /**< The height of the trajectory. */
           startUp, /**< Point in phase, when lifting the foot. */
           startAir, /**< Point in phase, when swing forward the foot. */
           startDown, /**< Point in phase, when lowering the foot. */
           startGround, /**< Point in phase, when touching the ground. */
           firstPhase, /**< Length of the first phase. */
           secondPhase, /**< Length of the first phase. */
           thirdPhase, /**< Length of the first phase. */
           fourthPhase, /**< Length of the first phase. */
           velGround, /**< velocity of the ground phase in 1/sec. */
           velAir, /**< velocity of the air phase in 1/sec. */
           decel, /**< negative acceleration of the phase between ground and air in 1/sec². */
           accel; /**< positive acceleration of the phase between air and ground in 1/sec². */

    /**
    * The method calculates an untilted offset in the trajectory around origin (0, 0, 0).
    * @param phase The phase in the trajectory in the range of [0..1[.
    * @param xSize Half of the size of the trajectory in x-direction in mm.
    * @param ySize Half of the size of the trajectory in y-direction in mm.

    * @return The offset.
    */
    virtual Vector3<double> getOffset(double phase, double xSize, double ySize) const
    {
      Vector2<double> scale;
      if(phase < startUp)
      {
        scale = Vector2<double>(phase / startUp, 0);
      }
      else if(phase < startAir)
      {
        double timeInPhase = (phase - startUp);
        scale = Vector2<double>(1.0 + velGround * timeInPhase + 0.5 * decel * timeInPhase * timeInPhase, -cos((phase - startUp) / (1 - firstPhase) * 2 * pi) / 2.0 + 0.5);
      }
      else if(phase < startDown)
      {
        double timeInPhase = (phase - startAir);
        scale = Vector2<double>(1.0 + velGround * secondPhase + 0.5 * decel * secondPhase * secondPhase + velAir * timeInPhase, -cos((phase - startUp) / (1 - firstPhase) * 2 * pi) / 2.0 + 0.5);
      }
      else if(phase < startGround)
      {
        double timeInPhase = (phase - startDown);
        scale = Vector2<double>(1.0 + velGround * secondPhase + 0.5 * decel * secondPhase * secondPhase + velAir * (thirdPhase + timeInPhase) + 0.5 * accel * timeInPhase * timeInPhase, -cos((phase - startUp) / (1 - firstPhase) * 2 * pi) / 2.0 + 0.5);
      }
      else
      {
        double timeInPhase = (phase - startGround);
        scale = Vector2<double>(1.0 + velGround * (secondPhase + timeInPhase) + 0.5 * decel * secondPhase * secondPhase + velAir * (thirdPhase + fourthPhase) + 0.5 * accel * fourthPhase * fourthPhase, 0);
      }
      return Vector3<double>(-xSize * scale.x, -ySize * scale.x, zSize * scale.y);
    }

  public:
    /**
    * Constructor.
    * @param origin The origin of the trajectory.
    * @param tiltForward The tilt of the trajectory when moving forward.
    * @param tiltBackward The tilt of the trajectory when moving backward.
    * @param tiltSideways The tilt of the trajectory when moving sideways.
    * @param phaseShift The phase shift of this trajectory in the range of [0..1[.
    * @param zSize The height of a step in mm.
    * @param phases Phase ratios for ground time, up time, and air time for each step.
    */
    ConstAccel3D(const Vector3<double>& origin, 
                double tiltForward, double tiltBackward, double tiltSideways, 
                double phaseShift, double zSize, const double* phases)
      : Trajectory3D(origin, tiltForward, tiltBackward, tiltSideways, phaseShift),
        zSize(zSize),
        startUp(phases[0] / 2),
        startAir(startUp + phases[1]),
        startDown(startAir + phases[2]),
        startGround(1 - phases[0] / 2),
        firstPhase(phases[0]),
        secondPhase(phases[1]),
        thirdPhase(phases[2]),
        fourthPhase(1 - phases[0] - phases[1] - phases[2]),
        velGround(1 / startUp),
        velAir(-velGround * (firstPhase + 0.5 * secondPhase + 0.5 * fourthPhase) / (0.5 * secondPhase + thirdPhase + 0.5 * fourthPhase)),
        decel((velAir - velGround) / secondPhase),
        accel((velGround - velAir) / fourthPhase) {}
  };

  /**
  * A trapezoid-shaped 3-D trajectory.
  */
  class Trapezoid3D : public Trajectory3D
  {
  private:
    double zSize, /**< The height of the trajectory. */
           startUp, /**< Point in phase, when lifting the foot. */
           startAir, /**< Point in phase, when swing forward the foot. */
           startDown, /**< Point in phase, when lowering the foot. */
           startGround; /**< Point in phase, when touching the ground. */

    /**
    * The method calculates an untilted offset in the trajectory around origin (0, 0, 0).
    * @param phase The phase in the trajectory in the range of [0..1[.
    * @param xSize Half of the size of the trajectory in x-direction in mm.
    * @param ySize Half of the size of the trajectory in y-direction in mm.

    * @return The offset.
    */
    virtual Vector3<double> getOffset(double phase, double xSize, double ySize) const
    {
      Vector2<double> scale;
      if(phase < startUp)
        scale = Vector2<double>(phase / startUp, 0);
      else if(phase < startAir)
        scale = Vector2<double>(1.0 - 2.0 * (phase - startUp) / (startGround - startUp), (phase - startUp) / (startAir - startUp));
      else if(phase < startDown)
        scale = Vector2<double>(1.0 - 2.0 * (phase - startUp) / (startGround - startUp), 1.0);
      else if(phase < startGround)
        scale = Vector2<double>(1.0 - 2.0 * (phase - startUp) / (startGround - startUp), 1.0 - (phase - startDown) / (startGround - startDown));
      else
        scale = Vector2<double>((phase - 1) / startUp, 0);
      return Vector3<double>(-xSize * scale.x, -ySize * scale.x, zSize * scale.y);
    }

  public:
    /**
    * Constructor.
    * @param origin The origin of the trajectory.
    * @param tiltForward The tilt of the trajectory when moving forward.
    * @param tiltBackward The tilt of the trajectory when moving backward.
    * @param tiltSideways The tilt of the trajectory when moving sideways.
    * @param phaseShift The phase shift of this trajectory in the range of [0..1[.
    * @param zSize The height of a step in mm.
    * @param phases Phase ratios for ground time, up time, and air time for each step.
    */
    Trapezoid3D(const Vector3<double>& origin, 
                double tiltForward, double tiltBackward, double tiltSideways, 
                double phaseShift, double zSize, const double* phases)
      : Trajectory3D(origin, tiltForward, tiltBackward, tiltSideways, phaseShift),
        zSize(zSize),
        startUp(phases[0] / 2),
        startAir(startUp + phases[1]),
        startDown(startAir + phases[2]),
        startGround(1 - phases[0] / 2) {}
  };

  /**
  * A ellipsoid-shaped 3-D trajectory.
  */
  class Ellipse3D : public Trajectory3D
  {
  private:
    double zSize,
           startAir,
           startGround;

    /**
    * The method calculates an untilted offset in the trajectory around origin (0, 0, 0).
    * @param phase The phase in the trajectory in the range of [0..1[.
    * @param xSize Half of the size of the trajectory in x-direction in mm.
    * @param ySize Half of the size of the trajectory in y-direction in mm.

    * @return The offset.
    */
    virtual Vector3<double> getOffset(double phase, double xSize, double ySize) const
    {
      Vector2<double> scale;
      if(phase < startAir)
      {
        double angle = pi_2 * phase / startAir;
        scale = Vector2<double>(sin(angle), 0.5 - 0.5 * cos(angle));
      }
      else if(phase < startGround)
      {
        double angle = pi * (phase - startAir) / (startGround - startAir);
        scale = Vector2<double>(cos(angle), 0.5 + 0.5 * sin(angle));
      }
      else
      {
        double angle = pi_2 * (1 - phase) / startAir;
        scale = Vector2<double>(-sin(angle), 0.5 - 0.5 * cos(angle));
      }
      return Vector3<double>(-xSize * scale.x, -ySize * scale.x, zSize * scale.y);
    }

  public:
    /**
    * Constructor.
    * @param origin The origin of the trajectory.
    * @param tiltForward The tilt of the trajectory when moving forward.
    * @param tiltBackward The tilt of the trajectory when moving backward.
    * @param tiltSideways The tilt of the trajectory when moving sideways.
    * @param phaseShift The phase shift of this trajectory in the range of [0..1[.
    * @param zSize The height of a step in mm.
    * @param phases Phase ratio for ground time for each step.
    */
    Ellipse3D(const Vector3<double>& origin,
              double tiltForward, double tiltBackward, double tiltSideways, 
              double phaseShift, double zSize, const double* phases)
      : Trajectory3D(origin, tiltForward, tiltBackward, tiltSideways, phaseShift),
        zSize(zSize),
        startAir(phases[0] / 2),
        startGround(1 - phases[0] / 2) {}
  };

  /**
  * A half-ellipsoid-shaped 3-D trajectory.
  */
  class HalfEllipse3D : public Trajectory3D
  {
  private:
    double zSize,
           startAir,
           startGround;

    /**
    * The method calculates an untilted offset in the trajectory around origin (0, 0, 0).
    * @param phase The phase in the trajectory in the range of [0..1[.
    * @param xSize Half of the size of the trajectory in x-direction in mm.
    * @param ySize Half of the size of the trajectory in y-direction in mm.

    * @return The offset.
    */
    virtual Vector3<double> getOffset(double phase, double xSize, double ySize) const
    {
      Vector2<double> scale;
      if(phase < startAir)
        scale = Vector2<double>(phase / startAir, 0);
      else if(phase < startGround)
      {
        double angle = pi * (phase - startAir) / (startGround - startAir);
        scale = Vector2<double>(cos(angle), sin(angle));
      }
      else
        scale = Vector2<double>((phase - 1) / startAir, 0);
      return Vector3<double>(-xSize * scale.x, -ySize * scale.x, zSize * scale.y);
    }

  public:
    /**
    * Constructor.
    * @param origin The origin of the trajectory.
    * @param tiltForward The tilt of the trajectory when moving forward.
    * @param tiltBackward The tilt of the trajectory when moving backward.
    * @param tiltSideways The tilt of the trajectory when moving sideways.
    * @param phaseShift The phase shift of this trajectory in the range of [0..1[.
    * @param zSize The height of a step in mm.
    * @param phases Phase ratio for ground time for each step.
    */
    HalfEllipse3D(const Vector3<double>& origin,
                  double tiltForward, double tiltBackward, double tiltSideways, 
                  double phaseShift, double zSize, const double* phases)
      : Trajectory3D(origin, tiltForward, tiltBackward, tiltSideways, phaseShift),
        zSize(zSize),
        startAir(phases[0] / 2),
        startGround(1 - phases[0] / 2) {}
  };

  /**
  * A half-ellipsoid-shaped 3-D trajectory.
  */
  class InverseHalfEllipse3D : public Trajectory3D
  {
  private:
    double zSize,
           startAir,
           startGround;

    /**
    * The method calculates an untilted offset in the trajectory around origin (0, 0, 0).
    * @param phase The phase in the trajectory in the range of [0..1[.
    * @param xSize Half of the size of the trajectory in x-direction in mm.
    * @param ySize Half of the size of the trajectory in y-direction in mm.
    * @return The offset.
    */
    virtual Vector3<double> getOffset(double phase, double xSize, double ySize) const
    {
      Vector2<double> scale;
      if(phase < startAir)
      {
        double angle = pi_2 * phase / startAir;
        scale = Vector2<double>(sin(angle), 1.0 - cos(angle));
      }
      else if(phase < startGround)
        scale = Vector2<double>(1 - 2.0 * (phase - startAir) / (startGround - startAir), 1);
      else
      {
        double angle = pi_2 * (1 - phase) / startAir;
        scale = Vector2<double>(-sin(angle), 1.0 - cos(angle));
      }
      return Vector3<double>(-xSize * scale.x, -ySize * scale.x, zSize * scale.y);
    }

  public:
    /**
    * Constructor.
    * @param origin The origin of the trajectory.
    * @param tiltForward The tilt of the trajectory when moving forward.
    * @param tiltBackward The tilt of the trajectory when moving backward.
    * @param tiltSideways The tilt of the trajectory when moving sideways.
    * @param phaseShift The phase shift of this trajectory in the range of [0..1[.
    * @param zSize The height of a step in mm.
    * @param phases Phase ratio for ground time for each step.
    */
    InverseHalfEllipse3D(const Vector3<double>& origin,
                         double tiltForward, double tiltBackward, double tiltSideways, 
                         double phaseShift, double zSize, const double* phases)
      : Trajectory3D(origin, tiltForward, tiltBackward, tiltSideways, phaseShift),
        zSize(zSize),
        startAir(phases[0] / 2),
        startGround(1 - phases[0] / 2) {}
  };



#endif

