#include "ExG4DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4SDManager.hh"
#include "G4VSensitiveDetector.hh"
#include "G4ios.hh"

ExG4DetectorConstruction::ExG4DetectorConstruction():G4VUserDetectorConstruction()
{}

ExG4DetectorConstruction::~ExG4DetectorConstruction()
{}

G4LogicalVolume* ExG4DetectorConstruction::addCylinder(G4String name, G4double r_in, G4double r_out, G4double hz, G4Material *material, G4ThreeVector pos, G4RotationMatrix *rot, G4Color color)
{
  G4VSolid *solid = new G4Tubs(name, r_in, r_out, hz, 0*deg, 360*deg);
  G4LogicalVolume *volume = new G4LogicalVolume(solid, material, name);
  new G4PVPlacement(rot, pos, volume, name, logicWorld, false, 0, checkOverlaps);
  volume->SetVisAttributes(G4VisAttributes(color));
  return volume;
}

G4VPhysicalVolume* ExG4DetectorConstruction::Construct()
{

	//Coordinates:
	//toward LEGe -z, toward South +z
	//toward North -x, toward Wall +x
	//up -y, down +y
	//In this simulation, (0,0,0) is placed at the center of the target and the center of the chamber

  //set up materials and highest level volumes
  ConstructMaterials(); 

  solidWorld = new G4Box("worldBox",2*m,2*m,2*m);
  logicWorld = new G4LogicalVolume(solidWorld,vacuum,"logicWorld");
  physiWorld = new G4PVPlacement(0,G4ThreeVector(),logicWorld,"physiWorld",0,false,0,checkOverlaps);

  //  set up physical objects and detectors and things
  //MSD12 (delta E)
  G4ThreeVector MSD12_Center_Position(0,0,11.242*mm);
  G4double MSD12_Thickness = 0.01165*mm; // MSD12 active thickness 11.65 um
  G4double MSD12_Radius = 6*mm;
  G4double MSD12_Front_Window_Thickness = 0.000050*mm;//window thickness 0.05 um
  G4double MSD12_Back_Window_Thickness = 0.000300*mm;//window thickness 0.3 um
  logicMSD12 = addCylinder("MSD12", 0*mm, MSD12_Radius, MSD12_Thickness/2,silicon, MSD12_Center_Position, 0, G4Color::Grey()); //DE active area
  addCylinder("MSD12_Chip", MSD12_Radius, 8*mm, 0.140*mm/2, silicon, MSD12_Center_Position, 0, G4Color::Green());
  addCylinder("MSD12_Frame", 8*mm, 15*mm, 3.472*mm/2, FR4, MSD12_Center_Position, 0, G4Color::Red());
  addCylinder("MSD12_Holder", 15.05*mm, 21.6*mm, 6.3 /2*mm, PEEK, MSD12_Center_Position, 0, G4Color(0.0, 1.0, 1.0,0.5));
  addCylinder("MSD12_Window_Front", 0*mm, MSD12_Radius, MSD12_Front_Window_Thickness/2, silicon,
	      MSD12_Center_Position - G4ThreeVector(0,0, MSD12_Thickness/2 + MSD12_Front_Window_Thickness/2),
	      0, G4Color::Grey());
  addCylinder("MSD12_Window_Back", 0*mm, MSD12_Radius, MSD12_Back_Window_Thickness/2, silicon,
	      MSD12_Center_Position + G4ThreeVector(0,0, MSD12_Thickness/2 + MSD12_Back_Window_Thickness /2),
	      0, G4Color::Grey());
  
  //MSD26 (E)
  G4ThreeVector MSD26_Center_Position(0, 0, 16.193 * mm);
  G4double MSD26_Thickness = 1.000*mm; // MSD26 thickness 1000 um
  G4double MSD26_Radius = 13*mm;
  G4double MSD26_Front_Window_Thickness = 0.000050 * mm;//window thickness 0.05 um
  G4double MSD26_Back_Window_Thickness = 0.000800 * mm;//window thickness 0.3 um Al + 0.5 um Si. Treat as 0.8 um Si
  logicMSD26 = addCylinder("MSD26", 0*mm, MSD26_Radius, MSD26_Thickness/2, silicon, MSD26_Center_Position, 0, G4Color::Grey());
  addCylinder("MSD26_Chip", 13*mm, 15*mm, MSD26_Thickness/2, silicon, MSD26_Center_Position, 0, G4Color::Green());
  addCylinder("MSD26_Frame", 15*mm, 21.6*mm, 3.2/2*mm, FR4, MSD26_Center_Position, 0, G4Color::Blue());
  addCylinder("MSD26_Holder", 15 * mm, 21.6 * mm, 1.58 / 2 * mm, PEEK, MSD26_Center_Position + G4ThreeVector(0, 0, 3.2 / 2 * mm + 1.587 / 2 * mm), 0, G4Color(0.0, 1.0, 1.0, 0.5));
  addCylinder("MSD26_Window_Front", 0*mm, MSD26_Radius, MSD26_Front_Window_Thickness/2, silicon,
	      MSD26_Center_Position - G4ThreeVector(0,0,MSD26_Thickness/2 + MSD26_Front_Window_Thickness/2), 0, G4Color::Grey());
  addCylinder("MSD26_Window_Back", 0*mm, MSD26_Radius, MSD26_Back_Window_Thickness/2, silicon,
	      MSD26_Center_Position + G4ThreeVector(0,0,MSD26_Thickness/2 + MSD26_Back_Window_Thickness/2), 0, G4Color::Grey());
  addCylinder("MSD_Support_Rod1", 0*mm, 3.175*mm, 117/2*mm, PEEK, MSD26_Center_Position + G4ThreeVector(12.763 * mm, 12.763 * mm, 3.2 / 2 * mm + 1.587 * mm + 117 / 2 * mm), 0, G4Color(0.0, 1.0, 1.0, 0.5));
  addCylinder("MSD_Support_Rod2", 0*mm, 3.175*mm, 117/2*mm, PEEK, MSD26_Center_Position + G4ThreeVector(-12.763 * mm, 12.763 * mm, 3.2 / 2 * mm + 1.587 * mm + 117 / 2 * mm), 0, G4Color(0.0, 1.0, 1.0, 0.5));
  addCylinder("MSD_Support_Rod3", 0*mm, 3.175*mm, 117/2*mm, PEEK, MSD26_Center_Position + G4ThreeVector(12.763 * mm, -12.763 * mm, 3.2 / 2 * mm + 1.587 * mm + 117 / 2 * mm), 0, G4Color(0.0, 1.0, 1.0, 0.5));
  addCylinder("MSD_Support_Rod4", 0*mm, 3.175*mm, 117/2*mm, PEEK, MSD26_Center_Position + G4ThreeVector(-12.763 * mm, -12.763 * mm, 3.2 / 2 * mm + 1.587 * mm + 117 / 2 * mm), 0, G4Color(0.0, 1.0, 1.0, 0.5));


  //LEGe (X-ray detector). Currently configured for GL0510
  G4ThreeVector LEGe_Center_Position(0, 0, -21.82 * mm); // 10.97+5.6+10.5/2=21.82
  G4double LEGe_Thickness = 10.5*mm;
  G4double LEGe_Radius = 12.5 * mm; 
  G4double LEGe_Dead_Layer_Thickness = 5.0*um;
  G4double LEGE_EndCap_Window_Radius = 18.9*mm;
  G4double LEGe_EndCap_Window_Thickness = 0.13*mm;
  G4double LEGe_To_Window_Distance = 5.6*mm;//distance to active region
  logicLEGe = addCylinder("LEGe", 0*mm, LEGe_Radius, LEGe_Thickness/2, germanium, LEGe_Center_Position, 0, G4Color::Green());
  addCylinder("LEGe_Dead_Layer", 0*mm, LEGe_Radius, LEGe_Dead_Layer_Thickness/2, germanium, LEGe_Center_Position + G4ThreeVector(0, 0, LEGe_Thickness / 2 + LEGe_Dead_Layer_Thickness / 2), 0, G4Color::Green());
  addCylinder("LEGE_EndCap_Window", 0*mm, LEGE_EndCap_Window_Radius, LEGe_EndCap_Window_Thickness/2, beryllium, LEGe_Center_Position + G4ThreeVector(0, 0, LEGe_Thickness / 2 + LEGe_To_Window_Distance), 0, G4Color(0.5, 0.5, 0.5, 0.5));
  addCylinder("LEGE_Housing", LEGE_EndCap_Window_Radius, LEGE_EndCap_Window_Radius+1*mm, 28 * mm, aluminum, LEGe_Center_Position + G4ThreeVector(0, 0, LEGe_Thickness / 2 + LEGe_To_Window_Distance-28.1*mm), 0, G4Color(0.5, 0.5, 0.5, 0.5));
  
  //South (gamma-ray detector)
  G4ThreeVector South_Center_Position(0, 0, 191.7 * mm); // 139.7+12+80.0/2=191.7
  G4double South_Thickness = 80.0 * mm;
  G4double South_Radius = 39.9 * mm;
  G4double South_Dead_Layer_Thickness = 5.0* um;
  G4double South_EndCap_Window_Radius = 50.8 * mm;
  G4double South_EndCap_Window_Thickness = 0.6 * mm;
  G4double South_To_Window_Distance = 6.3 * mm;//distance to active region
  G4VSolid* solidSouth_Full
	  = new G4Tubs("solidSouth_Full", 0 * mm, South_Radius, South_Thickness / 2, 0. * deg, 360. * deg);
  G4VSolid* solidSouth_Hole
	  = new G4Tubs("solidSouth_Hole", 0. * mm, 15. / 2. * mm, South_Thickness / 2, 0. * deg, 360. * deg);
  G4SubtractionSolid* solidSouth
	  = new G4SubtractionSolid("solidSouth", solidSouth_Full, solidSouth_Hole, 0, G4ThreeVector(0, 0, 20 * mm));
  //logicSouth = addCylinder("South", 0 * mm, South_Radius, South_Thickness / 2, germanium, South_Center_Position, 0, G4Color::Blue());
  logicSouth
	  = new G4LogicalVolume(solidSouth, germanium, "logicSouth");
  physiSouth
	  = new G4PVPlacement(0, South_Center_Position, logicSouth, "physiSouth", logicWorld, false, 0, checkOverlaps);
  logicSouth->SetVisAttributes(G4Color(0.0, 0.2, 1.0, 1.0));
  addCylinder("South_Dead_Layer", 0 * mm, South_Radius, South_Dead_Layer_Thickness / 2, germanium, South_Center_Position - G4ThreeVector(0, 0, South_Thickness / 2 + South_Dead_Layer_Thickness / 2), 0, G4Color::Blue());
  addCylinder("South_EndCap_Window", 0 * mm, South_EndCap_Window_Radius, South_EndCap_Window_Thickness / 2, carbon, South_Center_Position - G4ThreeVector(0, 0, South_Thickness / 2 + South_To_Window_Distance), 0, G4Color(0.5, 0.5, 0.5, 0.5));
  addCylinder("South_Housing", South_EndCap_Window_Radius, South_EndCap_Window_Radius + 1 * mm, South_Thickness / 2 + South_To_Window_Distance, aluminum, South_Center_Position, 0, G4Color(0.5, 0.5, 0.5, 0.5));

  //North (gamma-ray detector)
  G4RotationMatrix* Northrot = new G4RotationMatrix;
  Northrot -> rotateY(-M_PI/2.*rad);

  G4ThreeVector North_Center_Position(-184.3 * mm, 0, 0); // 139.7+12+65.2/2=184.3
  G4double North_Thickness = 65.2 * mm;
  G4double North_Radius = 42.4 * mm;
  G4double North_Dead_Layer_Thickness = 5.0 * um;
  G4double North_EndCap_Window_Radius = 50.8 * mm;
  G4double North_EndCap_Window_Thickness = 0.6 * mm;
  G4double North_To_Window_Distance = 6.8 * mm; //distance to active region
  G4VSolid* solidNorth_Full
	  = new G4Tubs("solidNorth_Full", 0 * mm, North_Radius, North_Thickness / 2, 0. * deg, 360. * deg);
  G4VSolid* solidNorth_Hole
	  = new G4Tubs("solidNorth_Hole", 0. * mm, 15. / 2. * mm, North_Thickness / 2, 0. * deg, 360. * deg);
  G4SubtractionSolid* solidNorth
	  = new G4SubtractionSolid("solidNorth", solidNorth_Full, solidNorth_Hole, 0, G4ThreeVector(0, 0, -20 * mm));
  //logicNorth = addCylinder("North", 0 * mm, North_Radius, North_Thickness / 2, germanium, North_Center_Position, 0, G4Color::Red());
  logicNorth
	  = new G4LogicalVolume(solidNorth, germanium, "logicNorth");
  physiNorth
	  = new G4PVPlacement(Northrot, North_Center_Position, logicNorth, "physiNorth", logicWorld, false, 0, checkOverlaps);
  logicNorth->SetVisAttributes(G4Color(1.0, 0.2, 0.0, 1.0));

  addCylinder("North_Dead_Layer", 0 * mm, North_Radius, North_Dead_Layer_Thickness / 2, germanium, North_Center_Position + G4ThreeVector(North_Thickness / 2 + North_Dead_Layer_Thickness / 2,0,0), Northrot, G4Color::Red());
  addCylinder("North_EndCap_Window", 0 * mm, North_EndCap_Window_Radius, North_EndCap_Window_Thickness / 2, carbon, North_Center_Position + G4ThreeVector(North_Thickness / 2 + North_To_Window_Distance,0,0), Northrot, G4Color(0.5, 0.5, 0.5, 0.5));
  addCylinder("North_Housing", North_EndCap_Window_Radius, North_EndCap_Window_Radius + 1 * mm, North_Thickness / 2 + North_To_Window_Distance, aluminum, North_Center_Position, Northrot, G4Color(0.5, 0.5, 0.5, 0.5));

  //  target
  G4RotationMatrix* Rot_target = new G4RotationMatrix; // Rotates X and Z axes only
  Rot_target -> rotateY(-M_PI/4.*rad); // Rotates 90 degrees
  G4double target_thickness = 0.01*mm;
  addCylinder("target", 0*mm,7*mm, target_thickness/2, mylar, G4ThreeVector(0,0,0), Rot_target, G4Color::White());

  //Chamber
//   G4RotationMatrix* Rot = new G4RotationMatrix; // Rotates X and Z axes only
//   Rot -> rotateX(M_PI/2.*rad); // Rotates 90 degrees
//   addCylinder("chamber", 146.3/2*mm, 152.4/2*mm, 168/2*mm, stainless_steel, G4ThreeVector(0,0,0), Rot, G4Color(0.5,0.5,0.5,0.25));

  G4RotationMatrix* RotX90 = new G4RotationMatrix;
  RotX90->rotateX(M_PI / 2. * rad); // Rotates 90 degrees
  G4RotationMatrix* RotY90 = new G4RotationMatrix;
  RotY90->rotateY(M_PI / 2. * rad); // Rotates 90 degrees

  G4VSolid* solidChamber_Full
	  = new G4Tubs("solidChamber_Full", 146.3 / 2. * mm, 152.40 / 2. * mm, 209.55 / 2. * mm, 0. * deg, 360. * deg); //Chamber
  G4VSolid* solidChamber_Hole_South
	  = new G4Tubs("solidChamber_Hole_South", 0. * mm, 76.2 / 2. * mm, 50 / 2. * mm, 0. * deg, 360. * deg);
  G4VSolid* solidChamber_Hole_North
	  = new G4Tubs("solidChamber_Hole_North", 0. * mm, 76.2 / 2. * mm, 50 / 2. * mm, 0. * deg, 360. * deg);
  G4SubtractionSolid* solidChamber_Cut_South_Hole
	  = new G4SubtractionSolid("solidChamber_Cut_South_Hole", solidChamber_Full, solidChamber_Hole_South, RotX90, G4ThreeVector(0, -75, 0 * mm));
  G4SubtractionSolid* solidChamber_Cut_South_and_North_Holes
	  = new G4SubtractionSolid("solidChamber_Cut_South_and_North_Holes", solidChamber_Cut_South_Hole, solidChamber_Hole_North, RotY90, G4ThreeVector(-75, 0, 0 * mm));

  logicChamber
	  = new G4LogicalVolume(solidChamber_Cut_South_and_North_Holes, stainless_steel, "logicChamber");
  physiChamber
	  = new G4PVPlacement(RotX90, G4ThreeVector(0, 0, 0 * mm), logicChamber, "physiChamber", logicWorld, false, 0, checkOverlaps);

  DetectorVisAtt = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0, 0.3));
  //DetectorVisAtt->SetForceWireframe(true); //optional
  //DetectorVisAtt->SetVisibility(false); //optional
  logicChamber->SetVisAttributes(DetectorVisAtt); //Chamber

  G4ThreeVector South_Flange_Center_Position(0, 0, 137.9 * mm);
  G4double South_Flange_Thickness = 3.175 * mm;
  G4double South_Flange_Radius = 58.674 * mm;
  logicSouth_Flange = addCylinder("South_Flange", 0 * mm, South_Flange_Radius, South_Flange_Thickness / 2, stainless_steel, South_Flange_Center_Position, 0, G4Color(1.0, 1.0, 0.0, 0.3));
  addCylinder("South_Flange_Thick", 76.2/2. * mm, 58.674 * mm, 33 / 2. * mm, stainless_steel, South_Flange_Center_Position - G4ThreeVector(0, 0, South_Flange_Thickness / 2 + 33 / 2. * mm), 0, G4Color(1.0, 1.0, 0.0, 0.3));
  addCylinder("South_Flange_Tube", 72.898 /2. * mm, 76.2 / 2. * mm, 39 / 2. * mm, stainless_steel, South_Flange_Center_Position - G4ThreeVector(0, 0, South_Flange_Thickness / 2 + 39 / 2. * mm + 33 * mm), 0, G4Color(1.0, 1.0, 0.0, 0.3));

  G4ThreeVector North_Flange_Center_Position(-137.9 * mm, 0, 0);
  G4double North_Flange_Thickness = 3.175 * mm;
  G4double North_Flange_Radius = 58.674 * mm;
  logicNorth_Flange = addCylinder("North_Flange", 0 * mm, North_Flange_Radius, North_Flange_Thickness / 2, stainless_steel, North_Flange_Center_Position, RotY90, G4Color(1.0, 1.0, 0.0, 0.3));
  addCylinder("North_Flange_Thick", 76.2 / 2. * mm, 58.674 * mm, 33 / 2. * mm, stainless_steel, North_Flange_Center_Position + G4ThreeVector(North_Flange_Thickness / 2 + 33 / 2. * mm,0,0), RotY90, G4Color(1.0, 1.0, 0.0, 0.3));
  addCylinder("North_Flange_Tube", 72.898 / 2. * mm, 76.2 / 2. * mm, 39 / 2. * mm, stainless_steel, North_Flange_Center_Position + G4ThreeVector(North_Flange_Thickness / 2 + 39 / 2. * mm + 33 * mm,0,0), RotY90, G4Color(1.0, 1.0, 0.0, 0.3));

  // visualization attributes ------------------------------------------------
  /*DetectorVisAtt = new G4VisAttributes(G4Colour::Red());
  logicMSD12_Frame->SetVisAttributes(DetectorVisAtt);
  DetectorVisAtt = new G4VisAttributes(G4Colour::Blue());
  logicMSD26_Frame->SetVisAttributes(DetectorVisAtt);
  DetectorVisAtt = new G4VisAttributes(G4Colour::Green());
  logicMSD12_Chip->SetVisAttributes(DetectorVisAtt);
  DetectorVisAtt = new G4VisAttributes(G4Colour::Green());
  logicMSD26_Chip->SetVisAttributes(DetectorVisAtt);
  DetectorVisAtt = new G4VisAttributes(G4Colour::Yellow());
  logicLEGe->SetVisAttributes(DetectorVisAtt);
  DetectorVisAtt = new G4VisAttributes(G4Colour::Cyan());
  logicNorth->SetVisAttributes(DetectorVisAtt);
  DetectorVisAtt = new G4VisAttributes(G4Colour::Gray());
  logicSouth->SetVisAttributes(DetectorVisAtt);
  */

  return physiWorld;
}

