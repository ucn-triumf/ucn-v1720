CXX           = g++ #gcc 
LD            = g++ #gcc
CXXFLAGS      = -g -w -I`root-config --incdir`
LDFLAGS       = -g
ROOTLIBS      = `root-config --libs` -lMinuit
LIBS          = $(ROOTLIBS) -lm
LIBSALL       = $(ROOTLIBS) -lXm -lXt -lX11 -lm -lPW -lcurses

ucnrate:  ucnrate.o V1720DigitizerMC.o PMTSignal.o
	$(LD) $(LDFLAGS) ucnrate.o V1720DigitizerMC.o PMTSignal.o $(LIBS) -o ucnrate $(LIBS)

singleEvent:  singleEvent.o V1720DigitizerMC.o PMTSignal.o
	$(LD) $(LDFLAGS) singleEvent.o V1720DigitizerMC.o PMTSignal.o $(LIBS) -o singleEvent $(LIBS)
.C.o:
	$(CXX) $(CXXFLAGS) -c $<

.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm *.o ucnrate
	rm *.o singleEvent
