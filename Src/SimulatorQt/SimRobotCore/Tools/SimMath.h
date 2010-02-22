/**
 * @file SimMath.h
 * 
 * Implementation of two basic classes: Matrix3d and Vector3d
 * Collection of functions in Functions
 *
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 */ 

#ifndef SIMMATH_H_
#define SIMMATH_H_

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_PI2
#define M_PI2 6.28318530717958647692
#endif

const double EPSILON = 0.0000000001;

class Matrix3d;

/**
* @class Functions
* A collection of some mathematical functions
*/
class Functions
{
public:
  /** Checks if a value is between two other values
  * @param b1 The lower border
  * @param b2 The upper border
  * @param val The value to be checked
  * @return true, if b1<=v<=b2
  */
  static inline bool between(double b1, double b2, double val)
  { return ((b1<=val) && (val<=b2));}

  /** Converts an angle from degree to radians
  * @param angleInDegree The angle in degree
  * @return The angle in radians
  */
  static inline double toRad(double angleInDegree)
  { return (angleInDegree * M_PI / 180.0);}

  /** Returns the larger of two floating point values
  * @param a The first value
  * @param b The second value
  * @return The maximum
  */
  static inline double maximum(double a, double b)
  { return (a >= b ? a : b);}

  /** Returns the larger of integer values
  * @param a The first value
  * @param b The second value
  * @return The maximum
  */
  static inline int maximum(int a, int b)
  { return (a >= b ? a : b);}

  /** 
   * Converts an angle from radians to degree
   * @param angleInRadians The angle in radians
   * @return The angle in degree
   */
  static inline double toDeg(double angleInRad)
  { return (angleInRad * 180.0 / M_PI);}

};


/**
* @class Vector3d
* A class describing a vector in 3D-space
*/
class Vector3d
{
  public:
    double v[3];
  
    Vector3d()
    {
      v[0] = v[1] = v[2] = 0.0;
    }

    Vector3d(double x, double y, double z)
    {
      init(x,y,z);
    }

    Vector3d(const Vector3d& vec)
    {
      v[0] = vec.v[0];
      v[1] = vec.v[1];
      v[2] = vec.v[2];
    }

    void init(double x, double y, double z)
    {
      v[0] = x;
      v[1] = y;
      v[2] = z;
    }

    bool hasValues()
    {
      return (v[0] != 0.0 || v[1] != 0.0 || v[2] != 0.0);
    }

    void rotate(const Matrix3d& m);
    
    void rotateX(double angle)
    {
      double ca = cos(angle);
      double sa = sin(angle);
      //v[0] does not change
      double x2 = ca * v[1] - sa * v[2];
      double x3 = sa * v[1] + ca * v[2];
      v[1] = x2;
      v[2] = x3;
    }

    void rotateY(double angle)
    {
      double ca = cos(angle);
      double sa = sin(angle);
      double x1 = ca * v[0] + sa * v[2];
      //v[1] does not change
      double x3 = - sa * v[0] + ca * v[2];
      v[0] = x1;
      v[2] = x3;
    }

    void rotateZ(double angle)
    {
      double ca = cos(angle);
      double sa = sin(angle);
      double x1 = ca * v[0] - sa * v[1];
      double x2 = sa * v[0] + ca * v[1];
      //v[2] does not change
      v[0] = x1;
      v[1] = x2;
    }

    double getXRotation() const
    {
      return atan2(v[1],v[2]);
    }

    double getXRotationInDegrees() const
    {
      return Functions::toDeg(getXRotation());
    }

    double getYRotation() const
    {
      return atan2(v[0],v[2]);
    }

    double getYRotationInDegrees() const
    {
      return Functions::toDeg(getYRotation());
    }

    double getZRotation() const
    {
      return atan2(v[0],v[1]);
    }

    double getZRotationInDegrees() const
    {
      return Functions::toDeg(getZRotation());
    }

    void normalize()
    {
      double length = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
      if(length)
      {
        v[0] /= length;
        v[1] /= length;
        v[2] /= length;
      }
    }

