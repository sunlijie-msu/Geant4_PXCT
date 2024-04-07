#include "DecayScheme.hh"

#include <fstream>
#include <stdio.h>
#include <string>
#include <cstring>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <G4VAtomDeexcitation.hh>
#include <G4LossTableManager.hh>

using namespace std;
using namespace CLHEP;

const bool DEBUG_COMMENTS = false;

DecayState::~DecayState() {
  for(DecayTransition* transition : this->outgoing_transitions)
    delete transition;
}

void DecayState::print() {
  G4cout << "State: " << this->isotope_name << " " << this->energy/keV << "\n";
  for(DecayTransition *trans : outgoing_transitions) {
    trans->print();
  }
}


/*
  Reads in a decay scheme file, and sets the initial state to the highest energy state found.

  == Decay Scheme File Format Specification == 
  All characters following a # on a given
  line will be ignored. The file will consist of a arbitrary number of
  state transition tables. To start a table, write a single line with
  the table type. All energies should be in keV.

  GAMMA
  isotope name, initial energy in keV, final energy in keV, partial lifetime in seconds

  ELECTRON_CAPTURE
  initial isotope name, initial isotope energy, final isotope name, final energy, partial lifetime in second, e- frac 1, e- frac 2, ...

  BETA_PLUS or PROTON or ALPHA
  initial isotope name, initial isotope energy, final isotope name, final isotope energy, partial lifetime in seconds

  INTERNAL_CONVERSION
  isotope name, initial energy, final energy, e- frac 1, e- frac 2, ...

  The probabilities for vacancies created by electron capture or internal conversion should be ordered as: K1, L1, L2, L3, M1, ...
*/
DecayScheme::DecayScheme(G4String decay_scheme_fname, G4String isotopes_fname,
			 G4String atomic_widths_fname, G4SPSPosDistribution beam_spot_) :
  isotopeTable(isotopes_fname), atomicWidthsTable(atomic_widths_fname), beam_spot(beam_spot_)
{
  this->source.GetPosDist()->SetPosDisType("Point");
  
  if(DEBUG_COMMENTS)
    G4cout << "reading in decay scheme from: " << decay_scheme_fname << "\n";

  ifstream file;
  file.open(decay_scheme_fname);
  assert(file.is_open());
  const int MAX_LINE_LENGTH = 500;

  string line;
  string mode; //used to keep track of what type of table we're reading in

  while(std::getline(file, line)) {
    line = line.substr(0, line.find("#")); //remove comments
    if(DEBUG_COMMENTS)
      G4cout << "line read in:" << line << "\n";
    boost::trim(line); //remove trailing and leading white space
    
    if(line.size() == 0)
      continue;//empty line, ignore it
    if(line.find(",") == string::npos) { //no "," => mode command
      mode = boost::to_lower_copy(line);
      if(DEBUG_COMMENTS)
	G4cout << "mode changed to " << mode << "\n";
    } else {
      char cp_line[MAX_LINE_LENGTH]; //for use with strtok
      strcpy(cp_line, line.c_str());
      if(mode == "gamma") {
	string isotope_name(strtok(cp_line, " ,"));
	double gs_energy = this->isotopeTable.get_rest_energy(isotope_name);
	double E_init = atof(strtok(NULL, " ,"))*keV + gs_energy;
	double E_final = atof(strtok(NULL, " ,"))*keV + gs_energy;
	double partial_lifetime = atof(strtok(NULL, " ,"))*second;

	DecayState *initial_state = this->get_state(isotope_name, E_init, true);
	DecayState *final_state = this->get_state(isotope_name, E_final, true);
	new GammaTransition(partial_lifetime, initial_state, final_state); //constructor will register the transition with the init state

	if(DEBUG_COMMENTS)
	  G4cout << "gamma: " << isotope_name << " " << E_init << " " << E_final << "\n";
      } else if(mode == "electron_capture") {
	string isotope_init(strtok(cp_line, " ,"));
	double init_gs_energy = this->isotopeTable.get_rest_energy(isotope_init);
	double E_init = atof(strtok(NULL, " ,"))*keV + init_gs_energy;
	DecayState *previous_state = this->get_state(isotope_init, E_init, true);

	string isotope_final(strtok(NULL, " ,"));
	double final_gs_energy = this->isotopeTable.get_rest_energy(isotope_final);
	double E_final = atof(strtok(NULL, " ,"))*keV + final_gs_energy;
	DecayState *next_state = this->get_state(isotope_final, E_final, true);

	double partial_lifetime = atof(strtok(NULL, " ,"))*second;

	vector<double> capture_fractions;
	char* next_string = strtok(NULL, " ,");
	while(next_string != NULL) {
	  capture_fractions.push_back(atof(next_string));
	  next_string = strtok(NULL, " ,");
	}

	new ECTransition(partial_lifetime, previous_state, next_state, capture_fractions);

	if(DEBUG_COMMENTS)
	  G4cout << "EC: " << isotope_init << " " << E_init << " " << isotope_final << " " << E_final << "\n";
      } else if(mode == "internal_conversion") { 
	string isotope_name(strtok(cp_line, " ,"));
	double gs_energy = this->isotopeTable.get_rest_energy(isotope_name);
	double E_init = atof(strtok(NULL, " ,"))*keV + gs_energy;
	DecayState *previous_state = this->get_state(isotope_name, E_init, true);

	double E_final = atof(strtok(NULL, " ,"))*keV + gs_energy;
	DecayState *next_state = this->get_state(isotope_name, E_final, true);

	double partial_lifetime = atof(strtok(NULL, " ,"))*second;

	vector<double> eject_fractions;
	char* next_string = strtok(NULL, " ,");
	while(next_string != NULL) {
	  eject_fractions.push_back(atof(next_string));
	  next_string = strtok(NULL, " ,");
	}

	new ICTransition(partial_lifetime, previous_state, next_state, eject_fractions);

	if(DEBUG_COMMENTS)
	  G4cout << "IC: " << isotope_name << " " << E_init << " " << E_final << "\n";	
      

      } else if(mode == "beta_plus" || mode == "proton" || mode == "alpha") {
	string initial_isotope = strtok(cp_line, ", ");
	double init_gs_energy = this->isotopeTable.get_rest_energy(initial_isotope);
	double E_init = atof(strtok(NULL, ", "))*keV + init_gs_energy;

	string final_isotope = strtok(NULL, ", ");
	double final_gs_energy = this->isotopeTable.get_rest_energy(final_isotope);
	double E_final = atof(strtok(NULL, ", "))*keV + final_gs_energy;
	double partial_lifetime = atof(strtok(NULL, ", "))*second;

	DecayState *prev_state = this->get_state(initial_isotope, E_init, true);
	DecayState *next_state = this->get_state(final_isotope, E_final, true);
	if(mode == "beta_plus") 
	  new BetaPlusTransition(partial_lifetime, prev_state, next_state);
	else if(mode == "proton")
	  new ProtonEmissionTransition(partial_lifetime, prev_state, next_state);
	else if(mode == "alpha")
	  new AlphaEmissionTransition(partial_lifetime, prev_state, next_state);

	if(DEBUG_COMMENTS)
	  G4cout << initial_isotope << " " << E_init << " " << final_isotope << " " << E_final << "\n";
      }
    }
  }
  file.close();

  //set state to the state with the highest energy
  this->init_state = states[0];
  for(DecayState *state : this->states) {
    if(state->energy > this->init_state->energy)
      this->init_state = state;
  }
  
  if(DEBUG_COMMENTS)
    this->print();
}

