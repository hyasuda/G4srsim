/***************************************************************************
 *  musrSim - the program for the simulation of (mainly) muSR instruments. *
 *          More info on http://lmu.web.psi.ch/simulation/index.html .     *
 *          musrSim is based od Geant4 (http://geant4.web.cern.ch/geant4/) *
 *                                                                         *
 *  Copyright (C) 2009 by Paul Scherrer Institut, 5232 Villigen PSI,       *
 *                                                       Switzerland       *
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program; if not, write to the Free Software            *
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.              *
 ***************************************************************************/

#include "musrDetectorConstruction.hh"
#include "musrDetectorMessenger.hh"
#include "musrTabulatedElementField.hh"
#include "musrQuadrupole.hh"
#include "musrUniformField.hh"             // Uniform EM field (B,E) implemented by Toni Shiroka

#include "musrScintSD.hh"
#include "musrEventAction.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4SDManager.hh"
#include "G4PVParameterised.hh"
#include "G4UnionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4Para.hh"
#include "G4PVDivision.hh"
#include "G4UserLimits.hh"

// In order to implement overlaping field as was done by 
// Gumplinger in examples/extended/field/field04/
#include "F04GlobalField.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4ios.hh"

#include "G4Region.hh"
#include "G4RegionStore.hh"
#include "G4ProductionCuts.hh"

#include "musrRootOutput.hh"   //cks for storing some info in the Root output file
#include "musrParameters.hh"
#include "musrErrorMessage.hh"
#include "musrSteppingAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

