/**
* @author Aldebaran Robotics 
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/


#ifndef ALMATH_H
# define ALMATH_H

#include <iostream>
#include "stdlib.h"
#include "math.h"
#include "mersennetwister.h"
#include "alerror.h"
#include <vector>
#include <ctime>

#define _4_PI_ 12.56637056f
#define _2_PI_ 6.28318528f
#define PI 3.14159265f
#define PI_2 1.57079633f
#define PI_4 0.785398163f

/**
 * if you want to avoid too small numbers (<5.10^-29 if using float):
 * a = small_one;
 * a += killDeNormalNumber;
 * make_operations(a);
 * a -= killDeNormalNumber;
 * it will save a LOT of cpu (a multiplication with a denormal operand takes about 170 cycles !!!)
 *
 * for your information : denormal number = (FLOAT_MIN*2^32) = 5.0486911077.10^(-29)
 * the killDeNormalNumber const is chosen bigger than this denormal number
 *
 * for more informations, see : www.musicdsp.org/files/denormal.pdf
 */
#define killDeNormalNumber 1e-20f


/**
 * This header file is meant to contain many usefull fast methods for maths.
 * It contains mainly methods for :
 *    - working with 2D and 3D positions (transformations, matrices, etc...)
 *    - sin, cos lookup tables for fast computing of those functions (3.2 times faster than cosf)
 *    - random generator and random utilities
 *
 * **************************************************** *
 *             WHAT YOU CAN FIND IN THIS FILE
 * **************************************************** *
 *
 * Summary of the struct :
 *    - ALTransform : matrix 3*4 = rotation 3*3 + translation 1*3
 *    - Position2D
 *    - Position3D
 *    - Position6D : Position3D + angles
 *    - Rotation3D
 *    - Velocity3D
 *    - Velocity6D : Velocity3D + rotation speed around the axes
 *
 * Summary of the conversions :
 *    - convertToRad : convert from deg to rad
 *    - convertToDeg : convert from rad to deg
 *
 * Summary of the cos, sin lookup table :
 *    - the tables are 16384 long (must be 2^n for faster processing), this gives an approximation at 4.10-4 rad.
 *    - initInlineCosSin() : initialize the tables containing cos and sin values (automatically called when needed)
 *    - inlineCos (n) : return cos(n) in rad
 *    - inlineSin (n) : return sin(n) in rad
 *
 * Summary of the random functions :
 *    - inlineRand : random number generation using mersenne twister algorithm (float or int)
 *    - inlineRandGauss : random gaussian number generation
 *
 * Others :
 *    - inlineFactorial
 *
 * Rapid summary of the transformation/composition methods :
 *    - you can add, substract, etc... position, rotation and velocity with : +, -, +=, *, *=, /=, etc...
 *    - you can transform a given position with a given rotation / translation using ALTransformFromRotX, etc.
 *    - you can use cout with << for the position, rotation and velocity
 *    - not all the functions are listed here, search the following file for the function you are looking for
 *
 *
 * **************************************************** *
 *          WHAT WOULD BE NICE TO HAVE  (<=> TODO!)
 * **************************************************** *
 *    - complex numbers and operations
 *    - filters ?
 *
 **/


namespace ALMath
{
  /***************************************
   ***************************************
   * *    THE STRUCTS YOU CAN USE :    * *
   ***************************************
   ***************************************/

  //   TRANSFORMATION MATRICES :
  typedef struct
  {
    float r1_c1,r1_c2,r1_c3,r1_c4,
    r2_c1,r2_c2,r2_c3,r2_c4,
    r3_c1,r3_c2,r3_c3,r3_c4;
  }ALTransform;

  //  2D POSITIONS :
  typedef struct
  {
    float x, y;
  }Position2D;

  //  3D POSITIONS :
  typedef struct
  {
    float x, y, z;
  }Position3D;

  //  6D POSITIONS :
  typedef struct
  {
    float x, y, z, wx, wy, wz;
  }Position6D;

  //  3D ROTATIONS :
  typedef struct
  {
    float wx, wy, wz;
  }Rotation3D;

  //  3D VELOCITY :
  typedef struct
  {
    float xd, yd, zd;
  }Velocity3D;

  //  6D VELOCITY :
  typedef struct
  {
    float xd, yd, zd, wxd, wyd, wzd;
  }Velocity6D;

  /*****************************************
   *****************************************
   * *        UNIT CONVERSIONS :         * *
   *****************************************
   *****************************************/
  static const float toRad = 0.017453292f;
  static const float toDeg = 57.295779579f;

  inline static float convertToRad(float pAngle)
  {
    return pAngle*toRad;
  }

  inline static float convertToDeg(float pAngle)
  {
    return pAngle*toDeg;
  }

  /*****************************************
   *****************************************
   * *     COS, SIN LOOKUP TABLES :      * *
   *****************************************
   *****************************************/
  static const int inlineCosSinArraySize = 16384; // gives an approximation at 4.10-4 rad

  static float inlineCosArray[inlineCosSinArraySize];
  static float inlineSinArray[inlineCosSinArraySize];

  static const float freqForInlineCosSin = (float)inlineCosSinArraySize / (_2_PI_ );
  static const float stepForInlineCosSin = _2_PI_ / (float)inlineCosSinArraySize;

