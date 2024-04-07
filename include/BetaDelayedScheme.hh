#ifndef BETA_DELAYED_SCHEME_H
#define BETA_DELAYED_SCHEME_H

#include "DecayScheme.hh"

//Just like the normal DecayScheme class, but the  constructor deletes transitions
//from the initial state which populate states that don't emit protons/alphas, and
//deletes transitions from the states populated by the initial states that aren't
//alphas or protons, and updates the partial lifetimes appropriately.
class BetaDelayedScheme : public DecayScheme {
public:
    BetaDelayedScheme(G4String decay_scheme_fname, G4String isotopes_fname, G4String atomic_widths_fname, G4SPSPosDistribution beam_spot);

    virtual ~BetaDelayedScheme() = default;
};

#endif