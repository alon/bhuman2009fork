/**
* @file AngleEstimator.h
* Declaration of class AngleEstimator.
* @author Colin Graf
*/

#ifndef AngleEstimator_H
#define AngleEstimator_H

#include "Tools/Math/Matrix2x2.h"
#include "Tools/Math/Matrix.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"

template <class V = double> class Matrix2x3;
template <class V = double> class Matrix3x2;

/**
* @class Matrix2x3
* An incomplete implementation of Matrix2x3.
*/
template <class V> class Matrix2x3
{
public:
  Vector2<V> c0; /**< The first column of the matrix. */
  Vector2<V> c1; /**< The second column of the matrix. */
  Vector2<V> c2; /**< The third column of the matrix. */

  /**
  * Constructor; initializes each column of the matrix.
  * @param c0 The first column of the matrix.
  * @param c1 The second column of the matrix.
  * @param c2 The third column of the matrix.
  */
  Matrix2x3<V>(const Vector2<V>& c0, const Vector2<V>& c1, const Vector2<V>& c2) : 
    c0(c0), c1(c1), c2(c2) {}

  /**
  * Multiplication of this matrix by vector.
  * @param vector The vector this one is multiplied by 
  * @return A new vector containing the result of the calculation.
  */
  Vector2<V> operator*(const Vector3<V>& vector) const
  {
    return c0 * vector.x + c1 * vector.y + c2 * vector.z;
  }

  /**
  * Multiplication of this matrix by a 3x3 matrix.
  * @param matrix The other matrix this one is multiplied by .
  * @return A new matrix containing the result of the calculation.
  */
  Matrix2x3<V> operator*(const Matrix3x3<V>& matrix) const
  {
    return Matrix2x3<V>(*this * matrix.c[0], *this * matrix.c[1], *this * matrix.c[2]);
  }

  /**
  * Multiplication of this matrix by a 3x2 matrix.
  * @param matrix The other matrix this one is multiplied by .
  * @return A new matrix containing the result of the calculation.
  */
  Matrix2x2<V> operator*(const Matrix3x2<V>& matrix) const
  {
    return Matrix2x2<V>(*this * matrix.c0, *this * matrix.c1);
  }

  /**
  * Transposes the matrix.
  * @return A new object containing transposed matrix
  */
  Matrix3x2<V> transpose()
  {
    return Matrix2x3<V>(Vector2<V>(c0.x, c1.x, c2.x), Vector2<V>(c0.y, c1.y, c2.y));
  }
};

/**
* @class Matrix3x2
* An incomplete implementation of Matrix3x2.
*/
template <class V> class Matrix3x2
{
public:
  Vector3<V> c0; /**< The first column of the matrix. */
  Vector3<V> c1; /**< The second column of the matrix. */

  /**
  * Constructor; initializes each column of the matrix.
  * @param c0 The first column of the matrix.
  * @param c1 The second column of the matrix.
  */
  Matrix3x2<V>(const Vector3<V>& c0, const Vector3<V>& c1) : 
    c0(c0), c1(c1) {}

  /**
  * Adds another matrix to this one (component by component).
  * @param other The matrix to add.
  * @return A reference this object after the calculation.
  */
  Matrix3x2<V>& operator+=(const Matrix3x2<V>& other)
  {
    c0 += other.c0;
    c1 += other.c1;
    return *this;
  }

  /**
  * Division of this matrix by a factor.
  * @param factor The factor this matrix is divided by 
  * @return A reference to this object after the calculation.
  */
  Matrix3x2<V>& operator/=(const double& factor)
  {
    c0 /= factor;
    c1 /= factor;
    return *this;
  }

  /**
  * Transposes the matrix.
  * @return A new object containing transposed matrix
  */
  Matrix2x3<V> transpose()
  {
    return Matrix2x3<V>(Vector2<V>(c0.x, c1.x), Vector2<V>(c0.y, c1.y), Vector2<V>(c0.z, c1.z));
  }
};


/**
* @class AngleEstimator
* A Unscented Kalman Filter on SO(3) for estimating angleX and angleY based on gyroX, gyroY, accX, accY and accZ.
*/
class AngleEstimator
{
public:

  /**
  * Some RotationMatrix extentions for converting between the single vector format and the matrix representation.
  */
  class RotationMatrix : public ::RotationMatrix
  {
  public:

    /**
    * Default constructor; the identity transformation.
    */
    RotationMatrix();

    /**
    * Copy constructor.
    */
    RotationMatrix(const Matrix3x3<>& other);

    /**
    * Constructs a rotation matrix from a rotation given as angleAxis.
    * @param angleAxis The rotation.
    */
    RotationMatrix(const Vector2<>& angleAxis);

    /**
    * Constructs a rotation matrix from a rotation given as angleAxis.
    * @param angleAxis The rotation.
    */
    RotationMatrix(const Vector3<>& angleAxis);

    /**
    * Adds some world rotation given as angleAxis to this matrix.
    * @param angleAxis The rotation to add.
    * @return A reference this object after the calculation.
    */
    RotationMatrix operator+(const Vector2<>& angleAxis) const;

