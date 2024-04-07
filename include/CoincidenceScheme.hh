#ifndef COINCIDENCE_SCHEME_H
#define COINCIDENCE_SCHEME_H

#include "DecayScheme.hh"
#include <vector>

using namespace std;

/*
Class provides framework for simulating accidental coincidences.
*/
class CoincidenceScheme {
public:
    /*
    coincidence_window: width of window used to determine if two events are in coincidence
    primary_scheme: decay scheme which contains only events which would be gated on
    background_schemes and background_rates: Lists of decay schemes and the rate at which
                                             the events in those schemes occur.
    */
    CoincidenceScheme(double coincidence_window_, DecayScheme* primary_scheme_,
                        vector<DecayScheme*> background_schemes_, 
                        vector<double> background_rates_)
    : coincidence_window(coincidence_window_), primary_scheme(primary_scheme_),
      background_schemes(background_schemes_), background_rates(background_rates_)
    {
        assert(background_schemes.size() == background_rates.size());
    }


    double coincidence_window;
    DecayScheme* primary_scheme;
    vector<DecayScheme*> background_schemes;
    vector<double> background_rates;
    

     void do_decay(G4Event *event);
};

#endif