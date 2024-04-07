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

  analysisManager->CreateH1("h1MSD12n","h1MSD12n",10,0.,10.); // h1 ID=0, the range may be not suitable, the results are subject to nt
  analysisManager->CreateH1("h1MSD26n","h1MSD26n",10,0.,10.); // h1 ID=1, the range may be not suitable, the results are subject to nt
  analysisManager->CreateH1("h1LEGen","h1LEGen",10,0.,10.); // h1 ID=2, the range may be not suitable, the results are subject to nt
  analysisManager->CreateH1("h1MSD12e","h1MSD12e",60000,0.,60000); // h1 ID=3//6000 is range, in what unit is defined in FillH1
  analysisManager->CreateH1("h1MSD26e","h1MSD26e",60000,0.,60000); // h1 ID=4//6000 is range, in what unit is defined in FillH1
  analysisManager->CreateH1("h1LEGee","h1LEGee",60000,0.,60000); // h1 ID=5//6000 is range, in what unit is defined in FillH1
  analysisManager->CreateH2("MSD12XY","MSD12XY",80,80.,120.,80,80.,120.); // h2 ID = 0
  analysisManager->CreateH2("MSD26XY","MSD26XY",80,80.,120.,80,80.,120.); // h2 ID = 1
  analysisManager->CreateH2("LEGeXY","LEGeXY",80,80.,120.,80,80.,120.); // h2 ID = 2
  //CreatH1(name,title,nbins,vmin,vmax)
  //analysisManager/h1set hID nxbin xmin xmax [xunit] [xfcn] [xfcn] [binscheme]
  //analysisManager/h2set hID nxbin xmin xmax xunit xfcn nybin ymin ymax yunit yfcn
  //histogram IDs are attributed automatically. 1D and 2D histograms IDs are defined independently

  analysisManager->CreateNtuple("tree","tree");
  analysisManager->CreateNtupleIColumn("MSD12n"); //nt ID = 0
  analysisManager->CreateNtupleDColumn("MSD12e"); //nt ID = 1
  analysisManager->CreateNtupleDColumn("MSD12x");  //nt ID = 2
  analysisManager->CreateNtupleDColumn("MSD12y");  //nt ID = 3
  analysisManager->CreateNtupleDColumn("MSD12z");  //nt ID = 4
  analysisManager->CreateNtupleDColumn("MSD12t");  //nt ID = 5

  analysisManager->CreateNtupleIColumn("MSD26n"); //nt ID = 6
  analysisManager->CreateNtupleDColumn("MSD26e"); //nt ID = 7
  analysisManager->CreateNtupleDColumn("MSD26x");  //nt ID = 8
  analysisManager->CreateNtupleDColumn("MSD26y");  //nt ID = 9
  analysisManager->CreateNtupleDColumn("MSD26z");  //nt ID = 10
  analysisManager->CreateNtupleDColumn("MSD26t");  //nt ID = 11

  analysisManager->CreateNtupleIColumn("LEGen"); //nt ID = 12
  analysisManager->CreateNtupleDColumn("LEGee"); //nt ID = 13
  analysisManager->CreateNtupleDColumn("LEGex");  //nt ID = 14
  analysisManager->CreateNtupleDColumn("LEGey");  //nt ID = 15
  analysisManager->CreateNtupleDColumn("LEGez");  //nt ID = 16
  analysisManager->CreateNtupleDColumn("LEGet");  //nt ID = 17
  
  analysisManager->CreateNtupleIColumn("Northn"); //nt ID = 18
  analysisManager->CreateNtupleDColumn("Northe"); //nt ID = 19
  analysisManager->CreateNtupleDColumn("Northx");  //nt ID = 20
  analysisManager->CreateNtupleDColumn("Northy");  //nt ID = 21
  analysisManager->CreateNtupleDColumn("Northz");  //nt ID = 22
  analysisManager->CreateNtupleDColumn("Northt");  //nt ID = 23

  analysisManager->CreateNtupleIColumn("Southn"); //nt ID = 24
  analysisManager->CreateNtupleDColumn("Southe"); //nt ID = 25
  analysisManager->CreateNtupleDColumn("Southx");  //nt ID = 26
  analysisManager->CreateNtupleDColumn("Southy");  //nt ID = 27
  analysisManager->CreateNtupleDColumn("Southz");  //nt ID = 28
  analysisManager->CreateNtupleDColumn("Southt");  //nt ID = 29

//The following nt IDs will be off, they may be deleted soon

  analysisManager->CreateNtupleDColumn("MSD26_Ring1e"); //nt ID = 24
  analysisManager->CreateNtupleDColumn("MSD26_Ring1x");  //nt ID = 25
  analysisManager->CreateNtupleDColumn("MSD26_Ring1y");  //nt ID = 26
  analysisManager->CreateNtupleDColumn("MSD26_Ring2e"); //nt ID = 27
  analysisManager->CreateNtupleDColumn("MSD26_Ring2x");  //nt ID = 28
  analysisManager->CreateNtupleDColumn("MSD26_Ring2y");  //nt ID = 29
  analysisManager->CreateNtupleDColumn("MSD26_Ring3e"); //nt ID = 30
  analysisManager->CreateNtupleDColumn("MSD26_Ring3x");  //nt ID = 31
  analysisManager->CreateNtupleDColumn("MSD26_Ring3y");  //nt ID = 32

}

ExG4RunAction::~ExG4RunAction()
{
	G4cout<<"7$ after run, by run,ExG4RunAction::~ExG4RunAction()"<<G4endl;
	delete G4AnalysisManager::Instance();
}

void ExG4RunAction::BeginOfRunAction(const G4Run*)
{
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->OpenFile("ExG4");//output root file
  //ofstream outresultfile("out.txt",ios::out);//��������ļ�������outbeamfile���������ʽ�򿪴����ļ���������by run�ġ�
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
