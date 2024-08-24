#include "ExG4EventAction.hh"
#include "ExG4PrimaryGeneratorAction.hh"
#include "ExG4Analysis.hh"
#include "ExG4Hit.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
#include "G4UnitsTable.hh"
// #include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "G4Types.hh"
#include "Randomize.hh"
using namespace std;
ExG4EventAction::ExG4EventAction()
    : G4UserEventAction(),
      fDHC1ID(-1), fDHC2ID(-1), fDHC3ID(-1), fDHC4ID(-1), fDHC5ID(-1)
{
  G4RunManager::GetRunManager()->SetPrintProgress(1);
  ofstream outresultfile("out.txt", ios::out);//定义输出文件流对象outbeamfile，以输出方式打开磁盘文件，这里是by run的，也可以在RunAction.cc里打开文件。
  // eFile=new TFile("/home/sun/app/geant/first/ExG4old.root","read");
  // histo = new TH1F("histo","histo",100,-100,100);
  G4cout << "3$ before run, by run, ExG4EventAction::ExG4EventAction()" << G4endl;
}

ExG4EventAction::~ExG4EventAction()
{
  // eFile->Close();
  G4cout << "10$ after run, by run, ExG4EventAction::~ExG4EventAction()" << G4endl;
}

void ExG4EventAction::BeginOfEventAction(const G4Event *)
{
  // G4cout<<"(3)$ first in event, by event, ExG4EventAction::BeginOfEventAction(const G4Event*)"<<G4endl;
  if (fDHC1ID == -1 || fDHC2ID == -1 || fDHC3ID == -1 || fDHC4ID == -1 || fDHC5ID == -1) // the initial value of -1 for hits id allows you to be efficient in searching the hits by string: if id==-1 you need to search the collections, if not you already did this opeartion and you can skip it.
  {
    G4SDManager *sdManager = G4SDManager::GetSDMpointer();
    fDHC1ID = sdManager->GetCollectionID("MSD12/SDCollect"); // SensitiveDetectorName,collectionName[0]//按照SDName和HCName获取其HCID赋给fDHCID
    fDHC2ID = sdManager->GetCollectionID("MSD26/SDCollect"); // SensitiveDetectorName,collectionName[0]//按照SDName和HCName获取其HCID赋给fDHCID
    fDHC3ID = sdManager->GetCollectionID("LEGe/SDCollect"); // SensitiveDetectorName,collectionName[0]//按照SDName和HCName获取其HCID赋给fDHCID
    fDHC4ID = sdManager->GetCollectionID("North/SDCollect");  // SensitiveDetectorName,collectionName[0]//按照SDName和HCName获取其HCID赋给fDHCID
    fDHC5ID = sdManager->GetCollectionID("South/SDCollect");
  }//按照SDName retrieve the IDs of the two collections
	//ofstream outresultfile("out.txt",ios::out);//定义输出文件流对象outbeamfile，以输出方式打开磁盘文件，这里是by event的，不建议在这里打开outfile。
}

