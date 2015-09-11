#ifndef _TRateAnalysis_h_
#define _TRateAnalysis_h_

#include "PBeamBunch.h"
#include "TUCNTimeWindows.h"
#include <TMath.h>

#include <vector>

class TRateAnalysis {
 private:

  // each entry in the vector is for one "proton beam bunch"
  // nominall every 300 sec.
  std::vector< bool >    fIsBG;
  std::vector< double >  fRates;
  std::vector< double >  fRateErrs;
  std::vector< double >  fGammaRates;
  std::vector< double >  fGammaRateErrs;
  

 public:

  TRateAnalysis(  TUCNTimeWindows * tws, std::vector< PBeamBunch* > & pbs );

  void Print();

};


#endif
