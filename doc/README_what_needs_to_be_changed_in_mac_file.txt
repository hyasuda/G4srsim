To migrate the steering (*.mac) file of Toni Shiroka to a stearing file that
runs with the new migrated version of "musrSim", do the following changes:

1) replace all "lem4" by "musr", namely:
   /lem4/command  -->  /musr/command
    lem4/ScintSD  -->  musr/ScintSD
  but do not change "lem4" in the name of the field map file, if such file exists (?)

2) replace all "cones" by "cons" (This respects the naming convention of Geant4 solids "G4cons"):
   /musr/command construct cones   --->  /musr/command construct cons

3) modify the lines where uniform magnetic field is defined:
   old:  /musr/command globalfield Trigg0_field 0. 0. -1149.15 uniform log_TriggE0 0 0 0 0 0  0.373
   new:  /musr/command globalfield Trigg0_field 45 45 5      uniform 0. 0. -1149.15 log_TriggE0 0 0 0 0 0  0.373
  (the three coordinates which define the position of the centre of the field are shifted after the
   keyword "uniform", and the three half-dimensions of the box, in which the field is being defined, are
   specified before the keyword "uniform".  These three coorinates (e.g. 45 45 5) are taken from the
   definition of the logical volume (e.g. in our example from
   /musr/command construct box TriggE0 45 45 5      G4_Galactic 0 0 -57.15 log_TriggerV   norot dead 630 nofield )

4)  Replace the way how physics processes are defined, i.e.:
    Add:

###################################################################################
#########################  P H Y S I C S      P R O C E S S E S  ##################
###################################################################################
# --- Low Energy (default) ---
/musr/command process addDiscreteProcess gamma G4LowEnergyPhotoElectric
/musr/command process addDiscreteProcess gamma G4LowEnergyCompton
/musr/command process addDiscreteProcess gamma G4LowEnergyGammaConversion
/musr/command process addDiscreteProcess gamma G4LowEnergyRayleigh
/musr/command process addProcess         e-    G4MultipleScattering -1 1 1
#/musr/command process addDiscreteProcess e-    G4CoulombScattering
/musr/command process addProcess         e-    G4LowEnergyIonisation -1 2 2
/musr/command process addProcess         e-    G4LowEnergyBremsstrahlung -1 -1 3
/musr/command process addProcess         e+    G4MultipleScattering -1 1 1
#/musr/command process addDiscreteProcess e+    G4CoulombScattering
/musr/command process addProcess         e+    G4eIonisation    -1 2 2
/musr/command process addProcess         e+    G4eBremsstrahlung   -1 3 3
/musr/command process addProcess         e+    G4eplusAnnihilation 0 -1 4
#
# --- High Energy ---
#/musr/command process addDiscreteProcess gamma G4PhotoElectricEffect
#/musr/command process addDiscreteProcess gamma G4ComptonScattering
#/musr/command process addDiscreteProcess gamma G4GammaConversion
#/musr/command process addProcess         e-    G4MultipleScattering -1 1 1
##/musr/command process addDiscreteProcess e-    G4CoulombScattering
#/musr/command process addProcess         e-    G4eIonisation       -1 2 2
#/musr/command process addProcess         e-    G4eBremsstrahlung   -1 3 3
#/musr/command process addProcess         e+    G4MultipleScattering -1 1 1
##/musr/command process addDiscreteProcess e+    G4CoulombScattering
#/musr/command process addProcess         e+    G4eIonisation    -1 2 2
#/musr/command process addProcess         e+    G4eBremsstrahlung   -1 3 3
#/musr/command process addProcess         e+    G4eplusAnnihilation 0 -1 4
#
# --- Penelope ---
#/musr/command process addDiscreteProcess gamma G4PenelopePhotoElectric
#/musr/command process addDiscreteProcess gamma G4PenelopeCompton
#/musr/command process addDiscreteProcess gamma G4PenelopeGammaConversion
#/musr/command process addDiscreteProcess gamma G4PenelopeRayleigh
#/musr/command process addProcess         e-    G4MultipleScattering -1 1 1
##/musr/command process addDiscreteProcess e-    G4CoulombScattering
#/musr/command process addProcess         e-    G4PenelopeIonisation   -1 2 2
#/musr/command process addProcess         e-    G4PenelopeBremsstrahlung -1 -1 3
#/musr/command process addProcess         e+    G4MultipleScattering -1 1 1
##/musr/command process addDiscreteProcess e+    G4CoulombScattering
#/musr/command process addProcess         e+    G4PenelopeIonisation,     -1  2 2
#/musr/command process addProcess         e+    G4PenelopeBremsstrahlung, -1 -1 3
#/musr/command process addProcess         e+    G4PenelopeAnnihilation,    0 -1 4
#
# --- Muons ---
/musr/command process addProcess          mu+   G4MultipleScattering -1 1 1
#/musr/command process addProcess          mu+   MultipleAndCoulombScattering -1 1 1 goulombRegion
#/musr/command process addDiscreteProcess  mu+   G4CoulombScattering
/musr/command process addProcess          mu+   G4MuIonisation       -1 2 2
/musr/command process addProcess          mu+   G4MuBremsstrahlung   -1 3 3
/musr/command process addProcess          mu+   G4MuPairProduction   -1 4 4
/musr/command process addProcess          mu-   G4MultipleScattering -1 1 1
#/musr/command process addDiscreteProcess  mu-   G4CoulombScattering
/musr/command process addProcess          mu-   G4MuIonisation       -1 2 2
/musr/command process addProcess          mu-   G4MuBremsstrahlung   -1 3 3
/musr/command process addProcess          mu-   G4MuPairProduction   -1 4 4
# --- Muonium ---
/musr/command process addProcess          mu+   musrMuFormation     -1 -1 2
#cks - the following line not supported yet, has to be tested (at the moment, musrMuScatter is hard wired in the musrPhysicsList.cc):
#/musr/command process addProcess          Mu   musrMuScatter        -1 -1 1



   And remove the obsolete "/musr/command typeofprocesses" :