void ExG4DetectorConstruction::ConstructMaterials()
{
  G4NistManager* nistManager = G4NistManager::Instance();
  
  G4bool isotopes = false;
  O = nistManager->FindOrBuildElement("O" , isotopes);
  Si = nistManager->FindOrBuildElement("Si", isotopes);
  H = nistManager->FindOrBuildElement("H", isotopes);
  C = nistManager->FindOrBuildElement("C", isotopes);
  Fe = nistManager->FindOrBuildElement("Fe", isotopes);
  Cr = nistManager->FindOrBuildElement("Cr", isotopes);
  Ni = nistManager->FindOrBuildElement("Ni", isotopes);

  germanium=nistManager->FindOrBuildMaterial("G4_Ge");
  gold=nistManager->FindOrBuildMaterial("G4_Au");
  iron=nistManager->FindOrBuildMaterial("G4_Fe");
  carbon=nistManager->FindOrBuildMaterial("G4_C");
  hydrogen=nistManager->FindOrBuildMaterial("G4_H");
  aluminum=nistManager->FindOrBuildMaterial("G4_Al");
  silicon=nistManager->FindOrBuildMaterial("G4_Si");
  csi=nistManager->FindOrBuildMaterial("G4_CESIUM_IODIDE");
  plastic=nistManager->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");	// Scintillator	// (PolyVinylToluene, C_9H_10)
  mylar = nistManager->FindOrBuildMaterial("G4_MYLAR");
  stainless_steel=nistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL");
  beryllium = nistManager->FindOrBuildMaterial("G4_Be");
	
  // vacuum
  // nistManager->FindOrBuildMaterial("G4_Galatic");

  // vacuum : air with low density
  air = nistManager->FindOrBuildMaterial("G4_AIR");
  density = 1.0e-6*air->GetDensity();
  vacuum = nistManager->BuildMaterialWithNewDensity("VACUUM","G4_AIR",density);

  //P10 Gas 780 torr
  argon = nistManager->FindOrBuildMaterial("G4_Ar");
  methane = nistManager->FindOrBuildMaterial("G4_METHANE");
  PressureFactor = 1.0263;  // 780 torr/760 torr=1.0263
  density = PressureFactor*0.001564*g/cm3; // 1564*1.0263=1605
  pressure = PressureFactor*atmosphere;
  P10_780torr = new G4Material("P10_780torr", density, nel=2, kStateGas,temperature= 293.15*kelvin, pressure);
  P10_780torr->AddMaterial(argon, 0.90);
  P10_780torr->AddMaterial(methane, 0.10);
  
  //Epoxy (for FR4)
  epoxy = new G4Material("epoxy", 1.2*g/cm3, 2);
  epoxy->AddElement(H, 2);
  epoxy->AddElement(C, 2);

  //Glass (for FR4)
  SiO2 = new G4Material("SiO2", 2.2*g/cm3, 2);
  SiO2->AddElement(Si, 1);
  SiO2->AddElement(O, 2);
  
  //FR4 (Glass + Epoxy)
  density = 1.85*g/cm3;
  FR4 = new G4Material("FR4", density, 2);
  FR4->AddMaterial(SiO2, 0.528);
  FR4->AddMaterial(epoxy, 0.472);

  stainless_steel = new G4Material("stainless_steel", 8*g/cm3, 3);
  stainless_steel->AddElement(Fe, 0.74);
  stainless_steel->AddElement(Cr, 0.18);
  stainless_steel->AddElement(Ni, 0.08);

  PEEK = new G4Material("PEEK", 1.320*g/cm3, 3);
  PEEK -> AddElement(C, 21);
  PEEK -> AddElement(H, 18);
  PEEK -> AddElement(O, 3);

  G10 = new G4Material("G10", 1.700*g/cm3, 4);
  G10->AddElement(Si, 1);
  G10->AddElement(O, 2);
  G10->AddElement(C, 3);
  G10->AddElement(H, 3);

}

