/**
* @file AngleEstimator.h
* Implementation of class AngleEstimator.
* @author Colin Graf
*/

#include "AngleEstimator.h"

AngleEstimator::AngleEstimator() : cov(Matrix2x2<double>(
    1., 0., 
    0., 1.)) {}

void AngleEstimator::setUncertainty(const Vector2<>& uncertainty)
{
  cov = Matrix2x2<double>(
    uncertainty.x, 0., 
    0., uncertainty.y);
}

void AngleEstimator::processUpdate(const Vector3<>& angleAxis, const Matrix2x2<>& processNoise)
{
  lastXInverse = x.invert();

  generateSigmaPoints();

  // apply dynamic model
  RotationMatrix rotation(angleAxis);
  sigmaPX[0] *= rotation;
  sigmaPX[1] *= rotation;
  sigmaPX[2] *= rotation;
  sigmaPX[3] *= rotation;
  sigmaPX[4] *= rotation;

  // update the state
  x = meanOfSigmaPX();
  cov = covOfSigmaPX(x);
  cov += processNoise;
}

Vector2<> AngleEstimator::gyroSensorModel(const RotationMatrix& state) const
{
  const Vector3<>& gyro(((const RotationMatrix&)(lastXInverse * state)).getAngleAxis());
  return Vector2<>(gyro.x, gyro.y);
}

void AngleEstimator::gyroSensorUpdate(const Vector2<>& gyro, const Matrix2x2<>& measurementNoise)
{
  generateSigmaPoints();

  // apply measurement model
  sigmaPZgyro[0] = gyroSensorModel(sigmaPX[0]);
  sigmaPZgyro[1] = gyroSensorModel(sigmaPX[1]);
  sigmaPZgyro[2] = gyroSensorModel(sigmaPX[2]);
  sigmaPZgyro[3] = gyroSensorModel(sigmaPX[3]);
  sigmaPZgyro[4] = gyroSensorModel(sigmaPX[4]);

  // update the state
  Vector2<> mean = meanOfSigmaPZgyro();
  Matrix2x2<> covZX = covOfSigmaPZgyroAndSigmaPX(mean);
  Matrix2x2<> kalmanGain = covZX.transpose() * (covOfSigmaPZgyro(mean) + measurementNoise).invert();
  const Vector2<>& offset(kalmanGain * (gyro - mean));
  x += offset;
  cov -= kalmanGain * covZX;
}

Vector3<> AngleEstimator::accSensorModel(const RotationMatrix& state) const
{
  return Vector3<>(state.c[0].z, state.c[1].z, state.c[2].z) * -1;
}

void AngleEstimator::accSensorUpdate(const Vector3<>& acc, const Matrix3x3<>& measurementNoise)
{
  generateSigmaPoints();

  // apply measurement model
  sigmaPZacc[0] = accSensorModel(sigmaPX[0]);
  sigmaPZacc[1] = accSensorModel(sigmaPX[1]);
  sigmaPZacc[2] = accSensorModel(sigmaPX[2]);
  sigmaPZacc[3] = accSensorModel(sigmaPX[3]);
  sigmaPZacc[4] = accSensorModel(sigmaPX[4]);

  // update the state
  Vector3<> mean = meanOfSigmaPZacc();
  Matrix3x2<> covZX = covOfSigmaPZaccAndSigmaPX(mean);
  Matrix2x3<> kalmanGain = covZX.transpose() * (covOfSigmaPZacc(mean) + measurementNoise).invert();
  const Vector2<>& offset(kalmanGain * (acc - mean));
  x += offset;
  cov -= kalmanGain * covZX;
}

Matrix2x2<> AngleEstimator::sqrt(const Matrix2x2<>& a) const
{
  const double l11 = ::sqrt(a.c[0].x);
  const double l21 = (a.c[0].y + a.c[1].x) / (2 * l11);
  const double l22 = ::sqrt(a.c[1].y - l21 * l21);
  return Matrix2x2<>(l11, 0., l21, l22);
}

AngleEstimator::RotationMatrix::RotationMatrix() : ::RotationMatrix() {}

AngleEstimator::RotationMatrix::RotationMatrix(const Matrix3x3<>& other) : ::RotationMatrix(other) {}

