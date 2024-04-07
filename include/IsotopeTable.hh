#ifndef ISOTOPE_TABLE_H
#define ISOTOPE_TABLE_H

#include <unordered_map>
#include <string>
#include <G4ios.hh>
#include <G4String.hh>

using namespace std;

class IsotopeTable
{
public:
  IsotopeTable(G4String filename);

  double get_rest_energy(string isotope_name);
  int get_Z(string isotope_name);
  

  unordered_map<string, double> rest_energies; // (mass/c^2)
  unordered_map<string, int> num_protons;
};

#endif