  static bool inlineCosSinInitialized = false;
  /*
   * initialize the tables
   */
  inline void initInlineCosSin()
  {
    int i = -1;
    float *p_cos = inlineCosArray, *p_sin = inlineSinArray;

    while ( ++i < inlineCosSinArraySize)
    {
      *p_cos++ = cosf(0.0f + (float)i * stepForInlineCosSin);
      *p_sin++ = sinf(0.0f + (float)i * stepForInlineCosSin);
    }
    inlineCosSinInitialized = true;
  }

  /*
   * return sin(n) in rad
   */
  inline float inlineCos(float n)
  {
    if(!inlineCosSinInitialized)
      initInlineCosSin();

    if(n < 0.0f)
      n = -n;

    return inlineCosArray[ (int)(n*freqForInlineCosSin) % inlineCosSinArraySize ];//a - (a >> 14)<<14];// % inlineCosSinArraySize ];
  }

  /*
   * return sin(n) in rad
   */
  inline float inlineSin(float n)
  {
    if(!inlineCosSinInitialized)
      initInlineCosSin();

    if(n < 0.0f)
      return -inlineSinArray[ (int)(-n*freqForInlineCosSin) % inlineCosSinArraySize ];

    return inlineSinArray[ (int)(n*freqForInlineCosSin) % inlineCosSinArraySize ];
  }



  /***************************************
   ***************************************
   * *       STUFF FOR RANDOM :        * *
   ***************************************
   ***************************************/

  /**
   * initRandom: init the seed of the random
   * DON'T FORGET TO CALL THE INIT BEFORE USING RAND
   **/
  inline void initRandom()
  {
    mersennetwister::init_genrand((unsigned long)std::time(NULL));
  }

  /**
   * inlineRand: return a random between fLowerBound and fHigherBound
   * @param fLowerBound the lower bound of the random number you want
   * @param fHigherBound the higher bound of the random number you want
   **/

  inline float inlineRand(float pLowerBound, float pHigherBound)
  {
    return pLowerBound + (pHigherBound-pLowerBound)*((float)mersennetwister::genrand_real1());
  }

  /**
   * inlineRand: return a random integer between fLowerBound and fHigherBound
   * @param fLowerBound the lower bound of the random number you want
   * @param fHigherBound the higher bound of the random number you want
   **/
  inline unsigned int inlineRand(int pLowerBound, int pHigherBound)
  {
    AL_ASSERT( pLowerBound < pHigherBound );
    return pLowerBound + ((unsigned int) mersennetwister::genrand_int32()) % ( pHigherBound - pLowerBound  +1 ) ;
  }

  /**
   * inlineRandGauss: random gaussian generation
   * @param mu mean of the distribution
   * @param sigma standard deviation
   **/
  inline float inlineRandGauss(float pMu, float pSigma)
  {
    return (pMu+pSigma*(sqrtf(-2.0f*logf((float)mersennetwister::genrand_real1()))* inlineCos(_2_PI_*(float)mersennetwister::genrand_real1())));
  }

  /***************************************
   ***************************************
   * *         OTHER STUFF:            * *
   ***************************************
   ***************************************/
  /**
   * inlineFactoriel: return the factoriel of the given number
   * @param pNumber the factoriel you want to compute
   **/
  inline int inlineFactorial(int pNumber)
  {
    return pNumber> 1 ? (pNumber * inlineFactorial(pNumber-1)) : 1;
  }

  /***************************************
   ***************************************
   * *     THE METHODS/OPERATORS :     * *
   ***************************************
   ***************************************/

  /*********************************
   *         2D POSITIONS :        *
   *********************************/
  inline Position2D operator+ (const Position2D& pPos1, const Position2D& pPos2)
  {
    Position2D res;
    res.x = pPos1.x + pPos2.x;
    res.y = pPos1.y + pPos2.y;
    return res;
  };

  inline Position2D operator- (const Position2D& pPos1,const Position2D& pPos2)
  {
    Position2D res;
    res.x = pPos1.x - pPos2.x;
    res.y = pPos1.y - pPos2.y;
    return res;
  };

  /*********************************
   *         3D POSITIONS :        *
   *********************************/
  inline bool operator< (const Position3D& pPos1,const Position3D& pPos2)
  {
    if(pPos1.x> pPos2.x)
    return false;
    if(pPos1.y> pPos2.y)
    return false;
    if(pPos1.z> pPos2.z)
    return false;
    return true;
  };

  inline bool operator> (const Position3D& pPos1,const Position3D& pPos2)
  {
    if(pPos1.x < pPos2.x)
    return false;
    if(pPos1.y < pPos2.y)
    return false;
    if(pPos1.z < pPos2.z)
    return false;
    return true;
  };

  inline Position3D operator+ (const Position3D& pPos1,const Position3D& pPos2)
  {
    Position3D res;
    res.x = pPos1.x + pPos2.x;
    res.y = pPos1.y + pPos2.y;
    res.z = pPos1.z + pPos2.z;
    return res;
  };

  inline Position3D operator- (const Position3D& pPos1,const Position3D& pPos2)
  {
    Position3D res;
    res.x = pPos1.x - pPos2.x;
    res.y = pPos1.y - pPos2.y;
    res.z = pPos1.z - pPos2.z;
    return res;
  };

