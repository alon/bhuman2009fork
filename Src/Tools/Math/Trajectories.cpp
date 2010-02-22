/**
* @file Trajectories.cpp
* Contains class 
*
* @author <a href="mailto:sancho@informatik.uni-bremen.de">Dario Sancho-Pradel</a>
*/
//------------------------------------------------------------------------------

#include "Trajectories.h"
#include "Platform/GTAssert.h"

 const char* Trajectory1D::getShapeName(Trajectory1D::Shape shape)
{
  switch(shape)
  {
  case constant: return "constant";
  case rectangle: return "rectangle";
  case sineRectangle: return "sineRectangle";
  case sine: return "sine";
  case constAccel: return "constAccel";
  case sineSqrt: return "sineSqrt";
  case halfSine: return "halfSine";
  case doubleSine: return "doubleSine";
  case doubleSinePositive: return "doubleSinePositive";
  case flatSine: return "flatSine";
  case airSine: return "airSine";
  case doubleParabola: return "doubleParabola";
  case sqrRectangle: return "sqrRectangle";
  case expRectangle: return "expRectangle";
  case cubicRectangle: return "cubicRectangle";
  default: return "unknown";
  }
}

const char* Trajectory1D::getScaleModeName(Trajectory1D::ScaleMode scaleMode)
{
  switch(scaleMode)
  {
  case noScale: return "noScale";
  case sign: return "sign";
  case linear: return "linear";
  case absLinear: return "absLinear";
  case posLinear: return "posLinear";
  default: return "unknown";
  }
}

Trajectory1D* Trajectory1D::create(
  Trajectory1D::Shape shape,
  double origin, Trajectory1D::ScaleMode scaleMode, 
  double scale, double phaseShift, const double* phases)
{
  switch(shape)
  {
  case constant:
    return new Constant1D(origin);
  case rectangle:
    return new Rectangle1D(origin, scaleMode, scale, phaseShift, phases);
  case sineRectangle:
    return new SineRectangle1D(origin, scaleMode, scale, phaseShift, phases);
  case sine:
    return new Sine1D(origin, scaleMode, scale, phaseShift);
  case constAccel:
    return new ConstAccel1D(origin, scaleMode, scale, phaseShift, phases);
  case sineSqrt:
    return new SineSqrt1D(origin, scaleMode, scale, phaseShift);
  case halfSine:
    return new HalfSine1D(origin, scaleMode, scale, phaseShift);
  case doubleSine:
    return new DoubleSine1D(origin, scaleMode, scale, phaseShift);
  case doubleSinePositive:
    return new DoubleSinePositive1D(origin, scaleMode, scale, phaseShift);
  case flatSine:
    return new FlatSine1D(origin, scaleMode, scale, phaseShift);
  case airSine:
    return new AirSine1D(origin, scaleMode, scale, phaseShift, phases);
  case doubleParabola:
    return new DoubleParabola1D(origin, scaleMode, scale, phaseShift);
  case sqrRectangle:
    return new PowRectangle1D(origin, scaleMode, scale, phaseShift, phases, 2.0);
  case expRectangle:
    return new PowRectangle1D(origin, scaleMode, scale, phaseShift, phases, exp(1.0));
  case cubicRectangle:
    return new PowRectangle1D(origin, scaleMode, scale, phaseShift, phases, 3.0);

  default:
    ASSERT(false);
    return 0;
  }
}

const char* Trajectory3D::getShapeName(Trajectory3D::Shape shape)
{
  switch(shape)
  {
  case rectangle: return "rectangle";
  case trapezoid: return "trapezoid";
  case constAccel: return "constAccel";
  case ellipse: return "ellipse";
  case halfEllipse: return "halfEllipse";
  case inverseHalfEllipse: return "inverseHalfEllipse";
  default: return "unknown";
  }
}

Trajectory3D* Trajectory3D::create(
  Trajectory3D::Shape shape,
  const Vector3<double>& origin,
  double tiltForward, double tiltBackward, double tiltSideways, double phaseShift, double height, const double* phases)
{
  switch(shape)
  {
  case rectangle:
    return new Rectangle3D(origin, tiltForward, tiltBackward, tiltSideways, phaseShift, height, phases);
  case trapezoid:
    return new Trapezoid3D(origin, tiltForward, tiltBackward, tiltSideways, phaseShift, height, phases);
  case constAccel:
    return new ConstAccel3D(origin, tiltForward, tiltBackward, tiltSideways, phaseShift, height, phases);
  case ellipse:
    return new Ellipse3D(origin, tiltForward, tiltBackward, tiltSideways, phaseShift, height, phases);
  case halfEllipse:
    return new HalfEllipse3D(origin, tiltForward, tiltBackward, tiltSideways, phaseShift, height, phases);
  case inverseHalfEllipse:
    return new InverseHalfEllipse3D(origin, tiltForward, tiltBackward, tiltSideways, phaseShift, height, phases);
  default:
    ASSERT(false);
    return 0;
  }
}
