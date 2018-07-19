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

#include "musrScintSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include <algorithm>   // needed for the sort() function
#include "G4VProcess.hh"  // needed for the degugging message of the process name
#include "G4RunManager.hh"
#include "G4Run.hh"
#include "musrParameters.hh"
#include "musrErrorMessage.hh"
#include "musrSteppingAction.hh"
#include <vector>

//bool myREMOVEfunction (int i,int j) { return (i<j); }
//bool timeOrdering (musrScintHit hit1, musrScintHit hit2) { 
//  return (hit1.GetGlobalTime()<hit2.GetGlobalTime());
//}
//
//bool timeOrdering2 (std::map<int,double>::iterator i1, std::map<int,double>::iterator m2) {
//  return ( (*i1).first()<(*i2).second() );
//}
//
//bool timeOrdering2 (std::pair<int,double> p1, std::pair<int,double> p2) {
//  return ( p1.first()<p2.second() );
//}
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

musrScintSD::musrScintSD(G4String name)
:G4VSensitiveDetector(name)
{
  G4String HCname;
  collectionName.insert(HCname="scintCollection");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

musrScintSD::~musrScintSD(){ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void musrScintSD::Initialize(G4HCofThisEvent* HCE) {
  if (verboseLevel>1) G4cout<<"VERBOSE 2:  musrScintSD::Initialize\n";
  scintCollection = new musrScintHitsCollection
                          (SensitiveDetectorName,collectionName[0]); 
  static G4int HCID = -1;
  if(HCID<0) { 
    HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
    if (verboseLevel>1) G4cout<<"VERBOSE 2:  musrScintSD::HCID was <0\n, now HCID="<<HCID<<"\n";
  }
  HCE->AddHitsCollection( HCID, scintCollection ); 
  myStoreOnlyEventsWithHits = musrParameters::storeOnlyEventsWithHits;
  mySignalSeparationTime    = musrParameters::signalSeparationTime;
  myStoreOnlyTheFirstTimeHit= musrParameters::storeOnlyTheFirstTimeHit;
  myStoreOnlyEventsWithHitInDetID = musrParameters::storeOnlyEventsWithHitInDetID;
  musrSteppingAction* myMusrSteppingAction = musrSteppingAction::GetInstance();
  boolIsVvvInfoRequested = myMusrSteppingAction->IsVvvInfoRequested();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool musrScintSD::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{
  if (verboseLevel>1) G4cout<<"VERBOSE 2:  musrScintSD::ProcessHits\n";
  G4double edep = aStep->GetTotalEnergyDeposit();
  if(edep==0.) {
    return false;
  }

  G4Track* aTrack = aStep->GetTrack();
  G4String actualVolume=aTrack->GetVolume()->GetLogicalVolume()->GetName();

  // If requested, store only the hit that happened first (usefull for some special studies, not for a serious simulation)
  if (myStoreOnlyTheFirstTimeHit) {
    G4int NbHits = scintCollection->entries(); 
    if (NbHits>0) {
      aTrack->SetTrackStatus(fStopAndKill);
      return false;
    }
  }

  musrScintHit* newHit = new musrScintHit();
  newHit->SetParticleName (aTrack->GetDefinition()->GetParticleName());
  G4int particleID = aTrack->GetDefinition()->GetPDGEncoding();
  newHit->SetParticleID (particleID);
  newHit->SetEdep     (edep);
  newHit->SetPrePos   (aStep->GetPreStepPoint()->GetPosition());
  newHit->SetPostPos  (aStep->GetPostStepPoint()->GetPosition());
  newHit->SetPol      (aTrack->GetPolarization());
  G4LogicalVolume* hitLogicalVolume = aTrack->GetVolume()->GetLogicalVolume();
  newHit->SetLogVolName(hitLogicalVolume->GetName());
  newHit->SetGlobTime(aTrack->GetGlobalTime());
  //  Warning - aStep->IsFirstStepInVolume() only available in Geant version >= 4.8.2 !
  //  newHit->SetFirstStepInVolumeFlag(aStep->IsFirstStepInVolume());
  //  newHit->SetLastStepInVolumeFlag(aStep->IsLastStepInVolume());
  newHit->SetKineticEnergy(aTrack->GetKineticEnergy());
  // newHit->SetKineticEnergy(aTrack->GetKineticEnergy()+edep);
  G4double vertexKineticEnergy = aTrack->GetVertexKineticEnergy();
  newHit->SetVertexKineticEnergy(vertexKineticEnergy);
  G4ThreeVector vertexPosition = aTrack->GetVertexPosition();
  newHit->SetVertexPosition(vertexPosition);
  const G4LogicalVolume* vertexLogicalVolume = aTrack->GetLogicalVolumeAtVertex();
  G4String vertexLogicalVolumeName = vertexLogicalVolume->GetName();
  newHit->SetLogicalVolumeAtVertex(vertexLogicalVolumeName);
  G4String processName;
  if ((aTrack->GetCreatorProcess())!=0) { processName=aTrack->GetCreatorProcess()->GetProcessName(); }
  else {processName="initialParticle";}   //if no process found, the track comes from the generated particle
  newHit->SetCreatorProcessName(processName);
  G4int trackID = aTrack->GetTrackID();
  newHit->SetTrackID  (trackID);
  newHit->SetStepLength   (aStep->GetStepLength());

  scintCollection->insert( newHit );
  //  newHit->Print();
  newHit->Draw();
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void musrScintSD::EndOfEvent(G4HCofThisEvent*) {
  if (verboseLevel>1) { 
    G4cout<<"VERBOSE 2:  musrScintSD::EndOfEvent"<<G4endl;
    G4int NbHits = scintCollection->entries();
    G4cout << "\n-------->Hits Collection: in this event they are " << NbHits 
	   << " hits in the scint chambers: " << G4endl;
  } 
  
  //  Positron_momentum_already_stored=0;
  musrRootOutput* myRootOutput = musrRootOutput::GetRootInstance();

  G4RunManager* fRunManager = G4RunManager::GetRunManager();
  myRootOutput->SetRunID(fRunManager->GetCurrentRun()->GetRunID());
  myRootOutput->SetEventID(fRunManager->GetCurrentEvent()->GetEventID());

  G4int NbHits = scintCollection->entries(); 
  
  if (myStoreOnlyEventsWithHits) {
    if (NbHits<=0) {
      return;
    }
    else if (myStoreOnlyEventsWithHitInDetID!=0) {
      for (G4int i=0; i<NbHits; i++) {
	musrScintHit* aHit = (*scintCollection)[i];
	G4String aHitVolumeName = aHit->GetLogVolName();
	G4int    aHitVolumeID   = myRootOutput->ConvertVolumeToID(aHitVolumeName);
	if (aHitVolumeID==myStoreOnlyEventsWithHitInDetID) break;  // hit in the requested detector was identified
	if (i==(NbHits-1)) return;                              // no hit identified in the requested detector
      }
    }
  }

  //  Sort out hits and fill them into root
  if (NbHits>0) {
    const G4int det_IDmax = musrRootOutput::det_nMax;
    G4double det_edep[det_IDmax];
    G4int    det_nsteps[det_IDmax];
    G4double det_length[det_IDmax];
    G4int    det_ID[det_IDmax]; 
    G4double det_edep_el[det_IDmax];
    G4double det_edep_pos[det_IDmax];
    G4double det_edep_gam[det_IDmax];
    G4double det_edep_mup[det_IDmax];
    G4double det_time_start[det_IDmax];
    G4double det_time_end[det_IDmax];
    G4double det_x[det_IDmax];
    G4double det_y[det_IDmax];
    G4double det_z[det_IDmax];
    G4double det_kine[det_IDmax];
    G4double det_VrtxKine[det_IDmax];
    G4double det_VrtxX[det_IDmax];
    G4double det_VrtxY[det_IDmax];
    G4double det_VrtxZ[det_IDmax];
    G4int    det_VrtxVolID[det_IDmax];
    G4int    det_VrtxProcID[det_IDmax];
    G4int    det_VrtxTrackID[det_IDmax];
    G4int    det_VrtxParticleID[det_IDmax];
    G4int det_VvvTrackSign[det_IDmax];

    //  Sort hits according to the time.  Using std::map is convenient, because it sorts
    //  its entries according to the key (the first variable of the pair).
    std::multimap<G4double,G4int> myHitTimeMapping;    // "map" replaced by "multimap" (needed for radioactive decay,
                                                       // in which case times are huge and due to the limited rounding 
                                                       // precision become equal  --> map ignores the same "keys",
                                                       // multimap does not.
    std::map<G4double,G4int>::iterator it;
    for (G4int i=0; i<NbHits; i++) {
      musrScintHit* aHit = (*scintCollection)[i];
      G4double tmptime=aHit->GetGlobalTime();
      //      G4cout<<"Hit nr "<<i<<"  at time="<<tmptime<<"  with edep="<<aHit->GetEdep()/MeV
      //	    <<"   detID="<<myRootOutput->ConvertVolumeToID(aHit->GetLogVolName())<< G4endl;
      myHitTimeMapping.insert ( std::pair<G4double,G4int>(tmptime,i) );
    }

    //  Loop over all hits (which are sorted according to their time):
    G4int nSignals=0;
    for (it=myHitTimeMapping.begin(); it!=myHitTimeMapping.end(); it++) {
      //      G4cout << "Key:" << it->first;
      //      G4cout << "  Value:" << it->second << "\n";
      G4int ii = it->second;      // ii  is the index of the hits, which is sorted according to time
      musrScintHit* aHit = (*scintCollection)[ii];
      G4String aHitVolumeName = aHit->GetLogVolName();
      G4int    aHitVolumeID   = myRootOutput->ConvertVolumeToID(aHitVolumeName);
      G4double aHitTime       = aHit->GetGlobalTime();
      G4int    aHitTrackID    = aHit->GetTrackID();

      // Loop over all already defined signals and check whether the hit falls into any of them
      G4bool signalAssigned=false;
      for (G4int j=0; j<nSignals; j++) {
	if ( (aHitVolumeID==det_ID[j]) && ((aHitTime-det_time_end[j])<mySignalSeparationTime) ) {
	  signalAssigned=true;
	  det_edep[j]                 += aHit->GetEdep();
	  det_nsteps[j]++;
	  det_length[j]               += aHit->GetStepLength();
	  det_time_end[j]              = aHitTime;
	  G4String aParticleName       = aHit->GetParticleName();
	  if (aParticleName=="e-") {
	    det_edep_el[j]            += aHit->GetEdep();
	  }  else if (aParticleName=="e+") {
	    det_edep_pos[j]           += aHit->GetEdep();
	  }  else if (aParticleName=="gamma") {
	    det_edep_gam[j]           += aHit->GetEdep();
	  }  else if ((aParticleName=="mu+")||(aParticleName=="mu-")) {
	    det_edep_mup[j]           += aHit->GetEdep();
	  }  else {
	    char message[200];
	    sprintf(message,"musrScintSD.cc::EndOfEvent(): untreated particle \"%s\" deposited energy.",aParticleName.c_str());
	    musrErrorMessage::GetInstance()->musrError(WARNING,message,true);
	  }
	  // Check whether the signals consits of more then just one hit,  in which case make the track ID negative:
	  if (abs(det_VrtxTrackID[j])!=aHitTrackID) {
	    det_VrtxTrackID[j]=-1*abs(det_VrtxTrackID[j]);
	    if (boolIsVvvInfoRequested) {
	      if (det_VvvTrackSign[j]==1) {
		musrSteppingAction* myMusrSteppingAction = musrSteppingAction::GetInstance();
		if (!(myMusrSteppingAction->AreTracksCommingFromSameParent(aHitTrackID,abs(det_VrtxTrackID[j]),aHitVolumeName))) {det_VvvTrackSign[j]=-1;}
	      }
	    }
	  }
	  break;
	}
      }
      if (!signalAssigned) {    // The hit does not belong to any existing signal --> create a new signal.
	// Check, whether the maximum number of signals was not exceeded:
	if ( nSignals >= (det_IDmax-1) ) {
	  char message[200];
	  sprintf(message,"musrScintSD.cc::EndOfEvent(): number of signals exceeds maximal allowed value.");
	  musrErrorMessage::GetInstance()->musrError(WARNING,message,true);
	}
	else {
	  det_edep[nSignals]                  = aHit->GetEdep();
	  det_nsteps[nSignals]                = 1;
	  det_length[nSignals]                = aHit->GetStepLength();
	  det_ID[nSignals]                    = aHitVolumeID;
	  det_time_start[nSignals]            = aHitTime;
	  det_time_end[nSignals]              = aHitTime;
	  det_edep_el[nSignals]               = 0;
	  det_edep_pos[nSignals]              = 0;
	  det_edep_gam[nSignals]              = 0;
	  det_edep_mup[nSignals]              = 0;
	  G4String aParticleName              = aHit->GetParticleName();
	  if (aParticleName=="e-") {
	    det_edep_el[nSignals]            += aHit->GetEdep();
	  }  else if (aParticleName=="e+") {
	    det_edep_pos[nSignals]           += aHit->GetEdep();
	  }  else if (aParticleName=="gamma") {
	    det_edep_gam[nSignals]           += aHit->GetEdep();
	  }  else if ((aParticleName=="mu+")||(aParticleName=="mu-")) {
	    det_edep_mup[nSignals]           += aHit->GetEdep();
	  }  else {
	    char message[200];
	    sprintf(message,"musrScintSD.cc::EndOfEvent(): UNTREATED PARTICLE \"%s\" deposited energy.",aParticleName.c_str());
	    musrErrorMessage::GetInstance()->musrError(WARNING,message,true);
	  }
	  G4ThreeVector prePos = aHit->GetPrePos();
	  det_x[nSignals]=prePos.x();
	  det_y[nSignals]=prePos.y();
	  det_z[nSignals]=prePos.z();
	  det_kine[nSignals]             = aHit->GetKineticEnergy();
	  det_VrtxKine[nSignals]         = aHit->GetVertexKineticEnergy();
	  G4ThreeVector VrtxPos          = aHit->GetVertexPosition();
	  det_VrtxX[nSignals]            = VrtxPos.x();
	  det_VrtxY[nSignals]            = VrtxPos.y();
	  det_VrtxZ[nSignals]            = VrtxPos.z();
	  G4String logicalVolumeAtVertex = aHit->GetLogicalVolumeAtVertex();
	  det_VrtxVolID[nSignals]        = myRootOutput->ConvertVolumeToID(logicalVolumeAtVertex);
	  G4String creatorProcessName    = aHit->GetCreatorProcessName();
	  det_VrtxProcID[nSignals]       = myRootOutput->ConvertProcessToID(creatorProcessName);
	  det_VrtxTrackID[nSignals]      = aHit->GetTrackID();
	  det_VrtxParticleID[nSignals]   = aHit->GetParticleID();
	  det_VvvTrackSign[nSignals]     = 1;
	  nSignals++;
	}
      }   // end of "if (!signalAssigned)"
    }  // end of the for loop over the hits

    // Sort the signals according to the energy (in decreasing order)
    std::map<G4double,G4int> mySignalMapping;
    std::map<G4double,G4int>::iterator itt;
    for (G4int i=0; i<nSignals; i++) {
      mySignalMapping.insert ( std::pair<G4double,G4int>(-det_edep[i],i) );
    }

    // Write out the signals (sorted according to energy) to the musrRootOutput class:
    G4int jj=-1;
    for (itt=mySignalMapping.begin(); itt!=mySignalMapping.end(); itt++) {
      jj++;
      G4int ii = itt->second;
      myRootOutput->SetDetectorInfo(jj,det_ID[ii],det_VrtxParticleID[ii],det_edep[ii],
                                    det_edep_el[ii],det_edep_pos[ii],
				    det_edep_gam[ii],det_edep_mup[ii],det_nsteps[ii],det_length[ii],
				    det_time_start[ii],det_time_end[ii],det_x[ii],det_y[ii],det_z[ii],
				    det_kine[ii],det_VrtxKine[ii],det_VrtxX[ii],det_VrtxY[ii],det_VrtxZ[ii],
				    det_VrtxVolID[ii],det_VrtxProcID[ii],det_VrtxTrackID[ii] );

      if (boolIsVvvInfoRequested) {
	G4int oldTrackID = abs(det_VrtxTrackID[ii]);
	musrSteppingAction* myMusrSteppingAction = musrSteppingAction::GetInstance();
	G4int vvvParentTrackID= -1; G4int vvvPparticleID; G4double vvvKine; G4ThreeVector vvvPosition; G4String vvvLogVol; G4String vvvProcess;
	G4int vvvLogVolID=-999;
	G4bool oldTrackRetrievedOK;
	do {
	  if (vvvParentTrackID>-1) {oldTrackID=vvvParentTrackID;}
	  oldTrackRetrievedOK = myMusrSteppingAction->GetInfoAboutOldTrack(oldTrackID, 
				     vvvParentTrackID, vvvPparticleID, vvvKine, vvvPosition, vvvLogVol, vvvProcess);
	  if (oldTrackRetrievedOK) {
	    //	  G4cout<<"musrScintSD: Old Track seems to be achieved fine -> Lets save it to Root tree"<<G4endl;
	    vvvLogVolID=myRootOutput->ConvertVolumeToID(vvvLogVol);
	  }
	  else { 
	    G4cout<<" oldTrackRetrievedOK is false"<<G4endl; 
	    oldTrackID       = -999; 
	    vvvParentTrackID = -999;
	    vvvPparticleID   = -999;
	    vvvKine          = -999;
	    vvvPosition      = G4ThreeVector(-999,-999,-999);
	    vvvLogVol        = "undefined";
	    vvvProcess       = "undefined";
	  }  
	} while (oldTrackRetrievedOK && (vvvLogVolID==det_ID[ii]));

	if (oldTrackRetrievedOK) {
	  G4int vvvProcessID=myRootOutput->ConvertProcessToID(vvvProcess);
	  myRootOutput->SetDetectorInfoVvv(jj,vvvKine,vvvPosition.x(),vvvPosition.y(),vvvPosition.z(),
					 vvvLogVolID,vvvProcessID,oldTrackID*det_VvvTrackSign[ii],vvvPparticleID);
	}
      }   //end "boolIsVvvInfoRequested"

    }
  }   //end "if (NbHits>0)"

  myRootOutput->FillEvent();
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
