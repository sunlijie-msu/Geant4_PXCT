#include "ExG4PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"

#include "G4DynamicParticle.hh"
#include "G4PhysicalConstants.hh"
//#include "G4SystemOfUnits.hh"
#include "G4DecayProducts.hh"
#include "G4LorentzVector.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4PrimaryParticle.hh"
#include "Randomize.hh"
using namespace std;
ExG4PrimaryGeneratorAction::ExG4PrimaryGeneratorAction()
	: G4VUserPrimaryGeneratorAction(),
	fParticleGun(0)
{
	// The basic units are :
	//		millimeter              (millimeter)
	// 		nanosecond              (nanosecond)
	// 		Mega electron Volt      (MeV)
	// 		positron charge         (eplus)
	// 		degree Kelvin           (kelvin)
	//      the amount of substance (mole)
	//      luminous intensity      (candela)
	// 		radian                  (radian)
	//      steradian               (steradian)
	//	To output the data on the unit you wish you must divide the data by the corresponding unit.
		// /CLHEP::keV is equivalent to *1000
		// *CLHEP::keV is equivalent to /1000

	G4int nofParticles = 1;
	fParticleGun = new G4ParticleGun(nofParticles);

	// 	xygaus = new TF2("xygaus","xygaus",-15,15,-15,15);// predefined 2D Gaussian function  // 2D Gaussian beam spot
	// 	xygaus->SetParameters(1,0,3,0,3); // amplitude, mean_x, sigma_x, mean_y, sigma_y  in units of mm. // 2D Gaussian beam spot
	// 	xygaus->SetNpx(200);  // 2D Gaussian beam spot
	// 	xygaus->SetNpy(200);  // 2D Gaussian beam spot
	Rmax = 1 * CLHEP::mm;  // 2D Uniform beam spot // in units of mm. G4ThreeVector(mm)
	tiltAngle = 45. * CLHEP::deg; // Tilt angle of 45 degrees
	cosAngle = cos(tiltAngle);
	sinAngle = sin(tiltAngle);
	//G4cout << "++++++++++++  cosAngle=" << cosAngle << "  sinAngle=" << sinAngle << G4endl;

	//pFile = new TFile("depth.root", "read"); // put the depth.root file under the build directory
	//pFile=new TFile("/mnt/hgfs/HPGe/P26_pz.root","read");
	//pFile=new TFile("/mnt/hgfs/HPGe/Si25_pz.root","read");
	//pFile=new TFile("/mnt/hgfs/HPGe/Al22_pz.root","read");
	//pFile=new TFile("/mnt/hgfs/HPGe/S27simulate.root","read");
	//fout = fopen ("out.txt", "w");

//	numpeaks=1;//23 or 53 for 29Na, 46 or 88 for 30Na. with Doppler, 62 for 29Na, 1 for test
//	branch_lit_tot=0;
	//fin=fopen ("/mnt/hgfs/HPGe/Gamma30Naprecursor.dat","r");
//	wheretorun=3;
// 	if(wheretorun==1)//fishtank
// 	{
// 		ifstream fin("/user/sunli/HPGe/Gamma29Nadoppler.dat",ios::in);//fishtank下fscanf %lf报错
// 		for(int ii=0;ii<numpeaks;ii++)//fishtank
// 		{
// 			fin>>Eg_lit[ii]>>branch_lit[ii]>>En_lit[ii]>>Halflife_lit[ii];//Ecm keV; 1 ps = 0.001
// 		}//fishtank
// 	}
// 	if(wheretorun==2)//VMware
// 	{
// 		FILE *fin=fopen ("/mnt/hgfs/HPGe/Gamma29Nadoppler.dat","r");//VMware下ifstream报错
// 		for(int ii=0;ii<numpeaks;ii++)//VMware
// 	 	{
// 	 		fscanf(fin, "%lf", &Eg_lit[ii]);//keV
// 	 		fscanf(fin, "%lf", &branch_lit[ii]);
// 	 		fscanf(fin, "%lf", &En_lit[ii]);//Ecm keV
// 	 		fscanf(fin, "%lf", &Halflife_lit[ii]);//1 ps = 0.001
// 		}//VMware
// 	}
// 	if(wheretorun==3)//VMware test
// 	{
// 		FILE *fin=fopen ("/mnt/hgfs/HPGe/Onetest.dat","r");//VMware下ifstream报错
// 		for(int ii=0;ii<numpeaks;ii++)//VMware
// 		{
// 			fscanf(fin, "%lf", &Eg_lit[ii]);//keV
// 			fscanf(fin, "%lf", &branch_lit[ii]);
// 			fscanf(fin, "%lf", &En_lit[ii]);//Ecm keV
// 			fscanf(fin, "%lf", &Halflife_lit[ii]);//1 ps = 0.001
// 		}//VMware
// 	}
// 	for(int ii=0;ii<numpeaks;ii++)
// 	{
// 		branch_lit_tot+=branch_lit[ii];
// 		branch_lit_integ[ii]=branch_lit_tot;
// 	}
// 	for(int ii=0;ii<numpeaks+1;ii++)
// 	{
// 		G4cout<<"Eg=	"<<Eg_lit[ii]<<"	keV	br=	"<<branch_lit[ii]<<"	integ=	"<<branch_lit_integ[ii]<<"	En=	"<<En_lit[ii]<<"	keV	T=	"<<Halflife_lit[ii]<<"	ns"<<G4endl;
// 	}
// 	G4cout<<"brtot=	"<<branch_lit_tot<<G4endl;
	//std::ofstream outfile(filename,std::ios::out);//定义输出文件流对象outfile，以输出方式打开磁盘文件, formerly non-existent file can be created automatically
	//depth_3He = new TH1F("depth_3He", "depth_3He", 100, -1.5, 298.5);//unit nm, this statement cannot be eliminated, the new TH1F cannot be named the same as the TH1F in the input root
	// 	py_impl = new TH1F("py_impl","py_impl", 50,-25,25);//unit mm, this statement cannot be eliminated, the new TH1F cannot be named the same as the TH1F in the input root
	// 	pz_impl = new TH1D("pz_impl","pz_impl", 100,0,100);//unit um, this statement cannot be eliminated, the new TH1F cannot be named the same as the TH1F in the input root
	G4cout << "1$ before run, by run, ExG4PrimaryGeneratorAction::ExG4PrimaryGeneratorAction()" << G4endl;
}