void ExG4EventAction::EndOfEventAction(const G4Event *event)
{
  // histo = (TH1F*)eFile->Get("h1");
  // G4cout<<histo->GetMean()<<G4endl;
  // G4double MSD12Reso=0.1/2.35;
  // G4double MSD26Reso=0.1/2.35;

  G4HCofThisEvent *hce = event->GetHCofThisEvent();
  if (!hce)
  {
    //G4ExceptionDescription msg;
    //msg << "No hits collection of this event found. \n";
    //G4Exception("ExG4EventAction::EndOfEventAction()", "ExG4Cod001", JustWarning, msg);
    return;
  }

  // Get hits collections
  ExG4HitsCollection *dHC1 = static_cast<ExG4HitsCollection *>(hce->GetHC(fDHC1ID)); //按照fDHC1ID从hce中取出一个HitsCollection，称为dHC1
  ExG4HitsCollection *dHC2 = static_cast<ExG4HitsCollection *>(hce->GetHC(fDHC2ID)); //按照fDHC2ID从hce中取出一个HitsCollection，称为dHC2
  ExG4HitsCollection *dHC3 = static_cast<ExG4HitsCollection *>(hce->GetHC(fDHC3ID)); //按照fDHC3ID从hce中取出一个HitsCollection，称为dHC3
  ExG4HitsCollection *dHC4 = static_cast<ExG4HitsCollection *>(hce->GetHC(fDHC4ID)); //按照fDHC4ID从hce中取出一个HitsCollection，称为dHC4
  ExG4HitsCollection *dHC5 = static_cast<ExG4HitsCollection *>(hce->GetHC(fDHC5ID));

  //   if(!dHC1)
  //   {
  //     G4ExceptionDescription msg;
  //     msg << " Some of hits collection of this event not found!. \n";
  //     G4Exception("ExG4EventAction::EndOfEventAction()","ExG4Code001",JustWarning,msg);
  //     return;
  //   }
  ofstream outresultfile("out.txt", ios::app); //定义输出文件流对象outbeamfile，以追加方式打开磁盘文件. by event
  G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();

  //EMG response function
  TF1* EMGf1 = new TF1("EMGf1", "[0]*[3]/[1]*1.253314137*TMath::Exp(0.5*([3]*[3]/[1]/[1])+(x-[2])/[1])*TMath::Erfc(1/1.41421356*([3]/[1]+(x-[2])/[3]))", 0, 10000);//Glassman_PRC2019
  EMGf1->SetNpx(100000);//Set the number of points used to draw the function. [0]-N, [1]-tau, [2]-mu, [3]-sigma,
  // normalized cumulative Probability Density Function
  TF1* EMGf2 = new TF1("EMGf2", "[0]/2*(TMath::Exp((0.5*[3]*[3]-[1]*[2]+[1]*x)/([1]*[1]))*TMath::Erfc(1/1.41421356*([3]/[1]+(x-[2])/[3]))-TMath::Erfc((x-[2])/(1.41421356*[3])))+1", 0, 10000);//Glassman_PRC2019
  EMGf2->SetNpx(100000);//Set the number of points used to draw the function.

  EMG_North_Sigma_p1 = 0.0002357536;
  EMG_North_Sigma_p0 = 0.4955303549;
  EMG_North_Tau_p1 = 0.0000919463;
  EMG_North_Tau_p0 = 0.0320713121;

  EMG_South_Sigma_p1 = 0.0002555137;
  EMG_South_Sigma_p0 = 0.4842763931;
  EMG_South_Tau_p1 = 0.0001547217;
  EMG_South_Tau_p0 = 0.1230884788;

  EMG_LEGe_Sigma_p1 = 0.0014650;
  EMG_LEGe_Sigma_p0 = 0.0952743;
  EMG_LEGe_Tau_p1 = 0.0;
  EMG_LEGe_Tau_p0 = 0.048;

  TF1* fwhm_func_MSD26 = new TF1("fwhm_func", "sqrt(4 * (2 * log(2) * [0] * x * 0.00361) + pow([1], 2))", 0, 6000);
  fwhm_func_MSD26->SetParameter(0, 2.25161); // for Fano factor F
  fwhm_func_MSD26->SetParameter(1, -6.38283); // for electronics effect We

  TF1* fwhm_func_MSD12 = new TF1("fwhm_func", "sqrt(4 * (2 * log(2) * [0] * x * 0.00361) + pow([1], 2))", 0, 6000);
  fwhm_func_MSD12->SetParameter(0, 36.0222); // for Fano factor F
  fwhm_func_MSD12->SetParameter(1, 25.5378); // for electronics effect We

  if (dHC1)//For MSD12
  {
	  G4int n_hits = dHC1->entries();//MSD12SD hit 数
	  //G4cout<<"-----MSD12---"<<"dHC1 n_hits="<<n_hits<<G4endl;
	  analysisManager->FillH1(0, n_hits);//h1 ID=0, distribution of the number of hits
	  totalEmHit = 0;	totalEmE = 0;	ih = -100;	jh = -100; tof = -999; length = 0; velocity = 0;
	  for (G4int i = 0; i < n_hits; i++)//Within this loop, the histo are filled by hit, so the number can be much larger than the event number, the nt can only be filled by event (the last hit)
	  {
		  //if(i>0) break; //using this statement to extract the first hit for position check, the energy would be incomplete cuz only the first hit was recorded.
		  ExG4Hit* hit = (*dHC1)[i];//fHitsCollection中第i个hit的信息，有时一个event就产生1个hit，但打出次级粒子会有多个hit
		  eDep = hit->GetE();
		  totalEmHit++;
		  totalEmE += eDep;
	  }
	  //Outside the loop, the nt are filled by event, so the number of "nt>0" is exactly the event number!, you cannot use if(n>0)Fillnt; here, it's useless.
	  analysisManager->FillNtupleIColumn(0, dHC1->entries());//nt ID=0 filled by event
	  if(totalEmE>0.0005*CLHEP::MeV)
	  {
		  G4double Etemp = totalEmE;
		  //G4double resolution = 0.0090; // 0.95% for the sum peak of telescope, based on 241Am source measurement. 0.90% can be adopted as the MSD12 resolution.
		  //G4double fwhm = resolution * Etemp;
		  G4double fwhm = fwhm_func_MSD12->Eval(Etemp / CLHEP::keV);
		  G4double sigma = fwhm / 2.355;
		  totalEmE = G4RandGauss::shoot(Etemp, sigma * CLHEP::keV); // G4RandGauss::shoot(mu,sigma)
		  //G4cout << totalEmE / CLHEP::keV << " " << Etemp / CLHEP::keV << " keV. fwhm=" << fwhm << " sigma=" << sigma << G4endl;
	  }
		//excitE width should be added in PrimaryGeneratorAction.cc, while detector resolution should be added in EventAction.cc
	  analysisManager->FillH1(5, totalEmE / CLHEP::keV);//h1 ID=5 filled by event. Fill histogram using the values in units of keV. Don't change. 
	  analysisManager->FillNtupleDColumn(1, totalEmE / CLHEP::keV);//nt ID=1 filled by event
  }

  if (dHC2)//For MSD26
  {
	  G4int n_hits = dHC2->entries();//MSD26SD hit 数
	  //G4cout<<"-----MSD26---"<<"dHC2 n_hits="<<n_hits<<G4endl;
	  analysisManager->FillH1(1, n_hits);//h1 ID=1, distribution of the number of hits
	  totalEmHit = 0;	totalEmE = 0;	ih = -100;	jh = -100; tof = -999; length = 0; velocity = 0;
	  for (G4int i = 0; i < n_hits; i++)//Within this loop, the histo are filled by hit, so the number can be much larger than the event number, the nt can only be filled by event (the last hit)
	  {
		  //if(i>0) break; //using this statement to extract the first hit for position check, the energy would be incomplete cuz only the first hit was recorded.
		  ExG4Hit* hit = (*dHC2)[i];//fHitsCollection中第i个hit的信息，有时一个event就产生1个hit，但打出次级粒子会有多个hit
		  // hit->Print();
		  eDep = hit->GetE();
		  totalEmE += eDep;
	  }
	  //Outside the loop, the nt are filled by event, so the number of "nt>0" is exactly the event number!
	  analysisManager->FillNtupleIColumn(6, dHC2->entries());//nt ID=6
	  if (totalEmE > 0.0005 * CLHEP::MeV)
	  {
		  G4double Etemp = totalEmE;
		  //G4double resolution = 0.0031; // 0.31% for MSD26 based on 241Am source measurement.
		  //G4double fwhm = resolution * Etemp;
		  G4double fwhm = fwhm_func_MSD26->Eval(Etemp / CLHEP::keV);
		  G4double sigma = fwhm / 2.355;
		  totalEmE = G4RandGauss::shoot(Etemp, sigma * CLHEP::keV); // G4RandGauss::shoot(mu,sigma)
		  //G4cout << totalEmE / CLHEP::keV << " " << Etemp / CLHEP::keV << " keV. fwhm=" << fwhm << " sigma=" << sigma << G4endl;
	  }
			//excitE width should be added in PrimaryGeneratorAction.cc, while detector resolution should be added in EventAction.cc
	  analysisManager->FillH1(6, totalEmE / CLHEP::keV);//h1 ID=6. Fill histogram using the values in units of keV. Don't change. 
	  analysisManager->FillNtupleDColumn(7, totalEmE / CLHEP::keV);//nt ID=7

  }
  if (dHC3)//For LEGe
  {
	  G4int n_hits = dHC3->entries();//LEGeSD hit 数
	  //G4cout<<"-----LEGe---"<<"dHC3 n_hits="<<n_hits<<G4endl;
	  analysisManager->FillH1(2, n_hits);//h1 ID=2, distribution of the number of hits
	  totalEmE = 0;
	  for (G4int i = 0; i < n_hits; i++)//Within this loop, the histo are filled by hit, so the number can be much larger than the event number, the nt can only be filled by event (the last hit)
	  {
		  ExG4Hit* hit = (*dHC3)[i];//fHitsCollection中第i个hit的信息，有时一个event就产生1个hit，但打出次级粒子会有多个hit
		  eDep = hit->GetE();
		  totalEmHit++;
		  totalEmE += eDep;
	  }
	  //Outside the loop, the nt are filled by event, so the number of "nt>0" is exactly the event number!
	  analysisManager->FillNtupleIColumn(12, dHC3->entries());//nt ID=12
	  if (totalEmE > 0.5 * CLHEP::keV)// if you comment this part out, the EMG effect will be disabled.
	  {
		  EMG_LEGe_Sigma = EMG_LEGe_Sigma_p1 * totalEmE * 1000 + EMG_LEGe_Sigma_p0;
		  EMG_LEGe_Tau = EMG_LEGe_Tau_p1 * totalEmE * 1000 + EMG_LEGe_Tau_p0;
		  //G4cout<<"totalEmHit=	"<<totalEmHit<<"	totalEmE=	"<<totalEmE/CLHEP::keV<<"	EMG_LEGe_Sigma=	"<<EMG_LEGe_Sigma<<"	EMG_LEGe_Tau=	"<<EMG_LEGe_Tau<<G4endl;
		  EMGf2->SetParameter(0, 1.);//N
		  EMGf2->SetParameter(1, EMG_LEGe_Tau);//tau
		  EMGf2->SetParameter(2, totalEmE * 1000);//mu
		  EMGf2->SetParameter(3, EMG_LEGe_Sigma);//sigma
		  totalEmE = EMGf2->GetX(G4RandFlat::shoot(0.0, 1.0), 0, 10000) / 1000.0;//After this, totalEmE is the experimentally detected gamma energy in default units of MeV
		  //totalEmE = EMGf2->GetX(G4UniformRand(), 0,10000)/1000.0;//After this, totalEmE is the experimentally detected gamma energy in default units of MeV
		  // Without "/1000.0", it puts the value in units of keV in a variable in units of MeV, which is wrong.
		  //G4double Etemp=totalEmE;
		  //totalEmE=G4RandGauss::shoot(Etemp,TargetReso*Etemp);//G4RandGauss::shoot(mu,sigma)
	  }
	  //excitE width should be added in PrimaryGeneratorAction.cc, while detector resolution should be added in EventAction.cc

	  analysisManager->FillH1(7, totalEmE / CLHEP::keV);//h1 ID=7. Fill histogram using the values in units of keV. Don't change. 
	  analysisManager->FillNtupleDColumn(13, totalEmE / CLHEP::keV);//nt ID=13
  }

  if (dHC4)//For North Ge Detector
  {
	  G4int n_hits = dHC4->entries();//NorthSD hit 数
	  //G4cout<<"-----North---"<<"dHC4 n_hits="<<n_hits<<G4endl;
	  analysisManager->FillH1(3, n_hits);//h1 ID=3, distribution of the number of hits
	  totalEmHit = 0;	totalEmE = 0;	ih = -100;	jh = -100; tof = -999; length = 0; velocity = 0;
	  for (G4int i = 0; i < n_hits; i++)//Within this loop, the histo are filled by hit, so the number can be much larger than the event number, the nt can only be filled by event (the last hit)
	  {
		  //if(i>0) break; //using this statement to extract the first hit for position check, the energy would be incomplete cuz only the first hit was recorded.
		  ExG4Hit* hit = (*dHC4)[i];//fHitsCollection中第i个hit的信息，有时一个event就产生1个hit，但打出次级粒子会有多个hit
		  eDep = hit->GetE();
		  totalEmHit++;
		  totalEmE += eDep;
		  //G4cout<<"-----Clover---"<<"totalEmHit="<<totalEmHit<<" totalEmE="<<totalEmE/CLHEP::keV<<" eDep="<<eDep/CLHEP::keV<<G4endl;
	  }
	  //Outside the loop, the nt are filled by event, so the number of "nt>0" is exactly the event number!
	  analysisManager->FillNtupleIColumn(18, dHC4->entries());//nt ID=18
	  if (totalEmE > 100 * CLHEP::keV)// if you comment this part out, the EMG effect will be disabled.
	  {
		  EMG_North_Sigma = EMG_North_Sigma_p1 * totalEmE * 1000 + EMG_North_Sigma_p0;
		  EMG_North_Tau = EMG_North_Tau_p1 * totalEmE * 1000 + EMG_North_Tau_p0;
		  //G4cout<<"totalEmHit=	"<<totalEmHit<<"	totalEmE=	"<<totalEmE/CLHEP::keV<<"	EMG_North_Sigma=	"<<EMG_North_Sigma<<"	EMG_North_Tau=	"<<EMG_North_Tau<<G4endl;
		  EMGf2->SetParameter(0, 1.);//N
		  EMGf2->SetParameter(1, EMG_North_Tau);//tau
		  EMGf2->SetParameter(2, totalEmE * 1000);//mu
		  EMGf2->SetParameter(3, EMG_North_Sigma);//sigma
		  totalEmE = EMGf2->GetX(G4RandFlat::shoot(0.0, 1.0), 0, 10000) / 1000.0;//After this, totalEmE is the experimentally detected gamma energy in default units of MeV
		  //totalEmE = EMGf2->GetX(G4UniformRand(), 0,10000)/1000.0;//After this, totalEmE is the experimentally detected gamma energy in default units of MeV
		  // Without "/1000.0", it puts the value in units of keV in a variable in units of MeV, which is wrong.
		  //G4double Etemp=totalEmE;
		  //totalEmE=G4RandGauss::shoot(Etemp,TargetReso*Etemp);//G4RandGauss::shoot(mu,sigma)
	  }
	  //excitE width should be added in PrimaryGeneratorAction.cc, while detector resolution should be added in EventAction.cc
	  analysisManager->FillH1(8, totalEmE / CLHEP::keV);//h1 ID=8. Fill histogram using the values in units of keV. Don't change. 
	  analysisManager->FillNtupleDColumn(19, totalEmE / CLHEP::keV);//nt ID=19
  }
  
  if (dHC5)//For South Ge Detector
  {
	  G4int n_hits = dHC5->entries();//SouthSD hit 数
	  //G4cout<<"-----South---"<<"dHC4 n_hits="<<n_hits<<G4endl;
	  analysisManager->FillH1(4, n_hits);//h1 ID=4, distribution of the number of hits
	  totalEmHit = 0;	totalEmE = 0;	ih = -100;	jh = -100; tof = -999; length = 0; velocity = 0;
	  for (G4int i = 0; i < n_hits; i++)//Within this loop, the histo are filled by hit, so the number can be much larger than the event number, the nt can only be filled by event (the last hit)
	  {
		  //if(i>0) break; //using this statement to extract the first hit for position check, the energy would be incomplete cuz only the first hit was recorded.
		  ExG4Hit* hit = (*dHC5)[i];//fHitsCollection中第i个hit的信息，有时一个event就产生1个hit，但打出次级粒子会有多个hit
		  eDep = hit->GetE();
		  totalEmE += eDep;
		  //G4cout<<"-----Clover---"<<"totalEmHit="<<totalEmHit<<" totalEmE="<<totalEmE/CLHEP::keV<<" eDep="<<eDep/CLHEP::keV<<G4endl;
	  }
	  //Outside the loop, the nt are filled by event, so the number of "nt>0" is exactly the event number!
	  analysisManager->FillNtupleIColumn(24, dHC5->entries());//nt ID=24
	  if (totalEmE > 100 * CLHEP::keV)// if you comment this part out, the EMG effect will be disabled.
	  {
		  EMG_South_Sigma = EMG_South_Sigma_p1 * totalEmE * 1000 + EMG_South_Sigma_p0;
		  EMG_South_Tau = EMG_South_Tau_p1 * totalEmE * 1000 + EMG_South_Tau_p0;
		  //G4cout<<"totalEmHit=	"<<totalEmHit<<"	totalEmE=	"<<totalEmE/CLHEP::keV<<"	EMG_South_Sigma=	"<<EMG_South_Sigma<<"	EMG_South_Tau=	"<<EMG_South_Tau<<G4endl;
		  EMGf2->SetParameter(0, 1.);//N
		  EMGf2->SetParameter(1, EMG_South_Tau);//tau
		  EMGf2->SetParameter(2, totalEmE * 1000);//mu
		  EMGf2->SetParameter(3, EMG_South_Sigma);//sigma
		  totalEmE = EMGf2->GetX(G4RandFlat::shoot(0.0, 1.0), 0, 10000) / 1000.0;//After this, totalEmE is the experimentally detected gamma energy in default units of MeV
		  //totalEmE = EMGf2->GetX(G4UniformRand(), 0,10000)/1000.0;//After this, totalEmE is the experimentally detected gamma energy in default units of MeV
		  // Without "/1000.0", it puts the value in units of keV in a variable in units of MeV, which is wrong.
		  //G4double Etemp=totalEmE;
		  //totalEmE=G4RandGauss::shoot(Etemp,TargetReso*Etemp);//G4RandGauss::shoot(mu,sigma)
	  }
	  //excitE width should be added in PrimaryGeneratorAction.cc, while detector resolution should be added in EventAction.cc
	  analysisManager->FillH1(9, totalEmE / CLHEP::keV);//h1 ID=9. Fill histogram using the values in units of keV. Don't change. 
	  analysisManager->FillNtupleDColumn(25, totalEmE / CLHEP::keV);//nt ID=25
  }
  
  analysisManager->AddNtupleRow();//For all of the above dHC1+2+3+4, this statement can be written only once. Without this statement, all the branches in the tree are empty
  //G4cout<<"(4)$ last in event, by event, ExG4EventAction::EndOfEventAction(const G4Event* event)"<<G4endl;
  delete EMGf1;
  delete EMGf2;
}