    void normalizeToAngles()
    {
      while(v[0] > M_PI) v[0] -= M_PI2;
      while(v[0] < -M_PI) v[0] += M_PI2;
      while(v[1] > M_PI) v[1] -= M_PI2;
      while(v[1] < -M_PI) v[1] += M_PI2;
      while(v[2] > M_PI) v[2] -= M_PI2;
      while(v[2] < -M_PI) v[2] += M_PI2;
    }

    void toRad()
    {
      double factor(M_PI / 180.0);
      v[0] *= factor;
      v[1] *= factor;
      v[2] *= factor;
    }

    double getLength() const
    {
      return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    }

    double getQuadLength() const
    {
      return (v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    }

    void operator+=(const Vector3d& vec)
    {
      v[0] += vec.v[0];
      v[1] += vec.v[1];
      v[2] += vec.v[2];
    }

    void operator-=(const Vector3d& vec)
    {
      v[0] -= vec.v[0];
      v[1] -= vec.v[1];
      v[2] -= vec.v[2];
    }

    void operator*=(const double& scalar)
    {
      v[0] *= scalar;
      v[1] *= scalar;
      v[2] *= scalar;
    }

    void operator/=(const double& scalar)
    {
      v[0] /= scalar;
      v[1] /= scalar;
      v[2] /= scalar;
    }

    Vector3d operator*(const Vector3d& vec) const
    {
      Vector3d newVec(v[1] * vec.v[2] - vec.v[1] * v[2],
                      vec.v[0] * v[2] - v[0] * vec.v[2],
                      v[0] * vec.v[1] - vec.v[0] * v[1]);
      return newVec;
    }

    Vector3d operator*(double n) const
    {
      Vector3d newVec(v[0]*n, v[1]*n, v[2]*n);
      return newVec;
    }

    Vector3d operator/(double n) const
    {
      Vector3d newVec(v[0]/n, v[1]/n, v[2]/n);
      return newVec;
    }

    Vector3d operator+(const Vector3d& vec) const
    {
      Vector3d newVec(*this);
      newVec += vec;
      return newVec;
    }

    Vector3d operator-(const Vector3d& vec) const
    {
      Vector3d newVec(*this);
      newVec -= vec;
      return newVec;
    }

    bool operator==(const Vector3d& vec) const
    {
       return (v[0] == vec.v[0]) && (v[1] == vec.v[1]) && (v[2] == vec.v[2]);
    }

    bool operator!=(const Vector3d& vec) const
    {
      return !(*this == vec);
    }
   
    void operator=(const Vector3d& vec)
    {
      v[0] = vec.v[0];
      v[1] = vec.v[1];
      v[2] = vec.v[2];
    }

    double vecmul (const Vector3d& vec) const
    {
      return (v[0] * vec.v[0] + v[1] * vec.v[1] + v[2] * vec.v[2]);
    }
};


/**
* @class Matrix3d
* A class representing a 3x3-Matrix
*/
class Matrix3d
{
  public:
    Vector3d col[3];

    Matrix3d()
    {
      col[0].v[0] = 1; col[1].v[0] = 0; col[2].v[0] = 0;
      col[0].v[1] = 0; col[1].v[1] = 1; col[2].v[1] = 0;
      col[0].v[2] = 0; col[1].v[2] = 0; col[2].v[2] = 1;
    }

    Matrix3d(const Matrix3d& mat)
    {
      col[0] = mat.col[0];
      col[1] = mat.col[1];
      col[2] = mat.col[2];
    }

    Matrix3d(const Vector3d& rotations)
    {
      Matrix3d newMat;
      newMat.setRotation(rotations.v[0], rotations.v[1], rotations.v[2]);
      *this = newMat;
    }

