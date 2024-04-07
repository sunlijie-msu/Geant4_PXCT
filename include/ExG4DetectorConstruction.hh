#ifndef ExG4DetectorConstruction_h
#define ExG4DetectorConstruction_h

#include "G4VUserDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"
#include "G4String.hh"
#include "G4RotationMatrix.hh"
#include "G4NistManager.hh"
#include "G4FieldManager.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh" 
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SDManager.hh"
#include "G4VSensitiveDetector.hh"
#include "globals.hh"
#include "G4ios.hh"
#include "ExG4SD.hh"
#include <vector>

class G4VPhysicalVolume;
class G4Material;
class G4VSensitiveDetector;
class G4VisAttributes;
class G4GenericMessenger;

//Detector construction
class ExG4DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  ExG4DetectorConstruction();
  virtual ~ExG4DetectorConstruction();

  virtual G4VPhysicalVolume* Construct();
  virtual void ConstructSDandField();
    
  void ConstructMaterials();
  //convenience function to add a cylinder, since the current code is pretty messy
  G4LogicalVolume* addCylinder(G4String name, G4double r_in, G4double r_out, G4double hz,
		   G4Material *material, G4ThreeVector pos, G4RotationMatrix *rot,
		   G4Color color);

private:
  const G4bool checkOverlaps = true;
  
  G4VSolid* solidWorld;
  G4LogicalVolume* logicWorld;
  G4VPhysicalVolume* physiWorld;
  
  G4VPhysicalVolume* physiMSD12;
  G4VPhysicalVolume* physiMSD12_Chip;
  G4VPhysicalVolume* physiMSD12_Frame;
  G4VPhysicalVolume* physiMSD12_Window_Front;
  G4VPhysicalVolume* physiMSD12_Window_Back;
  G4VPhysicalVolume* physiMSD26;
  G4VPhysicalVolume* physiMSD26_Chip;
  G4VPhysicalVolume* physiMSD26_Frame;
  G4VPhysicalVolume* physiMSD26_Window_Front;
  G4VPhysicalVolume* physiMSD26_Window_Back;
  G4VPhysicalVolume* physiLEGe;
  G4VPhysicalVolume* physiNorth;
  G4VPhysicalVolume* physiSouth;
  G4VPhysicalVolume* physiChamber;

  G4LogicalVolume* logicMSD12;
  G4LogicalVolume* logicMSD26;
  G4LogicalVolume* logicLEGe;
  G4LogicalVolume* logicNorth;
  G4LogicalVolume* logicSouth;

  //Materials
  G4Element* O;
  G4Element* Si;
  G4Element* H;
  G4Element* C;
  G4Element* Fe;
  G4Element* Cr;
  G4Element* Ni;

  G4Material* silicon;
  G4Material* germanium;
  G4Material* gold;
  G4Material* iron;
  G4Material* carbon;
  G4Material* hydrogen;
  G4Material* aluminum;
  G4Material* csi;
  G4Material* vacuum;
  G4Material* plastic;
  G4Material *P10_780torr;
  G4Material *air;
  G4Material *methane;
  G4Material *argon;
  G4Material* mylar;
  G4Material* stainless_steel;
  G4Material* SiO2;
  G4Material* epoxy;
  G4Material* FR4;
  G4Material* PEEK;
  G4Material* G10;
  G4Material *beryllium;

  G4double density, temperature, pressure, PressureFactor;
  G4int nel;

  G4VisAttributes* DetectorVisAtt; //TODO: get rid of this; I don't think it'll actually be needed once I finish updating things
};

#endif