AngleEstimator::RotationMatrix::RotationMatrix(const Vector2<>& angleAxis)
{
  const double angle = angleAxis.abs();
  const Vector2<> axis(angleAxis / angle);
  //rotation is only possible with unit vectors
  const double &x = axis.x, &y = axis.y;
  //compute sine and cosine of angle because it is needed quite often for complete matrix
  const double si = sin(angle), co = cos(angle);
  //compute all components needed more than once for complete matrix
  const double v = 1 - co;
  const double xyv = x * y * v;
  const double xs = x * si;
  const double ys = y * si;
  c[0].x = x * x * v + co; c[1].x = xyv;            c[2].x = ys;
  c[0].y = xyv;            c[1].y = y * y * v + co; c[2].y = -xs;
  c[0].z = -ys;            c[1].z = xs;             c[2].z = co;
}

AngleEstimator::RotationMatrix::RotationMatrix(const Vector3<>& angleAxis)
{
  const double angle = angleAxis.abs();
  const Vector3<> axis(angleAxis / angle);
  //rotation is only possible with unit vectors
  const double &x = axis.x, &y = axis.y, &z = axis.z;
  //compute sine and cosine of angle because it is needed quite often for complete matrix
  const double si = sin(angle), co = cos(angle);
  //compute all components needed more than once for complete matrix
  const double v = 1 - co;
  const double xyv = x * y * v;
  const double xzv = x * z * v;
  const double yzv = y * z * v;
  const double xs = x * si;
  const double ys = y * si;
  const double zs = z * si;
  c[0].x = x * x * v + co; c[1].x = xyv - zs;       c[2].x = xzv + ys;
  c[0].y = xyv + zs;       c[1].y = y * y * v + co; c[2].y = yzv - xs;
  c[0].z = xzv - ys;       c[1].z = yzv + xs;       c[2].z = z * z * v + co;
}

AngleEstimator::RotationMatrix AngleEstimator::RotationMatrix::operator+(const Vector2<>& angleAxis) const
{
  //return *this * RotationMatrix(angleAxis);
  return *this * RotationMatrix(invert() * Vector3<>(angleAxis.x, angleAxis.y, 0.));
}

AngleEstimator::RotationMatrix& AngleEstimator::RotationMatrix::operator+=(const Vector2<>& angleAxis)
{
  //*this *= RotationMatrix(angleAxis);
  *this *= RotationMatrix(invert() * Vector3<>(angleAxis.x, angleAxis.y, 0.));
  return *this;
}
/*
Vector2<double> AngleEstimator::RotationMatrix::getAngleAxis() const
{
  double co = (c[0].x + c[1].y + c[2].z - 1.) / 2.;
  if(co > 1.)
    co = 1.;
  else if(co < -1.)
    co = -1.;
  const double angle = acos(co);
  if(angle == 0.)
    return Vector2<>();
  Vector2<double> result(
    c[1].z - c[2].y,
    c[2].x - c[0].z);
  result *= angle / (2. * sin(angle));
  return result;
}
*/
Vector3<double> AngleEstimator::RotationMatrix::getAngleAxis() const
{
  double co = (c[0].x + c[1].y + c[2].z - 1.) / 2.;
  if(co > 1.)
    co = 1.;
  else if(co < -1.)
    co = -1.;
  const double angle = acos(co);
  if(angle == 0.)
    return Vector3<>();
  Vector3<double> result(
    c[1].z - c[2].y,
    c[2].x - c[0].z,
    c[0].y - c[1].x);
  result *= angle / (2. * sin(angle));
  return result;
}

Vector2<double> AngleEstimator::RotationMatrix::operator-(const RotationMatrix& other) const
{
  //return ((const RotationMatrix&)(other.invert() * *this)).getAngleAxis();
  const Vector3<double>& result(other * ((const RotationMatrix&)(other.invert() * *this)).getAngleAxis());
  return Vector2<double>(result.x, result.y);
}

void AngleEstimator::generateSigmaPoints()
{
  l = sqrt(cov);
  sigmaPX[0] = x;
  sigmaPX[1] = x + l.c[0];
  sigmaPX[2] = x + l.c[1];
  sigmaPX[3] = x + (-l.c[0]);
  sigmaPX[4] = x + (-l.c[1]);
}

AngleEstimator::RotationMatrix AngleEstimator::meanOfSigmaPX() const
{
  RotationMatrix result(sigmaPX[0]);
  for(int i = 0; i < 5; ++i)
  {
    Vector2<> chunk(sigmaPX[0] - result);
    chunk += sigmaPX[1] - result;
    chunk += sigmaPX[2] - result;
    chunk += sigmaPX[3] - result;
    chunk += sigmaPX[4] - result;
    chunk /= 5.;
    result += chunk;
    if(chunk.abs() < 0.00001)
      break;
  };
  return result;
}
  
