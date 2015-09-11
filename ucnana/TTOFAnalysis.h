#ifndef _TTOFAnalysis_h_
#define _TTOFAnalysis_h_

#include "TUCNTimeWindows.h"
#include "TTimeOffsets.h"

#include <TMath.h>
#include <TTree.h>
#include <TDirectory.h>
#include <TH2D.h>


#include <vector>

const int TOFCHAN = 13; //< Channel with chopper signal
const double GUIDELEN = 1.0; //< Time of flight distance in [meters]
const double CHOPFREQ = 1.0; //< Chopper opening frequency [Hz]

/// TTOFAnalysis:  Class to look for chopper signal and build a
/// time of flight spectrum.  Uses the already determined 
/// UCN beam windows to pick times in the UCN window that are
/// free from backgrounds.
///
/// Author: Blair Jamieson (Aug. 2015)
class TTOFAnalysis {
 private:

  TH1D* hTOF;      //< Time of flight spectrum histogram
  TH2D* hTOFch;    //< TOF vs channel
  TH1D* hSpeed;    //< UCN speed histogram
  TH1D* hChopDt;   //< Time difference between chopper signals!
  TH2D* hTOFvsPBt; //< TOF versus proton beam time
  TH2D* hTOFvsPBP0; //< TOF versus proton beam pulse number (PMT0-6)
  TH2D* hTOFvsPBP1; //< TOF versus proton beam pulse number (PMT7,8)
  TDirectory * tofdir;
  ULong64_t base;   //< Sum of last time in files that CalcTOF was called for
  std::vector<double> vChops; //< Vector of times (s) when we get chopper signal

 public:

  TTOFAnalysis( int arun, TTree* atUCN, TUCNTimeWindows * tws );

  /// Method to add another file to the TOF.  Assumption will be that
  /// this file is directly after the previous one, and that any
  /// subsequent calls to CalcTOF are with new TTrees at later
  /// sequential times.
  void CalcTOF( int arun, TTree * atUCN, TUCNTimeWindows * tws );
  
  void Print();

  /// Method to find the chopper time in seconds for a 
  /// given neutron time in seconds
  double GetChopperTime( double aTime );

};


#endif
