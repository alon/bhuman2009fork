/**
* @file GaussianDistribution.cpp
* 
* Implementation of class GaussianDistribution
*
* @author <a href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</a>
*/

#include "Probabilistics.h"
#include "GaussianDistribution.h"


double GaussianDistribution::distanceTo(const GaussianDistribution& other) const
{
  double diff(mean - other.mean);
  double invVariance = 1.0/variance;
  return diff*(invVariance*diff);
}

double GaussianDistribution::probabilityAt(double p) const
{
  double diff(p - mean);
  double invVariance = 1.0/variance;
  double exponent(diff*invVariance*diff);
  double probability(1.0 / (sqrt(pi2) * sqrt(variance) ) );
  probability *= exp(-0.5*exponent);
  return probability;
}

void GaussianDistribution::generateDistributionFromMeasurements(double* x, int numOfX)
{
  if(numOfX < 2)
    return;
  mean = 0.0;
  for(int i=0; i<numOfX; i++)
    mean += x[i];
  mean /= numOfX;
  variance = 0.0;
  for(int i=0; i<numOfX; i++)
    variance += (x[i]-mean)*(x[i]-mean);
  variance *= 1.0 / (numOfX-1);
}
