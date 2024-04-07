#ifndef DECAY_TRANSITION_H
#define DECAY_TRANSITION_H

#include "DecayScheme.hh"

using namespace std;
using namespace CLHEP;

class DecayState;

typedef enum {
    GAMMA_TRANSITION, EC_TRANSITION, IC_TRANSITION, BETA_PLUS_TRANSITION,
    PROTON_TRANSITION, ALPHA_TRANSITION
} DecayTransitionType;

namespace TransitionHelpers {
  template <class T> T pick_by_weight(vector<double> weights, vector<T> pick_from);
}

class DecayTransition
{
public:
  DecayTransition(G4double partial_lifetime, DecayState *init_state, DecayState *final_state);

  virtual ~DecayTransition() {}
  
  G4double partial_lifetime;
  DecayState *initial_state, *final_state;

  //Any modifications made to theta/phi min/max of source distributions should
  //should be reset afterwards
  virtual void generate_particles(G4Event *event, G4SingleParticleSource *source) =0;

  virtual DecayTransitionType get_decay_type() = 0;

  /*
    Implement if decay will create a vacancy in the atomic shell. Return -1 if no electron
    should be removed. Otherwise return the shell from which electron should be removed.

    This method will be called by the decay scheme after generate_particles
   */
  virtual int remove_electron() {return -1;}

  virtual void print(); 
};

class GammaTransition : public DecayTransition
{
public:
  GammaTransition(G4double partial_lifetime, DecayState *init_state, DecayState *final_state);

  virtual void generate_particles(G4Event *event, G4SingleParticleSource *source) override;

  virtual DecayTransitionType get_decay_type() override; 
};

class ECTransition : public DecayTransition
{
public:
  ECTransition(G4double partial_lifetime, DecayState *init_state, DecayState *final_state,
	       vector<G4double> capture_fractions);

  virtual void generate_particles(G4Event *event, G4SingleParticleSource *source) override  {
    (void)event; (void)source;//silence unused variable warnings
  }

  virtual int remove_electron() override;

  virtual DecayTransitionType get_decay_type() override;

  vector<G4double> capture_fractions;
  vector<int> eshells;
  
};

class ICTransition : public DecayTransition
{  
public:
  ICTransition(G4double partial_lifetime, DecayState *init_state, DecayState *final_state,
	       vector<double> eject_fractions);

  virtual void generate_particles(G4Event *event, G4SingleParticleSource *source) override;
  virtual int remove_electron() override;

  virtual DecayTransitionType get_decay_type() override;

  vector<int> eshells;
  vector<G4double> eject_fractions;
  int eshell_selected;
};

class BetaPlusTransition : public DecayTransition {
public:
  BetaPlusTransition(G4double partial_lifetime, DecayState *init_state, DecayState *final_state);

  ~BetaPlusTransition() {
    delete spectrumSampler;
  }

  virtual void generate_particles(G4Event *event, G4SingleParticleSource *source) override;

  virtual DecayTransitionType get_decay_type() override;

  G4RandGeneral* spectrumSampler;
};

class NucleonEmissionTransition : public DecayTransition {
public:
  NucleonEmissionTransition(G4double partial_lifetime, DecayState *init_state, DecayState *final_state);

  virtual G4ParticleDefinition* get_particle() = 0;

  virtual void generate_particles(G4Event *event, G4SingleParticleSource *source) override;

  //constraints on the angles over which the particles emmitted will be directed
  double phi_min, phi_max, theta_min, theta_max; 

  static double default_phi_min, default_phi_max, default_theta_min, default_theta_max;
};

class ProtonEmissionTransition : public NucleonEmissionTransition {
public:
  ProtonEmissionTransition(G4double partial_lifetime, DecayState *init_state, DecayState *final_state);
  
  virtual DecayTransitionType get_decay_type() override;

  virtual G4ParticleDefinition* get_particle() override;
};

class AlphaEmissionTransition : public NucleonEmissionTransition {
public:
  AlphaEmissionTransition(G4double partial_lifetime, DecayState *init_state, DecayState *final_state);

  virtual DecayTransitionType get_decay_type() override;

  virtual G4ParticleDefinition* get_particle() override;
};
#endif