# Set processes from: lowenergy, penelope, coulombAndMultiple (default, Coul. only for CFoil), coulomb (for all, very slow).
/musr/command typeofprocesses coulombAndMultiple
#*/musr/command typeofprocesses penelope
#*/musr/command includeMuoniumProcesses false



5)  Replace "SetUserMinEkine":

   Old: 
     /musr/command SetUserMinEkine log_World 0.1

   New (and more general):
    #/musr/command SetUserLimits log_World ustepMax(mm) utrakMax(mm) utimeMax(ns) uekinMin(MeV) urangMin(mm)
    /musr/command SetUserLimits log_World -1 -1 -1 1e-7 -1



   Note:  The old command 
     /musr/command SetUserLimits   log_CFoil 1e-8
   should still be OK, but for safety reasons it might be better to replace it by
     /musr/command SetUserLimits   log_CFoil 1e-8 -1 -1 -1 -1

     However!!!  - this line was ignored in the old code (as one can see in the text output
                   of the simulation, so perhaps it should be commented out - such a small
                   step is probably danger to use!


6)  Add the following lines to anable/disable the Root output variables:
###################################################################################
#########################   R O O T     O U T P U T  ##############################
###################################################################################
#/musr/command rootOutput runID off
#/musr/command rootOutput eventID off
#/musr/command rootOutput weight off
#/musr/command rootOutput BFieldAtDecay off
#/musr/command rootOutput muIniPosX off
#/musr/command rootOutput muIniPosY off
#/musr/command rootOutput muIniPosZ off
#/musr/command rootOutput muIniMomX off
#/musr/command rootOutput muIniMomY off
#/musr/command rootOutput muIniMomZ off
#/musr/command rootOutput muIniPolX off
#/musr/command rootOutput muIniPolY off
#/musr/command rootOutput muIniPolZ off
#/musr/command rootOutput muDecayDetID off
#/musr/command rootOutput muDecayPosX off
#/musr/command rootOutput muDecayPosY off
#/musr/command rootOutput muDecayPosZ off
#/musr/command rootOutput muDecayTime off
#/musr/command rootOutput muDecayPolX off
#/musr/command rootOutput muDecayPolY off
#/musr/command rootOutput muDecayPolZ off
#/musr/command rootOutput muTargetTime off
#/musr/command rootOutput muTargetPolX off
#/musr/command rootOutput muTargetPolY off
#/musr/command rootOutput muTargetPolZ off
#/musr/command rootOutput muM0Time off
#/musr/command rootOutput muM0PolX off
#/musr/command rootOutput muM0PolY off
#/musr/command rootOutput muM0PolZ off
/musr/command rootOutput muM1Time off
/musr/command rootOutput muM1PolX off
/musr/command rootOutput muM1PolY off
/musr/command rootOutput muM1PolZ off
/musr/command rootOutput muM2Time off
/musr/command rootOutput muM2PolX off
/musr/command rootOutput muM2PolY off
/musr/command rootOutput muM2PolZ off
#/musr/command rootOutput posIniMomX off
#/musr/command rootOutput posIniMomY off
#/musr/command rootOutput posIniMomZ off
#/musr/command rootOutput fieldNomVal off
#/musr/command rootOutput det_ID off
#/musr/command rootOutput det_edep off
#/musr/command rootOutput det_edep_el off
#/musr/command rootOutput det_edep_pos off
#/musr/command rootOutput det_edep_gam off
#/musr/command rootOutput det_edep_mup off
#/musr/command rootOutput det_nsteps off
#/musr/command rootOutput det_length off
#/musr/command rootOutput det_start off
#/musr/command rootOutput det_end off
#/musr/command rootOutput det_x off
#/musr/command rootOutput det_y off
#/musr/command rootOutput det_z off
#/musr/command rootOutput det_kine off
/musr/command rootOutput det_VrtxKine off
/musr/command rootOutput det_VrtxX off
/musr/command rootOutput det_VrtxY off
/musr/command rootOutput det_VrtxZ off
/musr/command rootOutput det_VrtxVolID off
/musr/command rootOutput det_VrtxProcID off
/musr/command rootOutput det_VrtxTrackID off
/musr/command rootOutput det_VrtxParticleID off
/musr/command rootOutput det_VvvKine off
/musr/command rootOutput det_VvvX off
/musr/command rootOutput det_VvvY off
/musr/command rootOutput det_VvvZ off
/musr/command rootOutput det_VvvVolID off
/musr/command rootOutput det_VvvProcID off
/musr/command rootOutput det_VvvTrackID off
/musr/command rootOutput det_VvvParticleID off
### Root variables that are not written out by default, but can be switched on:
#/musr/command rootOutput fieldIntegralBx on
#/musr/command rootOutput fieldIntegralBy on
#/musr/command rootOutput fieldIntegralBz on
#/musr/command rootOutput fieldIntegralBz1 on
#/musr/command rootOutput fieldIntegralBz2 on
#/musr/command rootOutput fieldIntegralBz3 on



7)   Replace the (solid-name) keyword "trd" by "trd90y".

8)   Remove all keywords "nofield" at the end of the commands
     " /musr/command construct ... "  , since this does not have any meaning
     any more (it had been used before we switched to "GlobalField").

