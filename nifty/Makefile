CXX           = g++ #gcc 
LD            = g++ #gcc
CXXFLAGS      = -O3 -w -I`root-config --incdir`
LDFLAGS       = -O3
ROOTLIBS      = `root-config --libs` -lMinuit
LIBS          = $(ROOTLIBS) -lm
LIBSALL       = $(ROOTLIBS) -lXm -lXt -lX11 -lm -lPW 

all: ucnrate dataforfit makepdfs

ucnrate:  nifty.o INIReader.o ini.o
	$(LD) $(LDFLAGS) nifty.o INIReader.o ini.o $(LIBS) -o nifty $(LIBS)

dataforfit: dataforfit.o INIReader.o ini.o
	$(LD) $(LDFLAGS) dataforfit.o INIReader.o ini.o $(LIBS) -o dataforfit $(LIBS)

makepdfs: makepdfs.o INIReader.o ini.o
	$(LD) $(LDFLAGS) makepdfs.o INIReader.o ini.o $(LIBS) -o makepdfs $(LIBS)
.C.o:
	$(CXX) $(CXXFLAGS) -c $<

.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm *.o ucnrate
	rm *.o dataforfit
	rm *.o makepdfs