#ifndef _PBEAMBUNCH_H_
#define _PBEAMBUNCH_H_

#include "TVConstants.h"
#include <TH2D.h>
#include <TH1D.h>

const double    secperns  = 1.0e-9;    //< seconds per ns
//const double    clocktick = 4.0e-9;    //< clock tick length in seconds (4 ns) 
//const ULong64_t tickspers = 250000000; //< length of a second in clock ticks
const double    qsmax     = 10000.0;   //< maximum for short charge in histograms
const double    qlmax     = 20000.0;   //< maximum for long charge in histograms

/// class PBeamBunch
///   -- holds histograms for each proton beam bunch cycle
///   -- filling these histograms is done using the Fill method
///   -- getting the results from the Get methods
/// 
/// Author:  Blair Jamieson (Jul 2015)
class PBeamBunch {
 public:

  /// PBeamBunch constructor takes as arguments:
  //  the initial time when the proton beam pulse starts in 4ns clock cycles (mintime)
  //  the final time before next proton beam pulse starts in 4ns clock cycles (maxtime)
  /// Note that you can make this longer than the actual beam bunch and be okay.
  PBeamBunch( int bunchnum, ULong64_t mintime, ULong64_t maxtime );   

  /// Destructor does nothing!  Histograms are owned by any TFile
  /// opened before using this class
  ~PBeamBunch(){ return; };

  /// Fill method
  void Fill( int channel, ULong64_t curtime, double qs, double ql );

  /// Check if a given time in clock ticks is in this proton beam bunch
  /// Return 0 if it is, -1 if it is before this time, +1 if it is after this time
  int CheckTime( ULong64_t atime );

  /// Check if a given time in seconds is in this proton beam bunch
  bool CheckTimes( ULong64_t atime ){ 
    if ( atime>tmins && atime<tmaxs) {
      return true;
    } else {
      return false; 
    }
  };

  /// Get the charge histogram for a particular channel
  TH2D * GetQHist( int channel ){  
    if( channel>=0 && channel <PSD_MAXNCHAN*NDPPBOARDS) return hqsql[channel];
    else return NULL;  };

  /// Get array of charge histograms
  TH2D** GetQHists(){ return hqsql; };

  /// Get Rate histogram for a given channel
  TH1D* GetTHist( int channel ){  
    if( channel>=0 && channel <PSD_MAXNCHAN*NDPPBOARDS) return hevpers[channel];
    else return NULL; };

  /// Get array of rate histograms
  TH1D** GetTHists(){ return hevpers; };

  /// Get Overall Rate Histogram
  TH1D* GetTHist(){ return hevs; };

  /// Get Overall Gamma Rage Histogram
  TH1D* GetGHist(){ return hevsgamma; };

 private:

  TH2D * hqsql[ PSD_MAXNCHAN * NDPPBOARDS ];   //< Short vs Long Charge for this beam bunch (for each channel)
  TH2D * hpsdql[ PSD_MAXNCHAN * NDPPBOARDS ];   //< (QL-QS)/QL vs Long Charge for this beam bunch (for each channel)
  TH1D * hevpers[ PSD_MAXNCHAN * NDPPBOARDS ]; //< Events in 1 second intervals (for each channel)
  TH1D * hevs;                                 //< Total Events in 1 second intervals (for all channels summed)
  TH1D * hevsgamma;                            //< Total "Gamma cut" Events in 1 second intervals (for all channels summed)

  int ibunch;  //< Bunch number for ordering in file.

  double tmins;  //< Minimum time in histograms (seconds)
  double tmaxs;  //< Maximum time in histograms (seconds)

  ULong64_t tmin;  //< Minimum time in histograms (4ns clock cycles)
  ULong64_t tmax;  //< Maximum time in histograms (4ns clock cycles)

};
#endif
