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
  ofstream outresultfile("out.txt", ios::out); // ��������ļ�������outbeamfile���������ʽ�򿪴����ļ���������by run�ģ�Ҳ������RunAction.cc����ļ���
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
    fDHC1ID = sdManager->GetCollectionID("MSD12/SDCollect"); // SensitiveDetectorName,collectionName[0]//����SDName��HCName��ȡ��HCID����fDHCID
    fDHC2ID = sdManager->GetCollectionID("MSD26/SDCollect"); // SensitiveDetectorName,collectionName[0]//����SDName��HCName��ȡ��HCID����fDHCID
    fDHC3ID = sdManager->GetCollectionID("LEGe/SDCollect"); // SensitiveDetectorName,collectionName[0]//����SDName��HCName��ȡ��HCID����fDHCID
    fDHC4ID = sdManager->GetCollectionID("North/SDCollect");  // SensitiveDetectorName,collectionName[0]//����SDName��HCName��ȡ��HCID����fDHCID
    fDHC5ID = sdManager->GetCollectionID("South/SDCollect");
  } // ����SDName retrieve the IDs of the two collections
  // ofstream outresultfile("out.txt",ios::out);//��������ļ�������outbeamfile���������ʽ�򿪴����ļ���������by event�ģ��������������outfile��
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
    G4ExceptionDescription msg;
    msg << "No hits collection of this event found. \n";
    G4Exception("ExG4EventAction::EndOfEventAction()", "ExG4Cod001", JustWarning, msg);
    return;
  }

  // Get hits collections
  ExG4HitsCollection *dHC1 = static_cast<ExG4HitsCollection *>(hce->GetHC(fDHC1ID)); // ����fDHC1ID��hce��ȡ��һ��HitsCollection����ΪdHC1
  ExG4HitsCollection *dHC2 = static_cast<ExG4HitsCollection *>(hce->GetHC(fDHC2ID)); // ����fDHC2ID��hce��ȡ��һ��HitsCollection����ΪdHC2
  ExG4HitsCollection *dHC3 = static_cast<ExG4HitsCollection *>(hce->GetHC(fDHC3ID)); // ����fDHC3ID��hce��ȡ��һ��HitsCollection����ΪdHC3
  ExG4HitsCollection *dHC4 = static_cast<ExG4HitsCollection *>(hce->GetHC(fDHC4ID)); // ����fDHC4ID��hce��ȡ��һ��HitsCollection����ΪdHC4
  ExG4HitsCollection *dHC5 = static_cast<ExG4HitsCollection *>(hce->GetHC(fDHC5ID));

  //   if(!dHC1)
  //   {
  //     G4ExceptionDescription msg;
  //     msg << " Some of hits collection of this event not found!. \n";
  //     G4Exception("ExG4EventAction::EndOfEventAction()","ExG4Code001",JustWarning,msg);
  //     return;
  //   }
  ofstream outresultfile("out.txt", ios::app); // ��������ļ�������outbeamfile����׷�ӷ�ʽ�򿪴����ļ�. by event
  G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
  G4double eDep;
  G4double tof = -999;
  G4int totalEmHit = 0;
  G4double totalEmE = 0.;
  /*
  G4double px[100],py[100],pz[100],dx[100],length=0;//length is the sum of all the dx
  G4double t[100],dt[100],velocity=0;//velocity is dx/dt
  G4int ring1_hit, ring2_hit, ring3_hit;
  memset(px,0,sizeof(px));
  memset(py,0,sizeof(py));
  memset(pz,0,sizeof(pz));
  memset(dx,0,sizeof(dx));//distance between two step points
  memset(dt,0,sizeof(dt));//time difference between two step points
  memset(t,0,sizeof(t));//time stamp of each step point
*/
  if (dHC1) // For MSD12
  {
    G4int n_hits = dHC1->entries(); // MSD12SD hit ��
    // G4cout<<"-----MSD12---"<<"dHC1 n_hits="<<n_hits<<G4endl;
    analysisManager->FillH1(0, n_hits); // h1 ID=0, distribution of the number of hits
    totalEmHit = 0;
    totalEmE = 0;
    for (G4int i = 0; i < n_hits; i++) // Within this loop, the histo are filled by hit, so the number can be much larger than the event number, the nt can only be filled by event (the last hit)
    {
      // ih=-100;	jh=-100;	if(i>0) break; //using this statement to extract the first hit, the energy won't be correct cuz only the first hit was recorded.
      ExG4Hit *hit = (*dHC1)[i]; // fHitsCollection�е�i��hit����Ϣ����ʱһ��event�Ͳ���1��hit��������μ����ӻ��ж��hit
      eDep = hit->GetE();
      totalEmHit++;
      totalEmE += eDep;
    }
    // Outside the loop, the nt are filled by event, so the number of "nt>0" is exactly the event number!, you cannot use if(n>0)Fillnt; here, it's useless.
    analysisManager->FillNtupleIColumn(0, dHC1->entries()); // nt ID=0 filled by event
    if (totalEmE > 0)
    {
      G4double Etemp = totalEmE;
      G4double resolution = 0.01;
      G4double fwhm = resolution * Etemp;
      G4double sigma = fwhm / 2.355;
      totalEmE = G4RandGauss::shoot(Etemp, sigma); // G4RandGauss::shoot(��,��)
    }
    analysisManager->FillH1(3, totalEmE / CLHEP::keV);            // h1 ID=3 filled by event
    analysisManager->FillNtupleDColumn(1, totalEmE / CLHEP::keV); // nt ID=1 filled by event
    //      analysisManager->FillNtupleDColumn(5,t[0]);//nt ID=5 filled by event
  }

  if (dHC2) // For MSD26
  {
    G4int n_hits = dHC2->entries(); // MSD26SD hit ��
    // G4cout<<"-----MSD26---"<<"dHC2 n_hits="<<n_hits<<G4endl;
    analysisManager->FillH1(1, n_hits); // h1 ID=1, distribution of the number of hits
    totalEmHit = 0;
    totalEmE = 0;
    for (G4int i = 0; i < n_hits; i++) // Within this loop, the histo are filled by hit, so the number can be much larger than the event number, the nt can only be filled by event (the last hit)
    {
      ExG4Hit *hit = (*dHC2)[i]; // fHitsCollection�е�i��hit����Ϣ����ʱһ��event�Ͳ���1��hit��������μ����ӻ��ж��hit
      eDep = hit->GetE();
      totalEmHit++;
      totalEmE += eDep;
    }
    // Outside the loop, the nt are filled by event, so the number of "nt>0" is exactly the event number!
    analysisManager->FillNtupleIColumn(6, dHC2->entries()); // nt ID=6
    if (totalEmE > 0)
    {
      G4double Etemp = totalEmE;
      G4double resolution = 0.01;
      G4double fwhm = resolution * Etemp;
      G4double sigma = fwhm / 2.355;
      totalEmE = G4RandGauss::shoot(Etemp, sigma); // G4RandGauss::shoot(��,��)
    }
    analysisManager->FillH1(4, totalEmE / CLHEP::keV);            // h1 ID=4
    analysisManager->FillNtupleDColumn(7, totalEmE / CLHEP::keV); // nt ID=7
    analysisManager->FillNtupleDColumn(11, tof);
    ; // nt ID=11
  }
  if (dHC3) // For LEGe
  {
    G4int n_hits = dHC3->entries(); // LEGeSD hit ��
    // G4cout<<"-----LEGe---"<<"dHC3 n_hits="<<n_hits<<G4endl;
    analysisManager->FillH1(2, n_hits); // h1 ID=2, distribution of the number of hits
    totalEmHit = 0;
    totalEmE = 0;
    for (G4int i = 0; i < n_hits; i++) // Within this loop, the histo are filled by hit, so the number can be much larger than the event number, the nt can only be filled by event (the last hit)
    {
      ExG4Hit *hit = (*dHC3)[i]; // fHitsCollection�е�i��hit����Ϣ����ʱһ��event�Ͳ���1��hit��������μ����ӻ��ж��hit
      eDep = hit->GetE();
      totalEmHit++;
      totalEmE += eDep;
    }
    // Outside the loop, the nt are filled by event, so the number of "nt>0" is exactly the event number!
    analysisManager->FillNtupleIColumn(12, dHC3->entries()); // nt ID=12
    if (totalEmE > 0)
    {
      // https://www.mirion.com/learning-center/lab-experiments/high-resolution-gamma-ray-spectroscopy-with-hpge-detectors-lab-experiments
      // FWHM is proportional to sqrt(a*E + b) where E is the energy deposited and a&b are constants
      G4double Etemp = totalEmE;
      // GL0510/515
      double fwhm1 = 250 * CLHEP::eV;
      double fwhm2 = 550 * CLHEP::eV;

      // GLP-36360/13P4
      // double fwhm1 = 360*CLHEP::eV;
      // double fwhm2 = 585*CLHEP::eV;

      double E1 = 5.9 * CLHEP::keV;
      double E2 = 122 * CLHEP::keV;
      double a = (fwhm1 * fwhm1 - fwhm2 * fwhm2) / (E1 - E2);
      double b = fwhm1 * fwhm1 - a * E1;
      double fwhm = sqrt(a * Etemp + b);
      double sigma = fwhm / 2.355;
      totalEmE = G4RandGauss::shoot(Etemp, sigma); // G4RandGauss::shoot(��,��)
    }
    analysisManager->FillH1(5, totalEmE / CLHEP::keV);             // h1 ID=5
    analysisManager->FillNtupleDColumn(13, totalEmE / CLHEP::keV); // nt ID=13
  }

  if (dHC4) // For North (2749 HPGe detector)
  {
    G4int n_hits = dHC4->entries(); // LEGeSD hit ��
    // G4cout<<"-----DSSD4---"<<"dHC4 n_hits="<<n_hits<<G4endl;
    analysisManager->FillH1(2, n_hits); // h1 ID=2, distribution of the number of hits
    totalEmHit = 0;
    totalEmE = 0;
    for (G4int i = 0; i < n_hits; i++) // Within this loop, the histo are filled by hit, so the number can be much larger than the event number, the nt can only be filled by event (the last hit)
    {
      ExG4Hit *hit = (*dHC4)[i]; // fHitsCollection�е�i��hit����Ϣ����ʱһ��event�Ͳ���1��hit��������μ����ӻ��ж��hit
      eDep = hit->GetE();
      totalEmHit++;
      totalEmE += eDep;
    }
    // Outside the loop, the nt are filled by event, so the number of "nt>0" is exactly the event number!
    analysisManager->FillNtupleIColumn(18, dHC4->entries()); // nt ID=18

    // https://www.mirion.com/learning-center/lab-experiments/high-resolution-gamma-ray-spectroscopy-with-hpge-detectors-lab-experiments
    // FWHM is proportional to sqrt(a*E + b) where E is the energy deposited and a&b are constants
    G4double Etemp = totalEmE;
    // GL0510/515
    double fwhm1 = 1.93 * CLHEP::keV;
    double fwhm2 = 2.03 * CLHEP::keV;

    // GLP-36360/13P4
    // double fwhm1 = 360*CLHEP::eV;
    // double fwhm2 = 585*CLHEP::eV;

    double E1 = 1173.2 * CLHEP::keV;
    double E2 = 1332.5 * CLHEP::keV;
    double a = (fwhm1 * fwhm1 - fwhm2 * fwhm2) / (E1 - E2);
    double b = fwhm1 * fwhm1 - a * E1;
    double fwhm = sqrt(a * Etemp + b);
    double sigma = fwhm / 2.355;
    totalEmE = G4RandGauss::shoot(Etemp, sigma); // G4RandGauss::shoot(��,��)

    analysisManager->FillNtupleDColumn(19, totalEmE / CLHEP::keV); // nt ID=19
    analysisManager->FillNtupleDColumn(23, tof);                   // nt ID=23
  }
  if (dHC5) // For South (2750 HPGe detector **THE THREE RESOLUTION MEASUREMENTS ARE NOT ENTIRELY LINEAR IN THIS DETECTOR**)
  {
    G4int n_hits = dHC5->entries(); // LEGeSD hit ��
    // G4cout<<"-----DSSD4---"<<"dHC4 n_hits="<<n_hits<<G4endl;
    analysisManager->FillH1(2, n_hits); // h1 ID=2, distribution of the number of hits
    totalEmHit = 0;
    totalEmE = 0;
    for (G4int i = 0; i < n_hits; i++) // Within this loop, the histo are filled by hit, so the number can be much larger than the event number, the nt can only be filled by event (the last hit)
    {
      ExG4Hit *hit = (*dHC5)[i]; // fHitsCollection�е�i��hit����Ϣ����ʱһ��event�Ͳ���1��hit��������μ����ӻ��ж��hit
      eDep = hit->GetE();
      totalEmHit++;
      totalEmE += eDep;
    }
    // Outside the loop, the nt are filled by event, so the number of "nt>0" is exactly the event number!
    analysisManager->FillNtupleIColumn(24, dHC5->entries()); // nt ID=24

    // https://www.mirion.com/learning-center/lab-experiments/high-resolution-gamma-ray-spectroscopy-with-hpge-detectors-lab-experiments
    // FWHM is proportional to sqrt(a*E + b) where E is the energy deposited and a&b are constants
    G4double Etemp = totalEmE;
    // GL0510/515
    double fwhm1 = 1.92 * CLHEP::keV;
    double fwhm2 = 2.05 * CLHEP::keV;

    // GLP-36360/13P4
    // double fwhm1 = 360*CLHEP::eV;
    // double fwhm2 = 585*CLHEP::eV;

    double E1 = 1173.2 * CLHEP::keV;
    double E2 = 1332.5 * CLHEP::keV;
    double a = (fwhm1 * fwhm1 - fwhm2 * fwhm2) / (E1 - E2);
    double b = fwhm1 * fwhm1 - a * E1;
    double fwhm = sqrt(a * Etemp + b);
    double sigma = fwhm / 2.355;
    totalEmE = G4RandGauss::shoot(Etemp, sigma); // G4RandGauss::shoot(��,��)

    analysisManager->FillNtupleDColumn(25, totalEmE / CLHEP::keV); // nt ID=19
    analysisManager->FillNtupleDColumn(29, tof);                   // nt ID=23
  }
  // G4cout<<analysisManager->GetH1(1)->mean();
  analysisManager->AddNtupleRow(); // For all of the above dHC1+2+3+4, this statement can be written only once. Without this statement, all the branches in the tree are empty
  //  tof=G4RandGauss::shoot(tof,0.5);
  // G4cout<<"(4)$ last in event, by event, ExG4EventAction::EndOfEventAction(const G4Event* event)"<<G4endl;
}