    inline void setXRotation(const double angle)
    {
      double c = cos(angle);
      double s = sin(angle);
      col[0].v[0] = 1.0; col[1].v[0] = 0.0; col[2].v[0] = 0.0;
      col[0].v[1] = 0.0; col[1].v[1] = c;   col[2].v[1] = -s;
      col[0].v[2] = 0.0; col[1].v[2] = s;   col[2].v[2] = c;
    }

    inline void setYRotation(const double angle)
    {
      double c = cos(angle);
      double s = sin(angle);
      col[0].v[0] = c;   col[1].v[0] = 0.0; col[2].v[0] = s;
      col[0].v[1] = 0.0; col[1].v[1] = 1.0; col[2].v[1] = 0.0;
      col[0].v[2] = -s;   col[1].v[2] = 0.0; col[2].v[2] = c;
    }

    inline void setZRotation(const double angle)
    {
      double c = cos(angle);
      double s = sin(angle);
      col[0].v[0] = c;   col[1].v[0] = -s;  col[2].v[0] = 0.0;
      col[0].v[1] = s;   col[1].v[1] = c;   col[2].v[1] = 0.0;
      col[0].v[2] = 0.0; col[1].v[2] = 0.0; col[2].v[2] = 1.0;
    }

    inline void rotateX(double angle)
    {
      Matrix3d xMat;
      xMat.setXRotation(angle);
      xMat *= *this;
      *this = xMat;
    }

    inline void rotateY(double angle)
    {
      Matrix3d yMat;
      yMat.setYRotation(angle);
      yMat *= *this;
      *this = yMat;
    }

    inline void rotateZ(double angle)
    {
      Matrix3d zMat;
      zMat.setZRotation(angle);
      zMat *= *this;
      *this = zMat;
    }

    inline void setRotation(const double x, const double y, const double z)
    {
      Matrix3d newMatrix;
      newMatrix.rotateX(x);
      newMatrix.rotateY(y);
      newMatrix.rotateZ(z);
      *this = newMatrix;
    }

    inline void operator*=(const Matrix3d& mat)
    {
      (*this) = ((*this)*mat);
    }

    inline Matrix3d operator*(const Matrix3d& mat) const
    {
      Matrix3d m(*this);
      m.col[0].v[0] = mat.col[0].v[0] * col[0].v[0] 
                  + mat.col[0].v[1] * col[1].v[0]
                  + mat.col[0].v[2] * col[2].v[0];

      m.col[0].v[1] = mat.col[0].v[0] * col[0].v[1] 
                  + mat.col[0].v[1] * col[1].v[1]
                  + mat.col[0].v[2] * col[2].v[1];

      m.col[0].v[2] = mat.col[0].v[0] * col[0].v[2] 
                  + mat.col[0].v[1] * col[1].v[2]
                  + mat.col[0].v[2] * col[2].v[2];

      m.col[1].v[0] = mat.col[1].v[0] * col[0].v[0] 
                  + mat.col[1].v[1] * col[1].v[0]
                  + mat.col[1].v[2] * col[2].v[0];

      m.col[1].v[1] = mat.col[1].v[0] * col[0].v[1] 
                  + mat.col[1].v[1] * col[1].v[1]
                  + mat.col[1].v[2] * col[2].v[1];

      m.col[1].v[2] = mat.col[1].v[0] * col[0].v[2] 
                  + mat.col[1].v[1] * col[1].v[2]
                  + mat.col[1].v[2] * col[2].v[2];

      m.col[2].v[0] = mat.col[2].v[0] * col[0].v[0] 
                  + mat.col[2].v[1] * col[1].v[0]
                  + mat.col[2].v[2] * col[2].v[0];

      m.col[2].v[1] = mat.col[2].v[0] * col[0].v[1] 
                  + mat.col[2].v[1] * col[1].v[1]
                  + mat.col[2].v[2] * col[2].v[1];

      m.col[2].v[2] = mat.col[2].v[0] * col[0].v[2] 
                  + mat.col[2].v[1] * col[1].v[2]
                  + mat.col[2].v[2] * col[2].v[2];
      return m;
    }

