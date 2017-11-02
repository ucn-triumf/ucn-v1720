####################################################################
#
#  Name:         Makefile
#  Created by:   Stefan Ritt
#
#  Contents:     Makefile for the v1720 frontend
#
#  $Id: Makefile 3655 2007-03-21 20:51:28Z amaudruz $
#
#####################################################################
#

# Path to gcc 4.8.1 binaries (needed to use new C++ stuff)
PATH := /home/ucn/packages/newgcc/bin:$(PATH)

USE_SYSTEM_BUFFER=1
SIMULATION=0
# Hardware setup (if not simulating)
NBLINKSPERA3818=2 #2 Number of optical links used per A3818
NBLINKSPERFE=2 #2 Number of optical links controlled by each frontend
NB1720PERLINK=1 # Number of daisy-chained v1720s per optical link
NBV1720TOTAL=2 #2 Number of v1720 boards in total

HWFLAGS = -DUSE_SYSTEM_BUFFER=$(USE_SYSTEM_BUFFER) \
-DNBLINKSPERA3818=$(NBLINKSPERA3818) -DNBLINKSPERFE=$(NBLINKSPERFE) \
-DNB1720PERLINK=$(NB1720PERLINK) -DNBV1720TOTAL=$(NBV1720TOTAL)

#--------------------------------------------------------------------
# The MIDASSYS should be defined prior the use of this Makefile
ifndef MIDASSYS
missmidas::
	@echo "...";
	@echo "Missing definition of environment variable 'MIDASSYS' !";
	@echo "...";
endif

#--------------------------------------------------------------------
# The following lines contain specific switches for different UNIX
# systems. Find the one which matches your OS and outcomment the 
# lines below.
#
# get OS type from shell
OSTYPE = $(shell uname)

#-----------------------------------------
# This is for Linux
ifeq ($(OSTYPE),Linux)
OSTYPE = linux
endif

ifeq ($(OSTYPE),linux)
#OS_DIR = linux-m64
OS_DIR = linux
OSFLAGS = -DOS_LINUX -DLINUX
CFLAGS = -g -Wall -pthread -I$(MIDASSYS)/include -DHAVE_LIBUSB -I$(MIDASSYS)/mscb -I$(MIDASSYS)/drivers/divers
LDFLAGS = -g -lm -lutil -lnsl -lpthread -lrt -lc 
endif

#-----------------------------------------
# optimize?

# CFLAGS += -O2

#-----------------------------------------
# ROOT flags and libs
#
ifdef ROOTSYS
ROOTCFLAGS := $(shell  $(ROOTSYS)/bin/root-config --cflags)
ROOTCFLAGS += -DHAVE_ROOT -DUSE_ROOT
ROOTLIBS   := $(shell  $(ROOTSYS)/bin/root-config --libs) -Wl,-rpath,$(ROOTSYS)/lib
ROOTLIBS   += -lThread
else
missroot:
	@echo "...";
	@echo "Missing definition of environment variable 'ROOTSYS' !";
	@echo "...";
endif

# Google perf tools
PERFLIBS=
ifdef GPROFILE
CXXFLAGS += -I/usr/local/include/google/ # -I/home/lindner/tools/gperftools-2.1/src
PERFLIBS += -lprofiler # /home/lindner/tools/gperftools-2.1/libprofiler.la
endif


#-------------------------------------------------------------------
# The following lines define directories. Adjust if necessary
#
CONET2_DIR   = $(HOME)/Andrew/CAENStuff
CAENCOMM_DIR = $(CONET2_DIR)/CAENComm-1.2
CAENCOMM_LIB = $(CAENCOMM_DIR)/lib/x86
CAENDGTZ_DIR = $(CONET2_DIR)/CAENDigitizer_2.7.9
CAENDGTZ_LIB = $(CAENDGTZ_DIR)/lib/x86_64
CAENVME_DIR  = $(CONET2_DIR)/CAENVMELib-2.50
CAENVME_LIB  = $(CAENVME_DIR)/lib/x86
MIDAS_INC    = $(MIDASSYS)/include
MIDAS_LIB    = $(MIDASSYS)/$(OS_DIR)/lib
MIDAS_SRC    = $(MIDASSYS)/src
MIDAS_DRV    = $(MIDASSYS)/drivers/vme
ROOTANA      = $(HOME)/packages/rootana