  inline Position3D& operator+= (Position3D& pPos1, const Position3D& pPos2)
  {
    pPos1.x += pPos2.x;
    pPos1.y += pPos2.y;
    pPos1.z += pPos2.z;
    return pPos1;
  };

  inline Position3D operator* (const Position3D& pPos1, float pM)
  {
    Position3D res;
    res.x = pPos1.x * pM;
    res.y = pPos1.y * pM;
    res.z = pPos1.z * pM;
    return res;
  };

  inline Position3D operator/ (const Position3D& pPos1, float pM)
  {
    return pPos1 * (1.0f/pM);
  };

  inline Position3D& operator*= (Position3D& pPos1, float pM)
  {
    pPos1.x *=pM;
    pPos1.y *=pM;
    pPos1.z *=pM;
    return pPos1;
  };

  inline Position3D& operator/= (Position3D& pPos1, float pM)
  {
    pPos1 *= (1.0f/pM);
    return pPos1;
  };

  inline float quickInlineDistance3D(Position3D& pPos1, Position3D& pPos2)
  {
    return sqrtf((pPos1.x-pPos2.x)*(pPos1.x-pPos2.x)+(pPos1.y-pPos2.y)*(pPos1.y-pPos2.y)+(pPos1.z-pPos2.z)*(pPos1.z-pPos2.z));
  }

  inline float quickSquaredInlineDistance3D(Position3D& pPos1, Position3D& pPos2)
  {
    return (pPos1.x-pPos2.x)*(pPos1.x-pPos2.x)+(pPos1.y-pPos2.y)*(pPos1.y-pPos2.y)+(pPos1.z-pPos2.z)*(pPos1.z-pPos2.z);
  }

  inline float quickSquaredInlineDistance2D(Position2D& pPos1, Position2D& pPos2)
  {
    return (pPos1.x-pPos2.x)*(pPos1.x-pPos2.x)+(pPos1.y-pPos2.y)*(pPos1.y-pPos2.y);
  }

  inline float quickInlineDistance2D(Position2D& pPos1, Position2D& pPos2)
  {
    return sqrtf((pPos1.x-pPos2.x)*(pPos1.x-pPos2.x)+(pPos1.y-pPos2.y)*(pPos1.y-pPos2.y));
  }

  /*********************************
   *         3D ROTATIONS :        *
   *********************************/
  inline Rotation3D operator+ (const Rotation3D& pRot1,const Rotation3D& pRot2)
  {
    Rotation3D res;
    res.wx = pRot1.wx + pRot2.wx;
    res.wy = pRot1.wy + pRot2.wy;
    res.wz = pRot1.wz + pRot2.wz;
    return res;
  };

  inline Rotation3D operator- (const Rotation3D& pRot1,const Rotation3D& pRot2)
  {
    Rotation3D res;
    res.wx = pRot1.wx - pRot2.wx;
    res.wy = pRot1.wy - pRot2.wy;
    res.wz = pRot1.wz - pRot2.wz;
    return res;
  };

  inline Rotation3D& operator+= (Rotation3D& pRot1, const Rotation3D& pRot2)
  {
    pRot1.wx += pRot2.wx;
    pRot1.wy += pRot2.wy;
    pRot1.wz += pRot2.wz;
    return pRot1;
  };

  inline Rotation3D operator* (const Rotation3D& pRot1, float pM)
  {
    Rotation3D res;
    res.wx = pRot1.wx * pM;
    res.wy = pRot1.wy * pM;
    res.wz = pRot1.wz * pM;
    return res;
  };

  inline Rotation3D operator/ (const Rotation3D& pRot1, float pM)
  {
    return pRot1 * (1.0f/pM);
  };

  inline Rotation3D& operator*= (Rotation3D& pRot1, float pM)
  {
    pRot1.wx *=pM;
    pRot1.wy *=pM;
    pRot1.wz *=pM;
    return pRot1;
  };

  inline Rotation3D& operator/= (Rotation3D& pRot1, float pM)
  {
    pRot1 *= (1.0f/pM);
    return pRot1;
  };

  /*********************************
   *         6D POSITIONS :        *
   *********************************/
  inline Position6D operator+ (const Position6D& pPos1,const Position6D& pPos2)
  {
    Position6D res;
    res.x = pPos1.x + pPos2.x;
    res.y = pPos1.y + pPos2.y;
    res.z = pPos1.z + pPos2.z;
    res.wx = pPos1.wx + pPos2.wx;
    res.wy = pPos1.wy + pPos2.wy;
    res.wz = pPos1.wz + pPos2.wz;
    return res;
  };

  inline Position6D operator- (const Position6D& pPos1,const Position6D& pPos2)
  {
    Position6D res;
    res.x = pPos1.x - pPos2.x;
    res.y = pPos1.y - pPos2.y;
    res.z = pPos1.z - pPos2.z;
    res.wx = pPos1.wx - pPos2.wx;
    res.wy = pPos1.wy - pPos2.wy;
    res.wz = pPos1.wz - pPos2.wz;
    return res;
  };

  inline Position6D& operator+= (Position6D& pPos1, const Position6D& pPos2)
  {
    pPos1.x += pPos2.x;
    pPos1.y += pPos2.y;
    pPos1.z += pPos2.z;
    pPos1.wx += pPos2.wx;
    pPos1.wy += pPos2.wy;
    pPos1.wz += pPos2.wz;
    return pPos1;
  };

