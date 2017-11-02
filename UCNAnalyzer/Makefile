# Makefile

# Hardware setup
NDPPBOARDS=2
PSD_MAXNCHAN=8


CXXFLAGS = -g -Wall -Wuninitialized -O2

# optional ZLIB library
# added -lz -std=c++11 -lpthread A. Sikora
CXXFLAGS += -DHAVE_ZLIB -lz -std=c++11 -pthread

# ROOT libraries

ifdef ROOTSYS
ROOTGLIBS = -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --glibs) -lXMLParser -lThread -lRHTTP # added -lRHTTP (TL)
CXXFLAGS += -DHAVE_ROOT $(shell $(ROOTSYS)/bin/root-config --cflags)
endif

ifdef MIDASSYS
MIDASLIBS = $(MIDASSYS)/linux/lib/libmidas.a -lutil -lrt
CXXFLAGS += -DHAVE_MIDAS -DOS_LINUX -Dextname -I$(MIDASSYS)/include

endif

ROOTANADIR = $(HOME)/packages/rootana/
ROOTANAINC = -I$(ROOTANADIR)/include
ROOTANALIBS = $(ROOTANADIR)/lib/librootana.a

CAENDIGLIB = $(HOME)/Andrew/CAENStuff/CAENDigitizer_2.7.9/include
CAENCOMMLIB = $(HOME)/Andrew/CAENStuff/CAENComm-1.2/include

OBJS:=
OBJS += TV1720Histograms.o
OBJS += TUCNAnaManager.o
OBJS += PulseShapeStruct.o
OBJS += TUCNAnaViewer.o
OBJS += TUCNAnaViewer2.o
OBJS += TUCNAnaViewer3.o
OBJS += TV1720WaveformDisplay.o
OBJS += UCNRateHistogram.o
OBJS += TTimeHelper.o
#OBJS += TUCNAnaViewer4.o
#TUCNAnaViewer3 added June 15, 2016
#TUCNAnaViewer4 added June 20, 2016
all: $(OBJS) UCNAnalyzer.exe UCNDisplay.exe UCNRateMonitor.exe UCNDisplay3.exe 
#UCNDisplay3 added June 15, 2016
UCNDisplay.exe: UCNDisplay.cxx $(OBJS) 
	$(CXX) -o $@ $(CXXFLAGS) $(ROOTANAINC) $^ $(ROOTANALIBS) $(MIDASLIBS) $(ROOTGLIBS) -lm -lpthread $(RPATH)  -lrt -lutil

UCNRateMonitor.exe: UCNRateMonitor.cxx $(OBJS) 
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
