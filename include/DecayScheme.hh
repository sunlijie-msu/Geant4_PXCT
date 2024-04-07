#ifndef DECAY_SCHEME_H
#define DECAY_SCHEME_H

#include "globals.hh"
#include <vector>
#include "G4Event.hh"
#include <G4SingleParticleSource.hh>
#include <G4ParticleGun.hh>
#include "Randomize.hh"
#include "DecayTransition.hh"
#include "AtomicWidths.hh"
#include "IsotopeTable.hh"

using namespace std;
using namespace CLHEP;

class DecayTransition;

class DecayState
{
public:
  DecayState(G4String isotope_name_, G4double energy_, int Z_) : isotope_name(isotope_name_), energy(energy_), Z(Z_) {}

  virtual ~DecayState();

  void print();
   
  G4String isotope_name; //eg 73KR, etc
  G4double energy;
  int Z;

  //this will be updated automatically by the DecayTransition constructor
  vector<DecayTransition*> outgoing_transitions;
    vector<DecayTransition*> incoming_transitions;
};

/*
DecayScheme contains a set of states and possible transitions between
states. Each state is represented by the DecayState class which contains
a human readible name and an energy. The DecayTransition class contains
information about how DecayScheme a possible way that a transition could
occur between two states, and is also responsible for emmitting any
necessary particles if that transition occurs (the pure virtual
generate_particles function serves this purpose).

When constructing a decay scheme, decay state objects should be created
prior to any transitions connecting to them are created. When
DecayTransition objects are created, they will also add themselves to a
list of outgoing transitions maintained by the initial state. States
have knowledge of only their outgoing transitions, but transitions get
knowledge of both their initial and final states.

The DecayScheme destructor will delete all states and transitions added
to its vectors.

The G4GeneralParticleSource passed to DecayScheme::DoDecay should
already have the position distributiong defined.
 */

class DecayScheme
{
public:
  DecayScheme(G4String decay_scheme_fname, G4String isotopes_fname, G4String atomic_widths_fname, G4SPSPosDistribution beam_spot);
  
  //  Destructor just deletes all states
  virtual ~DecayScheme() {
    for(DecayState* state : this->states)
      delete state;
  }
 
  virtual void do_decay(G4Event *event);

  //Gets a pointer to the specified state, creating the state if necessary
  DecayState* get_state(string isotope_name, G4double energy, bool insert_if_new=false);

  void print() {
    G4cout << "==Decay Scheme==\n";
    for(DecayState *state : states) {
      state->print();
    }
  }

  //member variables
  vector<DecayState*> states;
  DecayState* init_state;

  IsotopeTable isotopeTable;
  AtomicWidthsTable atomicWidthsTable;

  G4SingleParticleSource source;
  G4ParticleGun gun;
  G4SPSPosDistribution beam_spot;

protected:
  void do_xrays(int Z, int eshell, G4ThreeVector location, G4Event *event);

};


#endif
