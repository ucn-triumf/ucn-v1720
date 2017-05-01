# Makefile

# Hardware setup
NDPPBOARDS=2
PSD_MAXNCHAN=8


CXXFLAGS = -g -Wall -Wuninitialized

# optional ZLIB library

CXXFLAGS += -DHAVE_ZLIB

# ROOT libraries

ifdef ROOTSYS
ROOTGLIBS = -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --glibs) -lXMLParser -lThread
CXXFLAGS += -DHAVE_ROOT $(shell $(ROOTSYS)/bin/root-config --cflags)
endif

ifdef MIDASSYS
MIDASLIBS = $(MIDASSYS)/linux/lib/libmidas.a -lutil -lrt
CXXFLAGS += -DHAVE_MIDAS -DOS_LINUX -Dextname -I$(MIDASSYS)/include

endif

ROOTANADIR = $(HOME)/packages/rootana/
ROOTANAINC = -I$(ROOTANADIR)/include
ROOTANALIBS = $(ROOTANADIR)/lib/librootana.a

CAENDIGLIB = $(HOME)/CAENDigitizer_2.6.0/include
CAENCOMMLIB = $(HOME)/CAENComm-1.2/include

OBJS:=
OBJS += TV1720Histograms.o
OBJS += TUCNAnaManager.o
OBJS += PulseShapeStruct.o
OBJS += TUCNAnaViewer.o
OBJS += TUCNAnaViewer2.o
OBJS += TUCNAnaViewer3.o
OBJS += TV1720WaveformDisplay.o
#OBJS += TUCNAnaViewer4.o
#TUCNAnaViewer3 added June 15, 2016
#TUCNAnaViewer4 added June 20, 2016
all: $(OBJS) UCNAnalyzer.exe UCNDisplay.exe UCNDisplay2.exe UCNDisplay3.exe 
#UCNDisplay3 added June 15, 2016
UCNDisplay.exe: UCNDisplay.cxx $(OBJS) 
	$(CXX) -o $@ $(CXXFLAGS) $(ROOTANAINC) $^ $(ROOTANALIBS) $(MIDASLIBS) $(ROOTGLIBS) -lm -lpthread $(RPATH)  -lrt -lutil

UCNDisplay2.exe: UCNDisplay2.cxx $(OBJS) 
	$(CXX) -o $@ $(CXXFLAGS) $(ROOTANAINC) $^ $(ROOTANALIBS) $(MIDASLIBS) $(ROOTGLIBS) -lm -lpthread $(RPATH)  -lrt -lutil

UCNDisplay3.exe: UCNDisplay3.cxx $(OBJS) 
	$(CXX) -o $@ $(CXXFLAGS) $(ROOTANAINC) $^ $(ROOTANALIBS) $(MIDASLIBS) $(ROOTGLIBS) -lm -lpthread $(RPATH)  -lrt -lutil

UCNAnalyzer.exe: UCNAnalyzer.cxx $(OBJS) 
	$(CXX) -o $@ $(CXXFLAGS) $(ROOTANAINC) $^ $(ROOTANALIBS) $(MIDASLIBS) $(ROOTGLIBS) -lm -lpthread $(RPATH)  -lrt -lutil

GetTimeComp.exe: GetTimeComp.C $(OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $(ROOTANAINC) $^ $(ROOTANALIBS) $(MIDASLIBS) $(ROOTGLIBS) -lm -lpthread $(RPATH)  -lrt -lutil

%Dict.o: %Dict.cxx
	$(CXX) $(CXXFLAGS) $(ROOTANAINC) -c $<

%Dict.cxx: %.h %_LinkDef.h
	LD_LIBRARY_PATH=$(ROOTSYS)/lib $(ROOTSYS)/bin/rootcint -f $@ -c -p $(CXXFLAGS) $(ROOTANAINC) $^


%.o: %.cxx
	$(CXX) $(CXXFLAGS) $(ROOTANAINC) -c $<

dox:
	doxygen

clean::
	-rm -f *.o *.a *.exe 

# end