void DecayScheme::do_decay(G4Event *event) {
  //TODO: emmit particles at different times beased on current time variable
  if(DEBUG_COMMENTS) {
    G4cout << "running decay\n" << std::flush;
  }
  
  
  //select a location in the beam spot configure the source to emit all decay products from this point
  G4ThreeVector location = this->beam_spot.GenerateOne();
  this->source.GetPosDist()->SetCentreCoords(location);//configured to point type in constructor

  //initialize variables and then do the decay
  DecayState *state = this->init_state;
  double current_time = 0; //time since started decay scheme
  int empty_eshell = -1; 
  
  //loop until the current state has no outgoing state transitions
  double chosen_dt = -1;
  while(state->outgoing_transitions.size() > 0) {
    if(DEBUG_COMMENTS) {
      G4cout << "current state: " << state->isotope_name << " " << state->energy / keV << "\n" << std::flush;
    }
    //for each outgoing transition, generate a decay time and select the lowest one
    DecayTransition *chosen_transition = NULL;
    for(DecayTransition *transition : state->outgoing_transitions) {
      //transition->print();      G4cout <<std::flush;
      double dt = RandExponential::shoot(transition->partial_lifetime);
      if(chosen_transition == NULL || dt < chosen_dt) {
	chosen_transition = transition;
	chosen_dt = dt;
      } 
    }
    if(DEBUG_COMMENTS) {
      G4cout << "selected transition: ";
      chosen_transition->print();
    }
    
    //emit an xray if appropriate
    if(empty_eshell != -1) {
      if(DEBUG_COMMENTS) {
	G4cout << "electron vacancy, checking if we need to emmit xrays\n" << std::flush;
      }
      //TODO: need to check/set auger and flo active?
      int Z = state->Z;
      double xray_dt = RandExponential::shoot(atomicWidthsTable.get_partial_lifetime(Z, empty_eshell));
      if(DEBUG_COMMENTS) {
	G4cout << "xray time, transition time = " << xray_dt << ", " << chosen_dt << "\n" << std::flush;
      }
      if(xray_dt < chosen_dt ) {
	if(DEBUG_COMMENTS)
	  G4cout << "need to emmit an xray\n" << std::flush;
	do_xrays(Z, empty_eshell, location, event);
	
	empty_eshell = -1;
      }
    }
    
    //update state
    if(DEBUG_COMMENTS) {
      G4cout << "updating state\n" << std::flush;
    }
    chosen_transition->generate_particles(event, &source);
    state = chosen_transition->final_state;
    current_time += chosen_dt;
    
    //create an electron hole if appropriate
    if(empty_eshell < 0) {
      empty_eshell = chosen_transition->remove_electron();
    }
  }
  if(empty_eshell != -1) {
    if(DEBUG_COMMENTS) {
      //G4cout << "doing xrays now, because reached end of decay chain with electron vacancy\n" << std::cout;
    }
    do_xrays(state->Z, empty_eshell, location, event);
  }
}

