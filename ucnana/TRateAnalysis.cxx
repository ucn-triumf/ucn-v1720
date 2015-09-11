
#include "TRateAnalysis.h"

TRateAnalysis::TRateAnalysis( TUCNTimeWindows * tws,  std::vector< PBeamBunch* > & pbs ){

  int ntws = tws->NBunches();

  if ( ntws != pbs.size() ){
    std::cout<<"<TRateAnalysis> Require consistent number of time windows."<<std::endl;
    exit(0);
  }

  // Loop over beam bunches
  for (int ibunch = 0; ibunch < pbs.size(); ibunch++) {
    PBeamBunch * bb = pbs.at( ibunch );
    TH1D* hsig = bb->GetTHist();
    TH1D* hgam = bb->GetGHist();
    bool isbg = tws->IsGVclosed( ibunch );
    
    double rate = hsig->GetEntries();
    double grate = hgam->GetEntries();
    
    fIsBG.push_back( isbg );
    fRates.push_back( rate );
    fRateErrs.push_back( TMath::Sqrt( rate ) );
    fGammaRates.push_back( grate );
    fGammaRateErrs.push_back( TMath::Sqrt( grate ) );
  }

  return;
}

void TRateAnalysis::Print(){
  
  std::cout<<"<TRateAnalysis> Printing overall counts for each beam bunch"
	   << std::endl;

  for (int ib=0; ib<fIsBG.size(); ib++){
    std::cout<<" Bunch "<<ib<<std::endl;
    if (fIsBG.at(ib)==true){
      std::cout<<"  Backgrnd : ";
    } else {
      std::cout<<"  Signal   : ";
    }
    std::cout<<fRates.at(ib)<<" +- "<<fRateErrs.at(ib)<<std::endl;
    std::cout<<"  Gammas   : ";
    std::cout<<fGammaRates.at(ib)<<" +- "<<fGammaRateErrs.at(ib)<<std::endl;

  }
}
