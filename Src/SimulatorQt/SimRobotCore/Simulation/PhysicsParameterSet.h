/**
 * @file PhysicsParameterSet.h
 * 
 * Definition of class PhysicsParameterSet
 *
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 */ 

#ifndef PHYSICSPARAMETERSET_H_
#define PHYSICSPARAMETERSET_H_


class AutomaticObjectDisablingParameters
{
public:
  AutomaticObjectDisablingParameters();
  double linearVelocityThreshold;
  double angularVelocityThreshold;
  int physicsSimulationSteps;
  double physicsSimulationTime;
};


class PhysicsParameterSet  
{
public:
  /** Constructor */
  PhysicsParameterSet();

  /** 
   * Gets the gravity of the physical world
   * @return The gravity
   */
  double getGravity() const {return gravity;}
  /** 
   * Gets the error reduction parameter for ODE
   * @return The error reduction parameter
   */
  double getErrorReductionParameter() const {return erp;}
  /** 
   * Gets the constraint force mixing parameter for ODE
   * @return The constarint force mixing parameter
   */
  double getConstraintForceMixing() const {return cfm;}
  /** 
   * Gets the default friction coefficient
   * @return The friction coefficient
   */
  double getDefaultFrictionCoefficient() const {return defaultFrictionCoefficient;}
  /** 
   * Gets the default rolling friction coefficient
   * @return The rolling friction coefficient
   */
  double getDefaultRollingFrictionCoefficient() const {return defaultRollingFrictionCoefficient;}
  /** 
   * Sets the gravity of the physical world
   * @return The gravity
   */
  void setGravity(double gravity) {this->gravity = gravity;}
  /** 
   * Sets the error reduction parameter for ODE
   * @param erp The error reduction parameter
   */
  void setErrorReductionParameter(double erp) {this->erp = erp;}
  /** 
   * Sets the constraint force mixing parameter for ODE
   * @param cfm The constarint force mixing parameter
   */
  void setConstraintForceMixing(double cfm) {this->cfm = cfm;}
  /** 
   * Sets the default friction coefficient
   * @param defaultFrictionCoeff The default friction coefficient
   */
  void setDefaultFrictionCoefficient(double defaultFrictionCoeff)
    {this->defaultFrictionCoefficient = defaultFrictionCoeff;}
  /** 
   * Sets the default rolling friction coefficient
   * @param defaultRollFrictionCoeff The default rolling friction coefficient
   */
  void setDefaultRollingFrictionCoefficient(double defaultRollFrictionCoeff)
    {this->defaultRollingFrictionCoefficient = defaultRollFrictionCoeff;}
  /**
   * Returns the parameter for restitution behavior of the surfaces for collision contacts
   * @return the parameter for restitution behavior
   */
  double getDefaultRestitutionCoefficient() const {return defaultRestitutionCoefficient;}
  /**
   * Returns the parameter for slip behavior of the surfaces for collision contacts
   * @return the parameter for slip behavior
   */
  double getSlipFactor() const {return slipFactor;}
  /**
   * Sets the parameter for restitution behavior of the surfaces for collision contacts
   * Clipps value for range [0.0...1.0]
   * @param newRestitutionCoefficient the parameter for restitution behavior
   */
  void setDefaultRestitutionCoefficient(double newRestitutionCoefficient);
  /**
   * Sets the parameter for slip behavior of the surfaces for collision contacts.
   * Clipps value for range [0.0...]
   * @param newSlipFactor the parameter for slip behavior
   */
  void setSlipFactor(double newSlipFactor);
  /**
   * Adds a flag for surface contact mode parameters
   * @param newMode the mode to add
   */
  void addContactMode(int newMode);
  /** 
   * Returns the mode for surface contact generation used by collision callback
   * @return the contact mode
   */
  int getContactMode() const {return contactModeFlags;}
  
  double getContactSoftnessERP() const {return contactSoftERP;}
  double getContactSoftnessCFM() const {return contactSoftCFM;}
  void setContactSoftnessERP(double newValue) {contactSoftERP = newValue;}
  void setContactSoftnessCFM(double newValue) {contactSoftCFM = newValue;}
  double getVelocityThresholdForElasticity() const {return velocityThresholdForElasticity;}
  void setVelocityThresholdForElasticity(double newValue) {velocityThresholdForElasticity = newValue;}

private:
  /** The gravity of the scene. The resulting force is oriented along the z axis. */
  double gravity;
  /** The error reduction parameter. Can be used for tweaking ODE */
  double erp;
  /** The constraint force mixing parameter. Can be used for tweaking ODE */
  double cfm;
  /** The default friction coefficient */
  double defaultFrictionCoefficient;
  /** The default rolling friction coefficient */
  double defaultRollingFrictionCoefficient;
  /** The parameter to control the default restitution behavior of the surfaces for collision contacts [0.0...1.0] */
  double defaultRestitutionCoefficient;
  /** The parameter to control the default slip behavior of the surfaces for collision contacts [0.0...] */
  double slipFactor;
  /** The flag to determine the used parameters for contact generation */
  int contactModeFlags;
  double contactSoftERP;
  double contactSoftCFM;
  double velocityThresholdForElasticity;
};


#include <ode/ode.h>
#ifdef __GNUC__
#pragma GCC system_header
#endif
const double dInfinityWithoutWarning = dInfinity;



#endif // PHYSICSPARAMETERSET_H_