  inline Position6D operator* (const Position6D& pPos1, float pM)
  {
    Position6D res;
    res.x = pPos1.x * pM;
    res.y = pPos1.y * pM;
    res.z = pPos1.z * pM;
    res.wx = pPos1.wx * pM;
    res.wy = pPos1.wy * pM;
    res.wz = pPos1.wz * pM;
    return res;
  };

  inline Position6D operator/ (const Position6D& pPos1, float pM)
  {
    return pPos1 * (1.0f/pM);
  };

  inline Position6D& operator*= (Position6D& pPos1, float pM)
  {
    pPos1.x *=pM;
    pPos1.y *=pM;
    pPos1.z *=pM;
    pPos1.wx *=pM;
    pPos1.wy *=pM;
    pPos1.wz *=pM;
    return pPos1;
  };

  inline Position6D& operator/= (Position6D& pPos1, float pM)
  {
    pPos1 *= (1.0f/pM);
    return pPos1;
  };

  /*********************************
   *         6D VELOCITY :         *
   *********************************/
  static inline Velocity6D IDENTITY_VELOCITY6D(void)
  {
    Velocity6D v =
    { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f};
    return v;
  };

  inline Velocity6D operator+ (const Velocity6D& pVel1, const Velocity6D& pVel2)
  {
    Velocity6D res;
    res.xd = pVel1.xd + pVel2.xd;
    res.yd = pVel1.yd + pVel2.yd;
    res.zd = pVel1.zd + pVel2.zd;
    res.wxd = pVel1.wxd + pVel2.wxd;
    res.wyd = pVel1.wyd + pVel2.wyd;
    res.wzd = pVel1.wzd + pVel2.wzd;
    return res;
  };

  inline Velocity6D operator- (const Velocity6D& pVel1, const Velocity6D& pVel2)
  {
    Velocity6D res;
    res.xd = pVel1.xd - pVel2.xd;
    res.yd = pVel1.yd - pVel2.yd;
    res.zd = pVel1.zd - pVel2.zd;
    res.wxd = pVel1.wxd - pVel2.wxd;
    res.wyd = pVel1.wyd - pVel2.wyd;
    res.wzd = pVel1.wzd - pVel2.wzd;
    return res;
  };

  /*********************************
   *        TRANSFORMATIONS :      *
   *********************************/
  static inline ALTransform IDENTITY_TRANSFORM(void)
  {
    ALTransform t =
    { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f};
    return t;
  };

  static inline ALTransform& operator*= (ALTransform& pT1, const ALTransform& pT2)
  {
    float c1 = pT1.r1_c1;
    float c2 = pT1.r1_c2;
    float c3 = pT1.r1_c3;
    pT1.r1_c1 = (c1 * pT2.r1_c1) + (c2 * pT2.r2_c1) + (c3 * pT2.r3_c1);
    pT1.r1_c2 = (c1 * pT2.r1_c2) + (c2 * pT2.r2_c2) + (c3 * pT2.r3_c2);
    pT1.r1_c3 = (c1 * pT2.r1_c3) + (c2 * pT2.r2_c3) + (c3 * pT2.r3_c3);
    pT1.r1_c4 = (c1 * pT2.r1_c4) + (c2 * pT2.r2_c4) + (c3 * pT2.r3_c4) + pT1.r1_c4;
    c1 = pT1.r2_c1;
    c2 = pT1.r2_c2;
    c3 = pT1.r2_c3;
    pT1.r2_c1 = (c1 * pT2.r1_c1) + (c2 * pT2.r2_c1) + (c3 * pT2.r3_c1);
    pT1.r2_c2 = (c1 * pT2.r1_c2) + (c2 * pT2.r2_c2) + (c3 * pT2.r3_c2);
    pT1.r2_c3 = (c1 * pT2.r1_c3) + (c2 * pT2.r2_c3) + (c3 * pT2.r3_c3);
    pT1.r2_c4 = (c1 * pT2.r1_c4) + (c2 * pT2.r2_c4) + (c3 * pT2.r3_c4) + pT1.r2_c4;
    c1 = pT1.r3_c1;
    c2 = pT1.r3_c2;
    c3 = pT1.r3_c3;
    pT1.r3_c1 = (c1 * pT2.r1_c1) + (c2 * pT2.r2_c1) + (c3 * pT2.r3_c1);
    pT1.r3_c2 = (c1 * pT2.r1_c2) + (c2 * pT2.r2_c2) + (c3 * pT2.r3_c2);
    pT1.r3_c3 = (c1 * pT2.r1_c3) + (c2 * pT2.r2_c3) + (c3 * pT2.r3_c3);
    pT1.r3_c4 = (c1 * pT2.r1_c4) + (c2 * pT2.r2_c4) + (c3 * pT2.r3_c4) + pT1.r3_c4;
    return pT1;
  };