    /**
    * Adds some world rotation given as angleAxis to this matrix.
    * @param angleAxis The rotation to add.
    * @return A reference this object after the calculation.
    */
    RotationMatrix& operator+=(const Vector2<>& angleAxis);

    /**
    * Converts the rotation matrix into the single vector format.
    * @return The rotation matrix as angleAxis.
    */
    Vector3<double> getAngleAxis() const;

    /**
    * Calculates the difference between two rotation matrices as angleAxis.
    * @return The difference.
    */
    Vector2<double> operator-(const RotationMatrix& other) const;
  };

  /**
  * Constructor; beginning with angleX = 0 and angleY = 0 with any covariance.
  */
  AngleEstimator();

  /**
  * Resets the covariance matrix of the estimation.
  * @param uncertainty The uncertainty of the estimation.
  */
  void setUncertainty(const Vector2<>& uncertainty);

  /**
  * Performes the process update. Call this in every frame.
  * @param angleAxis A calculated rotation offset.
  * @param processNoise The noise of the calculated rotatio offset.
  */
  void processUpdate(const Vector3<>& angleAxis, const Matrix2x2<>& processNoise);

  /**
  * Inserts a gyrometer measurement.
  * @param gyro The measurement.
  * @param measurementNoise The noise of this measurement.
  */
  void gyroSensorUpdate(const Vector2<>& gyro, const Matrix2x2<>& measurementNoise);

  /**
  * Inserts a acceleration sensor measurement.
  * @param acc The measurement.
  * @param measurementNoise The noise of this measurement.
  */
  void accSensorUpdate(const Vector3<>& acc, const Matrix3x3<>& measurementNoise);

  /**
  * Returns the estimate.
  * @return The angles.
  */
  Vector2<> getAngles();

  /**
  * Returns the offset between the previous and the current estimate.
  * @return The angle offset.
  */
  Vector3<> getAngleOffset();

  /**
  * Returns the current uncertainty of the estimate.
  * @return The uncertrainty of the x- and y-angle.
  */
  Vector2<> getUncertainty();

private:
  /**
  * Calculates the gyro value we would measure when we are in state \c state.
  * @param state The hypothetical state.
  */
  Vector2<> gyroSensorModel(const RotationMatrix& state) const;

  /**
  * Calculates the acc value we would measure when we are in state \c state.
  * @param state The hypothetical state.
  */
  Vector3<> accSensorModel(const RotationMatrix& state) const;

  void generateSigmaPoints();

  RotationMatrix meanOfSigmaPX() const;
  Matrix2x2<> covOfSigmaPX(const RotationMatrix& mean) const;

  Vector3<double> meanOfSigmaPZacc() const;
  Matrix3x3<> covOfSigmaPZacc(const Vector3<>& mean) const;
  Matrix3x2<> covOfSigmaPZaccAndSigmaPX(const Vector3<>& mean) const;

  Vector2<double> meanOfSigmaPZgyro() const;
  Matrix2x2<> covOfSigmaPZgyro(const Vector2<>& mean) const;
  Matrix2x2<> covOfSigmaPZgyroAndSigmaPX(const Vector2<>& mean) const;

  /** 
  * Calculates the cholesky decomposition matrix L of matrix A with L * L' = A.
  * @param A The matrix to calculate the cholesky decomposition of.
  */
  Matrix2x2<> sqrt(const Matrix2x2<>& A) const;

  /**
  * Calculates the tensor product of two vectors.
  * @param a The first vector.
  * @param b The second vector.
  */
  inline Matrix2x2<> tensor(const Vector2<>& a, const Vector2<>& b) const
  {
    return Matrix2x2<>(a * b.x, a * b.y);
  }

  inline Matrix2x2<> tensor(const Vector2<>& a) const
  {
    return tensor(a, a);
  }

  /**
  * Calculates the tensor product of two vectors.
  * @param a The first vector.
  * @param b The second vector.
  */
  inline Matrix3x3<> tensor(const Vector3<>& a, const Vector3<>& b) const
  {
    return Matrix3x3<>(a * b.x, a * b.y, a * b.z);
  }

  inline Matrix3x3<> tensor(const Vector3<>& a) const
  {
    return tensor(a, a);
  }

  /**
  * Calculates the tensor product of two vectors.
  * @param a The first vector.
  * @param b The second vector.
  */
  inline Matrix3x2<> tensor(const Vector3<>& a, const Vector2<>& b) const
  {
    return Matrix3x2<>(a * b.x, a * b.y);
  }

  RotationMatrix lastXInverse; /**< The inverse matrix of the previous state. */
  RotationMatrix x; /**< The estimated state. (A body2ground matrix.) */
  Matrix2x2<> cov; /**< The covariance of the state x. */
  RotationMatrix sigmaPX[5]; /**< A buffer for the sigma points. */
  Vector3<> sigmaPZacc[5]; /**< A buffer for expected acc sensor values at the sigma points. */
  Vector2<> sigmaPZgyro[5]; /**< A buffer for expected gyro sensor values at the sigma points. */
  Matrix2x2<> l; /**< The sqrt(cov) used for the last calculation of the sigma points. */
};

#endif // AngleEstimator_H