void DecayScheme::do_xrays(int Z, int empty_eshell, G4ThreeVector location, G4Event *event) {
  //code coppied with slight modification from G4NuclearDecayChannel
  G4VAtomDeexcitation* atomDeex = G4LossTableManager::Instance()->AtomDeexcitation();
  G4AtomicShellEnumerator as = G4AtomicShellEnumerator(empty_eshell);
  const G4AtomicShell* shell = atomDeex->GetAtomicShell(Z, as);    
  std::vector<G4DynamicParticle*> armProducts;
  const G4double deexLimit = 1.*keV;
  atomDeex->GenerateParticles(&armProducts, shell, Z, deexLimit, deexLimit);
  size_t narm = armProducts.size();
  if(DEBUG_COMMENTS)
    G4cout << "Emmitting ARM products\n" << std::flush;
  //ignores effects of recoil on the parent nucleus
  //emmit ARM products
  for (size_t i = 0; i<narm; ++i) {
    G4DynamicParticle* product = armProducts[i];
    this->gun.SetParticleDefinition(product->GetDefinition());
    this->gun.SetParticleEnergy(product->GetKineticEnergy());
    this->gun.SetParticleMomentumDirection(product->GetMomentumDirection());
    this->gun.SetNumberOfParticles(1);
    this->gun.SetParticlePosition(location);
    this->gun.GeneratePrimaryVertex(event);
    delete product;
    if(DEBUG_COMMENTS) {
      G4cout << "ARM PRODUCTS EMMITTED:"<< product->GetDefinition()->GetParticleName() << " " << product->GetKineticEnergy()/keV << "\n"<< std::flush;
    }
  }
}



DecayState* DecayScheme::get_state(string isotope_name, G4double energy, bool insert_if_new) {
  for(DecayState *state: this->states) {
    if(state->isotope_name.compare(isotope_name)==0 && state->energy == energy)
      return state;
  }
  if(insert_if_new) {
    int Z = this->isotopeTable.get_Z(isotope_name);
    DecayState *new_state = new DecayState(isotope_name, energy, Z);
    this->states.push_back(new_state);
    return new_state;
  }
  G4cout << "STATE NOT FOUND!" << isotope_name << " " << energy << " " << insert_if_new << "\n";
  return NULL;
}
