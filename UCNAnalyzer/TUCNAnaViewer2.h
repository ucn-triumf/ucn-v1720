#ifndef _TUCNAnaViewer2_h_
#define _TUCNAnaViewer2_h_


#include "UCNRateHistogram.h"
#include "TTimeHelper.h"

#include "TH1D.h"
#include "TV1720Histograms.h"
#include "TDataContainer.hxx"
#include "PulseShapeStruct.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"
#include <time.h>
#include <thread>
#include <iostream>
#include <fstream>


/// TUCNAnaViewer2
/// For looking at rate of events in last 3600 x 2 seconds
/// Every 3600 seconds save previous plot (except first time), and
/// update prev plot to be current one, and make new current one.
/// Saving of plots is into root file...
class TUCNAnaViewer2 {
  
 public:
  
  TUCNAnaViewer2();
  ~TUCNAnaViewer2();
  
  /// Processes the midas event, fills histograms, etc.
  int ProcessMidasEvent(TDataContainer& dataContainer, char CutChoice, float PSDMax, float PSDMin);

  /// Get "current" histogram
  UCNRateHistogram* GetCurrHisto() const { return hCurr; }//TH1D* to UCNRateHistogram*
  
  /// Get "previous" histogram
  UCNRateHistogram* GetPrevHisto() const { return hPrev; }//TH1D to UCNRateHistogram*

  /// Method to get "BeginRun" transition
  void BeginRun( int run );

  /// Method to get "EndRun" transition
  void EndRun( int run );

  /// Used to split up the amount of sub evetns being processed at one time using threads
  void SubEvLoop(UCNRateHistogram * hPrev, UCNRateHistogram * hCur, TTimeHelper tthelp[],
		 int ich, int ichan, int iboard, unsigned long *banktime, TMidasEvent sample,
		 int *loopstart, int* loopend );

  std::thread first;
  std::thread second;
  std::thread third;
  std::thread fourth;
  int Thr1loopstart, Thr2loopstart, Thr3loopstart, Thr4loopstart;
  int prevBin=0;

 private:
  
  TFile         * fOut;  //< Root output file
  UCNRateHistogram * hCurr; //< Most recent rate histogram
  UCNRateHistogram * hPrev; //< Previous rate histogram
  
  DPPBankHandler fDPP[NDPPBOARDS]; //< Data container for data read in from digitizer

  TTimeHelper tthelp[NDPPBOARDS*PSD_MAXNCHAN]; //< Time helper per channel
  
  int NCPU = 4; //< number of CPUs in DAQ cumputer;
  char threading = 0; //<  ==1 for thread mode ==0 for non thread mode
  float PSD;

  ofstream AverageRate;
  int totalSeconds;
  int totalEvents;

  ofstream EventCounter;
  long int totalPulses;
  long int valveOpenEvents;
  int valveOpen=0; //= 0 closed, 1 = open
  int valveChan = 14;


};

#endif
