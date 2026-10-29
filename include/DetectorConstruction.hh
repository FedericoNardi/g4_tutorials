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
/// \file include/DetectorConstruction.hh
/// \brief Definition of the B4::DetectorConstruction class
/// (adapted from B4/B4a: voxelised cube instead of sampling layers)

#ifndef B4DetectorConstruction_h
#define B4DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"

#include "G4Threading.hh"
#include "globals.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4GlobalMagFieldMessenger;

namespace B4
{

/// Detector construction class to define materials and geometry.
/// The calorimeter is a homogeneous cube divided into kNVoxPerAxis^3 cubic
/// voxels (2x2x2 = 8). The cube is centred at the origin and the beam enters
/// through the -z face.
///
/// Voxels are placements of one logical volume; the copy number is the voxel
/// index  i = ix + kNVoxPerAxis * (iy + kNVoxPerAxis * iz),  ix,iy,iz in [0, kNVoxPerAxis),
/// with ix = 0 on the -x side (same for y, z).
///
/// In addition a transverse uniform magnetic field is defined
/// via G4GlobalMagFieldMessenger class.

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    static constexpr G4int kNVoxPerAxis = 2;
    static constexpr G4int kNVoxels = kNVoxPerAxis * kNVoxPerAxis * kNVoxPerAxis;

    DetectorConstruction() = default;
    ~DetectorConstruction() override = default;

  public:
    G4VPhysicalVolume* Construct() override;
    void ConstructSDandField() override;

    // get methods
    //
    const G4LogicalVolume* GetVoxelLV() const;

  private:
    // methods
    //
    void DefineMaterials();
    G4VPhysicalVolume* DefineVolumes();

    // data members
    //
    static G4ThreadLocal G4GlobalMagFieldMessenger* fMagFieldMessenger;
    // magnetic field messenger

    G4LogicalVolume* fVoxelLV = nullptr;  // the voxel logical volume (all 8 placements)

    G4bool fCheckOverlaps = true;  // option to activate checking of volumes overlaps
};

// inline functions

inline const G4LogicalVolume* DetectorConstruction::GetVoxelLV() const
{
  return fVoxelLV;
}

}  // namespace B4

#endif
