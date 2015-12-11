CXX           = g++ #gcc 
LD            = g++ #gcc
CXXFLAGS      = -O3 -w -I`root-config --incdir`
LDFLAGS       = -O3
ROOTLIBS      = `root-config --libs` -lMinuit
LIBS          = $(ROOTLIBS) -lm
LIBSALL       = $(ROOTLIBS) -lXm -lXt -lX11 -lm -lPW 

ucnrate:  nifty.o
	$(LD) $(LDFLAGS) nifty.o $(LIBS) -o nifty $(LIBS)

.C.o:
	$(CXX) $(CXXFLAGS) -c $<

.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm *.o ucnrate
