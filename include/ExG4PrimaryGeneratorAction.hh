#ifndef ExG4PrimaryGeneratorAction_hh
#define ExG4PrimaryGeneratorAction_hh
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
#include "TF2.h"
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
#include "globals.hh"
#include "CLHEP/Units/SystemOfUnits.h"

class G4ParticleGun;
class G4Event;


class ExG4PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
	ExG4PrimaryGeneratorAction(
		//const G4String& particleName = "geantino",
		//G4double Erecoil = 1.*CLHEP::MeV,
		//G4ThreeVector position= G4ThreeVector(0,0,-40.*CLHEP::cm),
		//G4ThreeVector momentumDirection = G4ThreeVector(0,0,1)
	);
	~ExG4PrimaryGeneratorAction();

	virtual void GeneratePrimaries(G4Event*);

private:
	G4ParticleGun* fParticleGun;
	TFile* pFile;
	//FILE *fout;
	int wheretorun;
	TH1F* depth_3He;
	// 	TH1F *py_impl;
	//     TH1D *pz_impl;// TH1F *pz = new TH1F("pz","pz",4000,0,4000); The whole sentence cannot be written in .hh
	TF2* xygaus; // 2D Gaussian beam spot
	TF1* ad;
	double theta_beam, r_beam, Rmax; // 2D Uniform beam spot
	double x0, y0, z0; // 2D beam spot	
	double Eg_lit[100];
	double branch_lit[100];
	double En_lit[100], En1, En2;
	double Halflife_lit[100];
	double branch_lit_integ[100];
	double branch_lit_tot;
	int numpeaks;


	G4double Qvalue;
	G4double atomic_mass_unit; // 931.5 MeV/c2
	G4double Atarget; // AX
	G4double Abeam; // Aa
	G4double Arecoil; // AY
	G4double Aejectile; // Ab
	G4double Eejectile; // Eb
	G4double Erecoil; // EY
	G4double Ebeam; // Ea
	G4double Pejectile; // pb
	G4double Precoil; // pY
	G4double Pbeam; // pa
	G4int Zrecoil, Zejectile;
	G4double tau;
	G4double ioncharge;
	G4double lifetime;
	G4double branch_integ;
	//G4double emg_sig,emg_sigp0,emg_sigp1,emg_tau,emg_taup0,emg_taup1;
	G4double Excitation_recoil, Excitation_ejectile;
	G4double resolution;
	G4double costheta_p1, costheta_p2, costheta_recoil;
	G4double phi_p1, phi_p2, phi_recoil;
	G4double theta_p1, theta_p2, theta_recoil;
	G4double sintheta_p1, sintheta_p2, sintheta_recoil;
	G4double dirx;
	G4double diry;
	G4double dirz;
	G4ThreeVector position;
	G4ThreeVector momentumDirection;

};

#endif
