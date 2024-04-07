#include "CoincidenceScheme.hh"

void CoincidenceScheme::do_decay(G4Event *event) {
    primary_scheme->do_decay(event);
    for(unsigned int i = 0; i < background_schemes.size(); i++) {
        //determine how many times the background scheme should be run
        int events = CLHEP::RandPoisson::shoot(background_rates[i]*coincidence_window);
        //run the background scheme the determined number of times
        for(int n = 0; n < events; n++) {
            background_schemes[i]->do_decay(event);
        }
    }
}