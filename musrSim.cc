#include "musrDetectorConstruction.hh"
#include "musrPhysicsList.hh"
#include "musrPrimaryGeneratorAction.hh"
#include "musrRunAction.hh"
#include "musrEventAction.hh"
#include "musrSteppingAction.hh"
#include "musrSteppingVerbose.hh"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"

// The following two lines are needed to cope with the problem of 
// "Error in <TPluginManager::FindHandler>: Cannot find plugin handler for TVirtualStreamerInfo! 
//               Does $ROOTSYS/etc/plugins/TVirtualStreamerInfo exist?"
#include "TROOT.h"
#include "TPluginManager.h"

#include "Randomize.hh"

#include <X11/Xlib.h>

#ifdef G4VIS_USE
  // #include "musrVisManager.hh"
  #include "G4VisExecutive.hh"
  #include "G4TrajectoryDrawByCharge.hh"    // TS Trajectory drawing by ID or charge
#endif

#include "musrRootOutput.hh"
#include "musrParameters.hh"
#include "musrErrorMessage.hh"
//#include "F04GlobalField.hh"

int main(int argc,char** argv) {

  XInitThreads();

  // choose the Random engine
  //  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);   // the /musr/run/randomOption 2 does not work with RanecuEngine
  CLHEP::HepRandom::setTheEngine(new CLHEP::HepJamesRandom);

  //my Verbose output class
  G4VSteppingVerbose::SetInstance(new musrSteppingVerbose);
  
  // Run manager
  G4RunManager * runManager = new G4RunManager;

  // Create class "myParameters", which is a collection of many different parameters
  G4String steeringFileName=argv[1];
  musrParameters* myParameters = new musrParameters(steeringFileName);

  // Create class "musrErrorMessage"
  musrErrorMessage* myErrorMessage = new musrErrorMessage();

  // Create Root class for storing the output of the Geant simulation
  musrRootOutput* myRootOutput = new musrRootOutput();

// The following command is needed to cope with the problem of 
// "Error in <TPluginManager::FindHandler>: Cannot find plugin handler for TVirtualStreamerInfo! 
//               Does $ROOTSYS/etc/plugins/TVirtualStreamerInfo exist?"
//  /* magic line from Rene - for future reference! */
          gROOT->GetPluginManager()->AddHandler("TVirtualStreamerInfo",
                                                "*",
                                                "TStreamerInfo",
                                                "RIO",
                                                "TStreamerInfo()");


  // UserInitialization classes (mandatory)
  musrDetectorConstruction* musrdetector = new musrDetectorConstruction;
  if (argc>1) {
    G4int myRunNr=atoi(argv[1]);       // Get the run number from the name of the 
                                       // parameter file, if it starts with a number.
    if (myRunNr>0)  {runManager->SetRunIDCounter(myRunNr);}
    musrdetector->SetInputParameterFileName(argv[1]);
  }
  runManager->SetUserInitialization(musrdetector);
  runManager->SetUserInitialization(new musrPhysicsList);
  
#ifdef G4VIS_USE
  // Visualization, if you choose to have it!
  //  G4VisManager* visManager = new musrVisManager;
  G4VisManager* visManager = new G4VisExecutive;    // TS Trajectory drawing by ID or charge
  visManager->Initialize();
#endif
   
  // UserAction classes
  runManager->SetUserAction(new musrPrimaryGeneratorAction(musrdetector));
  runManager->SetUserAction(new musrRunAction);  
  runManager->SetUserAction(new musrEventAction);
  runManager->SetUserAction(new musrSteppingAction);

  //Initialize G4 kernel
  runManager->Initialize();
      
  //get the pointer to the User Interface manager 
  G4UImanager * UI = G4UImanager::GetUIpointer();  

  if(argc==1)
  // Define (G)UI terminal for interactive mode  
  { 
    // G4UIterminal is a (dumb) terminal.
    G4UIsession * session = 0;
#ifdef G4UI_USE_TCSH
      session = new G4UIterminal(new G4UItcsh);      
#else
      session = new G4UIterminal();
#endif    

    UI->ApplyCommand("/control/execute vis.mac");    
    session->SessionStart();
    delete session;
  }
  else
  // Batch mode
  { 
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UI->ApplyCommand(command+fileName);
    if (argc>2) {
      G4String SecondArgument = argv[2];
      if (SecondArgument=="idle") {
	G4UIsession * session = 0;
#ifdef G4UI_USE_TCSH
	session = new G4UIterminal(new G4UItcsh);      
#else
	session = new G4UIterminal();
#endif    
	session->SessionStart();
	delete session;
      }
    }
  }

#ifdef G4VIS_USE
  delete visManager;
#endif
  delete myRootOutput;
  delete myErrorMessage;
  delete myParameters;
  // cks  runManager->SetVerboseLevel(2);   // This line can help debug crashes during the runManager delete
  delete runManager;
  //  F04GlobalField* myGlobalField = F04GlobalField::getObject();
  //  if (myGlobalField!=NULL)  {delete myGlobalField;}

  return 0;
}



