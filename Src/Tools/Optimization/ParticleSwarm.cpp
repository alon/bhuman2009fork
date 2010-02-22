/**
* @file Tools/Optimization/ParticleSwarm.h
* Implementation of a simple partilce swarm optimization algorithm.
* @author Colin Graf
*/

#include <limits>

#include "ParticleSwarm.h"
#include "Tools/Math/Common.h"
#include "Tools/Streams/OutStreams.h"
#include "Tools/Streams/InStreams.h"
#include "Platform/GTAssert.h"

ParticleSwarm::ParticleSwarm(bool minimize, unsigned int particleCount, 
  const double& velocityFactor, const double& bestPositionFactor, 
  const double& globalBestPositionFactor, const double& randomPositionFactor) :
  minimize(minimize), 
  velocityFactor(velocityFactor), bestPositionFactor(bestPositionFactor), 
  globalBestPositionFactor(globalBestPositionFactor), randomPositionFactor(randomPositionFactor),
  particles(particleCount) {}

ParticleSwarm::~ParticleSwarm()
{
  if(!file.empty())
  {
    OutBinaryFile stream(file);
    if(stream.exists())
      stream << *this;
  }
}

void ParticleSwarm::init(const std::string& file, const double values[][3], unsigned int size)
{
  this->file = file;
  limits.resize(size);
  for(unsigned int j = 0; j < size; ++j)
  {
    limits[j].min = limits[j].max = values[j][1];
    limits[j].add(values[j][2]);
    ASSERT(limits[j].isInside(values[j][0])); // start value not in value range!
  }

  if(!file.empty())
  {
    InBinaryFile stream(file);
    if(stream.exists())
    {
      //unsigned int particleCount = particles.size();
      stream >> *this;
      //ASSERT(particles.size() == particleCount);
      ASSERT(particles[0].position.size() == size);
      ASSERT(particles[0].velocity.size() == size);
      ASSERT(particles[0].bestPosition.size() == size);
      //currentParticleIndex = currentParticleIndex == 0 ? (particles.size() - 1) : (currentParticleIndex - 1);
      for(unsigned int i = 0; i < particles.size(); ++i)
      {
        Particle& particle(particles[i]);
        particle.bestFitness = minimize ? std::numeric_limits<double>::max() : std::numeric_limits<double>::min();
        particle.rated = false;
      }
      bestParticleIndex = 0;
      currentParticleIndex = particles.size() - 1;
      return;
    }
  }

  for(unsigned int i = 0; i < particles.size(); ++i)
  {
    Particle& particle(particles[i]);
    particle.position.resize(size);
    particle.velocity.resize(size);
    for(unsigned int j = 0; j < size; ++j)
    {
      double random = i == 0 ? 0. : randomDouble(); // the first particles consists of the given start values
      particle.position[j] = values[j][0] + (random >= 0.5 ? 
        (values[j][1] - values[j][0]) * (random - 0.5) * 2. : (values[j][2] - values[j][0]) * random * 2.);
    }
    particle.bestPosition = particle.position;
    particle.bestFitness = minimize ? std::numeric_limits<double>::max() : std::numeric_limits<double>::min();
    particle.rated = false;
  }
  bestParticleIndex = 0;
  currentParticleIndex = particles.size() - 1;
}

void ParticleSwarm::getNextValues(double*& values)
{
  currentParticleIndex = (currentParticleIndex + 1) % particles.size();
  updateParticle(particles[currentParticleIndex]);
  values = &particles[currentParticleIndex].position[0];
}

void ParticleSwarm::setFitness(const double& fitness)
{
  Particle& particle(particles[currentParticleIndex]);
  if((minimize && fitness < particle.bestFitness) || (!minimize && fitness > particle.bestFitness))
  {
    particle.bestFitness = fitness;
    particle.bestPosition = particle.position;
  }
  particle.rated = true;

  Particle& bestParticle(particles[bestParticleIndex]);
  if((minimize && fitness < bestParticle.bestFitness) || (!minimize && fitness > bestParticle.bestFitness))
    bestParticleIndex = currentParticleIndex;
}

bool ParticleSwarm::isRated()
{
  return particles[currentParticleIndex].rated;
}

void ParticleSwarm::getBestValues(double*& values)
{
  values = &particles[bestParticleIndex].bestPosition[0];
}

double ParticleSwarm::getBestFitness()
{
  return particles[bestParticleIndex].bestFitness;
}

void ParticleSwarm::updateParticle(Particle& particle)
{
  if(!particle.rated)
    return;
  Particle& bestParticle(particles[bestParticleIndex]);
  for(unsigned int i = 0; i < particle.position.size(); ++i)
  {
    particle.velocity[i] = velocityFactor * particle.velocity[i]
      + bestPositionFactor * randomDouble() * (particle.bestPosition[i] - particle.position[i])
      + globalBestPositionFactor * randomDouble() * (bestParticle.bestPosition[i] - particle.position[i])
      + randomPositionFactor * randomDouble() * ((limits[i].min + randomDouble() * (limits[i].max - limits[i].min)) - particle.position[i]);
    if(limits[i].isInside(particle.position[i] + particle.velocity[i]))
      particle.position[i] += particle.velocity[i];
    else
    {
      particle.position[i] = limits[i].limit(particle.position[i] + particle.velocity[i]);
      particle.velocity[i] = 0.;
    }
  }
  particle.rated = false;
}
