#include "DecayTransition.hh"
#include <G4ParticleTable.hh>
#include <G4AtomicShells.hh>

using namespace std;
using namespace CLHEP;

const bool DEBUG_PRINTS = false;

template <class T> T TransitionHelpers::pick_by_weight(vector<double> weights, vector<T> pick_from) {
  assert(weights.size() == pick_from.size());
  assert(weights.size() > 0);
  double total_weight = 0;
  for(double weight : weights)
	  total_weight += weight;
  double target_number = RandFlat::shoot(0., total_weight);
  double counter = 0;
  for(unsigned int i = 0; i < weights.size(); i++) {
    counter += weights[i];
    if(counter >= target_number) {
      return pick_from[i];
    }
  }
  assert(false);
}

DecayTransition::DecayTransition(G4double partial_lifetime_, DecayState *init_state, DecayState *final_state_)
  :partial_lifetime(partial_lifetime_), initial_state(init_state), final_state(final_state_)
{
  this->initial_state->outgoing_transitions.push_back(this);
  this->final_state->incoming_transitions.push_back(this);
  assert(initial_state->energy > final_state->energy);
}

void DecayTransition::print() {
  G4cout << typeid(*this).name() << " to " << final_state->isotope_name << " " << final_state->energy/keV << " tau=" << partial_lifetime << "\n";
}

GammaTransition::GammaTransition(G4double partial_lifetime_, DecayState *init_state_, DecayState *final_state_)
  : DecayTransition(partial_lifetime_, init_state_, final_state_)
{
  assert(initial_state->Z == final_state->Z);
}

void GammaTransition::generate_particles(G4Event *event, G4SingleParticleSource *source) {
  //Emit a gamma ray isotropically with energy eqaul to the change in
  //energy between initial and final states
  G4ParticleDefinition* gamma = G4ParticleTable::GetParticleTable()->FindParticle("gamma");
  G4double energy = this->initial_state->energy - this->final_state->energy;
  source->SetParticleDefinition(gamma);
  source->GetAngDist()->SetAngDistType("iso");
  source->GetEneDist()->SetEnergyDisType("Mono");
  source->GetEneDist()->SetMonoEnergy(energy);
  source->SetNumberOfParticles(1);
  source->GeneratePrimaryVertex(event);
}


ECTransition::ECTransition(G4double partial_lifetime_, DecayState *init_state, DecayState *final_state_,
			   vector<G4double> capture_fractions_)
  : DecayTransition(partial_lifetime_, init_state, final_state_), capture_fractions(capture_fractions_)
{
  assert(initial_state->Z - 1 == final_state->Z);
  for(unsigned int i = 0; i<capture_fractions.size(); i++) {
    eshells.push_back(i);
  }
}

int ECTransition::remove_electron() {
  return TransitionHelpers::pick_by_weight(capture_fractions, eshells);
}

ICTransition::ICTransition(G4double partial_lifetime_, DecayState *init_state_, DecayState *final_state_,
	       vector<double> eject_fractions_)
  : DecayTransition(partial_lifetime_, init_state_, final_state_), eject_fractions(eject_fractions_), eshell_selected(-1)
{
  assert(initial_state->Z == final_state->Z);
  for(unsigned int i = 0; i<eject_fractions.size(); i++) {
    eshells.push_back(i);
  }
}

//note to self when updating this method: can use G4AtomicShells::GetBindingEnergy
void ICTransition::generate_particles(G4Event *event, G4SingleParticleSource *source) {
  //select shell from which electron will be ejected and calculate electron energy
  this->eshell_selected = TransitionHelpers::pick_by_weight(this->eject_fractions, this->eshells);
  G4double Q = initial_state->energy - final_state->energy;
  double energy = Q - G4AtomicShells::GetBindingEnergy(this->initial_state->Z, this->eshell_selected);

  //emmit electron
  G4ParticleDefinition *electron = G4ParticleTable::GetParticleTable()->FindParticle("e-");
  source->SetParticleDefinition(electron);
  source->GetAngDist()->SetAngDistType("iso");
  source->GetEneDist()->SetEnergyDisType("Mono");
  source->GetEneDist()->SetMonoEnergy(energy);
  source->SetNumberOfParticles(1);
  source->GeneratePrimaryVertex(event);
}

int ICTransition::remove_electron() {
  return this->eshell_selected;
}

BetaPlusTransition::BetaPlusTransition(G4double partial_lifetime_, DecayState *init_state_, DecayState *final_state_)
  : DecayTransition(partial_lifetime_, init_state_, final_state_)
{
  assert(initial_state->Z - 1 == final_state->Z);
  //configure spectrumSampler based on statistical factor only
  //if desire to do better in the future, reference G4BetaPlusDecay::SetUpBetaSpectrumSampler
  double mec2 = CLHEP::electron_mass_c2;
  double Q = initial_state->energy - final_state->energy - mec2;
  const int num_points = 100;
  double pdf[num_points];
  for(int i = 0; i < num_points; i++) {
    double E = (Q*i)/num_points;
    pdf[i] = std::sqrt(E*E + 2*E*mec2)*(Q-E)*(Q-E)*(E + mec2); //Krane eqn 9.25
  }
  spectrumSampler = new G4RandGeneral(pdf, num_points);
}

