/**
* @file Tools/Optimization/ParticleSwarm.h
* Implementation of a simple partilce swarm optimization algorithm.
* @author Colin Graf
*/

#ifndef ParticleSwarm_H
#define ParticleSwarm_H

#include <vector>
#include <string>

#include "Tools/Range.h"
#include "Tools/Streams/Streamable.h"

class ParticleSwarm : public Streamable
{
public:
  ParticleSwarm(bool minimize = true, unsigned int particleCount = 10,
    const double& velocityFactor = 0.6, const double& bestPositionFactor = 2., 
    const double& globalBestPositionFactor = 2., const double& randomPositionFactor = 0.01);
  ~ParticleSwarm();

  template <int size> void init(const std::string& file, const double (&values)[size][3]) {init(file, values, size);}

  void getNextValues(double*& values);

  void setFitness(const double& fitness);

  bool isRated();

  void getBestValues(double*& values);

  double getBestFitness();

private:
  class Particle : public Streamable
  {
  public:
    Particle() {}

    std::vector<double> position;
    std::vector<double> velocity;

    std::vector<double> bestPosition;
    double bestFitness;

    bool rated;

  private:
    virtual void serialize(In* in, Out* out)
    {
      STREAM_REGISTER_BEGIN();
        STREAM_VECTOR(position);
        STREAM_VECTOR(velocity);
        STREAM_VECTOR(bestPosition);
        STREAM(bestFitness);
        STREAM(rated);
      STREAM_REGISTER_FINISH();
    }
  };

  bool minimize;
  double velocityFactor;
  double bestPositionFactor;
  double globalBestPositionFactor;
  double randomPositionFactor;
  std::string file;
  std::vector<Range<double> > limits;

  std::vector<Particle> particles;
  unsigned int bestParticleIndex;
  unsigned int currentParticleIndex;

  void init(const std::string& file, const double values[][3], unsigned int size);
  void updateParticle(Particle& particle);

  virtual void serialize(In* in, Out* out)
  {
    STREAM_REGISTER_BEGIN();
      STREAM_VECTOR(particles);
      STREAM(bestParticleIndex);
      STREAM(currentParticleIndex);
    STREAM_REGISTER_FINISH();
  }
};

#endif
