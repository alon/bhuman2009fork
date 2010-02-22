/**
* @file GaussianDistribution2D.h
* 
* Definition of class GaussianDistribution2D
*
* @author <a href="mailto:Tim.Laue@dfki.de">Tim Laue</a>
*/

#ifndef __GaussianDistribution2D_h_
#define __GaussianDistribution2D_h_

#include "Vector2.h"
#include "Matrix2x2.h"

/**
* @class GaussianDistribution2D
* Represents a 2-dimensional normal distribution
*/
class GaussianDistribution2D
{
public:
  /** The mean value of the distribution*/
  Vector2<double> mean;
  /** The variance of the distribution*/
  Matrix2x2<double> covariance;

  /** Empty standard constructor*/
  GaussianDistribution2D() {}

  /** Copy constructor
  * @param other The other distribution that is copied to this one
  */
  GaussianDistribution2D(const GaussianDistribution2D& other) {*this = other;}

  /** Computes the Mahalanobis distance to another distribution.
  *   This function is used to compare two different distributions for checking, if
  *   they describe the same phenomenon.
  * @param other Another distribution
  * @return A value without any unit
  */
  double distanceTo(const GaussianDistribution2D& other) const;

  /** Computes the probability density at a given position
  * @param pos The position
  * @return The probability at pos.
  */
  virtual double probabilityAt(const Vector2<double>& pos) const;

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
  */
  void generateDistributionFromMeasurements(double* x, int numOfX, double* y, int numOfY);

  /** Assignment operator
  * @param other The other distribution that is assigned to this one
  * @return A reference to this object after the assignment.
  */
  GaussianDistribution2D& operator=(const GaussianDistribution2D & other)
  {
    mean = other.mean;
    covariance = other.covariance;
    return *this;
  }

  /** Addition of another distribution to this one.
  * @param other The other distribution that will be added to this one
  * @return A reference to this object after the calculation.
  */
  GaussianDistribution2D& operator+=(const GaussianDistribution2D& other);

  /** Merges this distribution with another distribution.
  *   The result is different from the += operator, since this operation
  *   creates a hypothesis compliant to the previous two hypotheses. The 
  *   result has a higher covariance than the operands.
  * @param other Another hypothesis
  */
  void merge(const GaussianDistribution2D& other);

  /** Computes a random tuple in the environment of the mean
   *  based on the covariance of the distribution
   * @return A tuple
   */
  Vector2<double> rand() const;

  /** Computes eigenvectors and eigenvalues of covariance matrix
  * @param eVec1 Returns the first eigenvector
  * @param eVec2 Returns the second eigenvector
  * @param eValue1 Returns the first eigenvalue
  * @param eValue2 Returns the second eigenvalue
  */
  void getEigenVectorsAndEigenValues(Vector2<double>& eVec1, Vector2<double>& eVec2, 
    double& eValue1, double& eValue2) const;

private:
  /** Computes a cholesky decomposition 'A=LLt' for a symmetric positive semidefinite A.
   * If the computation fails, the matrix is not S.P.D. and 'false' is returned.
   * Diagonal entries between \c [-eps..0] are treated as 0 leading to a valid
   * decomposition. Even diagonal entries infinity are allowed.
   * @author Udo Frese
   */
  bool choleskyDecomposition(const Matrix2x2<double>& A, Matrix2x2<double>& L, double eps=0) const;
};

#endif //__GaussianDistribution2D_h_
