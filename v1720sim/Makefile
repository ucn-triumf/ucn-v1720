CXX           = g++ #gcc 
LD            = g++ #gcc
CXXFLAGS      = -O2 -w -I`root-config --incdir`
LDFLAGS       = -O2
ROOTLIBS      = `root-config --libs` -lMinuit
LIBS          = $(ROOTLIBS) -lm
LIBSALL       = $(ROOTLIBS) -lXm -lXt -lX11 -lm -lPW -lcurses

ucnrate:  ucnrate.o V1720DigitizerMC.o PMTSignal.o
	$(LD) $(LDFLAGS) ucnrate.o V1720DigitizerMC.o PMTSignal.o $(LIBS) -o ucnrate $(LIBS)

.C.o:
	$(CXX) $(CXXFLAGS) -c $<

.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm *.o ucnrate
