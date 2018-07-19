SPECIAL SETTING SPECIFIED IN SOME FILES:

===========================================================================
1)  ~/.bashrc file:

The following lines were added to the ~/.bashrc file:

export ROOTSYS="/usr/local/root/5.20.00"
export PATH=$ROOTSYS/bin:$PATH
if [ $LD_LIBRARY_PATH ] ; then
export LD_LIBRARY_PATH=$ROOTSYS/lib:$LD_LIBRARY_PATH
else
export LD_LIBRARY_PATH=$ROOTSYS/lib
fi
if [ $CPPFLAGS ] ; then
export CPPLFLAGS=-I$ROOTSYS/include:$CPPFLAGS
else
export CPPFLAGS=-I$ROOTSYS/include
fi
source /afs/psi.ch/user/s/sedlak/.alias

===========================================================================
2) /home/geant4/4.9.1.p03/env.sh file:

Inside the "/home/geant4/4.9.1.p03/env.sh",  the following lines were inserted
just before the last two lines " PATH=${PATH}:${G4WORKDIR}/bin/${G4SYSTEM} " and
" export PATH " :

if [ $LD_LIBRARY_PATH ] ; then
LD_LIBRARY_PATH=${G4LIB}/${G4SYSTEM}:${CLHEP_BASE_DIR}/lib:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH
else
LD_LIBRARY_PATH=${G4LIB}/${G4SYSTEM}:${CLHEP_BASE_DIR}/lib
export LD_LIBRARY_PATH
fi
echo "On this machine the LD_LIBRARY_PATH=$LD_LIBRARY_PATH"

===========================================================================
3) ~/.alias file

The following alias can be used to set-up the environment variables for Geant4:

alias g4='export G4WORKDIR="/home/sedlak/bin_4.9.1.p03"; source /home/geant4/4.9.1.p03/env.sh; export G4VRMLFILE_VIEWER="vrmlview"; echo  "On this machine the G4VRMLFILE_VIEWER=$G4VRMLFILE_VIEWER"'

===========================================================================
