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

#include "musrRootOutput.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"
#include "musrErrorMessage.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

musrRootOutput::musrRootOutput() {
  pointerToRoot=this;
  boolIsAnySpecialSaveVolumeDefined=false;
  nFieldNomVal=0;

  ProcessIDMapping["DecayWithSpin"]=1;
  ProcessIDMapping["eIoni"]=2;
  ProcessIDMapping["eBrem"]=3;
  ProcessIDMapping["annihil"]=4;
  ProcessIDMapping["LowEnCompton"]=5;
  ProcessIDMapping["LowEnConversion"]=6;
  ProcessIDMapping["LowEnBrem"]=7;
  ProcessIDMapping["LowEnergyIoni"]=8;
  ProcessIDMapping["LowEnPhotoElec"]=9;
  ProcessIDMapping["RadioactiveDecay"]=10;
  ProcessIDMapping["muIoni"]=11;
  ProcessIDMapping["MuFormation"]=12;
  ProcessIDMapping["Decay"]=13;
  ProcessIDMapping["initialParticle"]=100;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

musrRootOutput::~musrRootOutput() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

musrRootOutput* musrRootOutput::pointerToRoot=0;
musrRootOutput* musrRootOutput::GetRootInstance() {
  return pointerToRoot;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4bool musrRootOutput::store_runID = true;
G4bool musrRootOutput::store_eventID = true;
G4bool musrRootOutput::store_weight = true;
G4bool musrRootOutput::store_BFieldAtDecay = true;
G4bool musrRootOutput::store_muIniTime = true;
G4bool musrRootOutput::store_muIniPosX = true;
G4bool musrRootOutput::store_muIniPosY = true;
G4bool musrRootOutput::store_muIniPosZ = true;
G4bool musrRootOutput::store_muIniMomX = true;
G4bool musrRootOutput::store_muIniMomY = true;
G4bool musrRootOutput::store_muIniMomZ = true;
G4bool musrRootOutput::store_muIniPolX = true;
G4bool musrRootOutput::store_muIniPolY = true;
G4bool musrRootOutput::store_muIniPolZ = true;
G4bool musrRootOutput::store_muDecayDetID= true;
G4bool musrRootOutput::store_muDecayPosX = true;
G4bool musrRootOutput::store_muDecayPosY = true;
G4bool musrRootOutput::store_muDecayPosZ = true;
G4bool musrRootOutput::store_muDecayTime = true;
G4bool musrRootOutput::store_muDecayPolX = true;
G4bool musrRootOutput::store_muDecayPolY = true;
G4bool musrRootOutput::store_muDecayPolZ = true;
G4bool musrRootOutput::store_muTargetTime = false;
G4bool musrRootOutput::store_muTargetPolX = false;
G4bool musrRootOutput::store_muTargetPolY = false;
G4bool musrRootOutput::store_muTargetPolZ = false;
G4bool musrRootOutput::store_muM0Time = false;
G4bool musrRootOutput::store_muM0PolX = false;
G4bool musrRootOutput::store_muM0PolY = false;
G4bool musrRootOutput::store_muM0PolZ = false;
G4bool musrRootOutput::store_muM1Time = false;
G4bool musrRootOutput::store_muM1PolX = false;
G4bool musrRootOutput::store_muM1PolY = false;
G4bool musrRootOutput::store_muM1PolZ = false;
G4bool musrRootOutput::store_muM2Time = false;
G4bool musrRootOutput::store_muM2PolX = false;
G4bool musrRootOutput::store_muM2PolY = false;
G4bool musrRootOutput::store_muM2PolZ = false;
G4bool musrRootOutput::store_posIniMomX = true;
G4bool musrRootOutput::store_posIniMomY = true;
G4bool musrRootOutput::store_posIniMomZ = true;
G4bool musrRootOutput::store_det_ID = true;
G4bool musrRootOutput::store_det_edep = true;
G4bool musrRootOutput::store_det_edep_el = true;
G4bool musrRootOutput::store_det_edep_pos = true;
G4bool musrRootOutput::store_det_edep_gam = true;
G4bool musrRootOutput::store_det_edep_mup = true;
G4bool musrRootOutput::store_det_nsteps = true;
G4bool musrRootOutput::store_det_length = true;
G4bool musrRootOutput::store_det_start = true;
G4bool musrRootOutput::store_det_end = true;
G4bool musrRootOutput::store_det_x = true;
G4bool musrRootOutput::store_det_y = true;
G4bool musrRootOutput::store_det_z = true;
G4bool musrRootOutput::store_det_kine = true;
G4bool musrRootOutput::store_det_VrtxKine = true;
G4bool musrRootOutput::store_det_VrtxX = true;
G4bool musrRootOutput::store_det_VrtxY = true;
G4bool musrRootOutput::store_det_VrtxZ = true;
G4bool musrRootOutput::store_det_VrtxVolID = true;
G4bool musrRootOutput::store_det_VrtxProcID = true;
G4bool musrRootOutput::store_det_VrtxTrackID = true;
G4bool musrRootOutput::store_det_VrtxParticleID = true;
G4bool musrRootOutput::store_det_VvvKine = true;
G4bool musrRootOutput::store_det_VvvX = true;
G4bool musrRootOutput::store_det_VvvY = true;
G4bool musrRootOutput::store_det_VvvZ = true;
G4bool musrRootOutput::store_det_VvvVolID = true;
G4bool musrRootOutput::store_det_VvvProcID = true;
G4bool musrRootOutput::store_det_VvvTrackID = true;
G4bool musrRootOutput::store_det_VvvParticleID = true;
G4bool musrRootOutput::store_fieldNomVal = true;
G4bool musrRootOutput::store_fieldIntegralBx = false;
G4bool musrRootOutput::store_fieldIntegralBy = false;
G4bool musrRootOutput::store_fieldIntegralBz = false;
G4bool musrRootOutput::store_fieldIntegralBz1 = false;
G4bool musrRootOutput::store_fieldIntegralBz2 = false;
G4bool musrRootOutput::store_fieldIntegralBz3 = false;
G4bool musrRootOutput::store_fieldIntegralEx = false;
G4bool musrRootOutput::store_fieldIntegralEy = false;
G4bool musrRootOutput::store_fieldIntegralEz = false;


G4int musrRootOutput::oldEventNumberInG4EqEMFieldWithSpinFunction=-1;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void musrRootOutput::BeginOfRunAction() {
  G4cout << "musrRootOutput::BeginOfRunAction()  Defining the Root tree and branches:"<<G4endl;
  G4int tmpRunNr=(G4RunManager::GetRunManager())->GetCurrentRun()->GetRunID();
  char RootOutputFileName[200];
  sprintf(RootOutputFileName, "data/musr_%i.root", tmpRunNr);
  rootFile=new TFile(RootOutputFileName,"recreate");
  rootTree=new TTree("t1","a simple Tree with simple variables");
  if (store_runID)        {rootTree->Branch("runID",&runID_t,"runID/I");}
  if (store_eventID)      {rootTree->Branch("eventID",&eventID_t,"eventID/I");}
  if (store_weight)       {rootTree->Branch("weight",&weight_t,"weight/D");}
  if (store_BFieldAtDecay) {rootTree->Branch("BFieldAtDecay",&B_t,"Bx/D:By:Bz:B3:B4:B5");}
  if (store_muIniTime)    {rootTree->Branch("muIniTime",&muIniTime_t,"muIniTime/D");}
  if (store_muIniPosX)    {rootTree->Branch("muIniPosX",&muIniPosX_t,"muIniPosX/D");}
  if (store_muIniPosY)    {rootTree->Branch("muIniPosY",&muIniPosY_t,"muIniPosY/D");}
  if (store_muIniPosZ)    {rootTree->Branch("muIniPosZ",&muIniPosZ_t,"muIniPosZ/D");}
  if (store_muIniMomX)    {rootTree->Branch("muIniMomX",&muIniMomX_t,"muIniMomX/D");}
  if (store_muIniMomY)    {rootTree->Branch("muIniMomY",&muIniMomY_t,"muIniMomY/D");}
  if (store_muIniMomZ)    {rootTree->Branch("muIniMomZ",&muIniMomZ_t,"muIniMomZ/D");}
  if (store_muIniPolX)    {rootTree->Branch("muIniPolX",&muIniPolX_t,"muIniPolX/D");}
  if (store_muIniPolY)    {rootTree->Branch("muIniPolY",&muIniPolY_t,"muIniPolY/D");}
  if (store_muIniPolZ)    {rootTree->Branch("muIniPolZ",&muIniPolZ_t,"muIniPolZ/D");}
  if (store_muDecayDetID) {rootTree->Branch("muDecayDetID",&muDecayDetID_t,"muDecayDetID/I");}
  if (store_muDecayPosX)  {rootTree->Branch("muDecayPosX",&muDecayPosX_t,"muDecayPosX/D");}
  if (store_muDecayPosY)  {rootTree->Branch("muDecayPosY",&muDecayPosY_t,"muDecayPosY/D");}
  if (store_muDecayPosZ)  {rootTree->Branch("muDecayPosZ",&muDecayPosZ_t,"muDecayPosZ/D");}
  if (store_muDecayTime)  {rootTree->Branch("muDecayTime",&muDecayTime_t,"muDecayTime/D");}
  if (store_muDecayPolX)  {rootTree->Branch("muDecayPolX",&muDecayPolX_t,"muDecayPolX/D");}
  if (store_muDecayPolY)  {rootTree->Branch("muDecayPolY",&muDecayPolY_t,"muDecayPolY/D");}
  if (store_muDecayPolZ)  {rootTree->Branch("muDecayPolZ",&muDecayPolZ_t,"muDecayPolZ/D");}
  if (store_muTargetTime) {rootTree->Branch("muTargetTime",&muTargetTime_t,"muTargetTime/D");}
  if (store_muTargetPolX) {rootTree->Branch("muTargetPolX",&muTargetPolX_t,"muTargetPolX/D");}
  if (store_muTargetPolY) {rootTree->Branch("muTargetPolY",&muTargetPolY_t,"muTargetPolY/D");}
  if (store_muTargetPolZ) {rootTree->Branch("muTargetPolZ",&muTargetPolZ_t,"muTargetPolZ/D");}
  if (store_muM0Time)     {rootTree->Branch("muM0Time",&muM0Time_t,"muM0Time/D");}
  if (store_muM0PolX)     {rootTree->Branch("muM0PolX",&muM0PolX_t,"muM0PolX/D");}
  if (store_muM0PolY)     {rootTree->Branch("muM0PolY",&muM0PolY_t,"muM0PolY/D");}
  if (store_muM0PolZ)     {rootTree->Branch("muM0PolZ",&muM0PolZ_t,"muM0PolZ/D");}
  if (store_muM1Time)     {rootTree->Branch("muM1Time",&muM1Time_t,"muM1Time/D");}
  if (store_muM1PolX)     {rootTree->Branch("muM1PolX",&muM1PolX_t,"muM1PolX/D");}
  if (store_muM1PolY)     {rootTree->Branch("muM1PolY",&muM1PolY_t,"muM1PolY/D");}
  if (store_muM1PolZ)     {rootTree->Branch("muM1PolZ",&muM1PolZ_t,"muM1PolZ/D");}
  if (store_muM2Time)     {rootTree->Branch("muM2Time",&muM2Time_t,"muM2Time/D");}
  if (store_muM2PolX)     {rootTree->Branch("muM2PolX",&muM2PolX_t,"muM2PolX/D");}
  if (store_muM2PolY)     {rootTree->Branch("muM2PolY",&muM2PolY_t,"muM2PolY/D");}
  if (store_muM2PolZ)     {rootTree->Branch("muM2PolZ",&muM2PolZ_t,"muM2PolZ/D");}
  if (store_posIniMomX)   {rootTree->Branch("posIniMomX",&posIniMomx_t,"posIniMomX/D");}
  if (store_posIniMomY)   {rootTree->Branch("posIniMomY",&posIniMomy_t,"posIniMomY/D");}
  if (store_posIniMomZ)   {rootTree->Branch("posIniMomZ",&posIniMomz_t,"posIniMomZ/D");}
  //  if (store_globalTime)   {rootTree->Branch("globalTime",&globalTime_t,"globalTime/D");}
  //  if (store_fieldValue)   {rootTree->Branch("fieldValue",&fieldValue_t,"fieldValue/D");}
  if (store_fieldNomVal)  {
    rootTree->Branch("nFieldNomVal",&nFieldNomVal,"nFieldNomVal/I");
    rootTree->Branch("fieldNomVal",&fieldNomVal,"fieldNomVal[nFieldNomVal]/D");
  }
  if (store_fieldIntegralBx) {rootTree->Branch("BxIntegral",&BxIntegral_t,"BxIntegral/D");}
  if (store_fieldIntegralBy) {rootTree->Branch("ByIntegral",&ByIntegral_t,"ByIntegral/D");}
  if (store_fieldIntegralBz) {rootTree->Branch("BzIntegral",&BzIntegral_t,"BzIntegral/D");}
  if (store_fieldIntegralBz1) {rootTree->Branch("BzIntegral1",&BzIntegral1_t,"BzIntegral1/D");}
  if (store_fieldIntegralBz2) {rootTree->Branch("BzIntegral2",&BzIntegral2_t,"BzIntegral2/D");}
  if (store_fieldIntegralBz3) {rootTree->Branch("BzIntegral3",&BzIntegral3_t,"BzIntegral3/D");}
  if (store_fieldIntegralEx) {rootTree->Branch("ExIntegral",&ExIntegral_t,"ExIntegral/D");}
  if (store_fieldIntegralEy) {rootTree->Branch("EyIntegral",&EyIntegral_t,"EyIntegral/D");}
  if (store_fieldIntegralEz) {rootTree->Branch("EzIntegral",&EzIntegral_t,"EzIntegral/D");}

  rootTree->Branch("det_n",&det_n,"det_n/I");
  if (store_det_ID)       {rootTree->Branch("det_ID",&det_ID,"det_ID[det_n]/I");}
  if (store_det_edep)     {rootTree->Branch("det_edep",&det_edep,"det_edep[det_n]/D");}
  if (store_det_edep_el)  {rootTree->Branch("det_edep_el",&det_edep_el,"det_edep_el[det_n]/D");}
  if (store_det_edep_pos) {rootTree->Branch("det_edep_pos",&det_edep_pos,"det_edep_pos[det_n]/D");}
  if (store_det_edep_gam) {rootTree->Branch("det_edep_gam",&det_edep_gam,"det_edep_gam[det_n]/D");}
  if (store_det_edep_mup) {rootTree->Branch("det_edep_mup",&det_edep_mup,"det_edep_mup[det_n]/D");}
  if (store_det_nsteps)   {rootTree->Branch("det_nsteps",&det_nsteps,"det_nsteps[det_n]/I");}
  if (store_det_length)   {rootTree->Branch("det_length",&det_length,"det_length[det_n]/D");}
  if (store_det_start)    {rootTree->Branch("det_time_start",&det_time_start,"det_time_start[det_n]/D");}
  if (store_det_end)      {rootTree->Branch("det_time_end",&det_time_end,"det_time_end[det_n]/D");}
  if (store_det_x)        {rootTree->Branch("det_x",&det_x,"det_x[det_n]/D");}
  if (store_det_y)        {rootTree->Branch("det_y",&det_y,"det_y[det_n]/D");}
  if (store_det_z)        {rootTree->Branch("det_z",&det_z,"det_z[det_n]/D");}
  if (store_det_kine)     {rootTree->Branch("det_kine",&det_kine,"det_kine[det_n]/D");}
  if (store_det_VrtxKine) {rootTree->Branch("det_VrtxKine",&det_VrtxKine,"det_VrtxKine[det_n]/D");}
  if (store_det_VrtxX)    {rootTree->Branch("det_VrtxX",&det_VrtxX,"det_VrtxX[det_n]/D");}
  if (store_det_VrtxY)    {rootTree->Branch("det_VrtxY",&det_VrtxY,"det_VrtxY[det_n]/D");}
  if (store_det_VrtxZ)    {rootTree->Branch("det_VrtxZ",&det_VrtxZ,"det_VrtxZ[det_n]/D");}
  if (store_det_VrtxVolID){rootTree->Branch("det_VrtxVolID",&det_VrtxVolID,"det_VrtxVolID[det_n]/I");}
  if (store_det_VrtxProcID){rootTree->Branch("det_VrtxProcID",&det_VrtxProcID,"det_VrtxProcID[det_n]/I");}
  if (store_det_VrtxTrackID){rootTree->Branch("det_VrtxTrackID",&det_VrtxTrackID,"det_VrtxTrackID[det_n]/I");}
  if (store_det_VrtxParticleID){rootTree->Branch("det_VrtxParticleID",&det_VrtxParticleID,"det_VrtxParticleID[det_n]/I");}
  if (store_det_VvvKine) {rootTree->Branch("det_VvvKine",&det_VvvKine,"det_VvvKine[det_n]/D");}
  if (store_det_VvvX)    {rootTree->Branch("det_VvvX",&det_VvvX,"det_VvvX[det_n]/D");}
  if (store_det_VvvY)    {rootTree->Branch("det_VvvY",&det_VvvY,"det_VvvY[det_n]/D");}
  if (store_det_VvvZ)    {rootTree->Branch("det_VvvZ",&det_VvvZ,"det_VvvZ[det_n]/D");}
  if (store_det_VvvVolID){rootTree->Branch("det_VvvVolID",&det_VvvVolID,"det_VvvVolID[det_n]/I");}
  if (store_det_VvvProcID){rootTree->Branch("det_VvvProcID",&det_VvvProcID,"det_VvvProcID[det_n]/I");}
  if (store_det_VvvTrackID){rootTree->Branch("det_VvvTrackID",&det_VvvTrackID,"det_VvvTrackID[det_n]/I");}
  if (store_det_VvvParticleID){rootTree->Branch("det_VvvParticleID",&det_VvvParticleID,"det_VvvParticleID[det_n]/I");}

  if (boolIsAnySpecialSaveVolumeDefined) {
    rootTree->Branch("save_n",&save_n,"save_n/I");
    rootTree->Branch("save_detID",&save_detID,"save_detID[save_n]/I");
    rootTree->Branch("save_particleID",&save_particleID,"save_particleID[save_n]/I");
    rootTree->Branch("save_ke",&save_ke,"save_ke[save_n]/D");
    rootTree->Branch("save_x",&save_x,"save_x[save_n]/D");
    rootTree->Branch("save_y",&save_y,"save_y[save_n]/D");
    rootTree->Branch("save_z",&save_z,"save_z[save_n]/D");
    rootTree->Branch("save_px",&save_px,"save_px[save_n]/D");
    rootTree->Branch("save_py",&save_py,"save_py[save_n]/D");
    rootTree->Branch("save_pz",&save_pz,"save_pz[save_n]/D");
  }

  //  htest1 = new TH1F("htest1","The debugging histogram 1",50,-4.,4.);
  //  htest2 = new TH1F("htest2","The debugging histogram 2",50,0.,3.142);
  htest1 = new TH2F("htest1","x, y",50,-200.,200.,50,-200.,200.);
  htest2 = new TH2F("htest2","R, z",50,0.,250.,50,-150.,150.);
  htest3 = new TH1F("htest3","Energy in MeV",55,0.,55.);
  htest4 = new TH1F("htest4","Radioactive electron kinetic energy",250,0.,2.5);
  htest5 = new TH1F("htest5","The debugging histogram 5",50,-4.,4.);
  htest6 = new TH1F("htest6","The debugging histogram 6",50,0.,3.142);
  htest7 = new TH1F("htest7","The debugging histogram 7",50,-4.,4.);
  htest8 = new TH1F("htest8","The debugging histogram 8",50,0.,3.142);

  G4cout << "musrRootOutput::BeginOfRunAction()  The Root tree and branches were defined."<<G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void musrRootOutput::EndOfRunAction() {
  G4cout << "musrRootOutput::EndOfRunAction() - Writing out the Root tree:"<<G4endl;
  rootTree->Write();
  htest1->Write();
  htest2->Write();
  htest3->Write();
  htest4->Write();
  htest5->Write();
  htest6->Write();
  htest7->Write();
  htest8->Write();
  // Variables exported from Geant simulation to the Root output
  //  static const Int_t nGeantParamD=10;
  TVectorD TVector_GeantParametersD(maxNGeantParameters);
  for (Int_t i=0; i<maxNGeantParameters; i++) {
    TVector_GeantParametersD[i]=GeantParametersD[i];
  }
  TVector_GeantParametersD.Write("geantParametersD");
  rootFile->Close();
  G4cout<<"musrRootOutput::EndOfRunAction() - Root tree written out."<<G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void musrRootOutput::FillEvent() {
  htest5->Fill(atan2(posIniMomy_t,posIniMomx_t));
  htest6->Fill(atan2(sqrt(posIniMomx_t*posIniMomx_t+posIniMomy_t*posIniMomy_t),posIniMomz_t));
  if (weight_t>0.) {
    rootTree->Fill();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void musrRootOutput::ClearAllRootVariables() {
  runID_t=-1000;
  eventID_t=-1000;
  weight_t=1.;
  B_t[0]=-1000.;B_t[1]=-1000.;B_t[2]=-1000.;B_t[3]=-1000.;B_t[4]=-1000.;B_t[5]=-1000.;
  muIniTime_t=-1000;
  muIniPosX_t=-1000; muIniPosY_t=-1000; muIniPosZ_t=-1000;
  muIniMomX_t=-1000; muIniMomY_t=-1000; muIniMomZ_t=-1000;
  muIniPolX_t=-1000; muIniPolY_t=-1000; muIniPolZ_t=-1000;
  muDecayDetID_t=-1000;
  muDecayPolX_t=-1000; muDecayPolY_t=-1000; muDecayPolZ_t=-1000;
  muTargetTime_t=-1000; muTargetPolX_t=-1000; muTargetPolY_t=-1000; muTargetPolZ_t=-1000;
  muM0Time_t=-1000; muM0PolX_t=-1000; muM0PolY_t=-1000; muM0PolZ_t=-1000;
  muM1Time_t=-1000; muM1PolX_t=-1000; muM1PolY_t=-1000; muM1PolZ_t=-1000;
  muM2Time_t=-1000; muM2PolX_t=-1000; muM2PolY_t=-1000; muM2PolZ_t=-1000;
  muDecayPosX_t=-1000;muDecayPosY_t=-1000;muDecayPosZ_t=-1000;
  muDecayTime_t=-1000;
  posIniMomx_t=-1000;posIniMomy_t=-1000;posIniMomz_t=-1000;
  BxIntegral_t = -1000; ByIntegral_t = -1000; BzIntegral_t = -1000; 
  BzIntegral1_t = -1000; BzIntegral2_t = -1000; BzIntegral3_t = -1000; 
  ExIntegral_t = -1000; EyIntegral_t = -1000; EzIntegral_t = -1000; 
  det_n=0;
  save_n=0;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void musrRootOutput::SetVolumeIDMapping(std::string logivol, int volumeID) {
  //  This function assigns a unique number to each sensitive detector name.
  //  The numbers are used in the root tree, as it is easier to work with numbers
  //  rather than with strings.
  if (SensDetectorMapping[logivol]) {
    char message[200];  
    sprintf(message,"musrRootOutput::SetVolumeIDMapping: Sensitive volume %s already assigned",logivol.c_str());
    musrErrorMessage::GetInstance()->musrError(FATAL,message,false);
  }
  else{
    SensDetectorMapping[logivol]=volumeID;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int musrRootOutput::ConvertVolumeToID(std::string logivol) {
  G4int volumeID = SensDetectorMapping[logivol];
  if (volumeID==0) {
    char message[200];  
    sprintf(message,"musrRootOutput::ConvertVolumeToID: No ID number assigned to sensitive volume %s .",logivol.c_str());
    musrErrorMessage::GetInstance()->musrError(SERIOUS,message,true);
  }
  return volumeID;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int musrRootOutput::ConvertProcessToID(std::string processName) {
  G4int processID = ProcessIDMapping[processName];
  if (processID==0) {
    char message[200];  
    sprintf(message,"musrRootOutput::ConvertProcessToID: No ID number assigned to the process \"%s\" .",processName.c_str());
    musrErrorMessage::GetInstance()->musrError(WARNING,message,true);
  }
  return processID;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void musrRootOutput::SetSaveDetectorInfo (G4int ID, G4int particleID, G4double ke,
                          G4double x, G4double y, G4double z, G4double px, G4double py, G4double pz) {
  if (save_n>=save_nMax) {
    char message[200];
    sprintf(message,"musrRootOutput.cc::SetSaveDetectorInfo(): more \"save\" hits then allowed: save_nMax=%i",save_nMax);
    musrErrorMessage::GetInstance()->musrError(SERIOUS,message,true);
  }
  else {
    save_detID[save_n]=ID;
    save_particleID[save_n]=particleID;
    save_ke[save_n]=ke/MeV;
    save_x[save_n]=x/mm;
    save_y[save_n]=y/mm;
    save_z[save_n]=z/mm;
    save_px[save_n]=px/MeV;
    save_py[save_n]=py/MeV;
    save_pz[save_n]=pz/MeV;

    save_n++;
  }
}

void musrRootOutput::SetFieldNomVal(G4int i, G4double value) {
  if (i<maxNFieldnNominalValues) {
    // cks the following will probably not be correct for electric field,
    //     because the units are tesla.  Should be modified.
    fieldNomVal[i]=value/tesla;
  } 
  else {
    char message[200];
    sprintf(message,
	    "musrRootOutput.cc::SetFieldNomVal(): more electromagnetic fields then allowed: maxNFieldnNominalValues=%i",
	    maxNFieldnNominalValues);
    musrErrorMessage::GetInstance()->musrError(SERIOUS,message,true);
  }
}


void musrRootOutput::SetDetectorInfo (G4int nDetectors, G4int ID, G4int particleID, G4double edep, 
			  G4double edep_el, G4double edep_pos, 
			  G4double edep_gam, G4double edep_mup,G4int nsteps, G4double length, G4double t1, 
			  G4double t2, G4double x, G4double y, G4double z,
			  G4double ek, G4double ekVertex, G4double xVertex, G4double yVertex, G4double zVertex, 
			  G4int idVolVertex, G4int idProcVertex, G4int idTrackVertex)
{
  if ((nDetectors<0)||(nDetectors>=(det_nMax-1))) {
    char message[200];  
    sprintf(message,"musrRootOutput.cc::SetDetectorInfo: nDetectors %i is larger than det_nMax = %i",nDetectors,det_nMax);
    musrErrorMessage::GetInstance()->musrError(SERIOUS,message,false);
    return;
  }
  else {
    det_n=nDetectors+1; 
    det_ID[nDetectors]=ID; 
    det_edep[nDetectors]=edep/MeV;
    det_edep_el[nDetectors]=edep_el/MeV; 
    det_edep_pos[nDetectors]=edep_pos/MeV;
    det_edep_gam[nDetectors]=edep_gam/MeV; 
    det_edep_mup[nDetectors]=edep_mup/MeV;
    det_nsteps[nDetectors]=nsteps; 
    det_length[nDetectors]=length/mm;
    det_time_start[nDetectors]=t1/microsecond;  
    det_time_end[nDetectors]=t2/microsecond;
    det_x[nDetectors]=x/mm;
    det_y[nDetectors]=y/mm;
    det_z[nDetectors]=z/mm;
    det_kine[nDetectors]=ek/MeV;
    det_VrtxKine[nDetectors]=ekVertex/MeV;
    det_VrtxX[nDetectors]=xVertex/mm;
    det_VrtxY[nDetectors]=yVertex/mm;
    det_VrtxZ[nDetectors]=zVertex/mm;
    det_VrtxVolID[nDetectors]=idVolVertex;
    det_VrtxProcID[nDetectors]=idProcVertex;
    det_VrtxTrackID[nDetectors]=idTrackVertex;
    det_VrtxParticleID[nDetectors]=particleID;
  }
}

void musrRootOutput::SetDetectorInfoVvv (G4int nDetectors,
			  G4double ekVertex, G4double xVertex, G4double yVertex, G4double zVertex, 
			  G4int idVolVertex, G4int idProcVertex, G4int idTrackVertex, G4int particleID)   {
  if ((nDetectors<0)||(nDetectors>=(det_nMax-1))) {
    char message[200];  
    sprintf(message,"musrRootOutput.cc::SetDetectorInfoVvv: nDetectors %i is larger than det_nMax = %i",nDetectors,det_nMax);
    musrErrorMessage::GetInstance()->musrError(SERIOUS,message,false);
    return;
  }
  else {
    det_VvvKine[nDetectors]=ekVertex/MeV;
    det_VvvX[nDetectors]=xVertex/mm;
    det_VvvY[nDetectors]=yVertex/mm;
    det_VvvZ[nDetectors]=zVertex/mm;
    det_VvvVolID[nDetectors]=idVolVertex;
    det_VvvProcID[nDetectors]=idProcVertex;
    det_VvvTrackID[nDetectors]=idTrackVertex;
    det_VvvParticleID[nDetectors]=particleID;
  }
}
