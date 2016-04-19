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
// $Id: B1DetectorConstruction.cc 94307 2015-11-11 13:42:46Z gcosmo $
//
/// \file B1DetectorConstruction.cc
/// \brief Implementation of the B1DetectorConstruction class

#include "B1DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1DetectorConstruction::B1DetectorConstruction()
: G4VUserDetectorConstruction(),
  fScoringVolume(0)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1DetectorConstruction::~B1DetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1DetectorConstruction::RCC( char * volname, 
				  double avx, double avy, double avz, 
				  double xhx, double ahy, double ahz,
				  double ar,
				  G4Material * mat, G4LogicalVolume * mothervol ){

  double dH = sqrt( xhx*xhx + xhy*xhy + xhz*xhz );
  // define the cylinder size
  G4Tubs* aRCC =    
    new G4Tubs(volname, 0.0, ar, dH/2.0, 0.0, 2.0*G4pi );

  // add the material to the cylinder
  G4LogicalVolume* logicRCC =                         
    new G4LogicalVolume( aRCC,           //its solid
			 mat,            //its material
                         volname );      //its name
  // locate the cylinder center
  G4ThreeVector RCCpos  = 
    G4ThreeVector( xvx + xhx/2.0, xvy + xhy/2.0, xvz/2.0 );

  // rotate the cylinder
  double atheta = std::atan2d( hy, hx );
  double aphi = std::acos( hz / dH );
  G4RotationMatrix *rm = new G4RotationMatrix();
  rm->setTheta( atheta );
  rm->setPhi( aphi );
               
  new G4PVPlacement(rm,                   // rotation
                    RCCpos,               //at position
                    logicRCC,             //its logical volume
                    volname,              //its name
                    mothervol,            //its mother  volume
                    false,                //no boolean operation
                    0,                    //copy number
                    true);                //overlaps checking
  return;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
ShapeAndLoc * B1DetectorConstruction::RCC( char * volname, 
					   double avx, double avy, double avz, 
					   double xhx, double ahy, double ahz,
					   double ar ){

  double dH = sqrt( xhx*xhx + xhy*xhy + xhz*xhz );
  // define the cylinder size
  G4Tubs* aRCC =    
    new G4Tubs(volname, 0.0, ar, dH/2.0, 0.0, 2.0*G4pi );

  // locate the cylinder center
  G4ThreeVector RCCpos  = 
    G4ThreeVector( xvx + xhx/2.0, xvy + xhy/2.0, xvz/2.0 );

  // rotation of the cylinder
  double atheta = std::atan2d( hy, hx );
  double aphi = std::acos( hz / dH );
  G4RotationMatrix *rm = new G4RotationMatrix();
  rm->setTheta( atheta );
  rm->setPhi( aphi );
               
  return ( new ShapeLocRot( aRCC, &RCCpos, rm ) );

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
ShapeAndLoc* B1DetectorConstruction::RPP( char * volname,
				       double xmin, double ymin, double zmin, 
				       double xmax, double ymax, double zmax ) {

  // define the box
  G4Box* aRPP =    
    new G4Box(volname,               
	      0.5*(xmax-xmin),
	      0.5*(ymax-ymin),
	      0.5*(zmax-zmin) );

  // locate the box
  G4ThreeVector RPPpos  = 
    G4ThreeVector(  
		  ( xmin + xmax )/2.0,
		  ( ymin + ymax )/2.0,
		  ( zmin + zmax )/2.0 );

  return ( new ShapeAndLoc( aRPP, &RPPpos ) );
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void B1DetectorConstruction::RPP( char * volname,
				  double xmin, double ymin, double zmin, 
				  double xmax, double ymax, double zmax,
				  G4Material * mat, G4LogicalVolume * mothervol ) {
  // define the box
  G4Box* aRPP =    
    new G4Box(volname,               
	      0.5*(xmax-xmin),
	      0.5*(ymax-ymin),
	      0.5*(zmax-zmin) );
  // add the material to the box
  G4LogicalVolume* logicRPP =                         
    new G4LogicalVolume( aRPP,           //its solid
			 mat,            //its material
                         volname );      //its name
  // locate the box
  G4ThreeVector RPPpos  = 
    G4ThreeVector(  
		  ( xmin + xmax )/2.0,
		  ( ymin + ymax )/2.0,
		  ( zmin + zmax )/2.0 );
               
  new G4PVPlacement(0,                       //no rotation
                    RPPpos,                    //at position
                    logicRPP,             //its logical volume
                    volname,                //its name
                    mothervol,                //its mother  volume
                    false,                   //no boolean operation
                    0,               //copy number
                    true);          //overlaps checking
  return;
}


G4VPhysicalVolume* B1DetectorConstruction::Construct()
{  
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();
  
    // Option to switch on/off checking of volumes overlaps
  //
  G4bool checkOverlaps = true;

  //     
  // World
  //
  G4double world_sizeX  = 4000.0 * cm;
  G4double world_sizeY  = 4200.0 * cm;
  G4double world_sizeZ  = 2300.0 * cm;
  G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");
  
  G4Box* solidWorld =    
    new G4Box("World",                       //its name
       0.5*world_sizeX, 0.5*world_sizeY, 0.5*world_sizeZ);     //its size
      
  G4LogicalVolume* logicWorld =                         
    new G4LogicalVolume(solidWorld,          //its solid
                        world_mat,           //its material
                        "World");            //its name
                                   
  G4ThreeVector aGeomCenter( -400.0*cm, 0.0*cm, 1800.0*cm ); 
  G4VPhysicalVolume* physWorld = 
    new G4PVPlacement(0,                     //no rotation
                      aGeomCenter,           //center of world
                      logicWorld,            //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking
 
  //     
  // Soil below concrete
  //
  // Define the soil, made up of 
  // Dirt 1.52g/cm^3 (C + Si + Al + Fe + O)
  // Carbon 6 (1 %)
  // Silicon 14 (29 %)
  // Aluminum 13 (15 %)
  // Iron 26 (5 %)
  // Oxygen 8 (50 %)
  G4Material* C_mat  = nist->FindOrBuildMaterial("G4_C");
  G4Material* Si_mat = nist->FindOrBuildMaterial("G4_SI");
  G4Material* Al_mat = nist->FindOrBuildMaterial("G4_AL");
  G4Material* Fe_mat = nist->FindOrBuildMaterial("G4_FE");
  G4Material* O_mat  = nist->FindOrBuildMaterial("G4_O");
  G4double soil_density = 1.52 *g/cm3;

  G4Material* soil_mat = G4Material(name="Soil", soil_density, ncomponents=5);
  soil_mat->AddMaterial( C_mat , fractionmass= 1.0*perCent );
  soil_mat->AddMaterial( Si_mat, fractionmass=29.0*perCent );
  soil_mat->AddMaterial( Al_mat, fractionmass=14.0*perCent );
  soil_mat->AddMaterial( Fe_mat, fractionmass=26.0*perCent );
  soil_mat->AddMaterial( O_mat , fractionmass= 8.0*perCent );

  RPP( "Soil", 
       -1665.063561126*cm, 1530.7350354556*cm, -1897.858810434*cm,
       1452.6036669398*cm, -321.92*cm        ,  -121.92*cm,
       soil_mat, logicWorld );

  // Concrete floors
  G4Material* concrete_mat = nist->FindOrBuildMaterial("G4_CONCRETE");
  
  
  RPP( "flothick", 
       -1172.815*cm, 1383.9420990813*cm, 551.717*cm, 
       1452.6036669398*cm, -152.4*cm, 0.0*cm,
       concrete_mat, logicWorld );
       
  RPP("flothin",
      -1172.815*cm, 1530.7350354556*cm, -1897.858810434*cm,
      551.717*cm, -121.92*cm, 0.0*cm,
      concrete_mat, logicWorld );

  // automatically generated from fluka .inp file, but with some
  // work to make the union and disjoint shapes.

  ShapeAndLoc* targetw  = RPP("targetw",-7.200000*cm, 6.400000*cm, -4.299000*cm, 4.299000*cm, -3.250000*cm, 3.250000*cm);
  ShapeAndLoc* target1  = RPP("target1",4.000000*cm, 6.000000*cm, -3.900000*cm, 3.900000*cm, -2.850000*cm, 2.850000*cm     );
  ShapeAndLoc* target1c = RPP("target1c",3.914000*cm, 6.086000*cm, -3.986000*cm, 3.986000*cm, -2.936000*cm, 2.936000*cm   );
  ShapeAndLoc* target2  = RPP("target2",1.800000*cm, 3.800000*cm, -3.900000*cm, 3.900000*cm, -2.850000*cm, 2.850000*cm     );
  ShapeAndLoc* target2c = RPP("target2c",1.714000*cm, 3.886000*cm, -3.986000*cm, 3.986000*cm, -2.936000*cm, 2.936000*cm   );
  ShapeAndLoc* target3  = RPP("target3",-0.400000*cm, 1.600000*cm, -3.900000*cm, 3.900000*cm, -2.850000*cm, 2.850000*cm    );
  ShapeAndLoc* target3c = RPP("target3c",-0.486000*cm, 1.686000*cm, -3.986000*cm, 3.986000*cm, -2.936000*cm, 2.936000*cm  );
  ShapeAndLoc* target4  = RPP("target4",-3.600000*cm, -0.600000*cm, -3.900000*cm, 3.900000*cm, -2.850000*cm, 2.850000*cm   );
  ShapeAndLoc* target4c = RPP("target4c",-3.686000*cm, -0.514000*cm, -3.986000*cm, 3.986000*cm, -2.936000*cm, 2.936000*cm );
  ShapeAndLoc* target5  = RPP("target5",-6.800000*cm, -3.800000*cm, -3.900000*cm, 3.900000*cm, -2.850000*cm, 2.850000*cm   );
  ShapeAndLoc* target5c = RPP("target5c",-6.886000*cm, -3.714000*cm, -3.986000*cm, 3.986000*cm, -2.936000*cm, 2.936000*cm );


  // Tungsten target pieces
  G4Material* W_mat = nist->FindOrBuildMaterial("G4_W"); //tungsten
  PlaceVolume( target1, W_mat, logicWorld );
  PlaceVolume( target2, W_mat, logicWorld );
  PlaceVolume( target3, W_mat, logicWorld );
  PlaceVolume( target4, W_mat, logicWorld );
  PlaceVolume( target5, W_mat, logicWorld );

  // collimator
  ShapeAndLoc* collimao = RCC("collimao",176.500000*cm, 0.000000*cm, 0.000000*cm, 30.800000*cm, 0.000000*cm, 0.000000*cm, 4.699000*cm );
  ShapeAndLoc* collimai = RCC("collimai",176.500000*cm, 0.000000*cm, 0.000000*cm, 30.800000*cm, 0.000000*cm, 0.000000*cm, 2.540000*cm );
  ShapeAndLoc* collimator = SubtractVol( "collimator", collimao, collimai );
  PlaceVolume( collimator, W_mat, logicWorld);

  // T1 target
  G4Material* Be_mat  = nist->FindOrBuildMaterial("G4_BE");
  RPP("T1",207.000000*cm, 208.200000*cm, 963.600000*cm, 964.800000*cm, 136.410000*cm, 137.910000*cm, Be_mat, logicalWorld );

  // Beamline
  // in vacuum
  double density = universe_mean_density;
  double pressure = 1e.-6*torr;
  double temperature = STP_Temperature;
  G4Material * vacuum = new G4Material(name="vacuum", z=2., a=4.0*g/mole,
				       density, kStateGas, temperature, pressure );
  
  RCC("cryptbAi",-14.000000*cm, 0.000000*cm, 0.000000*cm, 1400.000000*cm, 0.000000*cm, 0.000000*cm, 5.410200*cm, concrete_mat, logicalWorld );
  RCC("windtube",-14.000000*cm, 0.000000*cm, 0.000000*cm, 1400.000000*cm, 0.000000*cm, 0.000000*cm, 5.080000*cm, concrete_mat, logicalWorld );
  RCC("windtubi",-14.000000*cm, 0.000000*cm, 0.000000*cm, 1400.000000*cm, 0.000000*cm, 0.000000*cm, 4.775200*cm, concrete_mat, logicalWorld );
  RCC("cryptbBe",383.200000*cm, 596.314000*cm, 144.145000*cm, -201.200000*cm, 0.000000*cm, 0.000000*cm, 8.331200*cm, concrete_mat, logicalWorld );
  RCC("cryptbBi",383.200000*cm, 596.314000*cm, 144.145000*cm, -550.000000*cm, 0.000000*cm, 0.000000*cm, 7.620000*cm, concrete_mat, logicalWorld );
  RCC("targarm1",383.200000*cm, 596.314000*cm, 144.145000*cm, -120.000000*cm, 0.000000*cm, 0.000000*cm, 7.454900*cm, concrete_mat, logicalWorld );
  RCC("targarm2",263.200000*cm, 596.314000*cm, 144.145000*cm, -120.000000*cm, 0.000000*cm, 0.000000*cm, 7.454900*cm, concrete_mat, logicalWorld );
  RCC("targarm3",143.200000*cm, 596.314000*cm, 144.145000*cm, -120.000000*cm, 0.000000*cm, 0.000000*cm, 7.454900*cm, concrete_mat, logicalWorld );
  RCC("targarm4",23.200000*cm, 596.314000*cm, 144.145000*cm, -190.000000*cm, 0.000000*cm, 0.000000*cm, 7.454900*cm, concrete_mat, logicalWorld );
  RCC("watertb1",440.000000*cm, 594.794000*cm, 144.145000*cm, -550.000000*cm, 0.000000*cm, 0.000000*cm, 0.952500*cm, concrete_mat, logicalWorld );
  RCC("watertb2",440.000000*cm, 591.740000*cm, 144.145000*cm, -550.000000*cm, 0.000000*cm, 0.000000*cm, 0.952500*cm, concrete_mat, logicalWorld );
  RCC("xitwin1",6.400000*cm, 0.000000*cm, 0.000000*cm, 1.000000*cm, 0.000000*cm, 0.000000*cm, 4.775200*cm, concrete_mat, logicalWorld );
  RCC("xitwin2",44.800000*cm, 0.000000*cm, 0.000000*cm, 1.000000*cm, 0.000000*cm, 0.000000*cm, 4.775200*cm, concrete_mat, logicalWorld );


  // Tantallum coating on target pieces
  ShapeAndLoc* tacoat1 = SubtractVol( "tacoat1", target1c, target1 );
  ShapeAndLoc* tacoat2 = SubtractVol( "tacoat2", target2c, target2 );
  ShapeAndLoc* tacoat3 = SubtractVol( "tacoat3", tacoat3c, target3 );
  ShapeAndLoc* tacoat4 = SubtractVol( "tacoat4", tacoat4c, target4 );
  ShapeAndLoc* tacoat5 = SubtractVol( "tacoat5", tacoat5c, target5 );

  G4Material* Ta_mat = nist->FindOrBuildMaterial("G4_TA"); // tantallum
  PlaceVolume( tacoat1, Ta_mat, logicWorld );
  PlaceVolume( tacoat2, Ta_mat, logicWorld );
  PlaceVolume( tacoat3, Ta_mat, logicWorld );
  PlaceVolume( tacoat4, Ta_mat, logicWorld );
  PlaceVolume( tacoat5, Ta_mat, logicWorld );

  // Water around target
  ShapeAndLoc* tungstenw1 = SubtractVol( "tungstenw1", targetw, target1c );
  ShapeAndLoc* tungstenw2 = SubtractVol( "tungstenw2", targetw1, target2c );
  ShapeAndLoc* tungstenw3 = SubtractVol( "tungstenw3", targetw2, target3c );
  ShapeAndLoc* tungstenw4 = SubtractVol( "tungstenw4", targetw3, target4c );
  ShapeAndLoc* targetwater = SubtractVol( "targetwater", targetw4, target5c );
  G4Material* Water_mat = nist->FindOrBuildMaterial("G4_WATER");
  PlaceVolume( targetwater, Water_mat, logicWorld );


  // Define stainless steel
  // not sure what grade is applicable, so use SAE 310.
  G4Material* Mn_mat  = nist->FindOrBuildMaterial("G4_MN");
  G4Material* P_mat  = nist->FindOrBuildMaterial("G4_P");
  G4Material* S_mat  = nist->FindOrBuildMaterial("G4_S");
  G4Material* Cr_mat  = nist->FindOrBuildMaterial("G4_CR");
  G4Material* Ni_mat  = nist->FindOrBuildMaterial("G4_NI");
  G4double steel_density = 1.52 *g/cm3;
  G4Material* steel_mat = G4Material(name="Steel310", steel_density, ncomponents=7);
  steel_mat->AddMaterial( Fe_mat , fractionmass=52.70*perCent );
  steel_mat->AddMaterial( C_mat  , fractionmass= 0.24*perCent );
  steel_mat->AddMaterial( Mn_mat , fractionmass= 1.00*perCent );
  steel_mat->AddMaterial( P_mat  , fractionmass= 0.04*perCent );
  steel_mat->AddMaterial( S_mat  , fractionmass= 0.02*perCent );
  steel_mat->AddMaterial( Si_mat , fractionmass= 1.00*perCent );
  steel_mat->AddMaterial( Cr_mat , fractionmass=25.00*perCent );
  steel_mat->AddMaterial( Ni_mat , fractionmass=20.00*perCent );

  ShapeAndLoc * harpseal = RCC("harpseal",274.100000*cm, 0.000000*cm, -19.685000*cm, 0.000000*cm, 0.000000*cm, 55.626000*cm, 12.700000*cm );
  ShapeAndLoc * harpring = RCC("harpring",274.100000*cm, 0.000000*cm, 15.875000*cm, 0.000000*cm, 0.000000*cm, 0.355600*cm, 12.700000*cm );
  ShapeAndLoc * ddddddd  = RPP("ddddddd",320.540000*cm, 340.860000*cm, -10.160000*cm, 10.160000*cm, -19.685000*cm, 61.595000*cm  );
  ShapeAndLoc * ddddring = RPP("ddddring",320.540000*cm, 340.860000*cm, -10.160000*cm, -9.800000*cm, -19.685000*cm, 61.595000*cm );
  ShapeAndLoc * cryptbAe = RCC("cryptbAe",-14.000000*cm, 0.000000*cm, 0.000000*cm, 1400.000000*cm, 0.000000*cm, 0.000000*cm, 5.715000*cm );
  ShapeAndLoc * HARPzone01a = SubtractVol( "HARPzone01a", harpseal, cryptbAe );
  ShapeAndLoc * HARPzone01 = SubtractVol( "HARPzone01", HARPzone01a, harpring );
  ShapeAndLoc * HARPzone02a = SubtractVol( "HARPzone02a", ddddddd, cryptbAe );
  ShapeAndLoc * HARPzone02 = SubtractVol( "HARPzone02", HARPzone02a, ddddring );
  ShapeAndLoc * HARP = AddVol( HARPzone01, HARPzone02 );
  PlaceVolume( HARP, steel_mat, logicWorld );

  RPP("eeeeeee",382.500000*cm, 425.000000*cm, -10.800000*cm, 10.800000*cm, -26.035000*cm, 26.035000*cm, concrete_mat, logicalWorld );
  RPP("fffffff1",432.205000*cm, 453.795000*cm, -26.035000*cm, 26.035000*cm, -26.035000*cm, 26.035000*cm, concrete_mat, logicalWorld );
  RPP("fffffff2",474.105000*cm, 495.695000*cm, -26.035000*cm, 26.035000*cm, -26.035000*cm, 26.035000*cm, concrete_mat, logicalWorld );
  RCC("stFlange",0.000000*cm, 72.400000*cm, 199.845000*cm, 0.000000*cm, -2.400000*cm, 0.000000*cm, 7.250000*cm, concrete_mat, logicalWorld );
  RCC("guideo",0.000000*cm, 0.000000*cm, 0.000000*cm, 0.000000*cm, 425.547100*cm, 0.000000*cm, 4.750000*cm, concrete_mat, logicalWorld );



  RPP("surround",-1994.344009*cm, 1534.424059*cm, -1915.741978*cm, 1764.483364*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("blckhole",-2100.000000*cm, 1644.627824*cm, -2100.000000*cm, 2100.000000*cm, -500.000000*cm, 1736.858228*cm, concrete_mat, logicalWorld );
  RPP("immovab",-442.564447*cm, -77.842847*cm, 682.582272*cm, 869.661939*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("spyram",192.329473*cm, 531.485300*cm, 683.828644*cm, 853.561760*cm, 60.960000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("wpyram",-69.174700*cm, 280.000000*cm, 215.000000*cm, 684.000000*cm, 0.000000*cm, 731.520000*cm, concrete_mat, logicalWorld );
  RPP("nepyram",607.000000*cm, 1133.000000*cm, -111.000000*cm, -80.000000*cm, 0.000000*cm, 121.920000*cm, concrete_mat, logicalWorld );
  RPP("nepyramC",773.193280*cm, 960.000000*cm, -42.786953*cm, 276.498277*cm, 0.000000*cm, 243.840000*cm, concrete_mat, logicalWorld );
  RPP("nepyramD",958.000000*cm, 1035.000000*cm, -41.000000*cm, -20.000000*cm, 182.880000*cm, 243.840000*cm, concrete_mat, logicalWorld );
  RPP("nepyramE",628.000000*cm, 1060.000000*cm, 40.000000*cm, 410.000000*cm, 243.840000*cm, 467.360000*cm, concrete_mat, logicalWorld );
  RPP("M11Q7",-46.000000*cm, 46.000000*cm, 52.000000*cm, 98.000000*cm, -45.720000*cm, 45.720000*cm, concrete_mat, logicalWorld );
  RPP("M11B2",-160.020000*cm, 160.020000*cm, -57.150000*cm, 57.150000*cm, -98.425000*cm, 98.425000*cm, concrete_mat, logicalWorld );
  RPP("M11B2x",-58.616500*cm, 58.616500*cm, -84.422433*cm, 84.056872*cm, -10.795000*cm, 10.750000*cm, concrete_mat, logicalWorld );
  RPP("M11Q5",-28.467500*cm, 28.467500*cm, -40.321500*cm, 40.321500*cm, -28.467500*cm, 28.467500*cm, concrete_mat, logicalWorld );
  RPP("M11Q5i",-40.500000*cm, 40.500000*cm, -20.500000*cm, 20.500000*cm, -40.500000*cm, 40.500000*cm, concrete_mat, logicalWorld );
  RPP("M11Q3",-28.467500*cm, 28.467500*cm, -40.322500*cm, 40.322500*cm, -28.467500*cm, 28.467500*cm, concrete_mat, logicalWorld );
  RPP("M11Q3i",-40.500000*cm, 40.500000*cm, -20.320500*cm, 20.320500*cm, -40.500000*cm, 40.500000*cm, concrete_mat, logicalWorld );
  RPP("M11Q4",-30.000000*cm, 33.309163*cm, 61.322500*cm, 92.322500*cm, -37.211124*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11Q4i",-15.953487*cm, 18.554277*cm, 46.320500*cm, 106.320500*cm, -40.500000*cm, 40.500000*cm, concrete_mat, logicalWorld );
  RPP("M11F1",-52.911000*cm, 46.911000*cm, 47.750000*cm, 109.750000*cm, -42.910000*cm, 42.910000*cm, concrete_mat, logicalWorld );
  RPP("M11B1",-40.640000*cm, 40.640000*cm, -108.000000*cm, 108.000000*cm, -83.820000*cm, 76.200000*cm, concrete_mat, logicalWorld );
  RPP("M11B1i",-61.435000*cm, 61.435000*cm, -77.470000*cm, 77.470000*cm, -31.115000*cm, 31.115000*cm, concrete_mat, logicalWorld );
  RPP("M11B1x",-61.435000*cm, 61.435000*cm, -77.470000*cm, 77.470000*cm, -8.941000*cm, 8.941000*cm, concrete_mat, logicalWorld );
  RPP("M11Q2",-31.750000*cm, 31.750000*cm, -34.760000*cm, 34.760000*cm, -44.450000*cm, 44.450000*cm, concrete_mat, logicalWorld );
  RPP("M11Q1",-31.750000*cm, 31.750000*cm, -34.760000*cm, 34.760000*cm, -44.450000*cm, 44.450000*cm, concrete_mat, logicalWorld );
  RPP("warmo",-41.000000*cm, 41.000000*cm, -56.000000*cm, 50.000000*cm, 160.445000*cm, 231.500000*cm, concrete_mat, logicalWorld );
  RPP("warm",-40.000000*cm, 40.000000*cm, -55.000000*cm, 49.000000*cm, 161.445000*cm, 230.500000*cm, concrete_mat, logicalWorld );
  RPP("pif00",-566.959594*cm, -444.840369*cm, -848.151594*cm, 82.991571*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("nedm",-326.473138*cm, 153.614422*cm, -710.109893*cm, -618.468763*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("eedm",622.092590*cm, 720.768228*cm, -10.000000*cm, 131.000000*cm, 0.000000*cm, 243.840000*cm, concrete_mat, logicalWorld );
  RPP("lead",-437.255000*cm, -435.985000*cm, -830.883000*cm, 236.717000*cm, 2.235000*cm, 605.145000*cm, concrete_mat, logicalWorld );
  RPP("boxInn",302.756738*cm, 500.941186*cm, 497.414000*cm, 701.897175*cm, 116.500000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  //RPP("flothick",-1172.815000*cm, 1383.942099*cm, 551.717000*cm, 1452.603667*cm, -152.400000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  //RPP("flothin",-1172.815000*cm, 1530.735035*cm, -1897.858810*cm, 551.717000*cm, -121.920000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("cyclwall",-1665.063561*cm, -1173.984502*cm, -1834.123567*cm, 1448.690000*cm, -121.920000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("nortwall",-1172.815000*cm, 1390.785000*cm, -1900.203000*cm, -1788.283000*cm, 0.000000*cm, 890.000000*cm, concrete_mat, logicalWorld );
  RPP("woody",1086.507881*cm, 1249.550085*cm, 109.877519*cm, 261.132526*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("reflecto",302.756738*cm, 500.941186*cm, 497.414000*cm, 701.897175*cm, 174.800000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("soutwall",-1277.600277*cm, 1530.735035*cm, 1340.406336*cm, 1463.151069*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("piftop1",-1069.568000*cm, -612.368000*cm, -262.000000*cm, 291.009659*cm, 487.680000*cm, 548.640000*cm, concrete_mat, logicalWorld );
  RPP("piftop2",-1069.568000*cm, -703.808000*cm, -353.440000*cm, -262.000000*cm, 487.680000*cm, 548.640000*cm, concrete_mat, logicalWorld );
  RPP("sw01",-1177.619611*cm, -775.864191*cm, 765.463904*cm, 1223.026686*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e09",868.203103*cm, 959.366609*cm, 281.351756*cm, 342.440549*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e10",857.137000*cm, 983.489514*cm, 350.421802*cm, 716.100404*cm, 60.960000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e11",687.803455*cm, 857.137000*cm, 626.595888*cm, 718.025443*cm, 60.960000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e14",284.139073*cm, 558.272662*cm, 716.187293*cm, 870.733986*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e16",435.777072*cm, 558.272662*cm, 843.600992*cm, 886.676073*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e17",554.125941*cm, 574.970732*cm, 779.260915*cm, 818.538474*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e18",857.137000*cm, 886.975442*cm, 718.025443*cm, 901.096566*cm, 60.960000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e21",983.489514*cm, 1282.209811*cm, 413.588401*cm, 870.783269*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e26",983.489514*cm, 1344.881915*cm, 276.652014*cm, 786.878545*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e27",1344.881915*cm, 1530.735035*cm, 383.223576*cm, 786.878545*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e28",1282.209811*cm, 1530.735035*cm, 786.878545*cm, 901.805692*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e29",1345.211225*cm, 1467.247544*cm, 184.637250*cm, 383.223576*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e30",1352.284818*cm, 1422.085146*cm, 111.232016*cm, 184.637250*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e31",1352.284818*cm, 1416.951352*cm, 64.170058*cm, 124.664990*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e33",1322.716750*cm, 1352.284818*cm, 2.274687*cm, 208.435389*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e37",5.521446*cm, 97.914999*cm, -195.068894*cm, -8.347094*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e38",5.921638*cm, 67.164883*cm, -381.386460*cm, -183.847342*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e18b",857.137000*cm, 983.489514*cm, 716.100404*cm, 901.096566*cm, 60.960000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s01",923.602996*cm, 1197.281280*cm, 1037.937113*cm, 1161.018370*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s03",558.272662*cm, 740.812711*cm, 986.535311*cm, 1095.464223*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s04",71.965935*cm, 558.272662*cm, 1058.167332*cm, 1150.761599*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s05",71.965935*cm, 165.393534*cm, 1013.994976*cm, 1058.167332*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s06",74.714997*cm, 137.444762*cm, 951.658443*cm, 1013.994976*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s07",558.272662*cm, 983.489514*cm, 905.490978*cm, 1037.937113*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s08",558.272662*cm, 795.204492*cm, 847.458291*cm, 905.490978*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11w10",638.088587*cm, 857.137000*cm, 329.482392*cm, 621.880570*cm, 60.960000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11w12",278.253087*cm, 642.884334*cm, 406.605426*cm, 683.828644*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11n1",731.941883*cm, 1105.724266*cm, -652.289268*cm, -575.760547*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11n2",990.528402*cm, 1354.644000*cm, -451.622906*cm, -359.867221*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11n3",720.000000*cm, 773.193280*cm, -10.000000*cm, 131.000000*cm, 0.000000*cm, 243.840000*cm, concrete_mat, logicalWorld );
  RPP("M11n6",731.941883*cm, 806.859472*cm, -581.399506*cm, -42.786953*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11w15",638.088587*cm, 728.101471*cm, 285.067980*cm, 341.000000*cm, 0.000000*cm, 91.440000*cm, concrete_mat, logicalWorld );
  RPP("M11w17",165.134187*cm, 337.628001*cm, 870.733986*cm, 1058.167332*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e50",800.529648*cm, 1164.255147*cm, 565.737084*cm, 876.504450*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e52",1073.447078*cm, 1256.436447*cm, 316.358583*cm, 688.356111*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e53",688.824226*cm, 798.461029*cm, 564.095630*cm, 851.220904*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11w18",638.088587*cm, 1013.233935*cm, 398.555505*cm, 526.279296*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e56",1316.782198*cm, 1499.478650*cm, 332.362327*cm, 383.217254*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e57",1316.335377*cm, 1534.424059*cm, 379.643793*cm, 566.129073*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e58",1286.504244*cm, 1534.424059*cm, 596.508212*cm, 906.687011*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e59",1195.139462*cm, 1374.527620*cm, 718.735250*cm, 875.933558*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s9",609.182397*cm, 663.442023*cm, 750.162006*cm, 819.462873*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s13",558.272662*cm, 634.522979*cm, 686.050520*cm, 787.455744*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s15",337.628001*cm, 558.272662*cm, 778.922405*cm, 1058.167332*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s17",558.272662*cm, 634.522979*cm, 686.050520*cm, 768.127352*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s8",558.272662*cm, 690.000000*cm, 847.458291*cm, 925.386281*cm, 30.480000*cm, 60.960000*cm, concrete_mat, logicalWorld );
  RPP("M11s4",739.700000*cm, 794.427486*cm, 885.073544*cm, 925.386281*cm, 30.480000*cm, 60.960000*cm, concrete_mat, logicalWorld );
  RPP("M11s5",794.427486*cm, 977.685620*cm, 901.113167*cm, 925.386281*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s6",558.272662*cm, 746.726998*cm, 925.386281*cm, 993.903870*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s7",683.547582*cm, 795.364025*cm, 564.096383*cm, 826.331433*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e18b1",764.388092*cm, 953.317468*cm, 901.581223*cm, 1025.212057*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11w22",382.385300*cm, 558.272662*cm, 996.804988*cm, 1058.167332*cm, 60.960000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11w23",435.777072*cm, 558.272662*cm, 923.893078*cm, 997.687333*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e19",438.944812*cm, 560.611569*cm, 776.656792*cm, 843.600992*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11",-160.020000*cm, 160.020000*cm, -57.150000*cm, 57.150000*cm, -98.425000*cm, 98.425000*cm, concrete_mat, logicalWorld );
  RPP("M11w40",638.088587*cm, 857.137000*cm, 329.080945*cm, 621.699494*cm, 182.880000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("ft7_04",754.779111*cm, 981.823120*cm, 901.588379*cm, 1032.040695*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("ft07_05",794.034047*cm, 886.975442*cm, 716.100404*cm, 901.096566*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("ft07_01",764.410203*cm, 794.034047*cm, 716.100404*cm, 778.922405*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("ft07_06",741.002799*cm, 794.034047*cm, 778.922405*cm, 859.403675*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e65",435.777072*cm, 583.083229*cm, 778.922405*cm, 881.474364*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11w41",337.628001*cm, 435.777072*cm, 956.153899*cm, 996.804988*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11w42",337.628001*cm, 435.777072*cm, 895.481907*cm, 956.153899*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11w43",337.628001*cm, 435.777072*cm, 776.656792*cm, 895.481907*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e66",-103.931166*cm, 0.000000*cm, -61.728836*cm, 0.000000*cm, 182.880000*cm, 243.840000*cm, concrete_mat, logicalWorld );
  RPP("M11e67",1344.881915*cm, 1462.438950*cm, 102.032622*cm, 368.203103*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e68",-103.931166*cm, -58.524555*cm, 0.000000*cm, 12.596154*cm, 182.880000*cm, 243.840000*cm, concrete_mat, logicalWorld );
  RPP("M11w45",435.777072*cm, 558.272662*cm, 925.105655*cm, 1058.167332*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("souttop",-932.255466*cm, 1530.735035*cm, 776.982081*cm, 1326.244813*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("souttop1",-170.000000*cm, 1530.735035*cm, 780.000000*cm, 1340.406336*cm, 243.840000*cm, 609.600000*cm, concrete_mat, logicalWorld );
  RPP("souttop2",-1277.600277*cm, -170.000000*cm, 411.761751*cm, 1340.406336*cm, 243.840000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("cyclwal1",-1265.581644*cm, -1173.984502*cm, -1236.852717*cm, 1326.244813*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("surrM11",337.628001*cm, 983.489514*cm, 131.000000*cm, 1058.167332*cm, 0.000000*cm, 243.840000*cm, concrete_mat, logicalWorld );
  RPP("M11e48",995.253358*cm, 1086.507881*cm, 120.000000*cm, 250.000000*cm, 0.000000*cm, 243.840000*cm, concrete_mat, logicalWorld );
  RPP("M11s21",771.195171*cm, 951.992190*cm, 903.558315*cm, 1087.642552*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("ft07_6",554.125941*cm, 794.034047*cm, 778.922405*cm, 901.096566*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("sureast1",1281.843227*cm, 1534.424059*cm, -307.564981*cm, 271.084508*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("sureast2",1281.843227*cm, 1530.735035*cm, -307.564981*cm, 383.223576*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e49",5.573768*cm, 228.424180*cm, -398.480318*cm, 5.543558*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("sw02",-422.029007*cm, -330.981580*cm, 869.661939*cm, 1117.016215*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("sw03",-513.299408*cm, -330.981580*cm, 1117.016215*cm, 1179.448736*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11w21",71.965935*cm, 165.134187*cm, 923.980492*cm, 1058.167332*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("ceiling",-1740.631063*cm, 1530.735035*cm, -1348.093066*cm, 1456.329687*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("m11cover",628.000000*cm, 1530.735035*cm, 410.000000*cm, 780.000000*cm, 243.840000*cm, 467.360000*cm, concrete_mat, logicalWorld );
  RPP("surrM13",-77.842847*cm, 194.029533*cm, 682.582272*cm, 938.648046*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M13Q2",-22.450000*cm, 22.450000*cm, -145.321500*cm, -91.000000*cm, -22.450000*cm, 22.450000*cm, concrete_mat, logicalWorld );
  RPP("M13Q2i",-43.689000*cm, 43.689000*cm, -134.000000*cm, -103.000000*cm, -43.689000*cm, 43.689000*cm, concrete_mat, logicalWorld );
  RPP("M13Q1",-26.500000*cm, 26.500000*cm, -90.000000*cm, -45.000000*cm, -35.560000*cm, 50.800000*cm, concrete_mat, logicalWorld );
  RPP("M11s22",71.965935*cm, 165.134187*cm, 938.648046*cm, 1058.167332*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s23",165.134187*cm, 232.447198*cm, 868.458359*cm, 1052.152784*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s25",292.229957*cm, 337.628001*cm, 1007.843407*cm, 1058.167332*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s27",230.951565*cm, 337.628001*cm, 870.733986*cm, 1007.843407*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("spyram1",194.029533*cm, 337.628001*cm, 778.922405*cm, 870.733986*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s28",71.965935*cm, 165.134187*cm, 938.648046*cm, 1058.167332*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s29",147.595650*cm, 163.455201*cm, 1013.812163*cm, 1058.167332*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s30",165.134187*cm, 232.447198*cm, 870.733986*cm, 1058.167332*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s31",165.134187*cm, 232.447198*cm, 914.158708*cm, 952.797718*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s32",194.029533*cm, 232.447198*cm, 870.733986*cm, 914.158708*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s33",74.322482*cm, 147.595650*cm, 951.402826*cm, 1052.152784*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s34",74.527744*cm, 126.241994*cm, 951.818336*cm, 1013.994976*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11w31",352.818947*cm, 383.804712*cm, 997.687333*cm, 1031.943653*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s35",165.134187*cm, 232.447198*cm, 868.458359*cm, 952.797718*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s36",163.455201*cm, 230.951565*cm, 952.797718*cm, 1058.167332*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11w46",226.799125*cm, 561.091171*cm, 1058.167332*cm, 1149.716690*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11w47",163.233470*cm, 193.677620*cm, 1058.167332*cm, 1181.521069*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11w48",72.373020*cm, 116.929922*cm, 955.819648*cm, 1021.497699*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s37",165.134187*cm, 230.951565*cm, 949.494968*cm, 1058.167332*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s38",165.134187*cm, 230.951565*cm, 914.158708*cm, 949.494968*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s39",200.543159*cm, 230.951565*cm, 870.733986*cm, 914.158708*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );

  RPP("M13_z79",73.907033*cm, 135.329631*cm, 1026.792759*cm, 1149.406566*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s40",165.393534*cm, 561.319373*cm, 1058.167332*cm, 1151.398813*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M13bridg",-77.842847*cm, 194.029533*cm, 683.828644*cm, 920.992886*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M13_z82",73.907033*cm, 132.575394*cm, 1029.114284*cm, 1149.820948*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M13_z84",337.628001*cm, 554.125941*cm, 778.922405*cm, 867.995609*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s41",740.812711*cm, 982.010831*cm, 900.977261*cm, 1032.040695*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("sw05",-900.000000*cm, -716.690384*cm, 641.143366*cm, 765.000000*cm, 0.000000*cm, 182.880000*cm, concrete_mat, logicalWorld );
  RPP("sw06",-808.649452*cm, -625.519886*cm, 580.705856*cm, 765.000000*cm, 0.000000*cm, 182.880000*cm, concrete_mat, logicalWorld );
  RPP("sw07",-716.873975*cm, -474.004723*cm, 519.149134*cm, 703.259695*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("sw12",-686.655220*cm, -321.791737*cm, 299.500720*cm, 488.818458*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("sw13",-716.873975*cm, -534.867162*cm, 488.818458*cm, 580.705856*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("sw14",-625.519886*cm, -442.564447*cm, 519.149134*cm, 703.259695*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("sw15",-717.106255*cm, -373.753674*cm, 488.818458*cm, 703.259695*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("sw16",-808.906044*cm, -626.243261*cm, 642.024029*cm, 765.463904*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("sw17",-899.640189*cm, -808.906044*cm, 733.587469*cm, 765.463904*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("sw19",-261.429179*cm, -77.842847*cm, 500.946948*cm, 531.409249*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("sw20",-220.920800*cm, -77.842847*cm, 404.538807*cm, 466.201562*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11S1_0",343.696638*cm, 414.022186*cm, 893.893507*cm, 954.760955*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif01",-931.624990*cm, -566.959594*cm, -864.391794*cm, -613.828706*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif02",-932.000599*cm, -870.356995*cm, -614.012316*cm, -582.933400*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif03",-871.966453*cm, -596.715767*cm, -633.548949*cm, -551.854485*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif04",-1145.305512*cm, -1052.949097*cm, -800.268254*cm, -614.400802*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif05",-1052.949097*cm, -566.289061*cm, -1080.041743*cm, -985.737772*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif06",-994.168748*cm, -628.272498*cm, -1110.250448*cm, -1079.158279*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pillar02",-585.791802*cm, -555.542443*cm, -1125.914222*cm, -1094.822053*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pillar01",-1102.136899*cm, -1071.887540*cm, -1125.914222*cm, -1094.822053*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif09",-1144.572111*cm, -1052.949097*cm, -1111.369289*cm, -925.803700*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif10",-1173.984502*cm, -958.772619*cm, 234.048525*cm, 499.228899*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif11",-958.772619*cm, -837.107610*cm, 234.048525*cm, 326.912323*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif12",-958.772619*cm, -901.210993*cm, 322.436959*cm, 454.348029*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif13",-716.463506*cm, -444.840369*cm, 112.094861*cm, 299.500720*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif14",-716.463506*cm, -656.269932*cm, 19.790483*cm, 122.509042*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif15",-747.113139*cm, -716.463506*cm, 202.720978*cm, 296.703618*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif17",-716.463506*cm, -686.655220*cm, 299.500720*cm, 391.805098*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif19",-716.463506*cm, -627.657857*cm, 112.622732*cm, 299.500720*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif20",-533.490790*cm, -444.840369*cm, 112.622732*cm, 299.500720*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif21",-627.657857*cm, -533.490790*cm, 83.489030*cm, 299.500720*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif22",-1173.984502*cm, -1144.572111*cm, -1084.041240*cm, -900.259484*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif23",-1173.984502*cm, -1143.972548*cm, -777.402620*cm, -593.620865*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif24",-990.522663*cm, -647.485298*cm, 238.384954*cm, 329.908162*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif25",-625.827134*cm, -535.002577*cm, 94.752315*cm, 277.509379*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif26",-535.002577*cm, -444.840369*cm, 121.710331*cm, 305.267503*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif27",-1173.984502*cm, -1085.429141*cm, -1053.329640*cm, -869.388952*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif28",-1085.429141*cm, -1053.407662*cm, -1084.126928*cm, -900.828222*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif29",-1173.984502*cm, -628.600899*cm, -1052.982326*cm, -808.454673*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif30",-1173.984502*cm, -628.600899*cm, -778.012920*cm, -594.407585*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif31",-628.600899*cm, -566.214520*cm, -808.831789*cm, -563.353970*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif32",-566.214520*cm, -444.840369*cm, -823.922638*cm, 93.601010*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif33",-534.984845*cm, -444.840369*cm, 93.601010*cm, 277.111640*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif34",-625.712367*cm, -534.984845*cm, 93.601010*cm, 215.979726*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif35",-562.822073*cm, -444.840369*cm, 321.747640*cm, 411.761751*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif36",-1173.984502*cm, -562.822073*cm, 222.598307*cm, 411.761751*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif37",-1173.984502*cm, -538.640646*cm, -813.400143*cm, -626.963523*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif38",-1173.984502*cm, -447.052469*cm, -626.963523*cm, 411.761751*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pif39",-1176.477953*cm, -566.813451*cm, 291.009659*cm, 411.761751*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pillar03",-325.418819*cm, -295.169460*cm, -937.403843*cm, -906.311674*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pillar04",190.787542*cm, 221.036901*cm, -937.403843*cm, -906.311674*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pillar05",509.791473*cm, 540.040832*cm, -1126.486173*cm, -1095.394004*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("pillar06",1193.039892*cm, 1223.289251*cm, -1126.486173*cm, -1095.394004*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("souttop3",-931.672501*cm, 1530.735035*cm, 782.167243*cm, 1326.244813*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("souttop4",-1021.565673*cm, -841.272276*cm, 413.564190*cm, 782.167243*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("souttop5",-382.920923*cm, 346.268924*cm, 682.582272*cm, 782.167243*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("souttop6",346.268924*cm, 711.266270*cm, 720.813885*cm, 782.167243*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("souttop7",1106.236056*cm, 1472.038247*cm, 597.068919*cm, 782.167243*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("souttop8",-383.287648*cm, -18.592119*cm, 780.428777*cm, 981.818511*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("souttop9",-383.287648*cm, -245.793241*cm, 981.818511*cm, 1086.616077*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("soutto10",-306.156639*cm, -245.793241*cm, 1086.616077*cm, 1178.348879*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("soutto11",-78.256866*cm, -17.707161*cm, 981.818511*cm, 1178.348879*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("surrpif",-1173.984502*cm, -444.840369*cm, -1188.283000*cm, 1326.244813*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  //RPP("soil",-1665.063561*cm, 1530.735035*cm, -1897.858810*cm, 1452.603667*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s3",1227.528895*cm, 1288.346291*cm, 1053.160790*cm, 1146.376014*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s42",1288.346291*cm, 1534.424059*cm, 1068.954732*cm, 1161.860270*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s43",923.602996*cm, 1288.346291*cm, 1032.040695*cm, 1153.675488*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11w49",72.513461*cm, 163.233470*cm, 1017.990507*cm, 1150.813392*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11w50",193.677620*cm, 226.799125*cm, 1058.167332*cm, 1150.813392*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("B1AM632",-321.272642*cm, -290.827840*cm, 949.000000*cm, 980.000000*cm, 132.150000*cm, 142.150000*cm, concrete_mat, logicalWorld );
  RPP("B1AM663",-260.288195*cm, -230.127924*cm, 949.000000*cm, 980.000000*cm, 132.150000*cm, 142.150000*cm, concrete_mat, logicalWorld );
  RPP("B1AQ7i",8.500000*cm, 42.000000*cm, -32.329000*cm, 32.329000*cm, -32.329000*cm, 32.329000*cm, concrete_mat, logicalWorld );
  RPP("B1AQ7",0.000000*cm, 50.500000*cm, -19.050000*cm, 19.050000*cm, -19.050000*cm, 19.050000*cm, concrete_mat, logicalWorld );
  RPP("B1AQ8i",79.000000*cm, 112.000000*cm, -32.329000*cm, 32.329000*cm, -32.329000*cm, 32.329000*cm, concrete_mat, logicalWorld );
  RPP("B1AQ8",70.500000*cm, 121.000000*cm, -19.050000*cm, 19.020000*cm, -19.050000*cm, 19.050000*cm, concrete_mat, logicalWorld );
  RPP("B1AM71",-3.900538*cm, 26.544264*cm, 948.980001*cm, 979.980001*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("sw3",-422.029007*cm, -330.981580*cm, 928.655603*cm, 990.386975*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("B1A01",238.177301*cm, 307.097220*cm, 935.000000*cm, 993.316033*cm, 101.600000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("B1A05",558.272662*cm, 740.812711*cm, 996.804988*cm, 1087.893406*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("B1A05a",566.546242*cm, 635.452001*cm, 932.290282*cm, 996.804988*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("B1A05b",660.785000*cm, 729.015213*cm, 932.290282*cm, 996.804988*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("B1A06",315.720368*cm, 335.872575*cm, 912.325923*cm, 1007.843407*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("B1A07",558.272662*cm, 740.812711*cm, 1025.185400*cm, 1086.985109*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("B1A08",923.751948*cm, 1288.346291*cm, 1037.699671*cm, 1161.213396*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s45",740.812711*cm, 923.602996*cm, 1032.040695*cm, 1099.923688*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("B1A11",71.965935*cm, 134.405085*cm, 951.402826*cm, 1013.994976*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("B1A06b",315.720368*cm, 335.872575*cm, 912.325923*cm, 1007.843407*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("B1A13",230.951565*cm, 337.628001*cm, 870.733986*cm, 1058.167332*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("B1A15",1013.463177*cm, 1288.274646*cm, 1037.676831*cm, 1159.814967*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("Q09EFF",247.687300*cm, 297.587300*cm, 935.000000*cm, 993.316033*cm, 101.600000*cm, 172.720000*cm, concrete_mat, logicalWorld );
  RPP("Q10EFF",576.449100*cm, 625.549100*cm, 932.290282*cm, 996.804988*cm, 91.440000*cm, 187.960000*cm, concrete_mat, logicalWorld );
  RPP("cryptbx",408.800000*cm, 422.000000*cm, 599.600000*cm, 604.734100*cm, 143.000000*cm, 145.565100*cm, concrete_mat, logicalWorld );
  RPP("M11s46",284.139073*cm, 558.272662*cm, 716.187293*cm, 778.922405*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s47",292.636412*cm, 558.272662*cm, 446.780368*cm, 716.187293*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s48",194.029533*cm, 284.139073*cm, 622.204856*cm, 778.922405*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s49",176.576021*cm, 194.029533*cm, 622.204856*cm, 702.080426*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11w51",194.029533*cm, 337.628001*cm, 778.922405*cm, 870.733986*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s50",558.272662*cm, 638.088587*cm, 415.329414*cm, 686.050520*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e23",558.272662*cm, 622.092590*cm, 167.640000*cm, 280.608511*cm, 0.000000*cm, 243.840000*cm, concrete_mat, logicalWorld );
  RPP("pif16",-837.107610*cm, -747.167538*cm, 234.048525*cm, 296.703618*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s51",176.576021*cm, 221.482521*cm, 622.204856*cm, 706.656863*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("body15",1322.716750*cm, 1345.211225*cm, 208.435389*cm, 224.322027*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("body16",1404.491961*cm, 1426.986436*cm, 169.716820*cm, 184.637250*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s52",292.636412*cm, 558.272662*cm, 309.279449*cm, 716.187293*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s53",194.029533*cm, 284.139073*cm, 778.922405*cm, 870.733986*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s54",337.628001*cm, 435.777072*cm, 857.018165*cm, 1058.167332*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e24",822.431603*cm, 857.137000*cm, 621.880570*cm, 685.358249*cm, 60.960000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e25",688.395812*cm, 791.825005*cm, 718.025443*cm, 825.198740*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("ft7_5",791.825005*cm, 857.137000*cm, 685.358249*cm, 901.588379*cm, 60.960000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11w52",435.777072*cm, 739.429947*cm, 853.561760*cm, 986.535311*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s55",558.272662*cm, 638.088587*cm, 686.050520*cm, 726.500426*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11w53",337.628001*cm, 382.385300*cm, 996.804988*cm, 1031.829472*cm, 106.680000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("sureasb1",337.628001*cm, 382.385300*cm, 1031.829472*cm, 1058.167332*cm, 121.920000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e63",792.866170*cm, 857.137000*cm, 685.358249*cm, 901.096566*cm, 60.960000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("sureasb2",1322.716750*cm, 1345.211225*cm, 208.435389*cm, 224.322027*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s26",286.827722*cm, 314.857403*cm, 915.636203*cm, 1007.843407*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11w54",130.024401*cm, 558.645991*cm, 1058.167332*cm, 1149.716690*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11w55",337.628001*cm, 382.385300*cm, 1031.829472*cm, 1058.167332*cm, 106.680000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("eedm1",-51.989153*cm, 292.636412*cm, 156.276101*cm, 251.164316*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("eedm2",259.155284*cm, 622.092590*cm, -10.345166*cm, 167.640000*cm, 0.000000*cm, 243.840000*cm, concrete_mat, logicalWorld );
  RPP("eedm3",-170.000000*cm, 284.139073*cm, 595.000000*cm, 780.000000*cm, 243.840000*cm, 350.000000*cm, concrete_mat, logicalWorld );
  RPP("M11s57",292.636412*cm, 558.272662*cm, 446.780368*cm, 716.187293*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11w57",337.628001*cm, 435.777072*cm, 902.882740*cm, 1058.167332*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11w58",337.628001*cm, 435.777072*cm, 1043.384233*cm, 1058.167332*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("B1A9",1013.780638*cm, 1288.346291*cm, 1037.699671*cm, 1161.213396*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e69",284.139073*cm, 466.216991*cm, 716.187293*cm, 778.922405*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("M11e70",284.139073*cm, 375.833299*cm, 747.376121*cm, 778.922405*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("cabltra1",-75.000000*cm, 176.576021*cm, 623.944913*cm, 682.582272*cm, 0.000000*cm, 45.720000*cm, concrete_mat, logicalWorld );
  RPP("cabltra0",-444.840369*cm, -75.000000*cm, 650.343649*cm, 682.582272*cm, 0.000000*cm, 45.720000*cm, concrete_mat, logicalWorld );
  RPP("body17",383.200000*cm, 420.000000*cm, 588.859100*cm, 603.768900*cm, 140.555987*cm, 147.852606*cm, concrete_mat, logicalWorld );
  RPP("swcon1",-321.791737*cm, -75.000000*cm, 344.000000*cm, 488.818458*cm, 0.000000*cm, 243.840000*cm, concrete_mat, logicalWorld );
  RPP("swcon2",-442.564447*cm, -75.000000*cm, 488.818458*cm, 682.582272*cm, 0.000000*cm, 243.840000*cm, concrete_mat, logicalWorld );
  RPP("wcon1",-75.000000*cm, 106.000000*cm, 251.164316*cm, 622.204856*cm, 0.000000*cm, 125.000000*cm, concrete_mat, logicalWorld );
  RPP("wcon2",-75.000000*cm, 106.000000*cm, 251.164316*cm, 580.000000*cm, 125.000000*cm, 190.000000*cm, concrete_mat, logicalWorld );
  RPP("wcon3",-75.000000*cm, 106.000000*cm, 251.164316*cm, 622.204856*cm, 190.000000*cm, 350.000000*cm, concrete_mat, logicalWorld );
  RPP("con_hand",106.000000*cm, 160.000000*cm, 251.164316*cm, 520.000000*cm, 0.000000*cm, 150.000000*cm, concrete_mat, logicalWorld );
  RPP("wste1",106.000000*cm, 292.636412*cm, 520.000000*cm, 622.204856*cm, 0.000000*cm, 100.000000*cm, concrete_mat, logicalWorld );
  RPP("wste2",106.000000*cm, 292.636412*cm, 520.000000*cm, 622.204856*cm, 100.000000*cm, 120.000000*cm, concrete_mat, logicalWorld );
  RPP("wste3",106.000000*cm, 292.636412*cm, 375.000000*cm, 580.000000*cm, 120.000000*cm, 190.000000*cm, concrete_mat, logicalWorld );
  RPP("wste4",106.000000*cm, 292.636412*cm, 375.000000*cm, 622.204856*cm, 190.000000*cm, 243.840000*cm, concrete_mat, logicalWorld );
  RPP("cste_out",284.139073*cm, 558.272662*cm, 446.780368*cm, 716.187293*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("cste_in",302.756738*cm, 500.941186*cm, 497.414000*cm, 701.897175*cm, 116.500000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("wenesol",160.000000*cm, 292.636412*cm, 251.164316*cm, 520.000000*cm, 0.000000*cm, 150.000000*cm, concrete_mat, logicalWorld );
  RPP("nenesol",292.636412*cm, 558.272662*cm, 167.640000*cm, 446.780368*cm, 0.000000*cm, 243.840000*cm, concrete_mat, logicalWorld );
  RPP("eenesol",558.272662*cm, 626.000000*cm, 280.608511*cm, 415.329414*cm, 0.000000*cm, 243.840000*cm, concrete_mat, logicalWorld );
  RPP("nwenesol",106.000000*cm, 292.636412*cm, 251.164316*cm, 375.000000*cm, 150.000000*cm, 243.840000*cm, concrete_mat, logicalWorld );
  RPP("targend",75.000000*cm, 182.000000*cm, 585.000000*cm, 617.000000*cm, 128.000000*cm, 160.000000*cm, concrete_mat, logicalWorld );
  RPP("nshield1",-380.000000*cm, -80.000000*cm, -906.311674*cm, -830.000000*cm, 0.000000*cm, 220.000000*cm, concrete_mat, logicalWorld );
  RPP("nshield2",-444.840369*cm, -380.000000*cm, -920.000000*cm, -830.000000*cm, 0.000000*cm, 243.840000*cm, concrete_mat, logicalWorld );
  RPP("nshield3",-444.840369*cm, -350.000000*cm, -1105.000000*cm, -920.000000*cm, 0.000000*cm, 243.840000*cm, concrete_mat, logicalWorld );
  RPP("uste",106.000000*cm, 276.000000*cm, 251.164316*cm, 595.000000*cm, 243.840000*cm, 350.000000*cm, concrete_mat, logicalWorld );
  RPP("unenesol",276.000000*cm, 550.000000*cm, 251.164316*cm, 440.000000*cm, 243.840000*cm, 350.000000*cm, concrete_mat, logicalWorld );
  RPP("usenesol",284.139073*cm, 628.000000*cm, 716.187293*cm, 780.000000*cm, 243.840000*cm, 350.000000*cm, concrete_mat, logicalWorld );
  RPP("ueenesol",558.272662*cm, 628.000000*cm, 372.000000*cm, 716.187293*cm, 243.840000*cm, 350.000000*cm, concrete_mat, logicalWorld );
  RPP("body7",-170.000000*cm, -75.000000*cm, 360.000000*cm, 595.000000*cm, 243.840000*cm, 350.000000*cm, concrete_mat, logicalWorld );
  RPP("body14",-170.000000*cm, 290.000000*cm, 180.000000*cm, 780.000000*cm, 350.000000*cm, 467.360000*cm, concrete_mat, logicalWorld );
  RPP("body18",230.000000*cm, 350.000000*cm, 235.000000*cm, 365.000000*cm, 350.000000*cm, 647.360000*cm, concrete_mat, logicalWorld );
  RPP("body20",260.000000*cm, 628.000000*cm, -10.345166*cm, 180.000000*cm, 350.000000*cm, 467.360000*cm, concrete_mat, logicalWorld );
  RPP("body21",290.000000*cm, 560.000000*cm, 220.000000*cm, 580.000000*cm, 350.000000*cm, 467.360000*cm, concrete_mat, logicalWorld );
  RPP("body22",560.000000*cm, 628.000000*cm, 370.000000*cm, 780.000000*cm, 350.000000*cm, 467.360000*cm, concrete_mat, logicalWorld );
  RPP("body23",194.029533*cm, 284.139073*cm, 580.000000*cm, 622.204856*cm, 120.000000*cm, 160.000000*cm, concrete_mat, logicalWorld );
  RPP("ncon",10.000000*cm, 340.000000*cm, -96.000000*cm, 156.276101*cm, 0.000000*cm, 243.840000*cm, concrete_mat, logicalWorld );
  RPP("body25",290.000000*cm, 628.000000*cm, 175.000000*cm, 370.000000*cm, 350.000000*cm, 467.360000*cm, concrete_mat, logicalWorld );
  RPP("body26",290.000000*cm, 560.000000*cm, 580.000000*cm, 780.000000*cm, 350.000000*cm, 467.360000*cm, concrete_mat, logicalWorld );
  RPP("body27",259.155284*cm, 628.000000*cm, -10.345166*cm, 251.164316*cm, 243.840000*cm, 350.000000*cm, concrete_mat, logicalWorld );
  RPP("body28",550.000000*cm, 628.000000*cm, 251.164316*cm, 372.000000*cm, 243.840000*cm, 350.000000*cm, concrete_mat, logicalWorld );
  RPP("body29",10.000000*cm, 340.000000*cm, -96.000000*cm, 156.276101*cm, 243.840000*cm, 350.000000*cm, concrete_mat, logicalWorld );
  RPP("body30",-170.000000*cm, 1530.000000*cm, 450.000000*cm, 780.000000*cm, 467.360000*cm, 609.600000*cm, concrete_mat, logicalWorld );
  RPP("swcon4",-442.564447*cm, -75.000000*cm, 622.204856*cm, 682.582272*cm, 175.000000*cm, 243.840000*cm, concrete_mat, logicalWorld );
  RPP("cable1",-442.564447*cm, -75.000000*cm, 622.582272*cm, 682.582272*cm, 190.000000*cm, 243.840000*cm, concrete_mat, logicalWorld );
  RPP("cable2",-502.564447*cm, -442.564447*cm, 488.818458*cm, 703.259695*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("cable3",-442.564447*cm, -321.791737*cm, 488.818458*cm, 518.818458*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("cable4",-321.791737*cm, -291.791737*cm, 344.000000*cm, 518.818458*cm, 190.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("body33",10.000000*cm, 340.000000*cm, -96.000000*cm, 180.000000*cm, 350.000000*cm, 467.360000*cm, concrete_mat, logicalWorld );
  RPP("body31",-442.564447*cm, 106.000000*cm, 580.000000*cm, 630.000000*cm, 120.000000*cm, 190.000000*cm, concrete_mat, logicalWorld );
  RPP("body35",558.272662*cm, 650.000000*cm, 686.050520*cm, 778.922405*cm, 182.880000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("body36",822.431603*cm, 857.137000*cm, 621.699494*cm, 716.100404*cm, 182.880000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("body37",764.410203*cm, 857.137000*cm, 716.100404*cm, 778.922405*cm, 182.880000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("body38",466.216991*cm, 764.410203*cm, 716.100404*cm, 778.922405*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RPP("dog",180.000000*cm, 400.000000*cm, 185.000000*cm, 415.000000*cm, 467.360000*cm, 747.360000*cm, concrete_mat, logicalWorld );
  RPP("body39",260.000000*cm, 320.000000*cm, 185.000000*cm, 235.000000*cm, 467.360000*cm, 617.360000*cm, concrete_mat, logicalWorld );
  RPP("dog1",180.000000*cm, 370.000000*cm, 75.000000*cm, 185.000000*cm, 467.360000*cm, 747.360000*cm, concrete_mat, logicalWorld );
  RPP("dog2",180.000000*cm, 320.000000*cm, 125.000000*cm, 185.000000*cm, 467.360000*cm, 617.360000*cm, concrete_mat, logicalWorld );
  RPP("dog3",400.000000*cm, 500.000000*cm, 185.000000*cm, 380.000000*cm, 467.360000*cm, 747.360000*cm, concrete_mat, logicalWorld );
  RPP("dog4",400.000000*cm, 450.000000*cm, 185.000000*cm, 330.000000*cm, 535.000000*cm, 595.000000*cm, concrete_mat, logicalWorld );
	   
  RCC("stopbeam",1390.000000*cm, 0.000000*cm, 0.000000*cm, 5.000000*cm, 0.000000*cm, 0.000000*cm, 5.715000*cm, concrete_mat, logicalWorld );

  RCC("M11Q6",0.000000*cm, -1.000000*cm, 0.000000*cm, 0.000000*cm, 26.035000*cm, 0.000000*cm, 33.200000*cm, concrete_mat, logicalWorld );
  RCC("M11Q6i",0.000000*cm, 5.668000*cm, 0.000000*cm, 0.000000*cm, 12.700000*cm, 0.000000*cm, 40.200000*cm, concrete_mat, logicalWorld );
  RCC("M11Q6x",0.000000*cm, -1.000000*cm, 0.000000*cm, 0.000000*cm, 100.000000*cm, 0.000000*cm, 15.000000*cm, concrete_mat, logicalWorld );
  RCC("magpipe4",0.000000*cm, -150.000000*cm, 0.000000*cm, 0.000000*cm, 342.000000*cm, 0.000000*cm, 10.560000*cm, concrete_mat, logicalWorld );
  RCC("magpii4",0.000000*cm, -150.000000*cm, 0.000000*cm, 0.000000*cm, 342.000000*cm, 0.000000*cm, 10.160000*cm, concrete_mat, logicalWorld );
  RCC("M11B2i",817.000000*cm, 217.000000*cm, 98.742000*cm, 0.000000*cm, 0.000000*cm, 76.835000*cm, 84.328000*cm, concrete_mat, logicalWorld );
  RCC("M11SX3",0.000000*cm, 0.000000*cm, 0.000000*cm, 0.000000*cm, 35.560000*cm, 0.000000*cm, 21.590000*cm, concrete_mat, logicalWorld );
  RCC("M11SX3i",0.000000*cm, 8.890000*cm, 0.000000*cm, 0.000000*cm, 17.780000*cm, 0.000000*cm, 29.210000*cm, concrete_mat, logicalWorld );
  RCC("M11SX3x",0.000000*cm, -440.121000*cm, 0.000000*cm, 0.000000*cm, 475.681000*cm, 0.000000*cm, 10.477500*cm, concrete_mat, logicalWorld );
  RCC("magpipe3",0.686082*cm, -510.794263*cm, 0.000000*cm, -0.468177*cm, 640.000000*cm, 0.000000*cm, 5.480000*cm, concrete_mat, logicalWorld );
  RCC("magpii3",0.686082*cm, -510.794263*cm, 0.000000*cm, -0.468177*cm, 640.000000*cm, 0.000000*cm, 5.080000*cm, concrete_mat, logicalWorld );
  RCC("M11SX25",0.000000*cm, 0.000000*cm, 0.000000*cm, 0.000000*cm, 26.035000*cm, 0.000000*cm, 32.766000*cm, concrete_mat, logicalWorld );
  RCC("M11SX25i",0.000000*cm, 6.668000*cm, 0.000000*cm, 0.000000*cm, 12.700000*cm, 0.000000*cm, 39.751000*cm, concrete_mat, logicalWorld );
  RCC("M11SX2",0.000000*cm, 0.000000*cm, 0.000000*cm, 0.000000*cm, 35.560000*cm, 0.000000*cm, 21.590000*cm, concrete_mat, logicalWorld );
  RCC("M11SX2i",0.000000*cm, 8.890000*cm, 0.000000*cm, 0.000000*cm, 17.780000*cm, 0.000000*cm, 29.210000*cm, concrete_mat, logicalWorld );
  RCC("M11SX1",0.000000*cm, 0.000000*cm, 0.000000*cm, 0.000000*cm, 26.035000*cm, 0.000000*cm, 32.760000*cm, concrete_mat, logicalWorld );
  RCC("M11SX1i",0.000000*cm, 6.668000*cm, 0.000000*cm, 0.000000*cm, 12.700000*cm, 0.000000*cm, 39.751000*cm, concrete_mat, logicalWorld );
  RCC("M11SX1x",0.000000*cm, 0.000000*cm, 0.000000*cm, 0.000000*cm, 26.035000*cm, 0.000000*cm, 15.557500*cm, concrete_mat, logicalWorld );
  RCC("magpipe2",-1.279086*cm, -146.000000*cm, 0.000000*cm, 3.436147*cm, 344.680608*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RCC("magpii2",-1.279086*cm, -146.000000*cm, 0.000000*cm, 3.436147*cm, 344.680608*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RCC("cryo",0.000000*cm, 320.000000*cm, 172.500000*cm, 0.000000*cm, 0.000000*cm, 329.020000*cm, 31.100000*cm, concrete_mat, logicalWorld );
  RCC("cryoii",0.000000*cm, 320.000000*cm, 172.500000*cm, 0.000000*cm, 0.000000*cm, 329.020000*cm, 31.000000*cm, concrete_mat, logicalWorld );
  RCC("cryoi",0.000000*cm, 320.000000*cm, 239.300000*cm, 0.000000*cm, 0.000000*cm, 109.400000*cm, 40.000000*cm, concrete_mat, logicalWorld );
  RCC("akira",0.000000*cm, 320.000000*cm, 244.300000*cm, 0.000000*cm, 0.000000*cm, 50.000000*cm, 35.000000*cm, concrete_mat, logicalWorld );

  RCC("warmo",0.000000*cm, -3.000000*cm, 160.445000*cm, 0.000000*cm, 0.000000*cm, 107.200000*cm, 47.200000*cm, concrete_mat, logicalWorld );
  RCC("nwarmo",0.000000*cm, -3.000000*cm, 160.445000*cm, 0.000000*cm, 0.000000*cm, 21.444000*cm, 41.000000*cm, concrete_mat, logicalWorld );
  RCC("nwarm",0.000000*cm, -3.000000*cm, 163.445000*cm, 0.000000*cm, 0.000000*cm, 17.944000*cm, 40.500000*cm, concrete_mat, logicalWorld );
  RCC("nwarmbot",0.000000*cm, -3.000000*cm, 160.845000*cm, 0.000000*cm, 0.000000*cm, 2.600000*cm, 10.000000*cm, concrete_mat, logicalWorld );
  RCC("d2osid13",0.000000*cm, -3.000000*cm, 262.645000*cm, 0.000000*cm, 0.000000*cm, 5.000000*cm, 47.200000*cm, concrete_mat, logicalWorld );
  RCC("d2osid14",0.000000*cm, -3.000000*cm, 262.645000*cm, 0.000000*cm, 0.000000*cm, 5.000000*cm, 43.000000*cm, concrete_mat, logicalWorld );
  RCC("d2osid11",0.000000*cm, -3.000000*cm, 238.645000*cm, 0.000000*cm, 0.000000*cm, 29.000000*cm, 43.000000*cm, concrete_mat, logicalWorld );
  RCC("d2osid12",0.000000*cm, -3.000000*cm, 238.645000*cm, 0.000000*cm, 0.000000*cm, 26.500000*cm, 42.700000*cm, concrete_mat, logicalWorld );
  RCC("d2oside9",0.000000*cm, -3.000000*cm, 236.145000*cm, 0.000000*cm, 0.000000*cm, 2.500000*cm, 47.200000*cm, concrete_mat, logicalWorld );
  RCC("d2osid10",0.000000*cm, -3.000000*cm, 236.145000*cm, 0.000000*cm, 0.000000*cm, 2.500000*cm, 42.700000*cm, concrete_mat, logicalWorld );
  RCC("d2oside7",0.000000*cm, -3.000000*cm, 233.645000*cm, 0.000000*cm, 0.000000*cm, 2.500000*cm, 47.200000*cm, concrete_mat, logicalWorld );
  RCC("d2oside8",0.000000*cm, -3.000000*cm, 233.645000*cm, 0.000000*cm, 0.000000*cm, 2.500000*cm, 38.700000*cm, concrete_mat, logicalWorld );
  RCC("d2oside5",0.000000*cm, -3.000000*cm, 160.445000*cm, 0.000000*cm, 0.000000*cm, 73.200000*cm, 39.200000*cm, concrete_mat, logicalWorld );
  RCC("d2oside6",0.000000*cm, -3.000000*cm, 160.445000*cm, 0.000000*cm, 0.000000*cm, 73.200000*cm, 38.700000*cm, concrete_mat, logicalWorld );
  RCC("d2oside1",0.000000*cm, -3.000000*cm, 160.445000*cm, 0.000000*cm, 0.000000*cm, 94.850000*cm, 38.200000*cm, concrete_mat, logicalWorld );
  RCC("d2oside2",0.000000*cm, -3.000000*cm, 160.445000*cm, 0.000000*cm, 0.000000*cm, 93.650000*cm, 38.000000*cm, concrete_mat, logicalWorld );
  RCC("d2oside3",0.000000*cm, -3.000000*cm, 160.445000*cm, 0.000000*cm, 0.000000*cm, 66.700000*cm, 37.500000*cm, concrete_mat, logicalWorld );
  RCC("d2oside4",0.000000*cm, -3.000000*cm, 160.445000*cm, 0.000000*cm, 0.000000*cm, 64.200000*cm, 37.000000*cm, concrete_mat, logicalWorld );
  RCC("guide",0.000000*cm, 0.000000*cm, 0.000000*cm, 0.000000*cm, 428.000000*cm, 0.000000*cm, 4.250000*cm, concrete_mat, logicalWorld );


  RCC("guideoo",0.000000*cm, 0.000000*cm, 0.000000*cm, 0.000000*cm, 405.547100*cm, 0.000000*cm, 14.000000*cm, concrete_mat, logicalWorld );
  RCC("boron0",0.000000*cm, 6.322349*cm, 0.000000*cm, 0.000000*cm, 5.000000*cm, 0.000000*cm, 14.000000*cm, concrete_mat, logicalWorld );
  RCC("guideooo",0.000000*cm, 0.000000*cm, 0.000000*cm, 0.000000*cm, 405.547100*cm, 0.000000*cm, 14.500000*cm, concrete_mat, logicalWorld );
  RCC("bottleo",0.000000*cm, 0.000000*cm, 0.000000*cm, 75.598667*cm, 0.000000*cm, 0.000000*cm, 8.800000*cm, concrete_mat, logicalWorld );
  RCC("flange",0.000000*cm, 0.000000*cm, 0.000000*cm, -2.000000*cm, 0.000000*cm, 0.000000*cm, 14.000000*cm, concrete_mat, logicalWorld );
  RCC("flangei",0.000000*cm, 0.000000*cm, 0.000000*cm, 6.000000*cm, 0.000000*cm, 0.000000*cm, 10.815000*cm, concrete_mat, logicalWorld );
  RCC("capsulm1",22.890000*cm, 0.000000*cm, 0.000000*cm, 51.500000*cm, 0.000000*cm, 0.000000*cm, 27.900000*cm, concrete_mat, logicalWorld );
  RCC("capsulm2",22.890000*cm, 0.000000*cm, 0.000000*cm, 51.500000*cm, 0.000000*cm, 0.000000*cm, 26.900000*cm, concrete_mat, logicalWorld );
  RCC("capsulm3",22.890000*cm, 0.000000*cm, 0.000000*cm, 51.500000*cm, 0.000000*cm, 0.000000*cm, 24.900000*cm, concrete_mat, logicalWorld );
  RCC("capsulm4",22.890000*cm, 0.000000*cm, 0.000000*cm, 51.500000*cm, 0.000000*cm, 0.000000*cm, 23.900000*cm, concrete_mat, logicalWorld );
  RCC("bottle",-2.000000*cm, 0.000000*cm, 0.000000*cm, 77.470265*cm, 0.000000*cm, 0.000000*cm, 8.400000*cm, concrete_mat, logicalWorld );
  RCC("tube11",67.100000*cm, -19.400000*cm, 0.000000*cm, 9.200000*cm, 0.000000*cm, 0.000000*cm, 3.000000*cm, concrete_mat, logicalWorld );
  RCC("tube31",84.596369*cm, -12.600000*cm, 0.000000*cm, 0.000000*cm, 15.600000*cm, 0.000000*cm, 3.000000*cm, concrete_mat, logicalWorld );
  RCC("tube21",74.174952*cm, -20.200523*cm, 0.000000*cm, 11.720615*cm, 0.000000*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RCC("tube41",81.600000*cm, 0.000000*cm, 0.000000*cm, 22.000000*cm, 0.000000*cm, 0.000000*cm, 3.000000*cm, concrete_mat, logicalWorld );
  RCC("tube51",47.137789*cm, 38.472289*cm, 0.000000*cm, 52.243716*cm, 0.000000*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RCC("tube52",47.137789*cm, 38.472289*cm, 0.000000*cm, 48.771427*cm, 0.000000*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RCC("tube61",94.373098*cm, 83.535200*cm, 0.000000*cm, 0.000000*cm, 370.000000*cm, 0.000000*cm, 3.000000*cm, concrete_mat, logicalWorld );
  RCC("tube62",94.373098*cm, 83.535200*cm, 0.000000*cm, 0.000000*cm, 370.000000*cm, 0.000000*cm, 2.650000*cm, concrete_mat, logicalWorld );
  RCC("tube63",94.373098*cm, 83.535200*cm, 0.000000*cm, 0.000000*cm, 370.000000*cm, 0.000000*cm, 4.900000*cm, concrete_mat, logicalWorld );
  RCC("tube64",94.373098*cm, 83.535200*cm, 0.000000*cm, 0.000000*cm, 370.000000*cm, 0.000000*cm, 5.250000*cm, concrete_mat, logicalWorld );
  RCC("tube53",47.137789*cm, 38.472289*cm, 0.000000*cm, 48.771427*cm, 0.000000*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RCC("tube54",47.137789*cm, 38.472289*cm, 0.000000*cm, 48.771427*cm, 0.000000*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RCC("tube01",48.665498*cm, 23.500000*cm, 0.000000*cm, 0.000000*cm, 18.670000*cm, 0.000000*cm, 3.000000*cm, concrete_mat, logicalWorld );
  RCC("tube02",48.665498*cm, 23.500000*cm, 0.000000*cm, 0.000000*cm, 18.670000*cm, 0.000000*cm, 2.650000*cm, concrete_mat, logicalWorld );
  RCC("tube03",48.665498*cm, 26.000000*cm, 0.000000*cm, 0.000000*cm, 16.170000*cm, 0.000000*cm, 4.900000*cm, concrete_mat, logicalWorld );
  RCC("tube04",48.665498*cm, 26.000000*cm, 0.000000*cm, 0.000000*cm, 16.170000*cm, 0.000000*cm, 5.250000*cm, concrete_mat, logicalWorld );
  RCC("tube51",47.137789*cm, 68.472289*cm, 0.000000*cm, 52.243716*cm, 0.000000*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RCC("tube52",47.137789*cm, 68.472289*cm, 0.000000*cm, 48.771427*cm, 0.000000*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RCC("tube61",94.373098*cm, 113.535200*cm, 0.000000*cm, 0.000000*cm, 370.000000*cm, 0.000000*cm, 3.000000*cm, concrete_mat, logicalWorld );
  RCC("tube62",94.373098*cm, 113.535200*cm, 0.000000*cm, 0.000000*cm, 370.000000*cm, 0.000000*cm, 2.650000*cm, concrete_mat, logicalWorld );
  RCC("tube63",94.373098*cm, 113.535200*cm, 0.000000*cm, 0.000000*cm, 370.000000*cm, 0.000000*cm, 4.900000*cm, concrete_mat, logicalWorld );
  RCC("tube64",94.373098*cm, 113.535200*cm, 0.000000*cm, 0.000000*cm, 370.000000*cm, 0.000000*cm, 5.250000*cm, concrete_mat, logicalWorld );
  RCC("tube53",47.137789*cm, 68.472289*cm, 0.000000*cm, 48.771427*cm, 0.000000*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RCC("tube54",47.137789*cm, 68.472289*cm, 0.000000*cm, 48.771427*cm, 0.000000*cm, 0.000000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RCC("tube01",48.665498*cm, 24.800000*cm, 0.000000*cm, 0.000000*cm, 49.200000*cm, 0.000000*cm, 3.000000*cm, concrete_mat, logicalWorld );
  RCC("tube02",48.665498*cm, 24.800000*cm, 0.000000*cm, 0.000000*cm, 49.200000*cm, 0.000000*cm, 2.650000*cm, concrete_mat, logicalWorld );
  RCC("tube03",48.665498*cm, 27.300000*cm, 0.000000*cm, 0.000000*cm, 46.700000*cm, 0.000000*cm, 4.900000*cm, concrete_mat, logicalWorld );
  RCC("tube04",48.665498*cm, 27.300000*cm, 0.000000*cm, 0.000000*cm, 45.000000*cm, 0.000000*cm, 5.250000*cm, concrete_mat, logicalWorld );
  RCC("bottlei",19.000000*cm, 0.000000*cm, 0.000000*cm, 55.800000*cm, 0.000000*cm, 0.000000*cm, 8.000000*cm, concrete_mat, logicalWorld );
  RCC("bottleii",19.000000*cm, 0.000000*cm, 0.000000*cm, 55.800000*cm, 0.000000*cm, 0.000000*cm, 7.600000*cm, concrete_mat, logicalWorld );
  RCC("guidf",0.000000*cm, 0.000000*cm, 0.000000*cm, 0.000000*cm, 396.200000*cm, 0.000000*cm, 4.250000*cm, concrete_mat, logicalWorld );
  RCC("guidfo",0.000000*cm, 0.000000*cm, 0.000000*cm, 0.000000*cm, 396.200000*cm, 0.000000*cm, 4.750000*cm, concrete_mat, logicalWorld );
  RCC("guidfoo",0.000000*cm, 0.000000*cm, 0.000000*cm, 0.000000*cm, 396.200000*cm, 0.000000*cm, 14.000000*cm, concrete_mat, logicalWorld );
  RCC("guidfooo",0.000000*cm, 0.000000*cm, 0.000000*cm, 0.000000*cm, 396.200000*cm, 0.000000*cm, 14.500000*cm, concrete_mat, logicalWorld );
  RCC("boron1",0.000000*cm, 66.367734*cm, 0.000000*cm, 0.000000*cm, 5.000000*cm, 0.000000*cm, 14.000000*cm, concrete_mat, logicalWorld );
  RCC("boron3",0.000000*cm, 174.838088*cm, 0.000000*cm, 0.000000*cm, 5.000000*cm, 0.000000*cm, 14.000000*cm, concrete_mat, logicalWorld );
  RCC("guidg",0.000000*cm, 0.000000*cm, 0.000000*cm, 0.000000*cm, -390.000000*cm, 0.000000*cm, 4.250000*cm, concrete_mat, logicalWorld );
  RCC("guidgo",0.000000*cm, 0.000000*cm, 0.000000*cm, 0.000000*cm, -390.000000*cm, 0.000000*cm, 4.750000*cm, concrete_mat, logicalWorld );
  RCC("guidgoo",0.000000*cm, 6.200000*cm, 0.000000*cm, 0.000000*cm, -396.200000*cm, 0.000000*cm, 14.000000*cm, concrete_mat, logicalWorld );
  RCC("guidgooo",0.000000*cm, 6.200000*cm, 0.000000*cm, 0.000000*cm, -396.200000*cm, 0.000000*cm, 14.500000*cm, concrete_mat, logicalWorld );
  RCC("boron2",0.000000*cm, -66.910064*cm, 0.000000*cm, 0.000000*cm, -5.000000*cm, 0.000000*cm, 14.000000*cm, concrete_mat, logicalWorld );
  RCC("boron4",0.000000*cm, -224.333475*cm, 0.000000*cm, 0.000000*cm, -5.000000*cm, 0.000000*cm, 14.000000*cm, concrete_mat, logicalWorld );
  RCC("reflecti",402.000000*cm, 599.500000*cm, 174.800000*cm, 0.000000*cm, 0.000000*cm, 68.300000*cm, 88.900000*cm, concrete_mat, logicalWorld );
  RCC("RCC",0.000000*cm, 402.000000*cm, 599.500000*cm, 174.800000*cm, 0.000000*cm, 0.000000*cm, 69.040000*cm, concrete_mat, logicalWorld );
  RCC("leadbase",401.849000*cm, 599.655600*cm, 116.500000*cm, 0.000000*cm, 0.000000*cm, 58.300000*cm, 98.900000*cm, concrete_mat, logicalWorld );
  RCC("BL1A",-770.688971*cm, 964.200000*cm, 137.160000*cm, 2301.424007*cm, 0.000000*cm, 0.000000*cm, 5.480000*cm, concrete_mat, logicalWorld );
  RCC("BL1Aii",-1006.000000*cm, 964.200000*cm, 137.160000*cm, 2536.735035*cm, 0.000000*cm, 0.000000*cm, 5.080000*cm, concrete_mat, logicalWorld );
  RCC("BL1Ax",-1006.000000*cm, 964.200000*cm, 137.160000*cm, 2536.735035*cm, 0.000000*cm, 0.000000*cm, 7.500000*cm, concrete_mat, logicalWorld );
  RCC("M13hole1",2.837969*cm, 830.239103*cm, 0.000000*cm, 0.000000*cm, 0.000000*cm, 45.720000*cm, 15.005681*cm, concrete_mat, logicalWorld );
  RCC("M13hole2",52.340839*cm, 756.429202*cm, 0.000000*cm, 0.000000*cm, 0.000000*cm, 45.720000*cm, 15.468942*cm, concrete_mat, logicalWorld );
  RCC("M13hole3",140.350604*cm, 765.576284*cm, 0.000000*cm, 0.000000*cm, 0.000000*cm, 45.720000*cm, 15.468942*cm, concrete_mat, logicalWorld );
  RCC("M13pipe",0.084823*cm, 3.260276*cm, 0.000000*cm, 0.000000*cm, -170.000000*cm, 0.000000*cm, 9.400000*cm, concrete_mat, logicalWorld );
  RCC("M13pii",0.084823*cm, 3.260276*cm, 0.000000*cm, 0.000000*cm, -170.000000*cm, 0.000000*cm, 9.000000*cm, concrete_mat, logicalWorld );
  RCC("T1hole1",208.000000*cm, 960.000000*cm, 144.145000*cm, 0.000000*cm, 0.000000*cm, 130.175000*cm, 17.509716*cm, concrete_mat, logicalWorld );
  RCC("T1hole2",177.563137*cm, 960.305459*cm, 144.145000*cm, 0.000000*cm, 0.000000*cm, 130.175000*cm, 0.000000*cm, concrete_mat, logicalWorld );
  RCC("T1hole3",519.000000*cm, 960.000000*cm, 137.160000*cm, 0.000000*cm, 0.000000*cm, 137.160000*cm, 10.160000*cm, concrete_mat, logicalWorld );
  RCC("T1hole4",476.000000*cm, 960.305459*cm, 137.160000*cm, 0.000000*cm, 0.000000*cm, 137.160000*cm, 9.685912*cm, concrete_mat, logicalWorld );
  RCC("T1hole5",79.178057*cm, 868.272563*cm, 203.200000*cm, 0.000000*cm, 0.000000*cm, 130.175000*cm, 18.000000*cm, concrete_mat, logicalWorld );
  RCC("T1hole6",118.000000*cm, 829.000000*cm, 203.200000*cm, 0.000000*cm, 0.000000*cm, 130.175000*cm, 13.000000*cm, concrete_mat, logicalWorld );
  RCC("B1AM632x",-306.046807*cm, 949.000000*cm, 137.150000*cm, 0.000000*cm, 31.000000*cm, 0.000000*cm, 3.500000*cm, concrete_mat, logicalWorld );
  RCC("B1AM663x",-245.000000*cm, 949.000000*cm, 137.150000*cm, 0.000000*cm, 31.000000*cm, 0.000000*cm, 3.500000*cm, concrete_mat, logicalWorld );
  RCC("B1AM71x",-306.046807*cm, 949.000000*cm, 137.150000*cm, 0.000000*cm, 31.000000*cm, 0.000000*cm, 3.500000*cm, concrete_mat, logicalWorld );
  RCC("B1AM71x1",11.121464*cm, 948.980001*cm, 137.150000*cm, 0.000000*cm, 31.000000*cm, 0.000000*cm, 3.500000*cm, concrete_mat, logicalWorld );
  RCC("BL1A01x",238.177301*cm, 964.200000*cm, 137.160000*cm, 69.000000*cm, 0.000000*cm, 0.000000*cm, 10.500000*cm, concrete_mat, logicalWorld );
  RCC("BL1A06x",671.088101*cm, 960.500000*cm, 137.160000*cm, -112.815439*cm, -0.000000*cm, -0.000000*cm, 13.000000*cm, concrete_mat, logicalWorld );	   
  //
  //always return the physical World
  //
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
