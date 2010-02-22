#include "Noise.h"
#include <cmath>
#include <stdlib.h>
#include <vector>

Noise::Noise()
{
  NoiseTexPtr = 0;
}

Noise::~Noise()
{
  if(NoiseTexPtr)
  {
    delete [] NoiseTexPtr;
    NoiseTexPtr = 0;
  }
}

void Noise::SetNoiseFrequency(int frequency)
{
  start = 1;
  B = frequency;
  BM = B-1;
}

double Noise::noise1(double arg)
{
  int bx0, bx1;
  double rx0, rx1, sx, t, u, v, vec[1];

  vec[0] = arg;
  if (start)
  {
    start = 0;
    initNoise();
  }

  setup_pn(0, bx0, bx1, rx0, rx1);

  sx = s_curve_pn(rx0);
  u = rx0 * g1[p[bx0]];
  v = rx1 * g1[p[bx1]];

  return(lerp_pn(sx, u, v));
}

double Noise::noise2(double vec[2])
{
  int bx0, bx1, by0, by1, b00, b10, b01, b11;
  double rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
  int i, j;

  if (start)
  {
    start = 0;
    initNoise();
  }

  setup_pn(0, bx0, bx1, rx0, rx1);
  setup_pn(1, by0, by1, ry0, ry1);

  i = p[bx0];
  j = p[bx1];

  b00 = p[i + by0];
  b10 = p[j + by0];
  b01 = p[i + by1];
  b11 = p[j + by1];

  sx = s_curve_pn(rx0);
  sy = s_curve_pn(ry0);

  q = g2[b00]; u = at2_pn(rx0, ry0);
  q = g2[b10]; v = at2_pn(rx1, ry0);
  a = lerp_pn(sx, u, v);

  q = g2[b01]; u = at2_pn(rx0, ry1);
  q = g2[b11]; v = at2_pn(rx1, ry1);
  b = lerp_pn(sx, u, v);

  return lerp_pn(sy, a, b);
}

double Noise::noise3(double vec[3])
{
  int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
  double rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
  int i, j;

  if (start)
  {
    start = 0;
    initNoise();
  }

  setup_pn(0, bx0, bx1, rx0, rx1);
  setup_pn(1, by0, by1, ry0, ry1);
  setup_pn(2, bz0, bz1, rz0, rz1);

  i = p[bx0];
  j = p[bx1];

  b00 = p[i + by0];
  b10 = p[j + by0];
  b01 = p[i + by1];
  b11 = p[j + by1];

  t  = s_curve_pn(rx0);
  sy = s_curve_pn(ry0);
  sz = s_curve_pn(rz0);

  q = g3[b00 + bz0]; u = at3_pn(rx0, ry0, rz0);
  q = g3[b10 + bz0]; v = at3_pn(rx1, ry0, rz0);
  a = lerp_pn(t, u, v);

  q = g3[b01 + bz0]; u = at3_pn(rx0, ry1, rz0);
  q = g3[b11 + bz0]; v = at3_pn(rx1, ry1, rz0);
  b = lerp_pn(t, u, v);

  c = lerp_pn(sy, a, b);

  q = g3[b00 + bz1]; u = at3_pn(rx0, ry0, rz1);
  q = g3[b10 + bz1]; v = at3_pn(rx1, ry0, rz1);
  a = lerp_pn(t, u, v);

  q = g3[b01 + bz1]; u = at3_pn(rx0, ry1, rz1);
  q = g3[b11 + bz1]; v = at3_pn(rx1, ry1, rz1);
  b = lerp_pn(t, u, v);

  d = lerp_pn(sy, a, b);

  return lerp_pn(sz, c, d);
}

void Noise::normalize2(double v[2])
{
  double s;

  s = sqrt(v[0] * v[0] + v[1] * v[1]);
  v[0] = v[0] / s;
  v[1] = v[1] / s;
}

void Noise::normalize3(double v[3])
{
  double s;

  s = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  v[0] = v[0] / s;
  v[1] = v[1] / s;
  v[2] = v[2] / s;
}