    void operator=(const Matrix3d& m)
    {
      col[0] = m.col[0];
      col[1] = m.col[1];
      col[2] = m.col[2];
    }

    void transpose()
    {
      Matrix3d newMatrix;
      newMatrix.col[0].v[0] = col[0].v[0];
      newMatrix.col[0].v[1] = col[1].v[0];
      newMatrix.col[0].v[2] = col[2].v[0];
      newMatrix.col[1].v[0] = col[0].v[1];
      newMatrix.col[1].v[1] = col[1].v[1];
      newMatrix.col[1].v[2] = col[2].v[1];
      newMatrix.col[2].v[0] = col[0].v[2];
      newMatrix.col[2].v[1] = col[1].v[2];
      newMatrix.col[2].v[2] = col[2].v[2];
      *this = newMatrix;
    }

    double getXAngle() const
    { 
      double h = sqrt(col[2].v[1] * col[2].v[1] + col[2].v[2] * col[2].v[2]);
      if(h)
      {
        h = col[2].v[2] / h;
        if(h > 1)
          h = 1;
        else if(h < -1)
          h = -1;
        return acos(h) * (col[2].v[1] > 0 ? -1 : 1);
      }
      else
        return 0;
    }

    double getYAngle() const
    { 
      double h = sqrt(col[0].v[0] * col[0].v[0] + col[0].v[1] * col[0].v[1]);
      if(h > 1)
        h = 1;
      return (h ? -acos(h) : -M_PI) * (col[0].v[2] < 0 ? -1 : 1);
    }

    double getZAngle() const
    {
      double h = sqrt(col[0].v[0] * col[0].v[0] + col[0].v[1] * col[0].v[1]);
      if(h)
      {
        h = col[0].v[0] / h;
        if(h > 1)
          h = 1;
        else if(h < -1)
          h = -1;
        return acos(h) * (col[0].v[1] < 0 ? -1 : 1);
      }
      else
        return 0;
    }

    void setRotationAroundAxis(const Vector3d& axis, double angle)
    {
      Matrix3d newRotation;
      Matrix3d xAxisRotation;
      Matrix3d yAxisRotation;
      double d(sqrt(axis.v[1]*axis.v[1] + axis.v[2]*axis.v[2]));
      if(d != 0)
      {
        double sina(axis.v[1] / d);
        double cosa(axis.v[2] / d);
        xAxisRotation.col[1].v[1] = cosa;
        xAxisRotation.col[1].v[2] = sina;
        xAxisRotation.col[2].v[1] = -sina;
        xAxisRotation.col[2].v[2] = cosa;
      }
      double sinb(-axis.v[0]);
      double cosb(d);
      yAxisRotation.col[0].v[0] = cosb;
      yAxisRotation.col[0].v[2] = -sinb;
      yAxisRotation.col[2].v[0] = sinb;
      yAxisRotation.col[2].v[2] = cosb;

      Matrix3d invXAxisRotation(xAxisRotation);
      invXAxisRotation.transpose();
      Matrix3d invYAxisRotation(yAxisRotation);
      invYAxisRotation.transpose();

      newRotation = xAxisRotation * newRotation;
      newRotation = yAxisRotation * newRotation;
      newRotation.rotateZ(angle);
      newRotation = invYAxisRotation * newRotation;
      newRotation = invXAxisRotation * newRotation;
      *this = newRotation;
    }

    inline Vector3d operator*(const Vector3d& vector) const
    {
      return (this->col[0]*vector.v[0] + this->col[1]*vector.v[1] + this->col[2]*vector.v[2]);
    }

    // linear interpolate this matrix with a given and 2 factors [0,1]
    void interpolate(const Matrix3d& m, const double& factor1, const double& factor2);
};

struct TexCoords
{
  double s;
  double t;
};

class Vertex
{
public:
  Vertex()
  {
    tex.s = 0.0;
    tex.t = 0.0;
  }
  Vector3d pos;
  TexCoords tex;
};

#endif //SIMMATH_H_
