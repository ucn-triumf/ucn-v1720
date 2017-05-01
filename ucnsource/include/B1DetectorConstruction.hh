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
// $Id: B1DetectorConstruction.hh 69565 2013-05-08 12:35:31Z gcosmo $
//
/// \file B1DetectorConstruction.hh
/// \brief Definition of the B1DetectorConstruction class

#ifndef B1DetectorConstruction_h
#define B1DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

/// Class to hold shapes that will be used in add and subtract
/// to build more complicated shapes
class ShapeAndLoc{
public:
  ShapeAndLoc( G4VSolid *asolid, G4ThreeVector *aPos ){
    fSolid = asolid;
    fPos = new G4ThreeVector( apos );
  }
  ~ShapeAndLoc(){ if (fPos) delete fPos; }
  G4ThreeVector * GetPos(){ return fPos; }
  G4VSolid * GetSolid(){ return fSolid; }
protected:
  G4ThreeVector * fPos;
  G4VSolid * fSolid;
};

/// Class to hold rotated shape.
class ShapeLocRot : ShapeAndLoc {
public:
  ShapeLocRot( G4VSolid *asolid, G4ThreeVector *aPos, G4RotationMatrix * arm ) :
    ShapeLocRot( asolid, aPos ) {
    fRM = arm;
  }
  G4RotationMatrix * GetRM(){ return fRM; }
protected:
  G4RotationMatrix * fRM;
};


/// Detector construction class to define materials and geometry.

class B1DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  B1DetectorConstruction();
  virtual ~B1DetectorConstruction();
  
  virtual G4VPhysicalVolume* Construct();
  
  G4LogicalVolume* GetScoringVolume() const { return fScoringVolume; }
  
  /// Method to make a rectangular volume 
  /// a.la Fluka, but with material definition
  void RPP( char * volname, double xmin, double ymin, double zmin, 
	    double xmax, double ymax, double zmax,
	    G4Material * mat, G4LogicalVolume * mothervol );

  /// Method to make a rectangular volume
  /// Don't place it yet, but return shape and loc
  /// for use with AddVol and SubtractVol methods
  ShapeAndLoc * RPP( char * volname, 
		     double xmin, double ymin, double zmin, 
		     double xmax, double ymax, double zmax );


  /// Method to make a right circular cylinder volume 
  /// a.la Fluka, but with material definition
  /// avx, avy, avz are center of one face of cylinder
  /// ahx, ahy, ahz are vector from face to other face
  /// ar is the radius of the cylinder 
  void RCC( char * volname, 
	    double avx, double avy, double avz, 
	    double xhx, double ahy, double ahz,
	    double ar,
	    G4Material * mat, G4LogicalVolume * mothervol );

  /// Method to make a right circular cylinder volume
  /// Don't place it yet, but return shape and loc
  /// for use with AddVol and SubtractVol methods
  ShapeAndLoc * RCC( char * volname, 
		     double avx, double avy, double avz, 
		     double xhx, double ahy, double ahz,
		     double ar );

  
  /// AddVol takes first ShapeAndLoc and adds second to it
  /// as a union, returns new shape and new loc.
  /// Does not delete aSL1 or aSL2.
  /// Provide a new name for the combined shape.
  ShapeAndLoc * AddVol( char* volname, ShapeAndLoc* aSL1, ShapeAndLoc* aSL2);

  /// SubtractVol takes first ShapeAndLoc and subtracts second 
  /// from it (making a void).  Returns new shape and new loc.
  /// Does not delete the volumes aSL1 or aSL2.
  /// Provide a new name for the combined shape.
  ShapeAndLoc * SubtractVol( char* volname, ShapeAndLoc* aSL1, ShapeAndLoc* aSL2);

  /// Place volume whose shape has already been built
  /// Also define the material at this point.
  void PlaceVolume( ShapeAndLoc* aSL, G4Material* mat, G4LogicalVolume* mothervol);

protected:
  G4LogicalVolume*  fScoringVolume;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

