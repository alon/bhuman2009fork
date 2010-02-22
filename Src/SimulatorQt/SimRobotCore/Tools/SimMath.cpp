/**
 * @file SimMath.cpp
 * 
 * Implementation of some math functions
 *
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 */

#include "SimMath.h"

void Vector3d::rotate(const Matrix3d& m)
{
  Vector3d v_old = (*this);
  v[0] = m.col[0].v[0]*v_old.v[0] + 
    m.col[1].v[0]*v_old.v[1] + 
    m.col[2].v[0]*v_old.v[2];
  v[1] = m.col[0].v[1]*v_old.v[0] + 
    m.col[1].v[1]*v_old.v[1] + 
    m.col[2].v[1]*v_old.v[2];
  v[2] = m.col[0].v[2]*v_old.v[0] + 
    m.col[1].v[2]*v_old.v[1] + 
    m.col[2].v[2]*v_old.v[2];
}

void Matrix3d::interpolate(const Matrix3d& m, const double& factor1, const double& factor2)
{
  Matrix3d newMatrix;
  newMatrix.col[0].v[0] = col[0].v[0]*factor1 + m.col[0].v[0]*factor2;
  newMatrix.col[0].v[1] = col[0].v[1]*factor1 + m.col[0].v[1]*factor2;
  newMatrix.col[0].v[2] = col[0].v[2]*factor1 + m.col[0].v[2]*factor2;
  newMatrix.col[1].v[0] = col[1].v[0]*factor1 + m.col[1].v[0]*factor2;
  newMatrix.col[1].v[1] = col[1].v[1]*factor1 + m.col[1].v[1]*factor2;
  newMatrix.col[1].v[2] = col[1].v[2]*factor1 + m.col[1].v[2]*factor2;
  newMatrix.col[2].v[0] = col[2].v[0]*factor1 + m.col[2].v[0]*factor2;
  newMatrix.col[2].v[1] = col[2].v[1]*factor1 + m.col[2].v[1]*factor2;
  newMatrix.col[2].v[2] = col[2].v[2]*factor1 + m.col[2].v[2]*factor2;
  *this = newMatrix;
}