void Noise::initNoise()
{
  int i, j, k;

  srand(seed);
  for (i = 0; i < B; i++)
  {
    p[i] = i;
    g1[i] = (double)((rand() % (B + B)) - B) / B;

    for (j = 0; j < 2; j++)
      g2[i][j] = (double)((rand() % (B + B)) - B) / B;
    normalize2(g2[i]);

    for (j = 0; j < 3; j++)
      g3[i][j] = (double)((rand() % (B + B)) - B) / B;
    normalize3(g3[i]);
  }

  while (--i)
  {
    k = p[i];
    p[i] = p[j = rand() % B];
    p[j] = k;
  }

  for (i = 0; i < B + 2; i++)
  {
    p[B + i] = p[i];
    g1[B + i] = g1[i];
    for (j = 0; j < 2; j++)
      g2[B + i][j] = g2[i][j];
    for (j = 0; j < 3; j++)
      g3[B + i][j] = g3[i][j];
  }
}

double Noise::PerlinNoise1D(double x,double alpha,double beta,int n)
{
  int i;
  double val,sum = 0;
  double p,scale = 1;

  p = x;
  for (i = 0; i < n; i++)
  {
    val = noise1(p);
    sum += val / scale;
    scale *= alpha;
    p *= beta;
  }
  return(sum);
}

double Noise::PerlinNoise2D(double x, double y, double alpha, double beta, int n)
{
  int i;
  double val, sum = 0;
  double p[2], scale = 1;

  p[0] = x;
  p[1] = y;
  for (i = 0; i < n; i++)
  {
    val = noise2(p);
    sum += val / scale;
    scale *= alpha;
    p[0] *= beta;
    p[1] *= beta;
  }
  return(sum);
}

double Noise::PerlinNoise3D(double x, double y, double z, double alpha, double beta, int n)
{
  int i;
  double val,sum = 0;
  double p[3],scale = 1;

  p[0] = x;
  p[1] = y;
  p[2] = z;
  for (i = 0; i < n; i++)
  {
    val = noise3(p);
    sum += val / scale;
    scale *= alpha;
    p[0] *= beta;
    p[1] *= beta;
    p[2] *= beta;
  }
  return(sum);
}

void Noise::make3DNoiseTexture(const int size)
{
  texSize = size;
  int f, i, j, k, inc;
  int startFrequency = 64;
  int numOctaves = 4;
  double ni[3];
  double inci, incj, inck;
  int frequency = startFrequency;
  unsigned char* ptr;
  double amp = 0.5;

  NoiseTexPtr = new unsigned char[texSize * texSize * texSize * 4];
  for (f = 0, inc = 0; f < numOctaves; ++f, frequency /= 2, ++inc)
  {
    SetNoiseFrequency(frequency);
    ptr = NoiseTexPtr;
    ni[0] = ni[1] = ni[2] = 0;

    inci = 1.0 / (texSize / frequency);
    for (i = 0; i < texSize; ++i, ni[0] += inci)
    {
      incj = 1.0 / (texSize / frequency);
      for (j = 0; j < texSize; ++j, ni[1] += incj)
      {
        inck = 1.0 / (texSize / frequency);
        for (k = 0; k < texSize; ++k, ni[2] += inck, ptr += 4)
          *(ptr + inc) = (unsigned char) (((noise3(ni) + 1.0) * amp) * 128.0);
      }
    }
  }
}

void Noise::make3DPachNoiseTexture(const int size)
{
  texSize = size;
  int f, i, j, k, inc;
  int numOctaves = 4;
  double ni[3];
  double inci, incj, inck;
  int startFrequency = 32;
  unsigned char* ptr;
  seed = 30756;

  // create array
  unsigned int numTexels = texSize * texSize * texSize * 4;
  NoiseTexPtr = new unsigned char[numTexels];

  // clear array
  for(unsigned int i=0; i < numTexels; i++)
    NoiseTexPtr[i] = 0;

  // increase values by multipass frequa
  for(inc = 0; inc < 4; inc++)
  {
    seed++;
    int frequency = startFrequency;
    for (f = 0; f < numOctaves; ++f, frequency *= 2)
    {
      SetNoiseFrequency(frequency);
      ptr = NoiseTexPtr;
      ni[0] = ni[1] = ni[2] = 0;

      inci = 1.0 / (texSize / frequency);
      for (i = 0; i < texSize; ++i, ni[0] += inci)
      {
        incj = 1.0 / (texSize / frequency);
        for (j = 0; j < texSize; ++j, ni[1] += incj)
        {
          inck = 1.0 / (texSize / frequency);
          for (k = 0; k < texSize; ++k, ni[2] += inck, ptr += 4)
            *(ptr + inc) += (unsigned char) ((noise3(ni) + 1.0) * 128.0);
        }
      }
    }
  }
}