  static inline ALTransform operator* (const ALTransform& pT1,const ALTransform& pT2)
  {
    ALTransform t;
    float c1 = pT1.r1_c1;
    float c2 = pT1.r1_c2;
    float c3 = pT1.r1_c3;
    t.r1_c1 = (c1 * pT2.r1_c1) + (c2 * pT2.r2_c1) + (c3 * pT2.r3_c1);
    t.r1_c2 = (c1 * pT2.r1_c2) + (c2 * pT2.r2_c2) + (c3 * pT2.r3_c2);
    t.r1_c3 = (c1 * pT2.r1_c3) + (c2 * pT2.r2_c3) + (c3 * pT2.r3_c3);
    t.r1_c4 = (c1 * pT2.r1_c4) + (c2 * pT2.r2_c4) + (c3 * pT2.r3_c4) + pT1.r1_c4;
    c1 = pT1.r2_c1;
    c2 = pT1.r2_c2;
    c3 = pT1.r2_c3;
    t.r2_c1 = (c1 * pT2.r1_c1) + (c2 * pT2.r2_c1) + (c3 * pT2.r3_c1);
    t.r2_c2 = (c1 * pT2.r1_c2) + (c2 * pT2.r2_c2) + (c3 * pT2.r3_c2);
    t.r2_c3 = (c1 * pT2.r1_c3) + (c2 * pT2.r2_c3) + (c3 * pT2.r3_c3);
    t.r2_c4 = (c1 * pT2.r1_c4) + (c2 * pT2.r2_c4) + (c3 * pT2.r3_c4) + pT1.r2_c4;
    c1 = pT1.r3_c1;
    c2 = pT1.r3_c2;
    c3 = pT1.r3_c3;
    t.r3_c1 = (c1 * pT2.r1_c1) + (c2 * pT2.r2_c1) + (c3 * pT2.r3_c1);
    t.r3_c2 = (c1 * pT2.r1_c2) + (c2 * pT2.r2_c2) + (c3 * pT2.r3_c2);
    t.r3_c3 = (c1 * pT2.r1_c3) + (c2 * pT2.r2_c3) + (c3 * pT2.r3_c3);
    t.r3_c4 = (c1 * pT2.r1_c4) + (c2 * pT2.r2_c4) + (c3 * pT2.r3_c4) + pT1.r3_c4;
    return t;
  };

  static inline void ALTransformPreMultiply(const ALTransform& pT1, ALTransform& pResult)
  {
    float r1 = pResult.r1_c1;
    float r2 = pResult.r2_c1;
    pResult.r1_c1 = (pT1.r1_c1 * r1) + (pT1.r1_c2 * r2) + (pT1.r1_c3 * pResult.r3_c1);
    pResult.r2_c1 = (pT1.r2_c1 * r1) + (pT1.r2_c2 * r2) + (pT1.r2_c3 * pResult.r3_c1);
    pResult.r3_c1 = (pT1.r3_c1 * r1) + (pT1.r3_c2 * r2) + (pT1.r3_c3 * pResult.r3_c1);

    r1 = pResult.r1_c2;
    r2 = pResult.r2_c2;
    pResult.r1_c2 = (pT1.r1_c1 * r1) + (pT1.r1_c2 * r2) + (pT1.r1_c3 * pResult.r3_c2);
    pResult.r2_c2 = (pT1.r2_c1 * r1) + (pT1.r2_c2 * r2) + (pT1.r2_c3 * pResult.r3_c2);
    pResult.r3_c2 = (pT1.r3_c1 * r1) + (pT1.r3_c2 * r2) + (pT1.r3_c3 * pResult.r3_c2);

    r1 = pResult.r1_c3;
    r2 = pResult.r2_c3;
    pResult.r1_c3 = (pT1.r1_c1 * r1) + (pT1.r1_c2 * r2) + (pT1.r1_c3 * pResult.r3_c3);
    pResult.r2_c3 = (pT1.r2_c1 * r1) + (pT1.r2_c2 * r2) + (pT1.r2_c3 * pResult.r3_c3);
    pResult.r3_c3 = (pT1.r3_c1 * r1) + (pT1.r3_c2 * r2) + (pT1.r3_c3 * pResult.r3_c3);

    r1 = pResult.r1_c4;
    r2 = pResult.r2_c4;
    pResult.r1_c4 = (pT1.r1_c1 * r1) + (pT1.r1_c2 * r2) + (pT1.r1_c3 * pResult.r3_c4) + pT1.r1_c4;
    pResult.r2_c4 = (pT1.r2_c1 * r1) + (pT1.r2_c2 * r2) + (pT1.r2_c3 * pResult.r3_c4) + pT1.r2_c4;
    pResult.r3_c4 = (pT1.r3_c1 * r1) + (pT1.r3_c2 * r2) + (pT1.r3_c3 * pResult.r3_c4) + pT1.r3_c4;
  };

  static inline Position3D operator* (const ALTransform& pT,const Position3D& pPos)
  {
    Position3D result;
    result.x = (pT.r1_c1 * pPos.x) + (pT.r1_c2 * pPos.y) + (pT.r1_c3 * pPos.z) + pT.r1_c4;
    result.y = (pT.r2_c1 * pPos.x) + (pT.r2_c2 * pPos.y) + (pT.r2_c3 * pPos.z) + pT.r2_c4;
    result.z = (pT.r3_c1 * pPos.x) + (pT.r3_c2 * pPos.y) + (pT.r3_c3 * pPos.z) + pT.r3_c4;
    return result;
  };

  static inline ALTransform& operator+= (ALTransform& pT,const Position3D& pPos)
  {
    pT.r1_c4 += pPos.x;
    pT.r2_c4 += pPos.y;
    pT.r3_c4 += pPos.z;
    return pT;
  };

