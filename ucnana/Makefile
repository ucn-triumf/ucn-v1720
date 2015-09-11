CXX           = g++
LD            = g++
CXXFLAGS      = -g -w -I`root-config --incdir`
LDFLAGS       = -g
ROOTLIBS      = `root-config --glibs` -lMinuit
LIBS          = $(ROOTLIBS) -lm
LIBSALL       = $(ROOTLIBS) -lXm -lXt -lX11 -lm -lPW -lcurses

all: TWindow.o PBeamBunch.o TUCNTimeWindows.o TRateAnalysis.o TTOFAnalysis.o TTimeOffsets.o TPulserAnalysis.o ucnana

ucnana:  TWindow.o TUCNTimeWindows.o PBeamBunch.o TRateAnalysis.o TTOFAnalysis.o  TTimeOffsets.o TPulserAnalysis.o TVConstants.h
	$(LD) $(LDFLAGS) TWindow.o TUCNTimeWindows.o PBeamBunch.o TRateAnalysis.o TTOFAnalysis.o TTimeOffsets.o TPulserAnalysis.o $(LIBS) -o ucnana

TUCNTimeWindows.o:	TUCNTimeWindows.cxx TUCNTimeWindows.h 
	$(CXX) $(CXXFLAGS) -c $<

PBeamBunch.o:	PBeamBunch.cxx PBeamBunch.h TVConstants.h
	$(CXX) $(CXXFLAGS) -c $<

TRateAnalysis.o:	TRateAnalysis.cxx TRateAnalysis.h TVConstants.h
	$(CXX) $(CXXFLAGS) -c $<

TTOFAnalysis.o:	TTOFAnalysis.cxx TTOFAnalysis.h TVConstants.h
	$(CXX) $(CXXFLAGS) -c $<

TTimeOffsets.o: TTimeOffsets.cxx TTimeOffsets.h TVConstants.h
	$(CXX) $(CXXFLAGS) -c $<

TPulserAnalysis.o: TPulserAnalysis.cxx TPulserAnalysis.h TVConstants.h
	$(CXX) $(CXXFLAGS) -c $<

.C.o:	Rate.C TVConstants.h
	$(CXX) $(CXXFLAGS) -c $<
clean:
	rm *.o ucnana



