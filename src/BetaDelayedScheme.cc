#include "BetaDelayedScheme.hh"

using namespace std;
using namespace CLHEP;

const bool DEBUG_COMMENTS = false;

BetaDelayedScheme::BetaDelayedScheme(G4String decay_scheme_fname, G4String isotopes_fname,
    G4String atomic_widths_fname, G4SPSPosDistribution beam_spot_) 
 : DecayScheme(decay_scheme_fname, isotopes_fname, atomic_widths_fname, beam_spot_) 
{
    if(DEBUG_COMMENTS) {
        G4cout << "Updating decay scheme so that only paths with beta delayed protons and alphas are left\n";
    }
    //Loop over each of the states one transition away from the initial state
    for(int i = init_state->outgoing_transitions.size() - 1; i >= 0; i--) {
        //at present, this class assumes that all outgoing transitions from the initial state
        //are beta plus or EC
        DecayTransition* i_transition = init_state->outgoing_transitions[i];
        assert(i_transition->get_decay_type() == EC_TRANSITION 
                || i_transition->get_decay_type() == BETA_PLUS_TRANSITION);
        double total_width = 0; //widths in units of 1/seconds
        double palpha_width = 0;
        DecayState* i_state = i_transition->final_state;
        if(DEBUG_COMMENTS) {
            G4cout << "Inpecting outgoing transitions from";
            i_state->print();
        }
        for(int n = i_state->outgoing_transitions.size()-1; n >= 0 ; n--) {
            DecayTransition* n_transition = i_state->outgoing_transitions[n];
            if(DEBUG_COMMENTS) {
                n_transition->print();
            }
            total_width += 1/n_transition->partial_lifetime;
            if(n_transition->get_decay_type() == PROTON_TRANSITION 
               || n_transition->get_decay_type() == ALPHA_TRANSITION)
            {
                palpha_width += 1/n_transition->partial_lifetime;
            } else {
                //remove the transition if it's not a proton or alpha
                delete n_transition;
                i_state->outgoing_transitions.erase(i_state->outgoing_transitions.begin() + n);
            }
        }
        if(palpha_width == 0) { 
            //remove transition if the state it populates can't emmit alphas or protons
            delete i_transition;
            init_state->outgoing_transitions.erase(init_state->outgoing_transitions.begin() + i);
        } else {
            //otherwise weight transition to the state by the probability that it emmits a proton or an alpha
	  for(DecayTransition *incoming_transition : i_state->incoming_transitions)
	    incoming_transition->partial_lifetime = incoming_transition->partial_lifetime/(palpha_width/total_width);
            
        }
    }
    if(DEBUG_COMMENTS) {
        print();
    }
}