  /**
   * Extracts the position coordinates from a Matrix
   * into a Position3D struct
   * @param pTransform The transform form which you want to extract the coordinates.
   */
  static inline Position3D ALTransformToPosition3D(const ALTransform& pTransform)
  {
    Position3D pos3D;
    pos3D.x = pTransform.r1_c4;
    pos3D.y = pTransform.r2_c4;
    pos3D.z = pTransform.r3_c4;
    return pos3D;
  };

  /**
   * Creates a 4*4 Homogenous Matrix from a rotation
   * about the X axis.
   * @param pTheta The rotation
   */
  static inline ALTransform ALTransformFromRotX(const float pTheta)
  {
    float c = inlineCos(pTheta);
    float s = inlineSin(pTheta);
    ALTransform T = IDENTITY_TRANSFORM();
    T.r2_c2 = c;
    T.r2_c3 = -s;
    T.r3_c2 = s;
    T.r3_c3 = c;
    return T;
  };

  /**
   * Creates a 4*4 Homogenous Matrix from a rotation
   * about the Y axis
   * @param pTheta The rotation
   */
  static inline ALTransform ALTransformFromRotY (const float pTheta)
  {
    float c = inlineCos(pTheta);
    float s = inlineSin(pTheta);
    ALTransform T = IDENTITY_TRANSFORM();
    T.r1_c1 = c;
    T.r1_c3 = s;
    T.r3_c1 = -s;
    T.r3_c3 = c;
    return T;
  };

  /**
   * Creates a 4*4 Homogenous Matrix from a rotation
   *  about the Z axis
   * @param pTheta The rotation
   */
  static inline ALTransform ALTransformFromRotZ (const float pTheta)
  {
    float c = inlineCos(pTheta);
    float s = inlineSin(pTheta);
    ALTransform T = IDENTITY_TRANSFORM();
    T.r1_c1 = c;
    T.r1_c2 = -s;
    T.r2_c1 = s;
    T.r2_c2 = c;
    return T;
  };

  /**
   * Creates a 4*4 transform matrix from XYZ coordinates
   * indicating the translational offsets.
   */
  static inline ALTransform ALTransformFromPosition(const Position3D& pPosition)
  {
    ALTransform T = IDENTITY_TRANSFORM();
    T.r1_c4 = pPosition.x;
    T.r2_c4 = pPosition.y;
    T.r3_c4 = pPosition.z;
    return T;
  };

  /**
   * Creates a 4*4 transform matrix from XYZ coordinates
   * indicating the translational offsets.
   */
  static inline ALTransform ALTransformFromPosition(const float x, float y, float z)
  {
    ALTransform T = IDENTITY_TRANSFORM();
    T.r1_c4 = x;
    T.r2_c4 = y;
    T.r3_c4 = z;
    return T;
  };

  /**
   * Creates a 4*4 Homogenous Matrix from a Roll, Pitch and yaw Angle in radian
   * @param pRotation Rotation structure
   */
  static inline ALTransform ALTransformFromRotation(const Rotation3D& pRotation)
  {
    ALTransform T = IDENTITY_TRANSFORM();
    T = ALTransformFromRotZ(pRotation.wz);
    T *= ALTransformFromRotY(pRotation.wy);
    T *= ALTransformFromRotX(pRotation.wx);
    return T;
  };

  /**
   * Return Rotation3D (Roll, Pitch, Yaw) corresponding to the rotational
   * part of the 4*4 Homogenous Matrix pT
   * @param pT 4*4 Homogenous Matrix pT
   */
  static inline Rotation3D RotationFromALTransform(const ALTransform& pT)
  {
    Rotation3D R;
    R.wz = atan2(pT.r2_c1,pT.r1_c1);
    float sy = inlineSin(R.wz);
    float cy = inlineCos(R.wz);
    R.wy = atan2(-pT.r3_c1, cy*pT.r1_c1+sy*pT.r2_c1);
    R.wx = atan2(sy*pT.r1_c3-cy*pT.r2_c3, cy*pT.r2_c2-sy*pT.r1_c2);
    return R;
  };

  /**
   * Return Rotation3D (Roll, Pitch, Yaw) corresponding to the rotational
   * part of the 4*4 Homogenous Matrix pT
   * @param pT 4*4 Homogenous Matrix pT
   */
  static inline Position6D Position6DFromALTransform(const ALTransform& pT)
  {
    Position6D R;
    R.x = pT.r1_c4;
    R.y = pT.r2_c4;
    R.z = pT.r3_c4;
    R.wz = atan2(pT.r2_c1,pT.r1_c1);
    float sy = inlineSin(R.wz);
    float cy = inlineCos(R.wz);
    R.wy = atan2(-pT.r3_c1, cy*pT.r1_c1+sy*pT.r2_c1);
    R.wx = atan2(sy*pT.r1_c3-cy*pT.r2_c3, cy*pT.r2_c2-sy*pT.r1_c2);
    return R;
  };

  /**
   * Creates a 4*4 transform matrix from a rotation(RPY) and a position
   * indicating the translational offsets.
   */
  static inline ALTransform ALTransformFromPosition6D(const Position6D& pPosition6D)
  {
    ALTransform m;
    m = ALTransformFromRotZ(pPosition6D.wz);
    m *= ALTransformFromRotY(pPosition6D.wy);
    m *= ALTransformFromRotX(pPosition6D.wx);

    m.r1_c4 = pPosition6D.x;
    m.r2_c4 = pPosition6D.y;
    m.r3_c4 = pPosition6D.z;
    return m;
  };

