#ifndef _TPulserAnalysis_h_
#define _TPulserAnalysis_h_

#include "TUCNTimeWindows.h"
#include "TTimeOffsets.h"

#include <TMath.h>
#include <TTree.h>
#include <TDirectory.h>
#include <TH2D.h>


#include <vector>

//const int    TPA_NCHAN = 3; //< Number of channels with a pulser signal
//const int    TPA_CHANS[ TPA_NCHAN ] = {       7,    10  ,    15      }; //< channel numbers with pulser signal
const int    TPA_NCHAN = 2; //< Number of channels with a pulser signal
const int    TPA_CHANS[ TPA_NCHAN ] = {       7,     15      }; //< channel numbers with pulser signal
//const double TPA_QLMIN[ TPA_NCHAN ] = { 6000.0,   6000.0,  6000.0    }; //< minimum charge long of pulser signal in each channel
//const double TPA_QLMAX[ TPA_NCHAN ] = { 8000.0,   8000.0,  8000.0    }; //< maximum charge long of pulser signal in each channel
const double TPA_QLMIN[ TPA_NCHAN ] = { 6000.0,    6000.0    }; //< minimum charge long of pulser signal in each channel
const double TPA_QLMAX[ TPA_NCHAN ] = { 12000.0,    12000.0    }; //< maximum charge long of pulser signal in each channel
const double TPA_PULSERFREQ = 1.0; //< Pulser frequency in Hz

/// TPulserAnalysis: Class to look at the 1Hz pulser data.
/// Comparisons to make are:
/// 1) time between pulses for each pulser channel
/// 2) time between pulses in different channels 
/// number of comparisons: 0-0 1-1 2-2 0-1 0-2 1-2 == 4*3/2 = (NCHAN+1)(NCHAN)/2
///
/// Author: Blair Jamieson (Aug. 2015)
class TPulserAnalysis {
 private:

  TH1D* hDT[ TPA_NCHAN*(TPA_NCHAN+1)/2 ];       //< Pulser time differences
  TH2D* hDTvsPBP[  TPA_NCHAN*(TPA_NCHAN+1)/2 ]; //< Pulser time difference vs proton beam bunch
  TDirectory * tpadir;
  ULong64_t base;   //< Sum of last time in files that CalcTOF was called for
  std::vector<double> vPulseT[ TPA_NCHAN ]; //< Vector of times (s) when we get chopper signal
  std::vector<double> vPulseDT[ TPA_NCHAN*(TPA_NCHAN+1)/2 ]; //< Vector of time differences

  void GetMeanRMS( std::vector<double> & vec, double &mean, double &rms );

 public:
  TPulserAnalysis( int arun, TTree* atUCN, TUCNTimeWindows * tws );

  /// Method to add another file to the Pulser Analysis.  Assumption
  /// will be that this file is directly after the previous one, and
  /// that any subsequent calls to CalcTOF are with new TTrees at
  /// later sequential times.
  void CalcPulser( int arun, TTree * atUCN, TUCNTimeWindows * tws );
  
  void Print();

};


#endif