musrDetectorConstruction::musrDetectorConstruction()
:parameterFileName("Unset"), checkOverlap(true), aScintSD(0)
{  
  DefineMaterials();
  detectorMessenger = new musrDetectorMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
musrDetectorConstruction::~musrDetectorConstruction()
{
  delete detectorMessenger;             
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4LogicalVolumeStore.hh"
G4VPhysicalVolume* musrDetectorConstruction::Construct()  {
  // Clean old geometry, if any
  //
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  //  G4cout<< "musrDetectorConstruction::Construct:  ParameterFileName="<<parameterFileName<<G4endl;

  //  G4double roundingErr=0.001*mm;     
  //  G4double roundingErr=0.01*mm;        // 0.01mm precision is OK for displaying subtracted volumes, while 0.001mm is not
  //----------------------

  musrRootOutput* myRootOutput = musrRootOutput::GetRootInstance();

  G4VPhysicalVolume* pointerToWorldVolume=NULL;
  //  Read detector configuration parameters from the steering file:
  FILE *fSteeringFile=fopen(parameterFileName.c_str(),"r");
  if (fSteeringFile==NULL) {
    if (parameterFileName=="Unset") {
      G4cout<<"musrDetectorConstruction:  No input macro file specified"<<G4endl;
    }
    else {
      G4cout << "E R R O R :    Failed to open detector configuration file " << parameterFileName << G4endl;
    }
    G4cout << "S T O P    F O R C E D" << G4endl;
    exit(1);
  }
  G4cout << "Detector configuration file \"" << parameterFileName << "\" was opened."<<G4endl;
  char  line[501];

  // Write out the configuration file into the output file:
  G4cout << "\n\n....oooOO0OOooo........oooOO0OOooo......Configuration file used for this run....oooOO0OOooo........oooOO0OOooo......"<<G4endl; 
  while (!feof(fSteeringFile)) {
    fgets(line,500,fSteeringFile);
    //    if ((line[0]!='#')&&(line[0]!='\n')&&(line[0]!='\r'))  // TS: Do not print comments
    G4cout << line;
  }
  G4cout <<"....oooOO0OOooo........oooOO0OOooo......End of the configuration file.....oooOO0OOooo........oooOO0OOooo......\n\n"<<G4endl;

  // Loop
  rewind (fSteeringFile);
  char eMessage[200];
  while (!feof(fSteeringFile)) {
    fgets(line,500,fSteeringFile);
    //    if (line[0]!='*') {
    if ((line[0]!='#')&&(line[0]!='\n')&&(line[0]!='\r')) {
      char tmpString0[100]="Unset";
      sscanf(&line[0],"%s",tmpString0);
      if ( (strcmp(tmpString0,"/musr/ignore")!=0) &&(strcmp(tmpString0,"/musr/command")!=0) ) continue;
      

      char tmpString1[100]="Unset",tmpString2[100]="Unset",tmpString3[100]="Unset";
      sscanf(&line[0],"%*s %s %s",tmpString1,tmpString2);

      // Define rotation matrix, which might be later on used for some volumes
      if (strcmp(tmpString1,"rotation")==0){
	char matrixName[100]="Unset";
	float pp1, pp2, pp3, pp4=0;
	sscanf(&line[0],"%*s %*s %s %g %g %g %g",matrixName,&pp1,&pp2,&pp3,&pp4);
	if (pp4==0) {
	  G4RotationMatrix* pRotMatrix = new G4RotationMatrix(pp1*deg,pp2*deg,pp3*deg);  // pp1=phi, pp2=theta, pp3=psi
	  pointerToRotationMatrix[matrixName]=pRotMatrix;
	}
	else {
	  G4RotationMatrix* pRotMatrix = new G4RotationMatrix(G4ThreeVector(pp1,pp2,pp3),pp4*deg);
	  pointerToRotationMatrix[matrixName]=pRotMatrix;
	}
      }

      
      else if (strcmp(tmpString1,"construct")==0){
	float x1=0,x2=0,x3=0,x4=0,x5=0,x6=0,x7=0; 
	float posx,posy,posz;
	char name[100];
	char mothersName[100];
	char material[100];
	//	G4CSGSolid* solid=NULL;
	G4VSolid* solid=NULL;
	G4String solidName="sol_";
	char rotMatrix[100]="norot";
	char sensitiveDet[100]="dead";
	int  volumeID=0;
	char actualFieldName[100]="nofield";

	if (strcmp(tmpString2,"tubs")==0){
	  sscanf(&line[0],"%*s %*s %*s %s %g %g %g %g %g %s %g %g %g %s %s",
		 name,&x1,&x2,&x3,&x4,&x5,material,&posx,&posy,&posz,mothersName,rotMatrix);
	  sscanf(&line[0],"%*s %*s %*s %*s %*g %*g %*g %*g %*g %*s %*g %*g %*g %*s %*s %s %d %s",sensitiveDet,&volumeID,actualFieldName);
	  solidName+=name;
	  solid = new G4Tubs(solidName,x1*mm,x2*mm,x3*mm,x4*deg,x5*deg);
	}
	else if (strcmp(tmpString2,"cons")==0){
	  sscanf(&line[0],"%*s %*s %*s %s %g %g %g %g %g %g %g %s %g %g %g %s %s",
		 name,&x1,&x2,&x3,&x4,&x5,&x6,&x7,material,&posx,&posy,&posz,mothersName,rotMatrix);
	  sscanf(&line[0],"%*s %*s %*s %*s %*g %*g %*g %*g %*g %*g %*g %*s %*g %*g %*g %*s %*s %s %d %s",sensitiveDet,&volumeID,actualFieldName);
	  solidName+=name;
	  solid = new G4Cons(solidName,x1*mm,x2*mm,x3*mm,x4*mm,x5*mm,x6*deg,x7*deg);
	}
	else if (strcmp(tmpString2,"box")==0){
	  sscanf(&line[0],"%*s %*s %*s %s %g %g %g %s %g %g %g %s %s",
		 name,&x1,&x2,&x3,material,&posx,&posy,&posz,mothersName,rotMatrix);
	  sscanf(&line[0],"%*s %*s %*s %*s %*g %*g %*g %*s %*g %*g %*g %*s %*s %s %d %s",sensitiveDet,&volumeID,actualFieldName);
	  solidName+=name;
	  solid = new G4Box(solidName,x1*mm,x2*mm,x3*mm);
	}
        else if ((strcmp(tmpString2,"trd")==0)||(strcmp(tmpString2,"trd90y")==0)) {
          sscanf(&line[0],"%*s %*s %*s %s %g %g %g %g %g %s %g %g %g %s %s",
                 name,&x1,&x2,&x3,&x4,&x5,material,&posx,&posy,&posz,mothersName,rotMatrix);
          sscanf(&line[0],"%*s %*s %*s %*s %*g %*g %*g %*g %*g %*s %*g %*g %*g %*s %*s %s %d %s",sensitiveDet,&volumeID,actualFieldName);
          solidName+=name;
          solid = new G4Trd(solidName,x1*mm,x2*mm,x3*mm,x4*mm,x5*mm);
        }
	else if (strcmp(tmpString2,"sphere")==0){
	  sscanf(&line[0],"%*s %*s %*s %s %g %g %g %g %g %g %s %g %g %g %s %s",
		 name,&x1,&x2,&x3,&x4,&x5,&x6,material,&posx,&posy,&posz,mothersName,rotMatrix);
	  sscanf(&line[0],"%*s %*s %*s %*s %*g %*g %*g %*g %*g %*g %*s %*g %*g %*g %*s %*s %s %d %s",sensitiveDet,&volumeID,actualFieldName);
	  solidName+=name;
	  solid = new G4Sphere(solidName,x1*mm,x2*mm,x3*deg,x4*deg,x5*deg,x6*deg);
	}
	else if (strcmp(tmpString2,"para")==0){  // NOT YET TESTED
	  sscanf(&line[0],"%*s %*s %*s %s %g %g %g %g %g %g %s %g %g %g %s %s",
		 name,&x1,&x2,&x3,&x4,&x5,&x6,material,&posx,&posy,&posz,mothersName,rotMatrix);
	  sscanf(&line[0],"%*s %*s %*s %*s %*g %*g %*g %*g %*g %*g %*s %*g %*g %*g %*s %*s %s %d %s",sensitiveDet,&volumeID,actualFieldName);
	  solidName+=name;
	  solid = new G4Para(solidName,x1*mm,x2*mm,x3*mm,x4*deg,x5*deg,x6*deg);
	}
        else if (strcmp(tmpString2,"cylpart")==0){  // Volume introduced by Pavel Bakule on 12 May 2009
          sscanf(&line[0],"%*s %*s %*s %s %g %g %g %g %s %g %g %g %s %s",
                 name,&x1,&x2,&x3,&x4,material,&posx,&posy,&posz,mothersName,rotMatrix);
          sscanf(&line[0],"%*s %*s %*s %*s %*g %*g %*g %*g %*s %*g %*g %*g %*s %*s %s %d %s",sensitiveDet,&volumeID,actualFieldName);
          solidName+=name;
          G4double roundingErr=0.01*mm;  // to avoid some displaying problems of the subtracted volumes
          G4Box* solidSubtractedBox = new G4Box("solidSubtractedBox",x2*mm,(x2-x4)*mm,x3*mm+roundingErr);
          G4Tubs* solidCylinder = new G4Tubs("solidCylinder",0.*mm,x2*mm,x3*mm,0.*deg,180.*deg);
          solid = new G4SubtractionSolid(solidName, solidCylinder, solidSubtractedBox);
        }
	else if (strcmp(tmpString2,"uprofile")==0){
	  // Create a U-profile geometry.  x1, x2, x3 define the outer dimensions of the U-profile (as a box),
	  //                               x4 is the wall thickness of the U-profile.  The centre of the U-profile
          //                               is in the centre of the box defined by x1,x2,x3.
	  sscanf(&line[0],"%*s %*s %*s %s %g %g %g %g %s %g %g %g %s %s",
		 name,&x1,&x2,&x3,&x4,material,&posx,&posy,&posz,mothersName,rotMatrix);
	  sscanf(&line[0],"%*s %*s %*s %*s %*g %*g %*g %*g %*s %*g %*g %*g %*s %*s %s %d %s",sensitiveDet,&volumeID,actualFieldName);
	  solidName+=name;
	  
	  G4double roundingErr=0.01*mm;
	  G4Box* box1 = new G4Box("Box1",x1*mm,x2*mm,x3*mm);
	  G4Box* box2 = new G4Box("Box2",(x1-x4)*mm,(x2-x4/2.+roundingErr)*mm,x3*mm);
	  G4RotationMatrix rot(0,0,0);
	  G4ThreeVector zTrans(0,x4/2.*mm,0);
	  G4Transform3D transform(rot,zTrans);
	  solid =  new G4SubtractionSolid(solidName, box1, box2, transform);
	}
	else if (strcmp(tmpString2,"alcSupportPlate")==0){
	  // Create an ALC holder geometry:  x1 half-width of the holder (as a box),
          //                                 x2 half-height of the spacer 
	  sscanf(&line[0],"%*s %*s %*s %s %g %g %g %g %s %g %g %g %s %s",
		 name,&x1,&x2,&x3,&x4,material,&posx,&posy,&posz,mothersName,rotMatrix);
	  sscanf(&line[0],"%*s %*s %*s %*s %*g %*g %*g %*g %*s %*g %*g %*g %*s %*s %s %d %s",sensitiveDet,&volumeID,actualFieldName);
	  solidName+=name;
	  
	  G4Box* box1 = new G4Box("Box1",x1*mm,4.7*mm,130*mm);
	  G4Box* box2 = new G4Box("Box2",(x1-0.4)*mm,3*mm,130*mm);
	  G4RotationMatrix rot(0,0,0);
	  G4ThreeVector zTrans(0,1.3*mm,0);
	  G4Transform3D transform(rot,zTrans);
	  G4SubtractionSolid* solid_1 =  new G4SubtractionSolid("Drzak", box1, box2, transform);
	  if (x2!=0) {
	    G4Box* box3 = new G4Box("Box3",12.5*mm,4.5*mm,4*mm);
	    G4ThreeVector zTrans2(0,(-4.7-x2)*mm,0);
	    G4Transform3D transform2(rot,zTrans2);
	    solid = new G4UnionSolid("solidName", solid_1, box3, transform2);
	  }
	  else {
	    solid = solid_1;
	  }
	  
	}
	else if (strcmp(tmpString2,"TubeWithTubeHole")==0) {
	  // Create a tube with a partial hole inside it:        ----------
          //                                                     |        |
          //                                                     |        |
          //                                                     |     ___|
          //                                                     |    | 6
          //                                                     |    |
          //                                                     |    |
          //                                                     |    |
          //                                                     |    |___
          //                                                     |        |
          //                                                     |        |
          //                                                     |        |
          //                                                     ----------
	  // First 5 parameters as for the outer tube, the 6th parameter is the depth of the hole.
	  sscanf(&line[0],"%*s %*s %*s %s %g %g %g %g %g %g %s %g %g %g %s %s",
		 name,&x1,&x2,&x3,&x4,&x5,&x6,material,&posx,&posy,&posz,mothersName,rotMatrix);
	  sscanf(&line[0],"%*s %*s %*s %*s %*g %*g %*g %*g %*g %*g %*s %*g %*g %*g %*s %*s %s %d %s",sensitiveDet,&volumeID,actualFieldName);
	  solidName+=name;
	  G4double roundingErr=0.01*mm;  // to avoid some displaying problems of the subtracted volumes
	  G4Tubs* solidInnerDetTube = new G4Tubs("SolidInnerDetTube",0,x1*mm,x6/2*mm+roundingErr,x4*deg,x5*deg);
	  G4Tubs* solidOuterDetTube = new G4Tubs("SolidOuterDetTube",0,x2*mm,x3*mm,x4*deg,x5*deg);
	  G4RotationMatrix rot(0,0,0);
	  G4ThreeVector zTrans(0,0,(x6/2.-x3)*mm);
	  G4Transform3D transform(rot,zTrans);
	  solid =  new G4SubtractionSolid(solidName, solidOuterDetTube, solidInnerDetTube, transform);
	}
	else if (strcmp(tmpString2,"TubeWithHoleAndTubeHole")==0) {
	  // Create a tube with a partial hole inside it:        ----------
          //                                                     |        |
          //                                                     |        |
          //                                                     |     _7_|
          //                                                     |    |6 
          //                                                     ------
          //                                                            
          //                                                     ------
          //                                                     |    |___
          //                                                     |        |
          //                                                     |        |
          //                                                     |        |
          //                                                     ----------
	  // First 5 parameters as for the outer tube, the 6th parameter is the depth of the hole.
	  sscanf(&line[0],"%*s %*s %*s %s %g %g %g %g %g %g %g %s %g %g %g %s %s",
		 name,&x1,&x2,&x3,&x4,&x5,&x6,&x7,material,&posx,&posy,&posz,mothersName,rotMatrix);
	  sscanf(&line[0],"%*s %*s %*s %*s %*g %*g %*g %*g %*g %*g %*g %*s %*g %*g %*g %*s %*s %s %d %s",sensitiveDet,&volumeID,actualFieldName);
	  solidName+=name;
	  G4double roundingErr=0.01*mm;  // to avoid some displaying problems of the subtracted volumes
	  G4Tubs* solidInnerDetTube = new G4Tubs("SolidInnerDetTube",0,x6*mm,x7/2*mm+roundingErr,x4*deg,x5*deg);
	  G4Tubs* solidOuterDetTube = new G4Tubs("SolidOuterDetTube",x1*mm,x2*mm,x3*mm,x4*deg,x5*deg);
	  G4RotationMatrix rot(0,0,0);
	  G4ThreeVector zTrans(0,0,(x7/2.-x3)*mm);
	  G4Transform3D transform(rot,zTrans);
	  solid =  new G4SubtractionSolid(solidName, solidOuterDetTube, solidInnerDetTube, transform);
	}
	else if (strcmp(tmpString2,"tubsbox")==0){
	  // Create a tube, from which center a box is cut out.  x1=box half-width; x2,x3,x4,x5 define the tube.
	  sscanf(&line[0],"%*s %*s %*s %s %g %g %g %g %g %s %g %g %g %s %s",
		 name,&x1,&x2,&x3,&x4,&x5,material,&posx,&posy,&posz,mothersName,rotMatrix);
	  sscanf(&line[0],"%*s %*s %*s %*s %*g %*g %*g %*g %*g %*s %*g %*g %*g %*s %*s %s %d %s",sensitiveDet,&volumeID,actualFieldName);
	  solidName+=name;
	  G4double roundingErr=0.01*mm;  // to avoid some displaying problems of the subtracted volumes
	  G4Box* solidInnerDetBox = new G4Box("SolidInnerDetBox",x1*mm,x1*mm,x3*mm+roundingErr);
	  G4Tubs* solidOuterDetTube = new G4Tubs("SolidOuterDetTube",0.*mm,x2*mm,x3*mm,x4*deg,x5*deg);
	  solid =  new G4SubtractionSolid(solidName, solidOuterDetTube, solidInnerDetBox);
	}
		else if (strcmp(tmpString2,"boxcut")==0){   //Volume introduced by Pavel Bakule on 9 Oct 2009
	  // Create a box, from which center a box is cut out.  x1,x2,x3 define outer box; x4,x5,x6 define inner box; (centers are aligned).
	  sscanf(&line[0],"%*s %*s %*s %s %g %g %g %g %g %g %s %g %g %g %s",
		 name,&x1,&x2,&x3,&x4,&x5,&x6,material,&posx,&posy,&posz,mothersName);
	  sscanf(&line[0],"%*s %*s %*s %*s %*g %*g %*g %*g %*g %*g %*s %*g %*g %*g %*s %s %s %d %s",rotMatrix,sensitiveDet,&volumeID,actualFieldName);
	  solidName+=name;
	  G4cout << "x1,x2,x3 ="<<x1<<"  "<<x2<<"  "<<x3<<"\n\r";
	  G4cout << "x4,x5,x6 ="<<x4<<"  "<<x5<<"  "<<x6<<"\n\r";
	  G4double roundingErr=0.01*mm;  // to avoid some displaying problems of the subtracted volumes
	  G4Box* solidOuterBox = new G4Box("SolidOuterBox",x1*mm,x1*mm,x3*mm+roundingErr);
	  G4Box* solidInnerBox = new G4Box("SolidInnerBox",x4*mm,x5*mm,x6*mm+roundingErr);
	  solid =  new G4SubtractionSolid(solidName, solidOuterBox, solidInnerBox);
	}
	else if (strcmp(tmpString2,"tubsboxsegm")==0){
	  // Create a volume that looks like an intersection of tube and box.
	  char orientation[100];
	  sscanf(&line[0],"%*s %*s %*s %s %s %g %g %g %g %g %s %g %g %g %s %s",
		 name,orientation,&x1,&x2,&x3,&x4,&x5,material,&posx,&posy,&posz,mothersName,rotMatrix);
	  sscanf(&line[0],"%*s %*s %*s %*s %*s %*g %*g %*g %*g %*g %*s %*g %*g %*g %*s %*s %s %d %s",sensitiveDet,&volumeID,actualFieldName);
	  solidName+=name;
	  G4double roundingErr=0.01*mm;  // to avoid some displaying problems of the subtracted volumes
	  G4Box* solidDetBox = new G4Box("SolidDetBox",x2*mm,x2*mm,x3*mm+roundingErr);
	  G4Tubs* solidDetTube = new G4Tubs("SolidDetTube",0.*mm,x2*mm,x3*mm,x4*deg,x5*deg);
	  G4RotationMatrix* yRot = new G4RotationMatrix;
	  G4ThreeVector zTrans;
	  if (strcmp(orientation,"D")==0) zTrans=G4ThreeVector((x1-x2)*mm,(-x1-x2)*mm,0);
	  else if (strcmp(orientation,"U")==0) zTrans=G4ThreeVector((x2-x1)*mm,(x1+x2)*mm,0);
	  else if (strcmp(orientation,"R")==0) zTrans=G4ThreeVector((-x1-x2)*mm,(x2-x1)*mm,0);
	  else if (strcmp(orientation,"L")==0) zTrans=G4ThreeVector((x1+x2)*mm,(x1-x2)*mm,0);
	  else {
	    G4cout<<"Unknown orientation of the tubsboxsegm volume!!"
		  <<"  orientation="<<orientation<<G4endl;
	    ReportGeometryProblem(line);
	  }

	  solid =  new G4IntersectionSolid(solidName, solidDetTube, solidDetBox, yRot, zTrans);
	}
	else ReportGeometryProblem(line);

	G4ThreeVector position = G4ThreeVector (posx*mm,posy*mm,posz*mm);
	//	G4cout << "New volume: "<<tmpString2<<" "<<name<<", solid geometry parameters="<<x1<<" "<<x2<<" "<<x3<<" "<<x4<<" "<<x5
	//	       << ", position="<<position<<", mother="<<mothersName<<G4endl;


	G4LogicalVolume* mothersVolume;
	if (strcmp(name,"World")==0) { mothersVolume=NULL; }
	else {
	  mothersVolume	= FindLogicalVolume(mothersName);
	  if (mothersVolume==NULL) {
	    G4cout << "ERROR! musrDetectorConstruction::Construct():"<<G4endl;
	    G4cout << "       Logical Volume \"" << mothersName <<"\" not found!"<<G4endl;
	    G4cout << "       (requested for the definition of the volume \""<<name<<"\")"<<G4endl;
	    G4cout << "S T O P    F O R C E D"<<G4endl;
	    exit(1);
	  }
	}
	
	G4cout<<"volumeID="<<volumeID<<"\t ";	
	G4RotationMatrix* pRot = pointerToRotationMatrix[rotMatrix];
	//	G4cout << "rotMatrix="<<rotMatrix<<"  pRot="<<pRot<<"        ";
	if (pRot!=NULL) {G4cout<<"    rotated, ";}                // OK, rotation matrix found.
	else if ((strcmp(rotMatrix,"norot"))) {                       // Problem, matrix not found.
	  G4cout <<"Rotation Matrix \""<<rotMatrix<<"\" not found (not defined at this point)!"
		 <<"    Check the geometry!"<<G4endl;
	  G4cout <<"S T O P    F O R C E D"<<G4endl;
	  ReportGeometryProblem(line);
	}
	else {G4cout<<"non-rotated, ";}
	if (strcmp(sensitiveDet,"dead")) {G4cout<<"     sensitive:   ";}
	else {G4cout<<" non-sensitive:   ";}
		 
        // For trapezoid "trd90y" rotate the trapezoid by 90 degrees in y direction
	// (in addition to the requested rotation)
        if (strcmp(tmpString2,"trd")==0){
          pRot->rotateY(90.0*deg);
        }

	G4FieldManager* pFieldMan = pointerToField[actualFieldName];
        if (pFieldMan!=NULL) {G4cout<<"    volume with field, ";}
        else if ((strcmp(actualFieldName,"nofield"))) {
          G4cout <<"Field Manager \""<<actualFieldName<<"\" not found (not defined at this point)!"
                 <<"    Check the geometry!"<<G4endl;
          G4cout <<"S T O P    F O R C E D"<<G4endl;
          ReportGeometryProblem(line);
        }

	G4Material* MATERIAL=CharToMaterial(material);
	G4String logicName="log_";  logicName+=name;
	G4LogicalVolume* logicVolume = new G4LogicalVolume(solid,MATERIAL,logicName,pFieldMan,0,0);
	//	G4cout << "musrDetectorConstruction.cc: logicVolume created ="<<logicVolume<<G4endl;
	G4String physName="phys_";  physName+=name;
	G4VPhysicalVolume* physiVolume = new G4PVPlacement(pRot,              // rotation 
					position,
					logicVolume,    // its logical volume
					physName,       // its name
					mothersVolume,  // its mother  volume
					false,           // no boolean operations
					0,               // no field specific to volume
					checkOverlap);  // Check for overlap
	//	G4cout << "musrDetectorConstruction.cc physiVolume created ="<<physiVolume<<G4endl;
	if (strcmp(name,"World")==0)  { pointerToWorldVolume=physiVolume; G4cout<<"World Volume"<<G4endl;}
	
	// If the name of the volume contains the string "save", it will be treated specifically 
        // (a position of where a particle went through the volume will be saved)
	if (strstr(name,"save")!=NULL) {
	  if (volumeID!=0) {    // do not store hits in special "save" volume if ID of this volume is 0 
	                        // (due to difficulties to distinguish between ID=0 and no save volume when using std::map)
	    musrSteppingAction* mySteppingAction = musrSteppingAction::GetInstance();
	    mySteppingAction->SetLogicalVolumeAsSpecialSaveVolume(logicName,volumeID);
	    musrRootOutput::GetRootInstance()->SetSpecialSaveVolumeDefined();
	  }
	}

	// Unless specified as "dead", set the volume sensitive volume:
	if (strcmp(sensitiveDet,"dead")) {
	  if (strcmp(sensitiveDet,"musr/ScintSD")==0) {
	    if(!aScintSD) {
	      G4SDManager* SDman = G4SDManager::GetSDMpointer();
	      G4String scintSDname = sensitiveDet;
	      aScintSD = new musrScintSD( scintSDname );
	      SDman->AddNewDetector( aScintSD );
	      G4cout<<"musrDetectorConstruction.cc:  aScintSD added: "<<aScintSD->GetFullPathName()<<G4endl;
	    }
	    logicVolume->SetSensitiveDetector( aScintSD );
	  }
	  else {
	    G4cout<<" musrDetectorConstruction.cc: unknown sensitive detector \""<<sensitiveDet<<"\" requested!"<<G4endl;
	    G4cout<<"\""<<line<<"\""<<G4endl;
	    G4cout<<"        S T O P    F O R C E D !!!"<<G4endl;
	    exit(1);
	  }
	}

	//  Set the volume ID (used only for the Root output).
	if (volumeID!=0) {
	  myRootOutput->SetVolumeIDMapping(logicName,volumeID);
	}
	
	// If the volume name is "Target", "M0", "M1" or "M2", let's save muon polarisation and time in these volumes:
        if ((strcmp(name,"Target")==0)||(strcmp(name,"target")==0)) {
	  musrRootOutput::store_muTargetTime = true; musrRootOutput::store_muTargetPolX = true;
	  musrRootOutput::store_muTargetPolY = true; musrRootOutput::store_muTargetPolZ = true;
	}
	if ((strcmp(name,"M0")==0)||(strcmp(name,"m0")==0))  {
	  musrRootOutput::store_muM0Time = true; musrRootOutput::store_muM0PolX = true;
	  musrRootOutput::store_muM0PolY = true; musrRootOutput::store_muM0PolZ = true;
	}
	if ((strcmp(name,"M1")==0)||(strcmp(name,"m1")==0))  {
	  musrRootOutput::store_muM1Time = true; musrRootOutput::store_muM1PolX = true;
	  musrRootOutput::store_muM1PolY = true; musrRootOutput::store_muM1PolZ = true;
	}
	if ((strcmp(name,"M2")==0)||(strcmp(name,"m2")==0))  {
	  musrRootOutput::store_muM2Time = true; musrRootOutput::store_muM2PolX = true;
	  musrRootOutput::store_muM2PolY = true; musrRootOutput::store_muM2PolZ = true;
	}
      }
      // 	if ((strcmp(name,"M2")==0)||(strcmp(name,"m2")==0))  {
      // 	  musrRootOutput::store_muM2Time = true; musrRootOutput::store_muM2PolX = true;
      // 	  musrRootOutput::store_muM2PolY = true; musrRootOutput::store_muM2PolZ = true;
      // 	}
      // }

      else if (strcmp(tmpString1,"visattributes")==0){
	sscanf(&line[0],"%*s %*s %*s %s",tmpString3);
	if (strncmp(tmpString2,"log_",4)==0) {
	  G4LogicalVolume* pLogVol = FindLogicalVolume(tmpString2);
	  if (pLogVol==NULL) {
	    sprintf(eMessage,"musrDetectorConstruction.cc::Construct(): visattributes requested for %s, but this volume was not found.",tmpString2); 
	    musrErrorMessage::GetInstance()->musrError(WARNING,eMessage,false);
	  }	
	  else {SetColourOfLogicalVolume(pLogVol,tmpString3);}
	}
	else {
	  G4LogicalVolumeStore* pLogStore = G4LogicalVolumeStore::GetInstance();
	  if (pLogStore==NULL) {
	    G4cout<<"ERROR:  musrDetectorConstruction.cc:    G4LogicalVolumeStore::GetInstance()  not found!"<<G4endl;
	  }
	  else {
	    for (unsigned int i=0; i<pLogStore->size(); i++) {
	      G4LogicalVolume* pLogVol=pLogStore->at(i);
	      G4String materialName=pLogVol->GetMaterial()->GetName(); 
	      if (tmpString2==materialName) {
		SetColourOfLogicalVolume(pLogVol,tmpString3);
	      }
	    }
	  }
	}
      }

      // cks:  Implementation of the Global Field (to allow overlapping fields) based
      //       on the Peter Gumplinger's implementation of G4BeamLine code into Geant4.
      //
      else if (strcmp(tmpString1,"globalfield")==0){
	// ensure the global field is initialized
	// perhaps should be placed at some separate position ?
	(void)F04GlobalField::getObject();

	char typeOfField[100]="Unset";
	float pp1=0; float pp2=0; float pp3=0;
	sscanf(&line[0],"%*s %*s %*s %g %g %g %s",&pp1,&pp2,&pp3,typeOfField);
	G4ThreeVector position = G4ThreeVector(pp1,pp2,pp3);
        float fieldValue=0.000000001;
	float fieldValueFinal=0;
	int fieldNrOfSteps=0;
	//	if (strcmp(tmpString2,"magnetic")==0){
	if (strcmp(typeOfField,"fromfile")==0) {
	  char fieldInputFileName[100];
	  char fieldTableType[100];
	  char logicalVolumeName[100];
	  sscanf(&line[0],"%*s %*s %*s %*g %*g %*g %*s %s %s %s %g %g %d",fieldTableType,fieldInputFileName,logicalVolumeName,
		 &fieldValue,&fieldValueFinal,&fieldNrOfSteps);
	  // Find out the logical volume, to which the field will be placed:
	  G4LogicalVolume* logVol = FindLogicalVolume(logicalVolumeName);
	  if (logVol==NULL) {
	    sprintf(eMessage,"musrDetectorConstruction.cc::Construct(): GLOBAL FIELD: Logical volume \"%s\" not found.",
		    logicalVolumeName);
	    musrErrorMessage::GetInstance()->musrError(FATAL,eMessage,false);
	  }
	  	  
	  // Construct the field
	  musrTabulatedElementField*  myElementTableField = 
	    new musrTabulatedElementField(fieldInputFileName, fieldTableType, fieldValue*tesla, logVol, position);
	  myElementTableField->SetElementFieldName(tmpString2);
	  if (fieldNrOfSteps>0) {
	    //cks The following line might require some correction for the electric field
	    myElementTableField->SetEventNrDependentField(fieldValue*tesla,fieldValueFinal*tesla,fieldNrOfSteps);
	  }
	  //	    FieldList* fields = F04GlobalField::getObject()->getFields();
	  //	    if (fields) {
	  //	      G4cout<<"DEBUG: Detector construction:   fields->size()="<<fields->size()<<G4endl;
	  //	    }
	}

	else if (strcmp(typeOfField,"uniform")==0) {
	  float fieldValue[6];
	  char logicalVolumeName[100];
	  float positionX, positionY, positionZ;
	  G4double half_x = pp1;  // halfwith of the box with the field
	  G4double half_y = pp2;  // halfhight of the box with the field
	  G4double half_z = pp3;  // halflenght of the box with the field

	  sscanf(&line[0],"%*s %*s %*s %*g %*g %*g %*s %g %g %g %s %g %g %g %g %g %g",
		 &positionX, &positionY, &positionZ, logicalVolumeName, 
		 &fieldValue[0],&fieldValue[1],&fieldValue[2],&fieldValue[3],&fieldValue[4],&fieldValue[5]);
	  G4ThreeVector position = G4ThreeVector(positionX,positionY,positionZ);
	  //old	  sscanf(&line[0],"%*s %*s %*s %*g %*g %*g %*s %s %g %g %g %g %g %g", ////// %g %d",
	  //old		    logicalVolumeName, &fieldValue[0],&fieldValue[1],&fieldValue[2],&fieldValue[3],&fieldValue[4],&fieldValue[5]);
	  G4LogicalVolume* logVol = FindLogicalVolume(logicalVolumeName);
	  if (logVol==NULL) {
	    sprintf(eMessage,"musrDetectorConstruction.cc::Construct(): GLOBAL FIELD (uniform): Logical volume \"%s\" not found.", logicalVolumeName);
	    musrErrorMessage::GetInstance()->musrError(FATAL,eMessage,false);
	  }
	  G4double fieldValue_tmp[6] = {
	  fieldValue[0]*tesla,        fieldValue[1]*tesla,        fieldValue[2]*tesla,
	  fieldValue[3]*(kilovolt/mm),fieldValue[4]*(kilovolt/mm),fieldValue[5]*(kilovolt/mm)};
	  
	  musrUniformField* myElementUniformField = new musrUniformField(fieldValue_tmp, half_x, half_y, half_z, logVol, position);
	  myElementUniformField->SetElementFieldName(tmpString2);
	}

	else if (strcmp(typeOfField,"quadrupole")==0) {
	  float halfLength, fieldRadius, gradientValue, gradientValueFinal, fringeFactor;
	  int gradientNrOfSteps;
	  char logicalVolumeName[100];
	  sscanf(&line[0],"%*s %*s %*s %*g %*g %*g %*s %g %g %g %s %g %g %d",&halfLength,&fieldRadius,&fringeFactor,logicalVolumeName,
		 &gradientValue,&gradientValueFinal,&gradientNrOfSteps);
	  G4LogicalVolume* logVol = FindLogicalVolume(logicalVolumeName);
	  if (logVol==NULL) {
	    sprintf(eMessage,"musrDetectorConstruction.cc::Construct(): GLOBAL FIELD: Logical volume \"%s\" not found.",
		    logicalVolumeName);
	    musrErrorMessage::GetInstance()->musrError(FATAL,eMessage,false);
	  }
	  musrQuadrupole* myMusrQuadrupole = new musrQuadrupole(halfLength*mm,fieldRadius*mm,gradientValue*(tesla/m),fringeFactor,logVol,position);
	  myMusrQuadrupole->SetElementFieldName(tmpString2);
	  if (gradientNrOfSteps>0) {
	    myMusrQuadrupole->SetEventNrDependentField(gradientValue*(tesla/m),gradientValueFinal*(tesla/m),gradientNrOfSteps);
	  }
	}

	else if (strcmp(tmpString2,"setparameter")==0){
	  // First check that the magnetic field already exists:
	  G4FieldManager*      fieldMgr  = G4TransportationManager::GetTransportationManager()->GetFieldManager();
	  G4PropagatorInField* propagMgr = G4TransportationManager::GetTransportationManager()->GetPropagatorInField();
	  if (fieldMgr==NULL) {
	    ReportProblemInStearingFile(line);
	    sprintf(eMessage,"musrDetectorConstruction.cc::Construct(): G4FieldManager not found: fieldMgr=NULL");
	    musrErrorMessage::GetInstance()->musrError(FATAL,eMessage,false);
	  }
	  if (propagMgr==NULL) {
	    ReportProblemInStearingFile(line);
	    sprintf(eMessage,"musrDetectorConstruction.cc::Construct(): G4PropagatorInField not found: propagMgr=NULL");
	    musrErrorMessage::GetInstance()->musrError(FATAL,eMessage,false);
	  }
	  else {
	    char parameterName[100];
	    float parameterValue;
	    sscanf(&line[0],"%*s %*s %*s %s %g",parameterName,&parameterValue);
	    if (strcmp(parameterName,"SetDeltaIntersection")==0){ fieldMgr->SetDeltaIntersection(parameterValue*mm); }
	    else if (strcmp(parameterName,"SetDeltaOneStep")==0){ fieldMgr->SetDeltaOneStep(parameterValue*mm); }
	    else if (strcmp(parameterName,"SetMinimumEpsilonStep")==0){ fieldMgr->SetMinimumEpsilonStep(parameterValue); }
	    else if (strcmp(parameterName,"SetMaximumEpsilonStep")==0){ fieldMgr->SetMaximumEpsilonStep(parameterValue); }
	    else if (strcmp(parameterName,"SetLargestAcceptableStep")==0) { propagMgr->SetLargestAcceptableStep(parameterValue*mm); }
	    else if (strcmp(parameterName,"SetMaxLoopCount")==0) {propagMgr->SetMaxLoopCount(int(parameterValue)); }
	    else {
	      G4cout<<"musrDetectorConstruction.cc: ERROR: Unknown parameterName \""
		    <<parameterName<<"\" ."<<G4endl;
	      ReportGeometryProblem(line);
	    }
	  }
	}

	else if (strcmp(tmpString2,"printparameters")==0){    // Print the accuracy parameters for the magnetic field
	  G4FieldManager* fieldMgr = G4TransportationManager::GetTransportationManager()->GetFieldManager();
	  G4PropagatorInField* propagMgr = G4TransportationManager::GetTransportationManager()->GetPropagatorInField();
	  if (fieldMgr==NULL) {
	    G4cout<<"musrDetectorConstruction: ERROR: Field manager not found!"<<G4endl;
	    G4cout<<"                          Can not print the accuracy parameters of the magnetic field."<<G4endl;
	  }
	  else { 
	    G4cout<<"GetDeltaIntersection()    = "<<fieldMgr->GetDeltaIntersection()/mm<<" mm"<<G4endl;
	    G4cout<<"GetDeltaOneStep()         = "<<fieldMgr->GetDeltaOneStep()/mm<<" mm"<<G4endl;
	    G4cout<<"GetMinimumEpsilonStep()   = "<<fieldMgr->GetMinimumEpsilonStep()<<G4endl;
	    G4cout<<"GetMaximumEpsilonStep()   = "<<fieldMgr->GetMaximumEpsilonStep()<<G4endl;
	  }

	  if (propagMgr==NULL) {
	    G4cout<<"musrDetectorConstruction: ERROR: G4PropagatorInField not found!"<<G4endl;
	    G4cout<<"                          Can not print the accuracy parameters of the magnetic field."<<G4endl;
	  }
	  else {
	    G4cout<<"GetLargestAcceptableStep()= "<<propagMgr->GetLargestAcceptableStep()/mm<<" mm"<<G4endl;
	    G4cout<<"GetMaxLoopCount()         = "<<propagMgr->GetMaxLoopCount()<<G4endl;
	  }	    
	}

	else if (strcmp(tmpString2,"printFieldValueAtPoint")==0){    // Print the fieldvalue at the given point
	  float p0, p1, p2;
	  sscanf(&line[0],"%*s %*s %*s %g %g %g",&p0,&p1,&p2);
	  if (F04GlobalField::Exists()) {
	    F04GlobalField* myGlobalField = F04GlobalField::getObject();
	    if (myGlobalField) {
	      // The global field is not properly initialised at this point.  Therefore the points have to
              // be stored in  "F04GlobalField" object  and printed later on in musrRunAction.cc .
	      myGlobalField->AddPointForFieldTesting(G4ThreeVector(p0,p1,p2));
	    }
	    else {
	      sprintf(eMessage,"musrDetectorConstruction.cc::Construct(): printFieldValueAtPoint requested, but field not found");
	      musrErrorMessage::GetInstance()->musrError(SERIOUS,eMessage,false);
	    }
	  }
	}
	
	else {ReportGeometryProblem(line);}
      }
      
      
      //   Set range cut for a given volume, if requested by the macro file
      else if (strcmp(tmpString1,"SetUserLimits")==0){
	G4LogicalVolume* pLogVol = FindLogicalVolume(tmpString2);
	if (pLogVol==NULL) {
	  G4cout << "ERROR! musrDetectorConstruction::Construct():  SetUserLimits: Logical Volume \""
		 << tmpString3 <<"\" not found!"<<G4endl<<"S T O P    F O R C E D"<<G4endl;
	  ReportGeometryProblem(line);
	}
	//	float maxStep, ;
	float ustepMax = -1;
	float utrakMax = -1;
	float utimeMax = -1;
	float uekinMin = -1;
	float urangMin = -1;
	sscanf(&line[0],"%*s %*s %*s %g %g %g %g %g", &ustepMax, &utrakMax, &utimeMax, &uekinMin, &urangMin);
	G4UserLimits* myUserLimits = new G4UserLimits();
	G4cout<<"musrDetectorConstruction.cc:  G4UserLimits in "<<tmpString2<<": ";
	if (ustepMax>0) {myUserLimits->SetMaxAllowedStep(ustepMax*mm);    G4cout<<"ustepMax = "<<ustepMax<<" mm, ";}
	if (utrakMax>0) {myUserLimits->SetUserMaxTrackLength(utrakMax*mm);G4cout<<"utrakMax = "<<utrakMax<<" mm, ";}
 	if (utimeMax>0) {myUserLimits->SetUserMaxTime(utimeMax*ns);       G4cout<<"utimeMax = "<<utimeMax<<" ns, ";}
 	if (uekinMin>0) {myUserLimits->SetUserMinEkine(uekinMin*MeV);     G4cout<<"uekinMin = "<<uekinMin<<" MeV, ";}
 	if (urangMin>0) {myUserLimits->SetUserMinRange(urangMin*mm);      G4cout<<"urangMin = "<<urangMin<<" mm, ";}
	G4cout<<G4endl;
	pLogVol->SetUserLimits(myUserLimits);
      }


      else if (strcmp(tmpString1,"storeOnlyEventsWithHitInDetID")==0){
	G4int variable;
	sscanf(&line[0],"%*s %*s %d",&variable);
	if (variable!=0){ 
	  musrParameters::storeOnlyEventsWithHits = true;
	  musrParameters::storeOnlyEventsWithHitInDetID = variable;
	  char eMessage[200];
	  sprintf(eMessage,
		  "musrDetectorConstruction.cc:: Only the events with at least one hit in the detector ID=%d are stored",
		  variable);
	  musrErrorMessage::GetInstance()->musrError(INFO,eMessage,false);
	}
      } 

      else if (strcmp(tmpString1,"storeOnlyEventsWithHits")==0){
	if (strcmp(tmpString2,"false")==0){ musrParameters::storeOnlyEventsWithHits = false; }
      } 

      else if (strcmp(tmpString1,"storeOnlyTheFirstTimeHit")==0){
	if (strcmp(tmpString2,"true")==0){ musrParameters::storeOnlyTheFirstTimeHit = true; }
      } 

      else if (strcmp(tmpString1,"killAllPositrons")==0){
	if (strcmp(tmpString2,"true")==0){ musrParameters::killAllPositrons = true; }
	else                             { musrParameters::killAllPositrons = false; }
      } 

      else if (strcmp(tmpString1,"killAllGammas")==0){
	if (strcmp(tmpString2,"true")==0){ musrParameters::killAllGammas = true; }
	else                             { musrParameters::killAllGammas = false; }
      } 

      else if (strcmp(tmpString1,"killAllNeutrinos")==0){
	if (strcmp(tmpString2,"true")==0){ musrParameters::killAllNeutrinos = true;  }
	else                             { musrParameters::killAllNeutrinos = false; }
      } 
      
      else if (strcmp(tmpString1,"getDetectorMass")==0){
	G4LogicalVolume* massVol = FindLogicalVolume(tmpString2);
	if (massVol==NULL) {
	  G4cout << "ERROR! musrDetectorConstruction::Construct():   Logical Volume \""
		 << tmpString3 <<"\" not found!"<<G4endl<<"S T O P    F O R C E D"<<G4endl;
	  ReportGeometryProblem(line);
	}
	G4cout<<"Mass of the detector "<<tmpString2<<" is "<<massVol->GetMass()/kg<<" kg."<<G4endl;
      }
      

      else if (strcmp(tmpString1,"signalSeparationTime")==0){
	float timeSeparation;
	sscanf(&line[0],"%*s %*s %g",&timeSeparation);
        musrParameters::signalSeparationTime = timeSeparation*nanosecond;
      }

      else if (strcmp(tmpString1,"maximumRunTimeAllowed")==0){   // in seconds
	float timeMax;
	sscanf(&line[0],"%*s %*s %g",&timeMax);
        musrEventAction::maximumRunTimeAllowed = timeMax;
      }
 

      else if (strcmp(tmpString1,"logicalVolumeToBeReweighted")==0){
	if (strcmp(tmpString2,"mu")==0) { 
	  int eventWeight;
	  char tmpLogVolName[100];
	  sscanf(&line[0],"%*s %*s %*s %s %d",tmpLogVolName,&eventWeight);
	  musrSteppingAction* mySteppingAction = musrSteppingAction::GetInstance();
	  mySteppingAction -> SetVolumeForMuonEventReweighting(G4String(tmpLogVolName),eventWeight);
	}
	else {
	  sprintf(eMessage,
		  "musrDetectorConstruction.cc:: logicalVolumeToBeReweighted - reweighting for particle %s not yet implemented",
		  tmpString2);
	  musrErrorMessage::GetInstance()->musrError(FATAL,eMessage,true);
	}
      }

      // Set G4Regions - intended mainly for the coulomb scattering or special production cuts  (added on 2008.08.21)
      else if (strcmp(tmpString1,"region")==0) {
	if (strcmp(tmpString2,"define")==0) {
	  char charRegionName[100];
	  char charLogicalVolumeName[100];
	  sscanf(&line[0],"%*s %*s %*s %s %s",charRegionName,charLogicalVolumeName);
	  G4String regionName = charRegionName;
	  G4String logicalVolumeName = charLogicalVolumeName;

	  G4Region* myRegion = G4RegionStore::GetInstance()->GetRegion(regionName,false);
	  if( myRegion == NULL )  { // First time - instantiate a region and a cut objects
	    myRegion = new G4Region(regionName);
	    G4cout<<"musrDetectorConstruction:  G4Region "<<regionName<<" newly created, ";
	    G4ProductionCuts*  cuts = new G4ProductionCuts();
	    myRegion->SetProductionCuts(cuts);
	  }
	  else {G4cout<<"musrDetectorConstruction:  G4Region "<<regionName<<" already exists,";}
	  //	  else  { // Second time - get a cut object from region
	  //        cuts = fRegGasDet->GetProductionCuts();
	  //      }
	  G4LogicalVolume* logicalVolume = FindLogicalVolume(logicalVolumeName);
	  if (logicalVolume != NULL) {
	    myRegion->AddRootLogicalVolume(logicalVolume);
	    G4cout<<" and volume "<<logicalVolumeName<<" assigned to it."<<G4endl;
	  }
	  else {
	    G4cout<<G4endl;
	    sprintf(eMessage,
		  "musrDetectorConstruction.cc:: logicalVolume %s not found when requesting its association with the G4Region %s.",
		    charLogicalVolumeName,charRegionName);
	    musrErrorMessage::GetInstance()->musrError(SERIOUS,eMessage,false);
	  }
	}
	else if (strcmp(tmpString2,"setProductionCut")==0) {
	  char charRegionName[100];
	  float fGammaCut=0, fElectronCut=0, fPositronCut=0;
	  sscanf(&line[0],"%*s %*s %*s %s %g %g %g",charRegionName,&fGammaCut,&fElectronCut,&fPositronCut);
	  G4String regionName = charRegionName;
	  G4Region* myRegion = G4RegionStore::GetInstance()->GetRegion(regionName,false);
	  if( myRegion == NULL )  { // G4Region does not exist 
	    G4cout<<"musrDetectorConstruction:  setProductionCut required for the G4Region "<<regionName<<","<<G4endl;
	    G4cout<<"         however this G4Region has not been defined yet.  Please correct the steering file."<<G4endl;
	    G4cout << "S T O P    F O R C E D"<<G4endl;
	    ReportGeometryProblem(line);
	    //    cuts = new G4ProductionCuts();
	    //    myRegion->SetProductionCuts(cuts);
	  }
	  else {  // G4Region exists, so set the cuts:
	    G4ProductionCuts* cuts = myRegion->GetProductionCuts();
	    if (cuts==NULL) {G4cout<<"musrDetectorConstruction: DEBUG - ERROR !!!!  cuts for G4Region not defined!!!!"<<G4endl;}
	    else {
	      if (fGammaCut!=0) cuts->SetProductionCut(fGammaCut,"gamma");
	      if (fElectronCut!=0) cuts->SetProductionCut(fElectronCut,"e-");
	      if (fPositronCut!=0) cuts->SetProductionCut(fPositronCut,"e+");
	    }
	  }
	}
	else {
	  G4cout << "ERROR! musrDetectorConstruction::Construct():   Unknown command requested for the \"region\" keyword."<<G4endl;
	  G4cout << "S T O P    F O R C E D"<<G4endl;
	  ReportGeometryProblem(line);
	}
      }


      //  Set the variables which the user does not want to store in the output root file.
      else if (strcmp(tmpString1,"rootOutput")==0){
	char storeIt[100];
	sscanf(&line[0],"%*s %*s %*s %s",storeIt);
	if (strcmp(storeIt,"off")==0) {
	  if (strcmp(tmpString2,"runID")==0)        {musrRootOutput::store_runID = false;}
	  if (strcmp(tmpString2,"eventID")==0)      {musrRootOutput::store_eventID = false;}
	  if (strcmp(tmpString2,"weight")==0)       {musrRootOutput::store_weight = false;}
	  if (strcmp(tmpString2,"BFieldAtDecay")==0){musrRootOutput::store_BFieldAtDecay = false;}
	  if (strcmp(tmpString2,"muIniTime")==0)    {musrRootOutput::store_muIniTime = false;}
	  if (strcmp(tmpString2,"muIniPosX")==0)    {musrRootOutput::store_muIniPosX = false;}
	  if (strcmp(tmpString2,"muIniPosY")==0)    {musrRootOutput::store_muIniPosY = false;}
	  if (strcmp(tmpString2,"muIniPosZ")==0)    {musrRootOutput::store_muIniPosZ = false;}
	  if (strcmp(tmpString2,"muIniMomX")==0)    {musrRootOutput::store_muIniMomX = false;}
	  if (strcmp(tmpString2,"muIniMomY")==0)    {musrRootOutput::store_muIniMomY = false;}
	  if (strcmp(tmpString2,"muIniMomZ")==0)    {musrRootOutput::store_muIniMomZ = false;}
	  if (strcmp(tmpString2,"muIniPolX")==0)    {musrRootOutput::store_muIniPolX = false;}
	  if (strcmp(tmpString2,"muIniPolY")==0)    {musrRootOutput::store_muIniPolY = false;}
	  if (strcmp(tmpString2,"muIniPolZ")==0)    {musrRootOutput::store_muIniPolZ = false;}
	  if (strcmp(tmpString2,"muDecayDetID")==0) {musrRootOutput::store_muDecayDetID = false;}
	  if (strcmp(tmpString2,"muDecayPosX")==0)  {musrRootOutput::store_muDecayPosX = false;}
	  if (strcmp(tmpString2,"muDecayPosY")==0)  {musrRootOutput::store_muDecayPosY = false;}
	  if (strcmp(tmpString2,"muDecayPosZ")==0)  {musrRootOutput::store_muDecayPosZ = false;}
	  if (strcmp(tmpString2,"muDecayTime")==0)  {musrRootOutput::store_muDecayTime = false;}
	  if (strcmp(tmpString2,"muDecayPolX")==0)  {musrRootOutput::store_muDecayPolX = false;}
	  if (strcmp(tmpString2,"muDecayPolY")==0)  {musrRootOutput::store_muDecayPolY = false;}
	  if (strcmp(tmpString2,"muDecayPolZ")==0)  {musrRootOutput::store_muDecayPolZ = false;}
	  if (strcmp(tmpString2,"muTargetTime")==0) {musrRootOutput::store_muTargetTime = false;}
	  if (strcmp(tmpString2,"muTargetPolX")==0) {musrRootOutput::store_muTargetPolX = false;}
	  if (strcmp(tmpString2,"muTargetPolY")==0) {musrRootOutput::store_muTargetPolY = false;}
	  if (strcmp(tmpString2,"muTargetPolZ")==0) {musrRootOutput::store_muTargetPolZ = false;}
	  if (strcmp(tmpString2,"muM0Time")==0)     {musrRootOutput::store_muM0Time = false;}
	  if (strcmp(tmpString2,"muM0PolX")==0)     {musrRootOutput::store_muM0PolX = false;}
	  if (strcmp(tmpString2,"muM0PolY")==0)     {musrRootOutput::store_muM0PolY = false;}
	  if (strcmp(tmpString2,"muM0PolZ")==0)     {musrRootOutput::store_muM0PolZ = false;}
	  if (strcmp(tmpString2,"muM1Time")==0)     {musrRootOutput::store_muM1Time = false;}
	  if (strcmp(tmpString2,"muM1PolX")==0)     {musrRootOutput::store_muM1PolX = false;}
	  if (strcmp(tmpString2,"muM1PolY")==0)     {musrRootOutput::store_muM1PolY = false;}
	  if (strcmp(tmpString2,"muM1PolZ")==0)     {musrRootOutput::store_muM1PolZ = false;}
	  if (strcmp(tmpString2,"muM2Time")==0)     {musrRootOutput::store_muM2Time = false;}
	  if (strcmp(tmpString2,"muM2PolX")==0)     {musrRootOutput::store_muM2PolX = false;}
	  if (strcmp(tmpString2,"muM2PolY")==0)     {musrRootOutput::store_muM2PolY = false;}
	  if (strcmp(tmpString2,"muM2PolZ")==0)     {musrRootOutput::store_muM2PolZ = false;}
	  if (strcmp(tmpString2,"posIniMomX")==0)   {musrRootOutput::store_posIniMomX = false;}
	  if (strcmp(tmpString2,"posIniMomY")==0)   {musrRootOutput::store_posIniMomY = false;}
	  if (strcmp(tmpString2,"posIniMomZ")==0)   {musrRootOutput::store_posIniMomZ = false;}
	  if (strcmp(tmpString2,"fieldNomVal")==0)  {musrRootOutput::store_fieldNomVal = false;}
	  if (strcmp(tmpString2,"det_ID")==0)       {musrRootOutput::store_det_ID = false;}
	  if (strcmp(tmpString2,"det_edep")==0)     {musrRootOutput::store_det_edep = false;}
	  if (strcmp(tmpString2,"det_edep_el")==0)  {musrRootOutput::store_det_edep_el = false;}
	  if (strcmp(tmpString2,"det_edep_pos")==0) {musrRootOutput::store_det_edep_pos = false;}
	  if (strcmp(tmpString2,"det_edep_gam")==0) {musrRootOutput::store_det_edep_gam = false;}
	  if (strcmp(tmpString2,"det_edep_mup")==0) {musrRootOutput::store_det_edep_mup = false;}
	  if (strcmp(tmpString2,"det_nsteps")==0)   {musrRootOutput::store_det_nsteps = false;}
	  if (strcmp(tmpString2,"det_length")==0)   {musrRootOutput::store_det_length = false;}
	  if (strcmp(tmpString2,"det_start")==0)    {musrRootOutput::store_det_start = false;}
	  if (strcmp(tmpString2,"det_end")==0)      {musrRootOutput::store_det_end = false;}
	  if (strcmp(tmpString2,"det_x")==0)        {musrRootOutput::store_det_x = false;}
	  if (strcmp(tmpString2,"det_y")==0)        {musrRootOutput::store_det_y = false;}
	  if (strcmp(tmpString2,"det_z")==0)        {musrRootOutput::store_det_z = false;}
	  if (strcmp(tmpString2,"det_kine")==0)	    {musrRootOutput::store_det_kine = false;}
	  if (strcmp(tmpString2,"det_VrtxKine")==0) {musrRootOutput::store_det_VrtxKine = false;}
	  if (strcmp(tmpString2,"det_VrtxX")==0)    {musrRootOutput::store_det_VrtxX = false;}
	  if (strcmp(tmpString2,"det_VrtxY")==0)    {musrRootOutput::store_det_VrtxY = false;}
	  if (strcmp(tmpString2,"det_VrtxZ")==0)    {musrRootOutput::store_det_VrtxZ = false;}
	  if (strcmp(tmpString2,"det_VrtxVolID")==0){musrRootOutput::store_det_VrtxVolID = false;}
	  if (strcmp(tmpString2,"det_VrtxProcID")==0){musrRootOutput::store_det_VrtxProcID = false;}
	  if (strcmp(tmpString2,"det_VrtxTrackID")==0){musrRootOutput::store_det_VrtxTrackID = false;}
	  if (strcmp(tmpString2,"det_VrtxParticleID")==0){musrRootOutput::store_det_VrtxParticleID = false;}
	  if (strcmp(tmpString2,"det_VvvKine")==0)  {musrRootOutput::store_det_VvvKine = false;}
	  if (strcmp(tmpString2,"det_VvvX")==0)     {musrRootOutput::store_det_VvvX = false;}
	  if (strcmp(tmpString2,"det_VvvY")==0)     {musrRootOutput::store_det_VvvY = false;}
	  if (strcmp(tmpString2,"det_VvvZ")==0)     {musrRootOutput::store_det_VvvZ = false;}
	  if (strcmp(tmpString2,"det_VvvVolID")==0) {musrRootOutput::store_det_VvvVolID = false;}
	  if (strcmp(tmpString2,"det_VvvProcID")==0){musrRootOutput::store_det_VvvProcID = false;}
	  if (strcmp(tmpString2,"det_VvvTrackID")==0){musrRootOutput::store_det_VvvTrackID = false;}
	  if (strcmp(tmpString2,"det_VvvParticleID")==0){musrRootOutput::store_det_VvvParticleID = false;}
	}
	else if(strcmp(storeIt,"on")==0) {
	  if (strcmp(tmpString2,"fieldIntegralBx")==0){musrRootOutput::store_fieldIntegralBx = true;}
	  if (strcmp(tmpString2,"fieldIntegralBy")==0){musrRootOutput::store_fieldIntegralBy = true;}
	  if (strcmp(tmpString2,"fieldIntegralBz")==0){musrRootOutput::store_fieldIntegralBz = true;}
	  if (strcmp(tmpString2,"fieldIntegralBz1")==0){musrRootOutput::store_fieldIntegralBz1 = true;}
	  if (strcmp(tmpString2,"fieldIntegralBz2")==0){musrRootOutput::store_fieldIntegralBz2 = true;}
	  if (strcmp(tmpString2,"fieldIntegralBz3")==0){musrRootOutput::store_fieldIntegralBz3 = true;}
	  if (strcmp(tmpString2,"fieldIntegralEx")==0){musrRootOutput::store_fieldIntegralEx = true;}
	  if (strcmp(tmpString2,"fieldIntegralEy")==0){musrRootOutput::store_fieldIntegralEy = true;}
	  if (strcmp(tmpString2,"fieldIntegralEz")==0){musrRootOutput::store_fieldIntegralEz = true;}


	  if ((musrRootOutput::store_fieldIntegralBx) ||(musrRootOutput::store_fieldIntegralBy) ||
	      (musrRootOutput::store_fieldIntegralBz) ||(musrRootOutput::store_fieldIntegralBz1)||
	      (musrRootOutput::store_fieldIntegralBz2)||(musrRootOutput::store_fieldIntegralBz3)||
	      (musrRootOutput::store_fieldIntegralEx) ||(musrRootOutput::store_fieldIntegralEy) ||
	      (musrRootOutput::store_fieldIntegralEz) ){
	    musrSteppingAction::GetInstance()->SetCalculationOfFieldIntegralRequested(true);
	  }
	  // This may cause the two roop of SetCalculationOfFieldIntegralRequested(true)
	  // if ((musrRootOutput::store_fieldIntegralEx)||(musrRootOutput::store_fieldIntegralEy)||
	  //     (musrRootOutput::store_fieldIntegralEz) ){
	  //   musrSteppingAction::GetInstance()->SetCalculationOfFieldIntegralRequested(true);
	  // }
	}
      }
      
      else if (strcmp(tmpString1,"process")==0) {
	;  // processes are interpreded later in  musrPhysicsList.cc
      }


      else ReportGeometryProblem(line);
      
    }
  }
  fclose(fSteeringFile);
  //  G4cout<< "musrDetectorConstruction.cc:  pointerToWorldVolume="<<pointerToWorldVolume<<G4endl;
  return pointerToWorldVolume;
}


void musrDetectorConstruction::DefineMaterials()
{

//--------- Material definitions ---------
  G4double a, z, density;  // a = mass of a mole,   z = mean number of protons;

  G4int ncomponents, natoms;
  G4double  fractionmass;

  G4NistManager* man = G4NistManager::Instance();
  
  //
  // define Elements
  //
  G4Element* H = man->FindOrBuildElement("H");
  G4Element* C = man->FindOrBuildElement("C");
  G4Element* N = man->FindOrBuildElement("N");
  G4Element* O = man->FindOrBuildElement("O");
  G4Element* F = man->FindOrBuildElement("F");
  // Elements required for Brass
  G4Element* Cu = man->FindOrBuildElement("Cu");
  G4Element* Zn = man->FindOrBuildElement("Zn");
  // elements required for Stainless Steel
  G4Element* Cr = man->FindOrBuildElement("Cr");
  G4Element* Fe = man->FindOrBuildElement("Fe");
  G4Element* Ni = man->FindOrBuildElement("Ni");
 // Elements required for MCPglass
  G4Element* Pb = man->FindOrBuildElement("Pb"); 
  G4Element* Si = man->FindOrBuildElement("Si"); 
  G4Element* K  = man->FindOrBuildElement("K" ); 
  G4Element* Rb = man->FindOrBuildElement("Rb"); 
  G4Element* Ba = man->FindOrBuildElement("Ba"); 
  G4Element* As = man->FindOrBuildElement("As"); 
  G4Element* Cs = man->FindOrBuildElement("Cs"); 
  G4Element* Na = man->FindOrBuildElement("Na"); 
  
  // Elements required for Macor
  G4Element* B  = man->FindOrBuildElement("B" );
  G4Element* Al = man->FindOrBuildElement("Al");
  G4Element* Mg = man->FindOrBuildElement("Mg");
  
  // Presurized Hydrogen gas
  G4double temperature=300.*kelvin;
  G4double pressure=49.35*atmosphere;
  G4Material* H2_50bar = new G4Material("H2_50bar", 4.131*mg/cm3, ncomponents=1, kStateGas, temperature, pressure);
  H2_50bar->AddElement(H, natoms=2);
  
  

  // compounds required for MCP Macor
  G4Material* MgO = new G4Material("MgO", 3.60*g/cm3, ncomponents=2);
  MgO->AddElement(Mg, natoms=1);
  MgO->AddElement(O,  natoms=1);

  G4Material* SiO2 = new G4Material("SiO2", 2.533*g/cm3, ncomponents=2); // quartz
  SiO2->AddElement(O,  natoms=2);
  SiO2->AddElement(Si, natoms=1);

  G4Material* Al2O3 = new G4Material("Al2O3", 3.985*g/cm3, ncomponents=2); // saphire
  Al2O3->AddElement (Al, natoms=2);
  Al2O3->AddElement (O,  natoms=3);
 
  G4Material* K2O = new G4Material("K2O", 2.350*g/cm3, ncomponents=2);
  K2O->AddElement(O, natoms=1);
  K2O->AddElement(K, natoms=2);
  
  G4Material* B2O3 = new G4Material("B2O3", 2.550*g/cm3, ncomponents=2);
  B2O3->AddElement (B, natoms=2);
  B2O3->AddElement (O, natoms=3);

  G4Material* Sci = 
    new G4Material("Scintillator", density= 1.032*g/cm3, ncomponents=2);
  Sci->AddElement(C, natoms=9);
  Sci->AddElement(H, natoms=10);

  G4Material* Myl = 
    new G4Material("Mylar", density= 1.397*g/cm3, ncomponents=3);
  Myl->AddElement(C, natoms=10);
  Myl->AddElement(H, natoms= 8);
  Myl->AddElement(O, natoms= 4);

  // Brass
  G4Material* brass = new G4Material("Brass", density= 8.40*g/cm3, ncomponents=2);
  brass -> AddElement(Zn, fractionmass = 30*perCent);
  brass -> AddElement(Cu, fractionmass = 70*perCent);

  // Stainless steel
  G4Material* steel = new G4Material("Steel", density= 7.93*g/cm3, ncomponents=3);
  steel->AddElement(Ni, fractionmass=0.11);
  steel->AddElement(Cr, fractionmass=0.18);
  steel->AddElement(Fe, fractionmass=0.71);

  G4Material* macor= // Macor (used in the MCP detector)
    new G4Material("Macor", density=2.52*g/cm3, ncomponents=5);
  macor->AddMaterial(SiO2, fractionmass=0.470); // quartz
  macor->AddMaterial(MgO,  fractionmass=0.180);
  macor->AddMaterial(Al2O3,fractionmass=0.170); // saphire
  macor->AddMaterial(K2O,  fractionmass=0.105);
  macor->AddMaterial(B2O3, fractionmass=0.075);
    
  G4Material* mcpglass = // Glass of the Multi Channel Plate
    new G4Material("MCPglass", density=2.0*g/cm3, ncomponents=9);
  mcpglass->AddElement(Pb, fractionmass= 0.480);
  mcpglass->AddElement(O,  fractionmass= 0.258);
  mcpglass->AddElement(Si, fractionmass= 0.182);
  mcpglass->AddElement(K,  fractionmass= 0.042);
  mcpglass->AddElement(Rb, fractionmass= 0.018);
  mcpglass->AddElement(Ba, fractionmass= 0.013);
  mcpglass->AddElement(As, fractionmass= 0.004);
  mcpglass->AddElement(Cs, fractionmass= 0.002);
  mcpglass->AddElement(Na, fractionmass= 0.001);

  // High density aerogel
  G4Material* HDAerogel = new G4Material("HDAerogel", 1.2*g/cm3, ncomponents=2);
  //  G4Material* HDAerogel = new G4Material("HDAerogel", 0.6*g/cm3, ncomponents=2);
  HDAerogel->AddElement(O,  natoms=2);
  HDAerogel->AddElement(Si, natoms=1);

  // Normal density aerogel
  G4Material* Aerogel = new G4Material("Aerogel", 0.2*g/cm3, ncomponents=2);
  Aerogel->AddElement(O,  natoms=2);
  Aerogel->AddElement(Si, natoms=1);

  G4Material* Aerogel04 = new G4Material("Aerogel04", 0.4*g/cm3, ncomponents=2);
  Aerogel04->AddElement(O,  natoms=2);
  Aerogel04->AddElement(Si, natoms=1);

  G4Material* Aerogel08 = new G4Material("Aerogel08", 0.8*g/cm3, ncomponents=2);
  Aerogel08->AddElement(O,  natoms=2);
  Aerogel08->AddElement(Si, natoms=1);

  G4Material* Aerogel0027 = new G4Material("Aerogel0027", 0.027*g/cm3, ncomponents=2);
  Aerogel0027->AddElement(O,  natoms=2);
  Aerogel0027->AddElement(Si, natoms=1);

  G4Material* Aerogel0030 = new G4Material("Aerogel0030", 0.030*g/cm3, ncomponents=2);
  Aerogel0030->AddElement(O,  natoms=2);
  Aerogel0030->AddElement(Si, natoms=1);

  G4Material* Aerogel005 = new G4Material("Aerogel005", 0.049*g/cm3, ncomponents=2);
  Aerogel005->AddElement(O,  natoms=2);
  Aerogel005->AddElement(Si, natoms=1);

  G4Material* Aerogel010 = new G4Material("Aerogel010", 0.099*g/cm3, ncomponents=2);
  Aerogel010->AddElement(O,  natoms=2);
  Aerogel010->AddElement(Si, natoms=1);

  G4Material* SilicaPlate = new G4Material("SilicaPlate", 2.203*g/cm3, ncomponents=2);
  SilicaPlate->AddElement(O,  natoms=2);
  SilicaPlate->AddElement(Si, natoms=1);

  // DME
  G4Material* DME = new G4Material("DME", 1.290*1.59*mg/cm3, ncomponents=3);
  DME->AddElement(C, natoms=2);
  DME->AddElement(H, natoms=6);
  DME->AddElement(O, natoms=1);

  // Tedlar
  G4Material* Tedlar = new G4Material("Tedlar", 1.7*mg/cm3, ncomponents=3);
  Tedlar->AddElement(C, natoms=2);
  Tedlar->AddElement(H, natoms=3);
  Tedlar->AddElement(F, natoms=1);

//
// define a material from elements.   case 2: mixture by fractional mass
//

  G4Material* Air = 
    new G4Material("Air"  , density= 1.290*mg/cm3, ncomponents=2);
  Air->AddElement(N, fractionmass=0.7);
  Air->AddElement(O, fractionmass=0.3);

//
// Lead
//
  G4Material* Lead = man->FindOrBuildMaterial("G4_Pb");
  G4cout<<"lead="<<Lead<<G4endl;
//
// examples of vacuum
//

//  G4Material* Vacuum =
  new G4Material("Vacuum", z=1., a=1.01*g/mole,density= universe_mean_density,
		   kStateGas, 2.73*kelvin, 3.e-18*pascal);
  
  new G4Material("ArgonGas", z= 18., a= 39.95*g/mole, density= 0.00000000001*mg/cm3);

  if (musrParameters::boolG4OpticalPhotons) {
    G4NistManager* man = G4NistManager::Instance();
    G4Material* scintik = man->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");   
    //    G4Material* scintik = G4Material::GetMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
    G4cout<<"scintik="<<scintik<<G4endl;
    if (scintik!=NULL) {
      const G4int nEntries = 14;
      G4double PhotonEnergy[nEntries] = 
	{ 2.695*eV,   2.75489*eV, 2.8175*eV,  2.88302*eV,                 // 460, 450, 440, 430 nm
	  2.95167*eV, 3.02366*eV, 3.09925*eV, 3.17872*eV, 3.26237*eV,     // 420, 410, 400, 390, 380 nm
	  3.30587*eV, 3.35054*eV, 3.44361*eV, 3.542*eV,   3.64618*eV };   // 375, 370, 360, 350, 340 nm
      G4double RefractiveIndex[nEntries] = 
	{ 1.58, 1.58, 1.58, 1.58, 
	  1.58, 1.58, 1.58, 1.58, 1.58,
	  1.58, 1.58, 1.58, 1.58, 1.58  };
      G4double Absorption[nEntries] =
	{ 8*cm, 8*cm, 8*cm, 8*cm, 
	  8*cm, 8*cm, 8*cm, 8*cm, 8*cm,
	  8*cm, 8*cm, 8*cm, 8*cm, 8*cm  };
      G4double ScintilFast[nEntries] =
	{ 0.01,  0.07, 0.15, 0.26,
	  0.375, 0.52, 0.65, 0.80, 0.95,
          1,     0.88, 0.44, 0.08, 0.01 };
      G4double ScintilSlow[nEntries] =
	{ 0.01,  0.07, 0.15, 0.26,
	  0.375, 0.52, 0.65, 0.80, 0.95,
          1,     0.88, 0.44, 0.08, 0.01 };
      G4MaterialPropertiesTable* myMPT1 = new G4MaterialPropertiesTable();
      myMPT1->AddProperty("RINDEX",        PhotonEnergy, RefractiveIndex, nEntries);
      myMPT1->AddProperty("ABSLENGTH",     PhotonEnergy, Absorption,      nEntries);
      myMPT1->AddProperty("FASTCOMPONENT", PhotonEnergy, ScintilFast,     nEntries);
      myMPT1->AddProperty("SLOWCOMPONENT", PhotonEnergy, ScintilSlow,     nEntries);
      myMPT1->AddConstProperty("SCINTILLATIONYIELD", 8400./MeV);
      myMPT1->AddConstProperty("RESOLUTIONSCALE",1.0);
      myMPT1->AddConstProperty("FASTTIMECONSTANT",1.6*ns);
      myMPT1->AddConstProperty("SLOWTIMECONSTANT",1.6*ns);
      myMPT1->AddConstProperty("YIELDRATIO",1.0);
      scintik->SetMaterialPropertiesTable(myMPT1);

      scintik->GetMaterialPropertiesTable()->DumpTable();
    }
    G4cout<<"OK konec"<<G4endl;
    exit(0);
  }
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


#include "G4RunManager.hh"

void musrDetectorConstruction::UpdateGeometry()
{
  G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
}


void musrDetectorConstruction::ReportGeometryProblem(char myString[501]) {
  G4cout<<"\nE R R O R  in musrDetectorConstruction.cc:    "
	<<"Unknown keyword requested in \""<< parameterFileName <<"\" :"<<G4endl;
  G4cout<<"\""<<myString<<"\""<<G4endl;
  G4cout<<"S T O P     F O R C E D!"<<G4endl;
  exit(1);
}

void musrDetectorConstruction::ReportProblemInStearingFile(char* myString) {
  G4cout<<"\nE R R O R  in musrDetectorConstruction.cc:    "
	<<"Problem to interpret/execute the following line in \""<< parameterFileName <<"\" :"<<G4endl;
  G4cout<<"\""<<myString<<"\""<<G4endl;
}


G4Material* musrDetectorConstruction::CharToMaterial(char myString[100]) {
  G4Material* Material=NULL;
  G4String materialName=myString;
  G4NistManager* man = G4NistManager::Instance();
  Material = man->FindOrBuildMaterial(materialName);   
  if (Material==NULL) {
    G4cout<<"\nE R R O R  in musrDetectorConstruction.cc::CharToMaterial    "
	<<"Unknown material requested:"<<G4endl;
    G4cout<<myString<<G4endl;
    G4cout<<"S T O P     F O R C E D!"<<G4endl;
    exit(1);
  }
  return Material;
}


G4LogicalVolume* musrDetectorConstruction::FindLogicalVolume(G4String LogicalVolumeName) {
  G4LogicalVolumeStore* pLogStore = G4LogicalVolumeStore::GetInstance();
  if (pLogStore==NULL) {
    G4cout<<"ERROR:  musrDetectorConstruction.cc:    G4LogicalVolumeStore::GetInstance()  not found!"<<G4endl;
  }
  else {
   //	  for (G4int i=0; i<pLogStore->entries(); i++) {
    for (unsigned int i=0; i<pLogStore->size(); i++) {
      G4LogicalVolume* pLogVol=pLogStore->at(i);
      G4String iLogName=pLogVol->GetName(); 
      if (iLogName==LogicalVolumeName) {
	return pLogVol;
      }
    }
  }
  return NULL;
}

void musrDetectorConstruction::SetColourOfLogicalVolume(G4LogicalVolume* pLogVol,char* colour) {
  if (pLogVol!=NULL) {
    if      (strcmp(colour,"red"    )==0) {pLogVol->SetVisAttributes(G4Colour(1,0,0));}
    else if (strcmp(colour,"green"  )==0) {pLogVol->SetVisAttributes(G4Colour(0,1,0));}
    else if (strcmp(colour,"blue"   )==0) {pLogVol->SetVisAttributes(G4Colour(0,0,1));}
    else if (strcmp(colour,"lightblue")==0) {pLogVol->SetVisAttributes(G4Colour(0,1,1));}
    else if (strcmp(colour,"white"  )==0) {pLogVol->SetVisAttributes(G4Colour(1,1,1));}
    else if (strcmp(colour,"yellow" )==0) {pLogVol->SetVisAttributes(G4Colour(1,1,0));}
    else if (strcmp(colour,"black"  )==0) {pLogVol->SetVisAttributes(G4Colour(0,0,0));}
    else if (strcmp(colour,"gray"   )==0) {pLogVol->SetVisAttributes(G4Colour(0.5,0.5,0.5));}
    else if (strcmp(colour,"cyan"   )==0) {pLogVol->SetVisAttributes(G4Colour(0,1,1));}
    else if (strcmp(colour,"magenta")==0) {pLogVol->SetVisAttributes(G4Colour(1,0,1));}
    else if (strcmp(colour,"invisible" )==0) {pLogVol->SetVisAttributes(G4VisAttributes::Invisible);}

    else if (strcmp(colour,"blue_style")==0) {pLogVol->SetVisAttributes(G4Colour(0.80,0.83,1));}
    //    else if (strcmp(colour,"lightblue")==0) {pLogVol->SetVisAttributes(G4Colour(0,0.5,1));}
    else if (strcmp(colour,"darkblue")==0) {pLogVol->SetVisAttributes(G4Colour(0,0.25,0.5));}
    else if (strcmp(colour,"fblue_style")==0) {pLogVol->SetVisAttributes(G4Colour(0.85,.88,0.92));}
    else if (strcmp(colour,"oxsteel")==0) {pLogVol->SetVisAttributes(G4Colour(0.9,0.8,0.75));}
    else if (strcmp(colour,"darkred")==0) {pLogVol->SetVisAttributes(G4Colour(0.5,0,0));}
    else if (strcmp(colour,"MCP_style")==0) {pLogVol->SetVisAttributes(G4Colour(0.5,0.2,.7));}
    else if (strcmp(colour,"MACOR_style")==0) {pLogVol->SetVisAttributes(G4Colour(0.9,0.9,.1));}
    else if (strcmp(colour,"SCINT_style")==0) {pLogVol->SetVisAttributes(G4Colour(0.5,0.5,.75));}
    else if (strcmp(colour,"dSCINT_style")==0) {pLogVol->SetVisAttributes(G4Colour(0.3,0.3,0.3));}
    else if (strcmp(colour,"VTBB_style")==0) {pLogVol->SetVisAttributes(G4Colour(0.9,0.9,.9));}
    else if (strcmp(colour,"Grid_style")==0) {pLogVol->SetVisAttributes(G4Colour(0.87,0.72,0.53));} //burlywood
    else if (strcmp(colour,"RA_style")==0) {pLogVol->SetVisAttributes(G4Colour(0.8549,0.6471,0.1255));} //goldenrod

    else {
      G4cout<<"ERROR: musrDetectorConstruction::SetColourOfLogicalVolume:   unknown colour requested: "<<colour<<G4endl;
      G4cout<<"    Ignoring and continuing"<<G4endl;
    }
  }
}

