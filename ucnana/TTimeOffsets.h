#ifndef _TTimeOffsets_h_
#define _TTimeOffsets_h_

#include <TROOT.h>
#include "TVConstants.h"

const int TTOFFSETS_MAXNRUNS = 5000;

/// Time correction constants for
/// the difference in frequency between module 0 and module 1.
const double TTOFF_TSLIP_p0 = 0.025362+0.15e-3;
const double TTOFF_TSLIP_p1 = 
  -0.00000317012 +
   0.000000057634551;

struct TTOffArray_struct {
  Long64_t toffset[ NDPPBOARDS*PSD_MAXNCHAN ];
};

/// TTimeOffsets class
/// Static class, singleton to hold time offsets per channel for calibration
/// Reads offsets in ns per run from file "timecorrectionbyrun.txt"
/// Author: Blair Jamieson (Aug. 2015)
class TTimeOffsets {
 private:
  TTimeOffsets();

  static TTimeOffsets* instance;
  
  TTOffArray_struct  TOff[ TTOFFSETS_MAXNRUNS ];
  
  int fMinRun;
  int fMaxRun;
  
  
  
 public:
  static TTimeOffsets* Get( );
  
  /// Get Time offset in 1ns increments
  Long64_t Offset( int irun, int ichan, ULong64_t atime ){
    double ts = atime * 1.0e-9;
    double tslip = TimeSlipSec( ts, ichan);
    Long64_t slip = Long64_t( tslip/1.0e-9 );
    return ( TOff[ irun ].toffset[ ichan ]+slip );
  }
  
  /// Get Time offset in seconds
  double OffsetSec( int irun, int ichan, double ats){
    Long64_t lts = Long64_t( ats/1.0e-9 );
    return ( Offset( irun, ichan, lts )*1.0e-9 ); 
  };

  /// module 0 time slip in seconds
  /// for a given time in seconds
  double TimeSlipSec( double atime, int ichan ){
    if (ichan<PSD_MAXNCHAN){
      //return 0.0;
      return ( -1.0*(TTOFF_TSLIP_p0 + TTOFF_TSLIP_p1*atime) );
    } else {
      return 0.0;
    }
  }
  
  ~TTimeOffsets( ){ return; };
};


#endif
