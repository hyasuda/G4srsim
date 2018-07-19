# $Id: GNUmakefile,v 1.2 2008/10/21 14:34:02 sedlak Exp $
# --------------------------------------------------------------
# GNUmakefile for examples module.  Gabriele Cosmo, 06/04/98.
# --------------------------------------------------------------

G4WORKDIR=./
name := musrSim
G4TARGET := $(name)
G4EXLIB := true
##LDFLAGS := $(shell root-config --glibs)
#ROOTLIBS := ${shell root-config --glibs}
#LDFLAGS  := -L./ ${ROOTLIBS}

# Root (exlude libNew and libpthread from library list)
ROOTINC       = -I$(ROOTSYS)/include/root
 
# Removed -lHtml for compatibility with root 5.18 (TS)
# Otherwise keep it, but add also -lThread.
##ROOTLIBS     = $(shell $(ROOTSYS)/bin/root-config --glibs) -lMinuit -lHtml
##ROOTLIBS     = $(shell $(ROOTSYS)/bin/root-config --glibs) -lMinuit -lHtml -lThread
ROOTLIBS       = $(shell $(ROOTSYS)/bin/root-config --glibs) -lMinuit
ROOTLIBS      := $(filter-out -lNew,$(ROOTLIBS))
ROOTLIBS      := $(filter-out -lpthread,$(ROOTLIBS))
#
#ROOTLIBS      := $(filter-out -lThread,$(ROOTLIBS))
#ROOTLIBS     := $(filter-out -lpthread,$(ROOTLIBS))
ROOTLIBS      := $(filter-out -pthread,$(ROOTLIBS))
 
# Extra flags for G4
CPPFLAGS += $(ROOTINC)
#LDLIBS   += $(ROOTLIBS)
EXTRALIBS += $(ROOTLIBS)
CPPFLAGS += -g
ifndef G4INSTALL
  G4INSTALL = ../../..
endif

.PHONY: all
all: lib bin

include $(G4INSTALL)/config/binmake.gmk
