#include "IsotopeTable.hh"

#include <fstream>
#include <stdio.h>
#include <cstring>
#include <boost/algorithm/string.hpp>
#include <CLHEP/Units/SystemOfUnits.h>

using namespace std;
using namespace CLHEP;

const bool DEBUG_COMMENTS = false;

/*
Reads in a table of Isotope masses and names.
== Isotope List File Format ==
<isotope name>, <# of protons>, <ground state mass in keV/c^2>
<next isotope name>, <# of protons>, <ground state mass in keV/c^2>

Ground state energies are for the nucleus only (doesn't include  electrons; is nuclear and not atomic mass)

All characters following a # on a given line will be ignored.
 */
IsotopeTable::IsotopeTable(G4String filename) {
  if (DEBUG_COMMENTS) {
    G4cout << "reading in isotope table from: " << filename << "\n";
  }

  ifstream file;
  file.open(filename);
  assert(file.is_open());
  const int MAX_LINE_LENGTH = 500;

  string line;

  while(std::getline(file, line)) {
    line = line.substr(0, line.find("#")); //remove comments
    if(DEBUG_COMMENTS)
      G4cout << "line read in:" << line << "\n";
    boost::trim(line); //remove trailing and leading white space
    
    if(line.size() == 0)
      continue;//empty line, ignore it

    char cp_line[MAX_LINE_LENGTH];
    strcpy(cp_line, line.c_str());
    string isotope_name(strtok(cp_line, ","));
    isotope_name = boost::to_upper_copy(isotope_name);
    this->num_protons[isotope_name] = atoi(strtok(NULL, ","));
    this->rest_energies[isotope_name]  = atof(strtok(NULL, ","))*keV;
  }
  if(DEBUG_COMMENTS) {
    for(auto i : rest_energies)
      G4cout << i.first << ": " << i.second << "\n";
  }
}

double IsotopeTable::get_rest_energy(string isotope_name) {
  return rest_energies[boost::to_upper_copy(isotope_name)];
}

int IsotopeTable::get_Z(string isotope_name) {
  return num_protons[isotope_name];
}
