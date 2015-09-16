#ifndef _CascadeTOF_h_
#define _CascadeTOF_h_


#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TGraph.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

//#include <time.h>
//#include <stdlib.h>
#include <ctime>

/// CascadeTOF: Class to hold the data from one Cascade data run that
/// is a count vs time series.   The constructor takes the inputfilename,
/// without any .tof or .txt, and opens the .txt file to read in the
/// run settings.  The .tof file is then read and turned into a
/// histogram with one bin per time, with the x-axis correctly scaled.
///
/// The date of the run is taken from the filename, and the start/end
/// time is taken from the .txt file.
///
/// Author: Blair Jamieson, Sept. 2015
class CascadeTOF {
public: 

  /// CacadeTOF constructor takes input filename without and .txt or
  /// .tof suffix.  The constructor does all of the work of reading
  /// the configuration file and reading in the datafile.
  CascadeTOF(char * infilename );
  ~CascadeTOF() { return; };

  /// Get Method to get a pointer to the count vs time histogram
  /// For time starting at 0.
  TH1D* GetHist0(){ return fTOF; };

  /// Get histogram with unix time on x-axis
  TH1D* GetHist(){ return fTOFt; }

  /// Get Method for start time as a unix timestamp in GMT
  std::time_t GetStartTime();

  /// Get Method for an end time as a unix timestamp in GMT
  std::time_t GetEndTime();
  
  /// Get date as a 6 digit YYMMDD integer
  int GetDate(){ return fBegYr*10000 + fBegMon*100 + fBegDay; };

  /// Get Run Length in 100 ns ticks
  int GetRunLength(){ return fRunLength; };

  /// Get Run Length in seconds
  float GetRunLengthSec(){ return float( fRunLength ) / 10000000.0; };

private:
  std::time_t fStartTime;   //< Start time as unix timestamp in GMT
  std::time_t fEndTime;     //< End time as unix timestamp in GMT 

  int  fBegYr;   //< Start year (YY) of run
  int  fBegMon;  //< Start month (MM) of run
  int  fBegDay;  //< Start day (DD) of run

  int   fBegHr;  //< Start hour of run 
  int   fBegMin; //< Start minute of run
  int   fBegSec; //< Start second of run

  int   fEndHr;  //< End Hr of run
  int   fEndMin; //< End Min of run
  int   fEndSec; //< End sec of run;

  int   fRunLength; //< Absolute time length of run in 100ns ticks

  TH1D* fTOF;    // Count vs time histogram (time from 0. to end of cycle)
  TH1D* fTOFt;    // Count vs time histogram (unix time)
};


#endif
