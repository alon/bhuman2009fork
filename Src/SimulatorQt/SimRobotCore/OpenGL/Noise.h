/**
* @file Noise.h
* 
* Definition of a generation class for static noise and perlin noise (1d/2d/3d) textures.
* Perlin Noise algorithmen and implementation is based on
* Ken Perlin's C implementation
* (available from http://www.texturingandmodeling.com/CODE/PERLIN/PERLIN.C)
* with additions of John Kessenich
* (available from http://developer.3dlabs.com)
*
* @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
*/ 
#ifndef NOISE_H_
#define NOISE_H_

#define MAXB 0x100 // 256
#define N 0x1000
#define NP 12   // 2^N
#define NM 0xfff

#define s_curve_pn(t) ( t * t * (3. - 2. * t) )
#define lerp_pn(t, a, b) ( a + t * (b - a) )
#define setup_pn(i, b0, b1, r0, r1)\
        t = vec[i] + N;\
        b0 = ((int)t) & BM;\
        b1 = (b0+1) & BM;\
        r0 = t - (int)t;\
        r1 = r0 - 1.;
#define at2_pn(rx, ry) ( rx * q[0] + ry * q[1] )
#define at3_pn(rx, ry, rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

class Noise
{
protected:
  int texSize;
  unsigned int seed;
  int start;
  int B;
  int BM;
  unsigned char* NoiseTexPtr; // GLubyte

  int p[MAXB + MAXB + 2];
  double g3[MAXB + MAXB + 2][3];
  double g2[MAXB + MAXB + 2][2];
  double g1[MAXB + MAXB + 2];

  void SetNoiseFrequency(int frequency);
  double noise1(double arg);
  double noise2(double vec[2]);
  double noise3(double vec[3]);

  void normalize2(double v[2]);
  void normalize3(double v[3]);
  void initNoise();
  double PerlinNoise1D(double x, double alpha, double beta, int n);
  double PerlinNoise2D(double x, double y, double alpha, double beta, int n);
  double PerlinNoise3D(double x, double y, double z, double alpha, double beta, int n);

public:
  Noise();
  ~Noise();

  unsigned char* getTexPtr() // returning GLubyte
  { return NoiseTexPtr; };

  void make3DNoiseTexture(const int size);
  void make3DPachNoiseTexture(const int size);
};

#endif // NOISE_H_
