/**
* @file GaussianDistribution3D.cpp
* 
* Implementation of class GaussianDistribution3D
*
* @author <a href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</a>
*/

#include "Probabilistics.h"
#include "GaussianDistribution3D.h"


double GaussianDistribution3D::distanceTo(const GaussianDistribution3D& other) const
{
  Vector3<double> diff(mean - other.mean);
  Matrix3x3<double> cov(covariance + other.covariance);
  return diff*(cov.invert()*diff);
}

double GaussianDistribution3D::probabilityAt(const Vector3<double>& pos) const
{
  Vector3<double> diff(pos - mean);
  double exponent(diff*(covariance.invert()*diff));
  double probability(1.0 / (pi2 * sqrt(covariance.det()) ) );
  probability *= exp(-0.5*exponent);
  return probability;
}

void GaussianDistribution3D::generateDistributionFromMeasurements(
        double* x, int numOfX, double* y, int numOfY, double* z, int numOfZ)
{
  if(numOfX < 2 || numOfY < 2 || numOfZ < 2)
    return;
  mean.x = mean.y = mean.z = 0.0;
  for(int i=0; i<numOfX; i++)
    mean.x += x[i];
  for(int i=0; i<numOfY; i++)
    mean.y += y[i];
  for(int i=0; i<numOfZ; i++)
    mean.z += z[i];
  mean.x /= numOfX;
  mean.y /= numOfY;
  mean.z /= numOfZ;
  double varianceX(0.0);
  double varianceY(0.0);
  double varianceZ(0.0);
  for(int i=0; i<numOfX; i++)
    varianceX += (x[i]-mean.x)*(x[i]-mean.x);
  varianceX *= 1.0 / (numOfX-1);
  for(int i=0; i<numOfY; i++)
    varianceY += (y[i]-mean.y)*(y[i]-mean.y);
  varianceY *= 1.0 / (numOfY-1);
  for(int i=0; i<numOfZ; i++)
    varianceZ += (z[i]-mean.z)*(z[i]-mean.z);
  varianceZ *= 1.0 / (numOfZ-1);
  covariance[0][0] = varianceX;
  covariance[1][1] = varianceY;
  covariance[2][2] = varianceZ;
  double cov_xy(0.0);
  int maxXY = numOfX > numOfY ? numOfY : numOfX;
  for(int i=0; i<maxXY; i++)
    cov_xy += (x[i] - mean.x)*(y[i] - mean.y);
  cov_xy *= 1.0 / (maxXY-1);
  covariance[0][1] = covariance[1][0] = cov_xy;
  double cov_xz(0.0);
  int maxXZ = numOfX > numOfZ ? numOfZ : numOfX;
  for(int i=0; i<maxXZ; i++)
    cov_xz += (x[i] - mean.x)*(z[i] - mean.z);
  cov_xz *= 1.0 / (maxXZ-1);
  covariance[0][2] = covariance[2][0] = cov_xz;
  double cov_yz(0.0);
  int maxYZ = numOfY > numOfZ ? numOfZ : numOfY;
  for(int i=0; i<maxYZ; i++)
    cov_yz += (y[i] - mean.y)*(z[i] - mean.z);
  cov_yz *= 1.0 / (maxYZ-1);
  covariance[1][2] = covariance[2][1] = cov_yz;
}

GaussianDistribution3D& GaussianDistribution3D::fuse(const GaussianDistribution3D& other)
{
  Matrix3x3<double> K(covariance);
  K *= (covariance + other.covariance).invert();
  mean += K * (other.mean - mean);
  covariance -= K * covariance;
  return *this;
}

void GaussianDistribution3D::merge(const GaussianDistribution3D& other)
{
  //Compute new mean and values for new covariance matrix:
  double p1(probabilityAtMean());
  double p2(other.probabilityAtMean());
  double pSum(p1+p2);
  double w1(p1/pSum);
  double w2(p2/pSum);
  Vector3<double> newMean(mean*w1 + other.mean*w2);
  Vector3<double> diffMean(mean - other.mean);
  Matrix3x3<double> meanMatrix;
  meanMatrix.c[0].x = diffMean.x*diffMean.x;
  meanMatrix.c[0].y = diffMean.x*diffMean.y;
  meanMatrix.c[0].z = diffMean.x*diffMean.z;
  meanMatrix.c[1].x = diffMean.x*diffMean.y;
  meanMatrix.c[1].y = diffMean.y*diffMean.y;
  meanMatrix.c[1].z = diffMean.z*diffMean.y;
  meanMatrix.c[1].x = diffMean.x*diffMean.z;
  meanMatrix.c[1].y = diffMean.z*diffMean.y;
  meanMatrix.c[1].z = diffMean.z*diffMean.z;
  //Set new values:
  mean = newMean;
  covariance = (covariance*w1 + other.covariance*w2 + meanMatrix*w1*w2);
}
