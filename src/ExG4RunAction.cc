#include "ExG4RunAction.hh"
#include "ExG4Analysis.hh"
#include "G4Run.hh"
#include "G4UnitsTable.hh"
//#include "G4SystemOfUnits.hh"
#include <iostream>
#include <fstream>
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
using namespace std;
ExG4RunAction::ExG4RunAction()
: G4UserRunAction()
{
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  G4cout<<"2$ before run, by run, ExG4RunAction::ExG4RunAction() Using "<<analysisManager->GetType()<<G4endl;

  analysisManager->SetVerboseLevel(1);
  //analysisManager->SetFileName("ExG4");//root filename

  //Book histograms, ntuple
  //

  analysisManager->CreateH1("hMSD12_n","hMSD12_n",10,0.,10.); // h1 ID=0, the range may be not suitable, the results are subject to nt
  analysisManager->CreateH1("hMSD26_n","hMSD26_n",10,0.,10.); // h1 ID=1, the range may be not suitable, the results are subject to nt
  analysisManager->CreateH1("hLEGe_n","hLEGe_n",10,0.,10.); // h1 ID=2, the range may be not suitable, the results are subject to nt
  analysisManager->CreateH1("hNorth_n","hNorth_n",10,0.,10.); // h1 ID=3, the range may be not suitable, the results are subject to nt
  analysisManager->CreateH1("hSouth_n","hSouth_n",10,0.,10.); // h1 ID=4, the range may be not suitable, the results are subject to nt
  analysisManager->CreateH1("hMSD12_e","hMSD12_e",60000,0.,6000); // h1 ID=5//6000 is range, in what unit is defined in FillH1
  analysisManager->CreateH1("hMSD26_e","hMSD26_e",60000,0.,6000); // h1 ID=6//6000 is range, in what unit is defined in FillH1
  analysisManager->CreateH1("hLEGe_e","hLEGe_e",20000,0.,2000); // h1 ID=7//20000 is range, in what unit is defined in FillH1
  analysisManager->CreateH1("hNorth_e","hNorth_e",80000,0.,8000); // h1 ID=8//60000 is range, in what unit is defined in FillH1
  analysisManager->CreateH1("hSouth_e","hSouth_e",80000,0.,8000); // h1 ID=9//60000 is range, in what unit is defined in FillH1

  analysisManager->CreateH2("h2_MSD12_xy","h2_MSD12_xy",80,80.,120.,80,80.,120.); // h2 ID = 0
  analysisManager->CreateH2("h2_MSD26_xy","h2_MSD26_xy",80,80.,120.,80,80.,120.); // h2 ID = 1
  analysisManager->CreateH2("h2_LEGe_xy","h2_LEGe_xy",80,80.,120.,80,80.,120.); // h2 ID = 2
  //CreatH1(name,title,nbins,vmin,vmax)
  //analysisManager/h1set hID nxbin xmin xmax [xunit] [xfcn] [xfcn] [binscheme]
  //analysisManager/h2set hID nxbin xmin xmax xunit xfcn nybin ymin ymax yunit yfcn
  //histogram IDs are attributed automatically. 1D and 2D histograms IDs are defined independently

  analysisManager->CreateNtuple("tree","tree");
  analysisManager->CreateNtupleIColumn("MSD12_n"); //nt ID = 0
  analysisManager->CreateNtupleDColumn("MSD12_e"); //nt ID = 1
  analysisManager->CreateNtupleDColumn("MSD12_x");  //nt ID = 2
  analysisManager->CreateNtupleDColumn("MSD12_y");  //nt ID = 3
  analysisManager->CreateNtupleDColumn("MSD12_z");  //nt ID = 4
  analysisManager->CreateNtupleDColumn("MSD12_t");  //nt ID = 5

  analysisManager->CreateNtupleIColumn("MSD26_n"); //nt ID = 6
  analysisManager->CreateNtupleDColumn("MSD26_e"); //nt ID = 7
  analysisManager->CreateNtupleDColumn("MSD26_x");  //nt ID = 8
  analysisManager->CreateNtupleDColumn("MSD26_y");  //nt ID = 9
  analysisManager->CreateNtupleDColumn("MSD26_z");  //nt ID = 10
  analysisManager->CreateNtupleDColumn("MSD26_t");  //nt ID = 11

  analysisManager->CreateNtupleIColumn("LEGe_n"); //nt ID = 12
  analysisManager->CreateNtupleDColumn("LEGe_e"); //nt ID = 13
  analysisManager->CreateNtupleDColumn("LEGe_x");  //nt ID = 14
  analysisManager->CreateNtupleDColumn("LEGe_y");  //nt ID = 15
  analysisManager->CreateNtupleDColumn("LEGe_z");  //nt ID = 16
  analysisManager->CreateNtupleDColumn("LEGe_t");  //nt ID = 17
  
  analysisManager->CreateNtupleIColumn("North_n"); //nt ID = 18
  analysisManager->CreateNtupleDColumn("North_e"); //nt ID = 19
  analysisManager->CreateNtupleDColumn("North_x");  //nt ID = 20
  analysisManager->CreateNtupleDColumn("North_y");  //nt ID = 21
  analysisManager->CreateNtupleDColumn("North_z");  //nt ID = 22
  analysisManager->CreateNtupleDColumn("North_t");  //nt ID = 23

  analysisManager->CreateNtupleIColumn("South_n"); //nt ID = 24
  analysisManager->CreateNtupleDColumn("South_e"); //nt ID = 25
  analysisManager->CreateNtupleDColumn("South_x");  //nt ID = 26
  analysisManager->CreateNtupleDColumn("South_y");  //nt ID = 27
  analysisManager->CreateNtupleDColumn("South_z");  //nt ID = 28
  analysisManager->CreateNtupleDColumn("South_t");  //nt ID = 29

//The following nt IDs will be off, they may be deleted soon

  //analysisManager->CreateNtupleDColumn("MSD26_Ring1_e"); //nt ID = 24
  //analysisManager->CreateNtupleDColumn("MSD26_Ring1_x");  //nt ID = 25
  //analysisManager->CreateNtupleDColumn("MSD26_Ring1_y");  //nt ID = 26
  //analysisManager->CreateNtupleDColumn("MSD26_Ring2_e"); //nt ID = 27
  //analysisManager->CreateNtupleDColumn("MSD26_Ring2_x");  //nt ID = 28
  //analysisManager->CreateNtupleDColumn("MSD26_Ring2_y");  //nt ID = 29
  //analysisManager->CreateNtupleDColumn("MSD26_Ring3_e"); //nt ID = 30
  //analysisManager->CreateNtupleDColumn("MSD26_Ring3_x");  //nt ID = 31
  //analysisManager->CreateNtupleDColumn("MSD26_Ring3_y");  //nt ID = 32

}

ExG4RunAction::~ExG4RunAction()
{
	G4cout<<"7$ after run, by run,ExG4RunAction::~ExG4RunAction()"<<G4endl;
	delete G4AnalysisManager::Instance();
}

void ExG4RunAction::BeginOfRunAction(const G4Run*)
{
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->OpenFile("ExG4_152Eu_LowEnergy_45keV.root"); // modify output root file 
  //ofstream outresultfile("out.txt",ios::out);//by run
  G4cout<<"5$ first in run, by run, ExG4RunAction::BeginOfRunAction(const G4Run*)"<<G4endl;
}

void ExG4RunAction::EndOfRunAction(const G4Run*)
{
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  //G4cout<<analysisManager->GetH1(2)->mean()<<G4endl;
  analysisManager->Write();
  analysisManager->CloseFile();
  G4cout<<"6$ last in run, by run, ExG4RunAction::EndOfRunAction(const G4Run*)"<<G4endl;
}
