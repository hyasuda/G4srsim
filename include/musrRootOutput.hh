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

#ifndef musrRootOutput_h
#define musrRootOutput_h 1
//#include "G4UserRunAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
//  ROOT
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TVectorD.h"
//
#include <map>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class musrRootOutput  {
public:
  musrRootOutput();
  ~musrRootOutput();
  static musrRootOutput* GetRootInstance();

public:
  void BeginOfRunAction();
  void EndOfRunAction();
  void FillEvent();
  void ClearAllRootVariables();
  void SetVolumeIDMapping(std::string logivol, int volumeID);
  G4int ConvertVolumeToID(std::string logivol);
  G4int ConvertProcessToID(std::string processName);
  void SetSpecialSaveVolumeDefined() {boolIsAnySpecialSaveVolumeDefined=true;};

  // Getting variables (just for debugging)
  G4double GetDecayPositionZ() {return muDecayPosZ_t;};
  G4double GetDecayTime()      {return muDecayTime_t*microsecond;};
  G4double GetTimeInTarget()   {return muTargetTime_t*microsecond;};

  // Setting variables common to the whole event:
  void SetRunID          (G4int id) {runID_t = id;};
  void SetEventID        (G4int id) {eventID_t = id;};
  void SetDecayDetectorID (std::string detectorName) {muDecayDetID_t = SensDetectorMapping[detectorName];};
  void SetBField       (G4double F[6]) {B_t[0]=F[0]/tesla; B_t[1]=F[1]/tesla; B_t[2]=F[2]/tesla; 
    B_t[3]=F[3]/tesla; B_t[4]=F[4]/tesla; B_t[5]=F[5]/tesla;};
  void SetDecayPolarisation (G4ThreeVector pol) {muDecayPolX_t=pol.x(); muDecayPolY_t=pol.y(); muDecayPolZ_t=pol.z();};
  void SetDecayPosition (G4ThreeVector pos) {muDecayPosX_t=pos.x()/mm; muDecayPosY_t=pos.y()/mm; 
    muDecayPosZ_t=pos.z()/mm;};
  void SetEventWeight  (G4double w) {weight_t *= w;}
  void SetDetectorInfo (G4int nDetectors, G4int ID, G4int particleID, G4double edep, 
			G4double edep_el, G4double edep_pos, 
			G4double edep_gam, G4double edep_mup,G4int nsteps, G4double length, G4double t1, 
			G4double t2, G4double x, G4double y, G4double z,
			G4double ek, G4double ekVertex, G4double xVertex, G4double yVertex, G4double zVertex, 
			G4int idVolVertex, G4int idProcVertex, G4int idTrackVertex) ;

  void SetDetectorInfoVvv (G4int nDetectors,
			   G4double ekVertex, G4double xVertex, G4double yVertex, G4double zVertex, 
			   G4int idVolVertex, G4int idProcVertex, G4int idTrackVertex, G4int particleID) ;

  void SetSaveDetectorInfo (G4int ID, G4int particleID, G4double ke, G4double x, G4double y, G4double z, 
			    G4double px, G4double py, G4double pz) ;

  void SetInitialMuonParameters(G4double x, G4double y, G4double z, G4double px, G4double py, G4double pz, 
				G4double xpolaris, G4double ypolaris, G4double zpolaris, G4double particleTime) {
    muIniTime_t=particleTime/microsecond;
    muIniPosX_t=x;  muIniPosY_t=y;  muIniPosZ_t=z;
    muIniMomX_t=px; muIniMomY_t=py; muIniMomZ_t=pz;
    muIniPolX_t=xpolaris; muIniPolY_t=ypolaris; muIniPolZ_t=zpolaris; 
  }
  void PrintInitialMuonParameters() {
    G4cout<<"musrRootOutput.hh: Initial muon parameters: x="<<muIniPosX_t<<", y="<<muIniPosY_t<<", z="<<muIniPosZ_t
	  <<", px="<<muIniMomX_t << ", py="<<muIniMomY_t<<", pz="<<muIniMomZ_t<<G4endl;
    G4cout<<"            polx="<<muIniPolX_t<<", poly="<<muIniPolY_t<<", polz="<<muIniPolZ_t<<G4endl;
    G4cout<<"            time at which muon was generated = "<<muIniTime_t<<G4endl;
    G4cout<<"            numberOfGeneratedEvents = "<<GeantParametersD[7]<<G4endl;
  }

  void SetPolInTarget(G4ThreeVector pol) {muTargetPolX_t=pol.x(); muTargetPolY_t=pol.y(); muTargetPolZ_t=pol.z();};
  void SetTimeInTarget(G4double time) {muTargetTime_t = time/microsecond;};
  void SetPolInM0(G4ThreeVector pol) {muM0PolX_t=pol.x(); muM0PolY_t=pol.y(); muM0PolZ_t=pol.z();};
  void SetTimeInM0(G4double time) {muM0Time_t = time/microsecond;};
  void SetPolInM1(G4ThreeVector pol) {muM1PolX_t=pol.x(); muM1PolY_t=pol.y(); muM1PolZ_t=pol.z();};
  void SetTimeInM1(G4double time) {muM1Time_t = time/microsecond;};
  void SetPolInM2(G4ThreeVector pol) {muM2PolX_t=pol.x(); muM2PolY_t=pol.y(); muM2PolZ_t=pol.z();};
  void SetTimeInM2(G4double time) {muM2Time_t = time/microsecond;};
  void SetInitialPositronMomentum(G4ThreeVector mom) {posIniMomx_t=mom.x();  posIniMomy_t=mom.y(); posIniMomz_t=mom.z();};
  void SetDecayTime(G4double time) {muDecayTime_t=time/microsecond;};
  void SetNrFieldNomVal(G4int n) {nFieldNomVal = n;}
  void SetFieldNomVal(G4int i, G4double value);
  G4int GetNrOfVolumes() {return det_nMax;}
  void SetBFieldIntegral(G4double BxInt,G4double ByInt,G4double BzInt,G4double BzInt1,G4double BzInt2,G4double BzInt3) {
    BxIntegral_t=BxInt/m/tesla; ByIntegral_t=ByInt/m/tesla; BzIntegral_t=BzInt/m/tesla;
    BzIntegral1_t=BzInt1/m/tesla;BzIntegral2_t=BzInt2/mm;BzIntegral3_t=BzInt3/mm;
  }
  void SetEFieldIntegral(G4double ExInt,G4double EyInt,G4double EzInt) {
    ExIntegral_t=ExInt/(kilovolt/mm)/m; EyIntegral_t=EyInt/(kilovolt/mm)/m; EzIntegral_t=EzInt/(kilovolt/mm)/m;
  }
  void StoreGeantParameter(Int_t i, Double_t value) {
    if (i<maxNGeantParameters) { GeantParametersD[i]=value; }
    else {G4cout<<"musrRootOutput.hh::StoreGeantParameter:  index="<<i<<" out of range"
		<<" (maxNGeantParameters=" <<maxNGeantParameters<<")"<<G4endl;}
  };
    

  TH2F *htest1, *htest2;
  TH1F *htest3, *htest4, *htest5, *htest6, *htest7, *htest8;

