/**
* @file GaussianDistribution3D.h
* 
* Definition of class GaussianDistribution3D
*
* @author <a href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</a>
*/

#ifndef __GaussianDistribution3D_h_
#define __GaussianDistribution3D_h_

#include "Vector3.h"
#include "Matrix.h"

/**
* @class GaussianDistribution3D
* Represents a 2-dimensional normal distribution
*/
class GaussianDistribution3D
{
public:
  /** The mean value of the distribution*/
  Vector3<double> mean;
  /** The variance of the distribution*/
  Matrix3x3<double> covariance;

  /** Empty standard constructor*/
  GaussianDistribution3D() {}

  /** Copy constructor
  * @param other The other distribution that is copied to this one
  */
  GaussianDistribution3D(const GaussianDistribution3D& other) {*this = other;}

  /** Computes the Mahalanobis distance to another distribution.
  *   This function is used to compare two different distributions for checking, if
  *   they describe the same phenomenon.
  * @param other Another distribution
  * @return A value without any unit
  */
  double distanceTo(const GaussianDistribution3D& other) const;

  /** Computes the probability density at a given position
  * @param pos The position
  * @return The probability at pos.
  */
  virtual double probabilityAt(const Vector3<double>& pos) const;

  /** Computes the probability density at the center of the distribution
  * @return The probability
  */
  double probabilityAtMean() const
    { return probabilityAt(mean);}

  /** Computes mean and covariance matrix of the distribution
  * @param x A list of measurements of a variable
  * @param numOfX The number of measurements
  * @param y A list of measurements of another variable
  * @param numOfY The number of measurements
  * @param z A list of measurements of the third variable
  * @param numOfZ The number of measurements
  */
  void generateDistributionFromMeasurements(double* x, int numOfX, double* y, int numOfY, double* z, int numOfZ);

  /** Assignment operator
  * @param other The other distribution that is assigned to this one
  * @return A reference to this object after the assignment.
  */
  GaussianDistribution3D& operator=(const GaussianDistribution3D & other)
  {
    mean = other.mean;
    covariance = other.covariance;
    return *this;
  }

  /** Addition of another distribution to this one (sensor fusion).
  * @param other The other distribution that will be added to this one
  * @return A reference to this object after the calculation.
  */
  GaussianDistribution3D& fuse(const GaussianDistribution3D& other);

  /** Merges this distribution with another distribution.
  *   The result is different from fusion, since this operation
  *   creates a hypothesis compliant to the previous two hypotheses. The 
  *   result has a higher covariance than the operands.
  * @param other Another hypothesis
  */
  void merge(const GaussianDistribution3D& other);
};

#endif //__GaussianDistribution3D_h_