ExG4PrimaryGeneratorAction::~ExG4PrimaryGeneratorAction()
{
	G4cout << "8$ after run, by run, ExG4PrimaryGeneratorAction::~ExG4PrimaryGeneratorAction()" << G4endl;
	//pFile->Close();
	//fclose(fout);
	//	fclose(fin);//VMware
	delete fParticleGun;
}

void ExG4PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	//G4cout<<"(1)$ before event, by event, ExG4PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent)"<<G4endl;
	gRandom->SetSeed(0);
	//	xygaus->GetRandom2(x0,y0); // 2D Gaussian beam spot  // Return two random numbers to x0 and y0 following this function shape.
	r_beam = sqrt(gRandom->Uniform(0, 1)) * Rmax;  // 2D Uniform beam spot
	theta_beam = gRandom->Uniform(0, 2. * 3.14159);  // 2D Uniform beam spot
	x0 = r_beam * cos(theta_beam);  // 2D Uniform beam spot
	y0 = r_beam * sin(theta_beam);  // 2D Uniform beam spot
	//depth_3He = (TH1F*)pFile->Get("depth");//Get("depth");//read implantation depth from root for 3He-implanted gold target
	//z0 = depth_3He->GetRandom() * CLHEP::nm;//Return a random number distributed according the histogram depth_3He
	z0=0*CLHEP::um;
	//G4cout<<"++++++++++++  x0="<<x0/CLHEP::mm<<"  y0="<<y0/CLHEP::mm<<"  z0="<<z0/CLHEP::nm<<G4endl;

	// Apply rotation around the Y-axis
	double x0_tilted = x0 * cosAngle + z0 * sinAngle;
	double y0_tilted = y0; // y-coordinate remains unchanged
	double z0_tilted = -x0 * sinAngle + z0 * cosAngle;

	// Update x0, y0, z0 with the tilted coordinates
	x0 = x0_tilted;
	y0 = y0_tilted;
	z0 = z0_tilted;

	// Now x0, y0, z0 are the coordinates of the source distribution tilted by 45 degrees

	int AngularDistribution = 0;
	if (AngularDistribution == 1) // slower than AD = 0
	{
		ad = new TF1("ad", "[0]+[1]*1./2.*(3.*x*x-1.)+[2]*1./8.*(35.*x*x*x*x-30.*x*x+3.)", -1.0, 1.0); // x means costheta
		ad->SetParameter(0, 1.0); // set value of parameter 0 (a0=1 always)
		ad->SetParameter(1, 0.0); // set value of parameter 1 (a2=0 means isotropic)
		ad->SetParameter(2, 0.0); // set value of parameter 2 (a4)
		costheta_p1 = ad->GetRandom(0.71, 1.0); // starting point, if angular distribution is needed.
	}

	if (AngularDistribution == 0)
	{
		costheta_p1 = CLHEP::RandFlat::shoot(-1.0, 1.0); // alpha particles emitted at forward angles along z-axis. starting point!
		//(0.50,1.) = <60 deg, (0.57,1.) = <55 deg (theta alpha 6390 31S), (0.71,1.) = <45 deg, (0.78,1.) = <39 deg, (0.90,1.) = <25.8 deg, (0.94,1.) = <20 deg for final DSL1 restuls, (0.97,1.) = <14 deg, (0.979,1.) = <11.6 deg, (-1.,1.) 180 deg
		//0.71 for 31S 6390, 0.79 for 23Mg 7787
	}
	//single particle isotropic emission
	phi_p1 = CLHEP::RandFlat::shoot(0., 2. * 3.14159);//isotropy or not, both use this statement
	theta_p1 = acos(costheta_p1);
	//G4cout<<"------------ phi="<<phi_p1<<' '<<"costheta="<<costheta_p1<<G4endl;
	sintheta_p1 = sqrt(1.0 - costheta_p1 * costheta_p1);//isotropy
	dirx = sintheta_p1 * cos(phi_p1);//isotropy
	diry = sintheta_p1 * sin(phi_p1);//isotropy
	dirz = costheta_p1;//isotropy
	Eejectile = 7.000 * CLHEP::MeV; // energy of the ejectile
	//calculate Eejectile from selected theta_p1, here it should be "+" sign before sqrt
	//Eejectile = (sqrt(Abeam * Aejectile * Ebeam) / (Arecoil + Aejectile) * costheta_p1 + sqrt(((Arecoil - Abeam) / (Arecoil + Aejectile) + Abeam * Aejectile / ((Arecoil + Aejectile) * (Arecoil + Aejectile)) * (costheta_p1 * costheta_p1)) * Ebeam + Arecoil / (Arecoil + Aejectile) * Qvalue)) * (sqrt(Abeam * Aejectile * Ebeam) / (Arecoil + Aejectile) * costheta_p1 + sqrt(((Arecoil - Abeam) / (Arecoil + Aejectile) + Abeam * Aejectile / ((Arecoil + Aejectile) * (Arecoil + Aejectile)) * (costheta_p1 * costheta_p1)) * Ebeam + Arecoil / (Arecoil + Aejectile) * Qvalue));
	//G4cout<<"++++++++++++  Ealpha=	"<<Eejectile/CLHEP::MeV<<"	MeV	theta_p1=	"<<theta_p1/CLHEP::deg<<"	deg"<<G4endl; // formula verified
	//G4ParticleDefinition* particle = G4IonTable::GetIonTable()->GetIon(Zejectile, Aejectile, Excitation_ejectile);
	//G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle("alpha");
	//G4cout<<"++++++++++++  x0="<<x0/CLHEP::mm<<"  y0="<<y0/CLHEP::mm<<"  z0="<<z0/CLHEP::mm<<G4endl;
	position = G4ThreeVector(x0 * CLHEP::mm, y0 * CLHEP::mm, z0 * CLHEP::mm);// 2D beam spot
	//position=G4ThreeVector(0.*CLHEP::mm,0.*CLHEP::mm,0*CLHEP::mm);// point source, usually for check, validation and test
	momentumDirection = G4ThreeVector(dirx, diry, dirz);//isotropy
	//momentumDirection = G4ThreeVector(-1,0,0);//(0,0,+1) unidirectional toward Z+ axis, usually for check, validation and test
	fParticleGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle("gamma"));
	fParticleGun->SetParticleEnergy(Eejectile);
	fParticleGun->SetParticlePosition(position);
	fParticleGun->SetParticleMomentumDirection(momentumDirection);
	//fParticleGun->SetParticleCharge(ioncharge);
	//fParticleGun->SetParticleTime(1000*CLHEP::ns);//meaningless, the generation time of particle
	fParticleGun->GeneratePrimaryVertex(anEvent);









	// *** for the second particle emitted in the same event *** In this case, it's recoiling 31S