public:
  static G4bool store_runID;
  static G4bool store_eventID;
  static G4bool store_weight;
  static G4bool store_BFieldAtDecay;
  static G4bool store_muIniTime;
  static G4bool store_muIniPosX;
  static G4bool store_muIniPosY;
  static G4bool store_muIniPosZ;
  static G4bool store_muIniMomX;
  static G4bool store_muIniMomY;
  static G4bool store_muIniMomZ;
  static G4bool store_muIniPolX;
  static G4bool store_muIniPolY;
  static G4bool store_muIniPolZ;
  static G4bool store_muDecayDetID;
  static G4bool store_muDecayPosX;
  static G4bool store_muDecayPosY;
  static G4bool store_muDecayPosZ;
  static G4bool store_muDecayTime;
  static G4bool store_muDecayPolX;
  static G4bool store_muDecayPolY;
  static G4bool store_muDecayPolZ;
  static G4bool store_muTargetTime;
  static G4bool store_muTargetPolX;
  static G4bool store_muTargetPolY;
  static G4bool store_muTargetPolZ;
  static G4bool store_muM0Time;
  static G4bool store_muM0PolX;
  static G4bool store_muM0PolY;
  static G4bool store_muM0PolZ;
  static G4bool store_muM1Time;
  static G4bool store_muM1PolX;
  static G4bool store_muM1PolY;
  static G4bool store_muM1PolZ;
  static G4bool store_muM2Time;
  static G4bool store_muM2PolX;
  static G4bool store_muM2PolY;
  static G4bool store_muM2PolZ;
  static G4bool store_posIniMomX;
  static G4bool store_posIniMomY;
  static G4bool store_posIniMomZ;
  static G4bool store_det_ID;
  static G4bool store_det_edep;
  static G4bool store_det_edep_el;
  static G4bool store_det_edep_pos;
  static G4bool store_det_edep_gam;
  static G4bool store_det_edep_mup;
  static G4bool store_det_nsteps;
  static G4bool store_det_length;
  static G4bool store_det_start;
  static G4bool store_det_end;
  static G4bool store_det_x;
  static G4bool store_det_y;
  static G4bool store_det_z;
  static G4bool store_det_kine;
  static G4bool store_det_VrtxKine;
  static G4bool store_det_VrtxX;
  static G4bool store_det_VrtxY;
  static G4bool store_det_VrtxZ;
  static G4bool store_det_VrtxVolID;
  static G4bool store_det_VrtxProcID;
  static G4bool store_det_VrtxTrackID;
  static G4bool store_det_VrtxParticleID;
  static G4bool store_det_VvvKine;
  static G4bool store_det_VvvX;
  static G4bool store_det_VvvY;
  static G4bool store_det_VvvZ;
  static G4bool store_det_VvvVolID;
  static G4bool store_det_VvvProcID;
  static G4bool store_det_VvvTrackID;
  static G4bool store_det_VvvParticleID;
  static G4bool store_fieldNomVal;
  static G4bool store_fieldIntegralBx;
  static G4bool store_fieldIntegralBy;
  static G4bool store_fieldIntegralBz;
  static G4bool store_fieldIntegralBz1;
  static G4bool store_fieldIntegralBz2;
  static G4bool store_fieldIntegralBz3;
  static G4bool store_fieldIntegralEx;
  static G4bool store_fieldIntegralEy;
  static G4bool store_fieldIntegralEz;


  static G4int oldEventNumberInG4EqEMFieldWithSpinFunction;