  static inline ALTransform ALTransformFromVelocity6D(const Velocity6D& pVel)
  {
    ALTransform m;
    m.r1_c1 = 1.0f;
    m.r2_c1 = pVel.wzd;
    m.r3_c1 = -pVel.wyd;

    m.r1_c2 = -pVel.wzd;
    m.r2_c2 = 1.0f;
    m.r3_c2 = -pVel.wxd;

    m.r1_c3 = pVel.wyd;
    m.r2_c3 = pVel.wxd;
    m.r3_c3 = 1.0f;

    m.r1_c4 = pVel.xd;
    m.r2_c4 = pVel.yd;
    m.r3_c4 = pVel.zd;
    return m;
  };

  ///**
  // * Creates a 4*4 transform matrix from a Column vector
  // * indicating the translational offsets
  //*/
  //static inline ALTransform ALTransformFromPosition(const ColumnVector& pXYZ) {
  //  return ALTransformFromPosition(pXYZ(1),pXYZ(2),pXYZ(3)) ;
  //};

  static inline void ALTransformInvertInPlace(ALTransform& pT)
  {
    float tmp0;
    tmp0 = pT.r1_c2;
    pT.r1_c2 = pT.r2_c1;
    pT.r2_c1 = tmp0;

    tmp0 = pT.r1_c3;
    pT.r1_c3 = pT.r3_c1;
    pT.r3_c1 = tmp0;

    tmp0 = pT.r2_c3;
    pT.r2_c3 = pT.r3_c2;
    pT.r3_c2 = tmp0;

    tmp0 = -(pT.r1_c1 * pT.r1_c4 + pT.r1_c2 * pT.r2_c4 + pT.r1_c3 * pT.r3_c4);
    float tmp1 = -(pT.r2_c1 * pT.r1_c4 + pT.r2_c2 * pT.r2_c4 + pT.r2_c3 * pT.r3_c4);
    pT.r3_c4 = -(pT.r3_c1 * pT.r1_c4 + pT.r3_c2 * pT.r2_c4 + pT.r3_c3 * pT.r3_c4);
    pT.r2_c4 = tmp1;
    pT.r1_c4 = tmp0;
  };

  static inline ALTransform ALTransformDiff(const ALTransform& pT1, const ALTransform &pT2)
  {
    ALTransform result;
    result.r1_c1 = 1.0f;
    result.r2_c2 = 1.0f;
    result.r3_c3 = 1.0f;

    result.r1_c4 = pT2.r1_c4 - pT1.r1_c4;
    result.r2_c4 = pT2.r2_c4 - pT1.r2_c4;
    result.r3_c4 = pT2.r3_c4 - pT1.r3_c4;

    result.r2_c3 = 0.5f * ( ((pT1.r2_c1 * pT2.r3_c1) - (pT1.r3_c1 * pT2.r2_c1)) + ((pT1.r2_c2 * pT2.r3_c2) - (pT1.r3_c2 * pT2.r2_c2)) + ((pT1.r2_c3 * pT2.r3_c3) - (pT1.r3_c3 * pT2.r2_c3)) );
    result.r3_c2 = -result.r2_c3;

    result.r1_c3 = 0.5f * ( ((pT1.r3_c1 * pT2.r1_c1) - (pT1.r1_c1 * pT2.r3_c1)) + ((pT1.r3_c2 * pT2.r1_c2) - (pT1.r1_c2 * pT2.r3_c2)) + ((pT1.r3_c3 * pT2.r1_c3) - (pT1.r1_c3 * pT2.r3_c3)) );
    result.r3_c1 = -result.r1_c3;

    result.r2_c1 = 0.5f * ( ((pT1.r1_c1 * pT2.r2_c1) - (pT1.r2_c1 * pT2.r1_c1)) + ((pT1.r1_c2 * pT2.r2_c2) - (pT1.r2_c2 * pT2.r1_c2)) + ((pT1.r1_c3 * pT2.r2_c3) - (pT1.r2_c3 * pT2.r1_c3)) );
    result.r1_c2 = - result.r2_c1;
    return result;
  };

  /**
   * Return a 6 differential motion require to move from a 4*4 Homogenous transform matrix Current to
   * a 4*4 Homogenous transform matrix target
   * @param  pCurrent 4*4 Homogenous transform matrix
   * @param  pTarget  4*4 Homogenous transform matrix
   */
  static inline Velocity6D ALTransformDiffToVelocity(const ALTransform& pCurrent, const ALTransform &pTarget)
  {
    Velocity6D result;
    result.xd = pTarget.r1_c4 - pCurrent.r1_c4;
    result.yd = pTarget.r2_c4 - pCurrent.r2_c4;
    result.zd = pTarget.r3_c4 - pCurrent.r3_c4;
    result.wxd = 0.5f * ( ((pCurrent.r2_c1 * pTarget.r3_c1) - (pCurrent.r3_c1 * pTarget.r2_c1)) + ((pCurrent.r2_c2 * pTarget.r3_c2) - (pCurrent.r3_c2 * pTarget.r2_c2)) + ((pCurrent.r2_c3 * pTarget.r3_c3) - (pCurrent.r3_c3 * pTarget.r2_c3)) );
    result.wyd = 0.5f * ( ((pCurrent.r3_c1 * pTarget.r1_c1) - (pCurrent.r1_c1 * pTarget.r3_c1)) + ((pCurrent.r3_c2 * pTarget.r1_c2) - (pCurrent.r1_c2 * pTarget.r3_c2)) + ((pCurrent.r3_c3 * pTarget.r1_c3) - (pCurrent.r1_c3 * pTarget.r3_c3)) );
    result.wzd = 0.5f * ( ((pCurrent.r1_c1 * pTarget.r2_c1) - (pCurrent.r2_c1 * pTarget.r1_c1)) + ((pCurrent.r1_c2 * pTarget.r2_c2) - (pCurrent.r2_c2 * pTarget.r1_c2)) + ((pCurrent.r1_c3 * pTarget.r2_c3) - (pCurrent.r2_c3 * pTarget.r1_c3)) );
    return result;
  };