//	particle = G4IonTable::GetIonTable()->GetIon(Zrecoil, Arecoil, Excitation_recoil); // the final state is determined by the /data/PhotonEvaporation3.2/z16.a31 file, the second gamma ray will only be emitted after the recoil stops. Gean4 can't handle the Doppler shift for the second gamma in a cascade.
//	particle->SetPDGStable(false); // for Doppler
//	particle->SetPDGLifeTime(tau); // this is lifetime τ = 1.443*T1/2 // for Doppler
//
//	Erecoil = Ebeam + Qvalue - Eejectile; // Conservation of total energy. Get Erecoil from Eejectile.
//	Pejectile = sqrt((Eejectile / (Aejectile * atomic_mass_unit) + 1) * (Eejectile / (Aejectile * atomic_mass_unit) + 1) - 1) * (Aejectile * atomic_mass_unit); // momentum in units of MeV/c
//	Precoil = sqrt((Erecoil / (Arecoil * atomic_mass_unit) + 1) * (Erecoil / (Arecoil * atomic_mass_unit) + 1) - 1) * (Arecoil * atomic_mass_unit); // momentum in units of MeV/c
//	Pbeam = sqrt((Ebeam / (Abeam * atomic_mass_unit) + 1) * (Ebeam / (Abeam * atomic_mass_unit) + 1) - 1) * (Abeam * atomic_mass_unit); // momentum in units of MeV/c
//	//sintheta_recoil=sqrt(2*Aejectile*Eejectile)*sqrt(1-costheta_p1*costheta_p1)/sqrt(2*Arecoil*Erecoil); // Conservation of momentum
//	sintheta_recoil = Pejectile * sintheta_p1 / Precoil; // Conservation of momentum.  Get theta_recoil from theta_ejectile.
//	//theta_recoil=-0.12150112*acos(costheta_p1)*acos(costheta_p1) + 0.32702559*acos(costheta_p1) - 0.00048110; // approximation test
//	costheta_recoil = sqrt(1.0 - sintheta_recoil * sintheta_recoil);
//	theta_recoil = asin(sintheta_recoil);
//	phi_recoil = CLHEP::RandFlat::shoot(0., 2. * 3.14159);//isotropy
//	//G4cout<<"------------ phi="<<phi<<' '<<"costheta="<<costheta<<G4endl;
//	//sintheta_recoil=sqrt(1.0-costheta_recoil*costheta_recoil);//isotropy
//	dirx = sintheta_recoil * cos(phi_recoil);//isotropy
//	diry = sintheta_recoil * sin(phi_recoil);//isotropy
//	dirz = costheta_recoil;//isotropy
//	//calculate Erecoil from selected theta_recoil, here it should be "-" sign before sqrt
//	//Erecoil=(sqrt(Abeam*Arecoil*Ebeam)/(Aejectile+Arecoil)*costheta_recoil-sqrt(((Aejectile-Abeam)/(Aejectile+Arecoil)+Abeam*Arecoil/((Aejectile+Arecoil)*(Aejectile+Arecoil))*(costheta_recoil*costheta_recoil))*Ebeam+Aejectile/(Aejectile+Arecoil)*Qvalue))*(sqrt(Abeam*Arecoil*Ebeam)/(Aejectile+Arecoil)*costheta_recoil-sqrt(((Aejectile-Abeam)/(Aejectile+Arecoil)+Abeam*Arecoil/((Aejectile+Arecoil)*(Aejectile+Arecoil))*(costheta_recoil*costheta_recoil))*Ebeam+Aejectile/(Aejectile+Arecoil)*Qvalue)); // check
//// 	G4cout<<"E_p1=	"<<Eejectile/CLHEP::MeV<<"	MeV	theta_p1=	"<<theta_p1/CLHEP::deg<<"	deg	P_p1=	"<<Pejectile<<"	MeV/c"<<G4endl; // formula verified
////  	G4cout<<"E_recoil=	"<<Erecoil/CLHEP::MeV<<"	MeV	theta_recoil=	"<<theta_recoil/CLHEP::deg<<"	deg	P_recoil=	"<<Precoil<<"	MeV/c"<<G4endl; // formula verified
////	G4cout<<Erecoil/CLHEP::MeV<<"	MeV	"<<theta_recoil/CLHEP::deg<<"	deg	"<<Precoil<<"	MeV/c	"<<Eejectile/CLHEP::MeV<<"	MeV	"<<theta_p1/CLHEP::deg<<"	deg	"<<Pejectile<<"	MeV/c"<<G4endl; // formula verified
//
//	//position=G4ThreeVector(x0*CLHEP::mm,y0*CLHEP::mm,z0*CLHEP::mm);// 2D beam spot
//	//position=G4ThreeVector(0.*CLHEP::mm,0.*CLHEP::mm,0.*CLHEP::mm);// point source, usually for check, validation and test
//	momentumDirection = G4ThreeVector(dirx, diry, dirz);//isotropy
//	//momentumDirection = G4ThreeVector(0,0,-1);//(0,0,+1) unidirectional toward Z+ axis, usually for check, validation and test
//	fParticleGun->SetParticleDefinition(particle);
//	fParticleGun->SetParticleEnergy(Erecoil);
//	fParticleGun->SetParticlePosition(position);//recoil uses the same position as projectile
//	fParticleGun->SetParticleMomentumDirection(momentumDirection);
//	fParticleGun->SetParticleCharge(ioncharge);
//	//fParticleGun->SetParticleTime(1000*CLHEP::ns);//meaningless, the generation time of particle
//	fParticleGun->GeneratePrimaryVertex(anEvent);// if you do not need the second particle in the same event, comment this statement out
}

