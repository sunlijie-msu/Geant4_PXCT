#ifndef ExG4EventAction_h
#define ExG4EventAction_h

#include <iostream>
#include <fstream>
#include <math.h>
#include <time.h>
#include <map>
#include <TROOT.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TCutG.h>
#include <TChain.h>
#include <TMinuit.h>
#include "TChain.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TEllipse.h"
#include "TString.h"
#include "TGraph.h"
#include "TSpectrum.h"
#include "TVirtualFitter.h"
#include "TRandom.h"
#include <TRandom3.h>
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TF1.h"
#include "TMath.h"
#include "TPaveText.h"
#include "TPad.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "TLegend.h"
#include "TPaletteAxis.h"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#include "G4UserEventAction.hh"
#include "globals.hh"
#include <iostream>
#include <fstream>
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

class ExG4EventAction : public G4UserEventAction
{
public:
  ExG4EventAction();
  virtual ~ExG4EventAction();

  virtual void BeginOfEventAction(const G4Event*);
  virtual void EndOfEventAction(const G4Event*);
  
private:
	G4int fDHC1ID;
	G4int fDHC2ID;
	G4int fDHC3ID;
	G4int fDHC4ID;
	G4int fDHC5ID;
	G4double EMG_North_Sigma, EMG_North_Sigma_p0, EMG_North_Sigma_p1, EMG_North_Tau, EMG_North_Tau_p0, EMG_North_Tau_p1;
	G4double EMG_South_Sigma, EMG_South_Sigma_p0, EMG_South_Sigma_p1, EMG_South_Tau, EMG_South_Tau_p0, EMG_South_Tau_p1;
	G4double EMG_LEGe_Sigma, EMG_LEGe_Sigma_p0, EMG_LEGe_Sigma_p1, EMG_LEGe_Tau, EMG_LEGe_Tau_p0, EMG_LEGe_Tau_p1;
	G4double ih = -1, jh = -1;  //which strip is hit
	G4double eDep;
	G4double tof = -999;
	G4int totalEmHit = 0;
	G4double totalEmE = 0.;
	G4double px[100], py[100], pz[100], dx[100], length = 0;
	G4double t[100], dt[100], velocity = 0;
	// length is the sum of all the dx
	// velocity is dx/dt
	// dx is the distance between two step points
	// dt is the time difference between two step points
	// t is the time stamp of each step point

	//TFile *eFile;
	//TH1F *histo;
};

#endif
