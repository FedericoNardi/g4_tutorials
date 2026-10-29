//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file src/DetectorConstruction.cc
/// \brief Implementation of the B4::DetectorConstruction class

#include "DetectorConstruction.hh"

#include "G4AutoDelete.hh"
#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"

namespace B4
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal G4GlobalMagFieldMessenger* DetectorConstruction::fMagFieldMessenger = nullptr;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Define materials
  DefineMaterials();

  // Define volumes
  return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
  // Voxel material: PbF2 is not in the NIST database, define it from elements
  auto nistManager = G4NistManager::Instance();
  G4double a;  // mass of a mole;
  G4double z;  // z=mean number of protons;
  G4double density;
  auto pbf2 = new G4Material("PbF2", density = 7.77 * g / cm3, 2);
  pbf2->AddElement(nistManager->FindOrBuildElement("Pb"), 1);
  pbf2->AddElement(nistManager->FindOrBuildElement("F"), 2);

  // Vacuum
  new G4Material("Galactic", z = 1., a = 1.01 * g / mole, density = universe_mean_density,
                 kStateGas, 2.73 * kelvin, 3.e-18 * pascal);

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::DefineVolumes()
{
  // Geometry parameters
  G4double cubeSize = 10. * cm;
  auto voxelSize = cubeSize / kNVoxPerAxis;

  auto worldSize = 1.2 * cubeSize;

  // Get materials
  auto defaultMaterial = G4Material::GetMaterial("Galactic");
  auto voxelMaterial = G4Material::GetMaterial("PbF2");

  if (!defaultMaterial || !voxelMaterial) {
    G4ExceptionDescription msg;
    msg << "Cannot retrieve materials already defined.";
    G4Exception("DetectorConstruction::DefineVolumes()", "MyCode0001", FatalException, msg);
  }

  //
  // World
  //
  auto worldS = new G4Box("World",  // its name
                          worldSize / 2, worldSize / 2, worldSize / 2);  // its size

  auto worldLV = new G4LogicalVolume(worldS,  // its solid
                                     defaultMaterial,  // its material
                                     "World");  // its name

  auto worldPV = new G4PVPlacement(nullptr,  // no rotation
                                   G4ThreeVector(),  // at (0,0,0)
                                   worldLV,  // its logical volume
                                   "World",  // its name
                                   nullptr,  // its mother  volume
                                   false,  // no boolean operation
                                   0,  // copy number
                                   fCheckOverlaps);  // checking overlaps

  //
  // Calorimeter: envelope cube holding the voxels
  //
  auto calorimeterS = new G4Box("Calorimeter",  // its name
                                cubeSize / 2, cubeSize / 2, cubeSize / 2);  // its size

  auto calorLV = new G4LogicalVolume(calorimeterS,  // its solid
                                     defaultMaterial,  // its material
                                     "Calorimeter");  // its name

  new G4PVPlacement(nullptr,  // no rotation
                    G4ThreeVector(),  // at (0,0,0)
                    calorLV,  // its logical volume
                    "Calorimeter",  // its name
                    worldLV,  // its mother  volume
                    false,  // no boolean operation
                    0,  // copy number
                    fCheckOverlaps);  // checking overlaps

  //
  // Voxels
  //
  auto voxelS = new G4Box("Voxel",  // its name
                          voxelSize / 2, voxelSize / 2, voxelSize / 2);  // its size

  fVoxelLV = new G4LogicalVolume(voxelS,  // its solid
                                 voxelMaterial,  // its material
                                 "Voxel");  // its name

  for (G4int iz = 0; iz < kNVoxPerAxis; ++iz) {
    for (G4int iy = 0; iy < kNVoxPerAxis; ++iy) {
      for (G4int ix = 0; ix < kNVoxPerAxis; ++ix) {
        auto centre = [&](G4int i) { return (i + 0.5) * voxelSize - cubeSize / 2; };
        G4int copyNo = ix + kNVoxPerAxis * (iy + kNVoxPerAxis * iz);
        new G4PVPlacement(nullptr,  // no rotation
                          G4ThreeVector(centre(ix), centre(iy), centre(iz)),  // its position
                          fVoxelLV,  // its logical volume
                          "Voxel",  // its name
                          calorLV,  // its mother  volume
                          false,  // no boolean operation
                          copyNo,  // copy number = voxel index
                          fCheckOverlaps);  // checking overlaps
      }
    }
  }

  //
  // print parameters
  //
  G4cout << G4endl << "------------------------------------------------------------" << G4endl
         << "---> The calorimeter is a " << cubeSize / cm << " cm cube of "
         << voxelMaterial->GetName() << " divided into " << kNVoxPerAxis << "x" << kNVoxPerAxis
         << "x" << kNVoxPerAxis << " voxels of " << voxelSize / cm << " cm" << G4endl
         << "------------------------------------------------------------" << G4endl;

  //
  // Visualization attributes
  //
  worldLV->SetVisAttributes(G4VisAttributes::GetInvisible());
  calorLV->SetVisAttributes(G4VisAttributes::GetInvisible());
  fVoxelLV->SetVisAttributes(G4VisAttributes(G4Colour::White()));

  //
  // Always return the physical World
  //
  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if
  // the field value is not zero.
  G4ThreeVector fieldValue;
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);

  // Register the field messenger for deleting
  G4AutoDelete::Register(fMagFieldMessenger);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B4