void BetaPlusTransition::generate_particles(G4Event *event, G4SingleParticleSource *source) {
  double mec2 = CLHEP::electron_mass_c2;
  double Q = initial_state->energy - final_state->energy - mec2;
  G4double energy = Q*spectrumSampler->shoot();
  if(DEBUG_PRINTS) {
    G4cout << "emmitting beta with KE = " << energy/keV << "\n";
  }
  //generate particle
  G4ParticleDefinition *beta_plus = G4ParticleTable::GetParticleTable()->FindParticle("e+");
  source->SetParticleDefinition(beta_plus);
  source->GetAngDist()->SetAngDistType("iso");
  source->GetEneDist()->SetEnergyDisType("Mono");
  source->GetEneDist()->SetMonoEnergy(energy);
  source->SetNumberOfParticles(1);
  source->GeneratePrimaryVertex(event);
}

double NucleonEmissionTransition::default_phi_min = 0;
double NucleonEmissionTransition::default_phi_max = twopi;
double NucleonEmissionTransition::default_theta_min = 0;
double NucleonEmissionTransition::default_theta_max = pi;

NucleonEmissionTransition::NucleonEmissionTransition(G4double partial_lifetime_, DecayState *init_state_, DecayState *final_state_)
 : DecayTransition(partial_lifetime_, init_state_, final_state_)
{
  phi_min = default_phi_min;
  theta_min = default_theta_min;
  phi_max = default_phi_max;
  theta_max = default_theta_max;
}

void NucleonEmissionTransition::generate_particles(G4Event *event, G4SingleParticleSource *source) {
  //Emit a particle isotropically with energy eqaul to the change in
  //energy between initial and final states minus it's rest energy
  G4ParticleDefinition* particle = this->get_particle();
  G4double Q = this->initial_state->energy - this->final_state->energy - particle->GetPDGMass();
  G4double kinetic_energy = Q/(1 + particle->GetPDGMass()/this->final_state->energy);
  if(DEBUG_PRINTS) {
    G4cout << "emmitting nucleon with KE = " << kinetic_energy/keV << "\n";
  }
  //G4cout << kinetic_energy/keV << "keV \n" <<std::flush;
  source->SetParticleDefinition(particle);
  //set up angular distribution
  source->GetAngDist()->SetAngDistType("iso");
  source->GetAngDist()->SetMinTheta(theta_min);
  source->GetAngDist()->SetMaxTheta(theta_max);
  source->GetAngDist()->SetMinPhi(phi_min);
  source->GetAngDist()->SetMaxPhi(phi_max);

  source->GetEneDist()->SetEnergyDisType("Mono");
  source->GetEneDist()->SetMonoEnergy(kinetic_energy);
  source->SetNumberOfParticles(1);
  source->GeneratePrimaryVertex(event);    

  //reset theta and phi min/max
  source->GetAngDist()->SetMinTheta(0);
  source->GetAngDist()->SetMaxTheta(pi);
  source->GetAngDist()->SetMinPhi(0);
  source->GetAngDist()->SetMaxPhi(twopi);
}

ProtonEmissionTransition::ProtonEmissionTransition(G4double partial_lifetime_, 
        DecayState *init_state_, DecayState *final_state_)
  : NucleonEmissionTransition(partial_lifetime_, init_state_, final_state_)
{
  assert(initial_state->Z - 1 == final_state->Z);
}
 
G4ParticleDefinition* ProtonEmissionTransition::get_particle() {
  return G4ParticleTable::GetParticleTable()->FindParticle("proton");
}

AlphaEmissionTransition::AlphaEmissionTransition(G4double partial_lifetime_, 
        DecayState *init_state_, DecayState *final_state_)
  : NucleonEmissionTransition(partial_lifetime_, init_state_, final_state_) 
{
  assert(initial_state->Z - 2 == final_state->Z);
}

G4ParticleDefinition* AlphaEmissionTransition::get_particle() {
  return G4ParticleTable::GetParticleTable()->FindParticle("alpha");
}


DecayTransitionType GammaTransition::get_decay_type() {return GAMMA_TRANSITION;}
DecayTransitionType ECTransition::get_decay_type() {return EC_TRANSITION;}
DecayTransitionType ICTransition::get_decay_type() {return IC_TRANSITION;}
DecayTransitionType BetaPlusTransition::get_decay_type() {return BETA_PLUS_TRANSITION;}
DecayTransitionType ProtonEmissionTransition::get_decay_type() {return PROTON_TRANSITION;}
DecayTransitionType AlphaEmissionTransition::get_decay_type() {return ALPHA_TRANSITION;}
