/**
 * @file PhysicsParameterSet.cpp
 * 
 * Implementation of class PhysicsParameterSet
 *
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 */

#include "PhysicsParameterSet.h"
#include <ode/ode.h>

AutomaticObjectDisablingParameters::AutomaticObjectDisablingParameters() : 
    linearVelocityThreshold(dInfinityWithoutWarning), 
    angularVelocityThreshold(dInfinityWithoutWarning),
    physicsSimulationSteps(-1), 
    physicsSimulationTime(-1.0)
{}

PhysicsParameterSet::PhysicsParameterSet() : gravity(-1.0), erp(0.02), cfm(0.00001), 
                                             defaultFrictionCoefficient(1.0), defaultRollingFrictionCoefficient(0.0),
                                             defaultRestitutionCoefficient(0.0), slipFactor(0.0),
                                             contactModeFlags(0),contactSoftERP(0.2), contactSoftCFM(0.005),
                                             velocityThresholdForElasticity(0.001)
{}

void PhysicsParameterSet::setDefaultRestitutionCoefficient(double newRestitutionCoefficient)
{
  if(newRestitutionCoefficient < 0.0)
    defaultRestitutionCoefficient = 0.0;
  else if(newRestitutionCoefficient > 1.0)
    defaultRestitutionCoefficient = 1.0;
  else
    defaultRestitutionCoefficient = newRestitutionCoefficient;
}

void PhysicsParameterSet::setSlipFactor(double newSlipFactor)
{
  if(newSlipFactor < 0.0)
    slipFactor = 0.0;
  else
    slipFactor = newSlipFactor;
}

void PhysicsParameterSet::addContactMode(int newMode)
{
  contactModeFlags = contactModeFlags | newMode;
}
