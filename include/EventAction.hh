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
/// \file B4/B4a/include/EventAction.hh
/// \brief Definition of the B4a::EventAction class

#ifndef B4aEventAction_h
#define B4aEventAction_h 1

#include "DetectorConstruction.hh"

#include "G4UserEventAction.hh"
#include "globals.hh"

#include <array>

class G4Event;

namespace B4a
{

/// Event action class
///
/// It defines a data member to hold the energy deposit in each voxel:
/// - fEnergy[i]  (i = voxel index, see DetectorConstruction)
/// which are collected step by step via the function
/// - AddVoxel()

class EventAction : public G4UserEventAction
{
  public:
    EventAction() = default;
    ~EventAction() override = default;

    void BeginOfEventAction(const G4Event* event) override;
    void EndOfEventAction(const G4Event* event) override;

    void AddVoxel(G4int voxel, G4double de);

  private:
    static constexpr G4int kNVoxels = B4::DetectorConstruction::kNVoxels;

    std::array<G4double, kNVoxels> fEnergy{};
};

// inline functions

inline void EventAction::AddVoxel(G4int voxel, G4double de)
{
  fEnergy[voxel] += de;
}

}  // namespace B4a
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