Matrix2x2<> AngleEstimator::covOfSigmaPX(const RotationMatrix& mean) const
{
  Matrix2x2<> result(tensor(sigmaPX[0] - mean));
  result += tensor(sigmaPX[1] - mean);
  result += tensor(sigmaPX[2] - mean);
  result += tensor(sigmaPX[3] - mean);
  result += tensor(sigmaPX[4] - mean);
  result /= 2.;
  return result;
}

Vector3<> AngleEstimator::meanOfSigmaPZacc() const
{
  Vector3<> result(sigmaPZacc[0]);
  for(int i = 0; i < 5; ++i)
  {
    Vector3<> chunk(sigmaPZacc[0] - result);
    chunk += sigmaPZacc[1] - result;
    chunk += sigmaPZacc[2] - result;
    chunk += sigmaPZacc[3] - result;
    chunk += sigmaPZacc[4] - result;
    chunk /= 5.;
    result += chunk;
    if(chunk.abs() < 0.00001)
      break;
  };
  return result;
}

Matrix3x3<> AngleEstimator::covOfSigmaPZacc(const Vector3<>& mean) const
{
  Matrix3x3<> result(tensor(sigmaPZacc[0] - mean));
  result += tensor(sigmaPZacc[1] - mean);
  result += tensor(sigmaPZacc[2] - mean);
  result += tensor(sigmaPZacc[3] - mean);
  result += tensor(sigmaPZacc[4] - mean);
  result /= 2.;
  return result;
}

Matrix3x2<> AngleEstimator::covOfSigmaPZaccAndSigmaPX(const Vector3<>& mean) const
{
  Matrix3x2<> result(tensor(sigmaPZacc[1] - mean, l.c[0]));
  result += tensor(sigmaPZacc[2] - mean, l.c[1]);
  result += tensor(sigmaPZacc[3] - mean, -l.c[0]);
  result += tensor(sigmaPZacc[4] - mean, -l.c[1]);
  result /= 2.;
  return result;
}

Vector2<> AngleEstimator::meanOfSigmaPZgyro() const
{
  Vector2<> result(sigmaPZgyro[0]);
  for(int i = 0; i < 5; ++i)
  {
    Vector2<> chunk(sigmaPZgyro[0] - result);
    chunk += sigmaPZgyro[1] - result;
    chunk += sigmaPZgyro[2] - result;
    chunk += sigmaPZgyro[3] - result;
    chunk += sigmaPZgyro[4] - result;
    chunk /= 5.;
    result += chunk;
    if(chunk.abs() < 0.00001)
      break;
  };
  return result;
}

Matrix2x2<> AngleEstimator::covOfSigmaPZgyro(const Vector2<>& mean) const
{
  Matrix2x2<> result(tensor(sigmaPZgyro[0] - mean));
  result += tensor(sigmaPZgyro[1] - mean);
  result += tensor(sigmaPZgyro[2] - mean);
  result += tensor(sigmaPZgyro[3] - mean);
  result += tensor(sigmaPZgyro[4] - mean);
  result /= 2.;
  return result;
}

Matrix2x2<> AngleEstimator::covOfSigmaPZgyroAndSigmaPX(const Vector2<>& mean) const
{
  Matrix2x2<> result(tensor(sigmaPZgyro[1] - mean, l.c[0]));
  result += tensor(sigmaPZgyro[2] - mean, l.c[1]);
  result += tensor(sigmaPZgyro[3] - mean, -l.c[0]);
  result += tensor(sigmaPZgyro[4] - mean, -l.c[1]);
  result /= 2.;
  return result;
}

Vector2<> AngleEstimator::getAngles()
{
  return Vector2<>(
    atan2(x.c[1].z, x.c[2].z),
    atan2(-x.c[0].z, x.c[2].z));
}

Vector3<> AngleEstimator::getAngleOffset()
{
  return ((const RotationMatrix&)(lastXInverse * x)).getAngleAxis();
}

Vector2<> AngleEstimator::getUncertainty()
{
  return Vector2<>(::sqrt(cov.c[0].x), ::sqrt(cov.c[1].y));
}