private:
  TFile* rootFile;
  TTree* rootTree;
  static musrRootOutput* pointerToRoot;
  static const Int_t maxNGeantParameters=30;
  Double_t GeantParametersD[maxNGeantParameters];   // parameters transfered from GEANT to Root
  // 0 ... fieldOption:  0 ... no field, 1 ... uniform, 2 ... gaussian, 3 ... from table
  // 1 ... fieldValue:   intensity of the magnetic field         
  // 2 ... minimum of the generated decay time of the muon (in microsecond)
  // 3 ... maximum of the generated decay time of the muon (in microsecond)
  // 4 ... muon mean life time (in microsecond)
  // 5 ... nr. of the last generated event
  // 6 ... run number
  // 7 ... numberOfGeneratedEvents (i.e. number of the generated events; 
  //                    in case of Turtle nr. of events tried); 

  // Variables common to the whole event:
  Int_t runID_t;
  Int_t eventID_t;
  Double_t weight_t;
  Double_t B_t[6];
  Double_t muIniTime_t;
  Double_t muIniPosX_t, muIniPosY_t, muIniPosZ_t;
  Double_t muIniMomX_t, muIniMomY_t, muIniMomZ_t;
  Double_t muIniPolX_t, muIniPolY_t, muIniPolZ_t;
  Int_t    muDecayDetID_t;
  Double_t muDecayPolX_t, muDecayPolY_t, muDecayPolZ_t;
  Double_t muTargetTime_t, muTargetPolX_t, muTargetPolY_t, muTargetPolZ_t;
  Double_t muM0Time_t, muM0PolX_t, muM0PolY_t, muM0PolZ_t;
  Double_t muM1Time_t, muM1PolX_t, muM1PolY_t, muM1PolZ_t;
  Double_t muM2Time_t, muM2PolX_t, muM2PolY_t, muM2PolZ_t;
  Double_t muDecayPosX_t, muDecayPosY_t, muDecayPosZ_t;
  Double_t muDecayTime_t;
  Double_t posIniMomx_t, posIniMomy_t, posIniMomz_t;

