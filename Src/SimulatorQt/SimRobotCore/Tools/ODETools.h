/**
 * @file ODETools.h
 * 
 * Utility functions for using the Open Dynamics Engine
 *
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 */ 

#ifndef ODE_TOOLS_H_
#define ODE_TOOLS_H_

#include <Tools/SimMath.h>
#include <ode/ode.h>


class ODETools
{
public:
  /** Converts a matrix from the SimRobot format to
  *   the ODE format
  * @param m The original matrix
  * @param mODE The ODE matrix
  */
  static void convertMatrix(const Matrix3d& m, dMatrix3& mODE)                          
  {
    mODE[0] = dReal(m.col[0].v[0]);
    mODE[1] = dReal(m.col[1].v[0]);
    mODE[2] = dReal(m.col[2].v[0]);
    mODE[3] = 0.0;
    mODE[4] = dReal(m.col[0].v[1]);
    mODE[5] = dReal(m.col[1].v[1]);
    mODE[6] = dReal(m.col[2].v[1]);
    mODE[7] = 0.0;
    mODE[8] = dReal(m.col[0].v[2]);
    mODE[9] = dReal(m.col[1].v[2]);
    mODE[10] = dReal(m.col[2].v[2]);
    mODE[11] = 0.0;
  }

  /** 
  * Converts a matrix from the ODE format to the SimRobot format
  * @param mODE The original ODE matrix
  * @param mSim The SimRobot matrix
  */
  static void convertMatrix(const dMatrix3& mODE, Matrix3d& mSim)                          
  {
    mSim.col[0].v[0] = mODE[0];
    mSim.col[1].v[0] = mODE[1];
    mSim.col[2].v[0] = mODE[2];
    mSim.col[0].v[1] = mODE[4];
    mSim.col[1].v[1] = mODE[5];
    mSim.col[2].v[1] = mODE[6];
    mSim.col[0].v[2] = mODE[8];
    mSim.col[1].v[2] = mODE[9];
    mSim.col[2].v[2] = mODE[10];
  }

  /** 
   * Converts a matrix from the ODE format to the SimRobot format
   * @param mODE The original ODE matrix as pointer to the first value
   * @param m The SimRobot matrix
   */
  static void convertMatrix(const dReal* mODE, Matrix3d& mSim)                          
  {
    mSim.col[0].v[0] = mODE[0];
    mSim.col[1].v[0] = mODE[1];
    mSim.col[2].v[0] = mODE[2];
    mSim.col[0].v[1] = mODE[4];
    mSim.col[1].v[1] = mODE[5];
    mSim.col[2].v[1] = mODE[6];
    mSim.col[0].v[2] = mODE[8];
    mSim.col[1].v[2] = mODE[9];
    mSim.col[2].v[2] = mODE[10];
  }

  /** 
  * Converts a 3-dimensional vector from the ODE format to the SimRobot format
  * @param vODE The original ODE vector
  * @param vSim The SimRobot vector
  */
  static void convertVector(const dReal* vODE, Vector3d& vSim)                          
  {
    vSim.v[0] = vODE[0];
    vSim.v[1] = vODE[1];
    vSim.v[2] = vODE[2];
  }

  /** 
  * Converts a 3-dimensional vector from the ODE format to the SimRobot format
  * @param vODE The original ODE vector
  * @return The SimRobot vector
  */
  static Vector3d convertVector(const dReal* vODE)
  {
    return Vector3d (vODE[0], vODE[1], vODE[2]);
  }
};


#endif //ODE_TOOLS_H_
