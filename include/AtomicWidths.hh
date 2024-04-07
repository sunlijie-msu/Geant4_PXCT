#ifndef ATOMIC_WIDTHS_H
#define ATOMIC_WIDTHS_H

#include <vector>
#include <unordered_map>
#include <G4String.hh>

using namespace std;

class AtomicWidthsTable {
public:
  AtomicWidthsTable(G4String filename);
  
  /*
    Return the partial lifetime for a vacancy in the listed shell.
    Z: number of protons in the nucleus
    eshell: Shell number, following the Geant4 shell indexing convention (eg K, L1, L2, L3, M1, ...)
    .       as defined in G4AtomicShellEnumerator.hh
  */
  double get_partial_lifetime(int Z, int eshell);

  unordered_map<int, vector<double>> partial_lifetimes;
};

#endif