public:
  static const Int_t maxNFieldnNominalValues=30;
private:
  Int_t     nFieldNomVal;
  Double_t  fieldNomVal[maxNFieldnNominalValues];
  Double_t  BxIntegral_t, ByIntegral_t, BzIntegral_t;
  Double_t  BzIntegral1_t, BzIntegral2_t, BzIntegral3_t;
  Double_t  ExIntegral_t, EyIntegral_t, EzIntegral_t;

  // Variables for a particle in a given detector within the event
public:
  static const Int_t maxNSubTracks=30;
private:
  // Variables for the activity inside a given detector
public:
  static const Int_t det_nMax=100;    // must be by 1 higher than the real number of detector "hits", because
  // else the detector nr. 0 is counted (0 is used if no
  // SensDetectorMapping correspond to a given logical volume).
private:
  G4int     det_n;
  G4int     det_ID[det_nMax];
  G4double  det_edep[det_nMax];
  G4int     det_nsteps[det_nMax];
  G4double  det_length[det_nMax];
  G4double  det_edep_el[det_nMax];
  G4double  det_edep_pos[det_nMax];
  G4double  det_edep_gam[det_nMax];
  G4double  det_edep_mup[det_nMax];
  G4double  det_time_start[det_nMax];
  G4double  det_time_end[det_nMax];
  G4double  det_x[det_nMax];
  G4double  det_y[det_nMax];
  G4double  det_z[det_nMax];
  G4double  det_kine[det_nMax];
  G4double  det_VrtxKine[det_nMax];
  G4double  det_VrtxX[det_nMax];
  G4double  det_VrtxY[det_nMax];
  G4double  det_VrtxZ[det_nMax];
  G4int     det_VrtxVolID[det_nMax];
  G4int     det_VrtxProcID[det_nMax];
  G4int     det_VrtxTrackID[det_nMax];
  G4int     det_VrtxParticleID[det_nMax];
  G4double  det_VvvKine[det_nMax];
  G4double  det_VvvX[det_nMax];
  G4double  det_VvvY[det_nMax];
  G4double  det_VvvZ[det_nMax];
  G4int     det_VvvVolID[det_nMax];
  G4int     det_VvvProcID[det_nMax];
  G4int     det_VvvTrackID[det_nMax];
  G4int     det_VvvParticleID[det_nMax];

public:
  static const Int_t save_nMax=2000;

private:
  G4int    save_n;
  G4int    save_detID[save_nMax];
  G4int    save_particleID[save_nMax];
  G4double save_ke[save_nMax];
  G4double save_x[save_nMax];
  G4double save_y[save_nMax];
  G4double save_z[save_nMax];
  G4double save_px[save_nMax];
  G4double save_py[save_nMax];
  G4double save_pz[save_nMax];

  G4bool boolIsAnySpecialSaveVolumeDefined;

  std::map<std::string,int> SensDetectorMapping;
  std::map<std::string,int> ProcessIDMapping;
};

#endif