void ExG4DetectorConstruction::ConstructSDandField()
{
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  G4String SDname;
  G4VSensitiveDetector* MSD12SD = new ExG4SD(SDname="/MSD12");//SensitiveDetectorName
  SDman->AddNewDetector(MSD12SD);
  logicMSD12->SetSensitiveDetector(MSD12SD);//attach SD to a logical volume

  G4VSensitiveDetector* MSD26SD = new ExG4SD(SDname="/MSD26");//SensitiveDetectorName
  SDman->AddNewDetector(MSD26SD);
  logicMSD26->SetSensitiveDetector(MSD26SD);

  G4VSensitiveDetector* LEGeSD = new ExG4SD(SDname="/LEGe");//SensitiveDetectorName
  SDman->AddNewDetector(LEGeSD);
  logicLEGe->SetSensitiveDetector(LEGeSD);

  G4VSensitiveDetector* NorthSD = new ExG4SD(SDname="/North");//SensitiveDetectorName
  SDman->AddNewDetector(NorthSD);
  logicNorth->SetSensitiveDetector(NorthSD);//attach SD to a logical volume

  G4VSensitiveDetector* SouthSD = new ExG4SD(SDname="/South");//SensitiveDetectorName
  SDman->AddNewDetector(SouthSD);
  logicSouth->SetSensitiveDetector(SouthSD);//attach SD to a logical volume
}