####################################################################
# Lines below here should not be edited
####################################################################
#
# compiler
CC   = gcc #-std=c99
CXX  = g++ -g -std=c++0x
#
# MIDAS library
LIBMIDAS=-L$(MIDAS_LIB) -lmidas
#
# CAENComm
LIBCAENCOMM=-L$(CAENCOMM_LIB) -lCAENComm
#
# CAENVME
LIBCAENVME=-L$(CAENVME_LIB) -lCAENVME
#
# CAENDigitizer
LIBCAENDGTZ =-L$(CAENDGTZ_LIB) -lCAENDigitizer
#
# All includes
MIDASINCS = -I. -I./include -I$(MIDAS_INC) -I$(MIDAS_DRV)
CAENINCS = -I$(CAENVME_DIR)/include -I$(CAENCOMM_DIR)/include -I$(CAENDGTZ_DIR)/include

DRIVERS         = hv.o multi.o null.o nulldev.o mscbdev.o mscb.o

####################################################################
# General commands
####################################################################

all: fe
	@echo "***** Finished"
	@echo "***** Use 'make doc' to build documentation"

fe : feoV1720.exe 

doc ::
	doxygen
	@echo "***** Use firefox --no-remote doc/html/index.html to view if outside gateway"

####################################################################
# Libraries/shared stuff
####################################################################

ov1720.o : $(MIDAS_DRV)/ov1720.c
	$(CC) -c $(CFLAGS) $(MIDASINCS) $(CAENINCS) $< -o $@ 

####################################################################
# Single-thread frontend
####################################################################

feoV1720.exe: $(MIDAS_LIB)/mfe.o  feoV1720.o ov1720.o v1720CONET2.o
	$(CXX) $(OSFLAGS) feoV1720.o v1720CONET2.o ov1720.o $(MIDAS_LIB)/mfe.o $(LIBMIDAS) $(LIBCAENCOMM) $(LIBCAENVME) $(LIBCAENDGTZ) -o $@ $(LDFLAGS)

feoV1720.o : feoV1720.cxx v1720CONET2.o
	$(CXX) $(CFLAGS) $(OSFLAGS) $(HWFLAGS) $(MIDASINCS) $(CAENINCS) -Ife -c $< -o $@

v1720CONET2.o : v1720CONET2.cxx
	$(CXX) $(CFLAGS) $(OSFLAGS) $(HWFLAGS) $(MIDASINCS) $(CAENINCS) -Ife -c $< -o $@


# hv.o: $(DRV_DIR)/class/hv.c
# 	$(CC) $(CFLAGS) $(OSFLAGS) -c $< -o $@

# multi.o: $(DRV_DIR)/class/multi.c
# 	$(CC) $(CFLAGS) $(OSFLAGS) -c $< -o $@

# nulldev.o: $(DRV_DIR)/device/nulldev.c
# 	$(CC) $(CFLAGS) $(OSFLAGS) -c $< -o $@

# mscbdev.o: $(DRV_DIR)/device/mscbdev.c
# 	$(CC) $(CFLAGS) $(OSFLAGS) -c $< -o $@

# mscb.o: $(MSCB_DIR)/mscb.c
# 	$(CC) $(CFLAGS) $(OSFLAGS) -c $< -o $@

# null.o: $(DRV_DIR)/bus/null.c
# 	$(CC) $(CFLAGS) $(OSFLAGS) -c $< -o $@

.c.o:


$(MIDAS_LIB)/mfe.o:
	@cd $(MIDASSYS) && make
####################################################################
# Clean
####################################################################

clean:
	rm -f *.o *.exe
	rm -f *~
	rm -rf html
	rm -rf stress

####################################################################
# Stress test program
####################################################################
stress: stress_test.c
	$(CC) $(CFLAGS) $(CAENINCS) -o $@ $(LDFLAGS) $< $(LIBCAENCOMM) $(LIBCAENVME)
stress2: stress_test_short.c
	$(CC) $(CFLAGS) $(CAENINCS) -o $@ $(LDFLAGS) $< $(LIBCAENCOMM) $(LIBCAENVME)

setcards: setcards.cxx
	$(CXX) $(CFLAGS) $(CAENINCS) -o $@ $(LDFLAGS) $< $(LIBCAENCOMM) $(LIBCAENVME)
#end file