  static inline std::vector<float> ALTransformToVector(const ALTransform& pT)
  {
    std::vector<float> returnVector;
    returnVector.reserve(16);
    returnVector.push_back(pT.r1_c1);
    returnVector.push_back(pT.r1_c2);
    returnVector.push_back(pT.r1_c3);
    returnVector.push_back(pT.r1_c4);

    returnVector.push_back(pT.r2_c1);
    returnVector.push_back(pT.r2_c2);
    returnVector.push_back(pT.r2_c3);
    returnVector.push_back(pT.r2_c4);

    returnVector.push_back(pT.r3_c1);
    returnVector.push_back(pT.r3_c2);
    returnVector.push_back(pT.r3_c3);
    returnVector.push_back(pT.r3_c4);

    returnVector.push_back(0.0f);
    returnVector.push_back(0.0f);
    returnVector.push_back(0.0f);
    returnVector.push_back(1.0f);
    return returnVector;
  };

  /*********************************
   *    OPERATORS <<  FOR COUT     *
   *********************************/

  static inline std::ostream& operator<< (std::ostream& pStream, const ALTransform& pT)
  {
    pStream.setf(std::ios::showpoint | std::ios::left | std::ios::showpos);
    //pStream << std::setw(10) << std::setprecision(6);
    pStream << pT.r1_c1;
    pStream << " ";
    pStream << pT.r1_c2;
    pStream << " ";
    pStream << pT.r1_c3;
    pStream << " ";
    pStream << pT.r1_c4;
    pStream << std::endl;
    pStream << pT.r2_c1;
    pStream << " ";
    pStream << pT.r2_c2;
    pStream << " ";
    pStream << pT.r2_c3;
    pStream << " ";
    pStream << pT.r2_c4;
    pStream << std::endl;
    pStream << pT.r3_c1;
    pStream << " ";
    pStream << pT.r3_c2;
    pStream << " ";
    pStream << pT.r3_c3;
    pStream << " ";
    pStream << pT.r3_c4;
    pStream << std::endl;
    pStream << "0.0 0.0 0.0 1.0";
    pStream << std::endl;
    return pStream;
  };

  static inline std::ostream& operator<< (std::ostream& pStream, const Position2D& p)
  {
    pStream.setf(std::ios::showpoint | std::ios::left | std::ios::showpos);
    pStream << p.x;
    pStream << " ";
    pStream << p.y;
    pStream << std::endl;
    return pStream;
  };

  static inline std::ostream& operator<< (std::ostream& pStream, const Position3D& p)
  {
    pStream.setf(std::ios::showpoint | std::ios::left | std::ios::showpos);
    pStream << "Position3D: {x: ";
    pStream << p.x;
    pStream << ", y:";
    pStream << p.y;
    pStream << ", z:";
    pStream << p.z;
    pStream << "}";
    return pStream;
  };

  static inline std::ostream& operator<< (std::ostream& pStream, const Rotation3D& p)
  {
    pStream.setf(std::ios::showpoint | std::ios::left | std::ios::showpos);
    pStream << p.wx;
    pStream << " ";
    pStream << p.wy;
    pStream << " ";
    pStream << p.wz;
    pStream << std::endl;
    return pStream;
  };

  static inline std::ostream& operator<< (std::ostream& pStream, const Position6D& p)
  {
    pStream.setf(std::ios::showpoint | std::ios::left | std::ios::showpos);
    pStream << "Position6D: {x: ";
    pStream << p.x;
    pStream << ", y:";
    pStream << p.y;
    pStream << ", z:";
    pStream << p.z;
    pStream << ", wx: ";
    pStream << p.wx;
    pStream << ", wy: ";
    pStream << p.wy;
    pStream << ", wz: ";
    pStream << p.wz;
    pStream << "}";
    pStream << std::endl;
    return pStream;
  };

  static inline std::ostream& operator<< (std::ostream& pStream, const Velocity6D& p)
  {
    pStream.setf(std::ios::showpoint | std::ios::left | std::ios::showpos);
    pStream << p.xd;
    pStream << " ";
    pStream << p.yd;
    pStream << " ";
    pStream << p.zd;
    pStream << " ";
    pStream << p.wxd;
    pStream << " ";
    pStream << p.wyd;
    pStream << " ";
    pStream << p.wzd;
    pStream << std::endl;
    return pStream;
  };

}
#endif

