#include "AtomicWidths.hh"


#include <fstream>
#include <stdio.h>
#include <cstring>
#include <boost/algorithm/string.hpp>
#include <CLHEP/Units/SystemOfUnits.h>
#include <G4ios.hh>

using namespace std;
using namespace CLHEP;

const bool DEBUG_COMMENTS = false;


/*
  == File Format ==
  Z of atom, width for first vacancy in seconds, width for next shell, ...

  Widths should have units of seconds (eg hbar/width in energy) and be listed in the format K1, L1, L2, L3, M1, ...
 */
AtomicWidthsTable::AtomicWidthsTable(G4String filename) {
  if(DEBUG_COMMENTS) {
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
    int Z = atoi((strtok(cp_line, " ,")));

    vector<double> times_to_add;
    char* next_tok = strtok(NULL, " ,");
    while(next_tok != NULL) {
      times_to_add.push_back(atof(next_tok)*second);
      next_tok = strtok(NULL, " ,");
    }
    
    partial_lifetimes[Z] = times_to_add;
  }
  if(DEBUG_COMMENTS) {

  }
}

double AtomicWidthsTable::get_partial_lifetime(int Z, int eshell) {
  assert((unsigned int)eshell <= partial_lifetimes[Z].size());
  return partial_lifetimes[Z][eshell];
}
