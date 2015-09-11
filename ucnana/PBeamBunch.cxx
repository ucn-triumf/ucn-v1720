

#include "PBeamBunch.h"
#include <iostream>
#include <TDirectory.h>

/// PBeamBunch constructor sets up histograms
PBeamBunch::PBeamBunch( int bunchnum, ULong64_t mintime, ULong64_t maxtime){
  ibunch = bunchnum;

  tmin = mintime;
  tmax = maxtime; //mintime + ULong64_t(bunchlength) * tickspers;

  tmins = tmin * secperns;
  tmaxs = tmax * secperns;

  int bunchlength = tmaxs - tmins;

  std::cout<<"PBeamBunch "<<bunchnum<<" from "
	   <<tmins<<" sec to "<< tmaxs<<" sec, total "<<bunchlength<<" sec"
	   <<std::endl;

  // set up histograms in directory
  char aname[100];
  TDirectory* rootdir = gDirectory;
  sprintf(aname,"bunch%06d",ibunch);
  TDirectory* curdir = gDirectory->mkdir(aname,aname);
  curdir->cd();

  char atitle[100];
  for (int ich=0; ich<PSD_MAXNCHAN*NDPPBOARDS; ich++) {
    // charge histograms per channel
    sprintf(aname,"b%06d_ch%02d_hqsql",ibunch,ich);
    sprintf(atitle,"Charge: Channel %02d bunch %d ; Q_{L} (ADC); Q_{S} (ADC)",
	    ich, ibunch );
    hqsql[ich] = new TH2D( aname, atitle, 200, 0., qlmax, 200, 0., qsmax );

    // psd histograms per channel
    sprintf(aname,"b%06d_ch%02d_hpsdql",ibunch,ich);
    sprintf(atitle,"PSD: Channel %02d bunch %d ; Q_{L} (ADC); PSD (Q_{L}-Q_{S})/Q_{L}",
	    ich, ibunch );
    hpsdql[ich] = new TH2D( aname, atitle, 200, 0., qlmax, 200, 0., 1.0 );

    // rate histograms per channel
    sprintf(aname,"b%06d_ch%02d_hevpers",ibunch, ich);
    sprintf(atitle,"Rate: Channel %02d bunch %d ; Time (s); Rate (Hz)",
	    ich, ibunch );
    hevpers[ich] = new TH1D( aname, atitle, bunchlength, 0.0, tmaxs-tmins );

  }
   
  // overall rate histograms
  sprintf(aname,"b%06d_all_hevper", ibunch);
  sprintf(atitle,"Rate: all channels bunch %d ; Time (s); Rate (Hz)",
	  ibunch );
  hevs = new TH1D( aname, atitle, bunchlength, 0.0, tmaxs-tmins );

  sprintf(aname,"b%06d_all_hevgamma", ibunch);
  sprintf(atitle,"Gamma rate: all channels bunch %d ; Time (s); Rate (Hz)",
	  ibunch );
  hevsgamma = new TH1D( aname, atitle, bunchlength, 0.0, tmaxs-tmins );

  rootdir->cd();
  gDirectory=rootdir;
  return;
}

int PBeamBunch::CheckTime( ULong64_t atime ){
  //std::cout<<"PBeamBunch::CheckTime tmin="<<tmin<<" tmax="<<tmax<<" atime="<<atime<<std::endl;
  if ( atime < tmin ) return -1;
  if ( atime > tmax ) return +1;
  return 0;
}
    
/// Fill method
void PBeamBunch::Fill( int channel, ULong64_t curtime, double qs, double ql ){

  if ( channel < 0 || channel>= PSD_MAXNCHAN*NDPPBOARDS ){
    std::cout<<"<PBeamBunch::Fill> Error channel = "<<channel<<" out of range"<<std::endl;
    return;
  }

  if ( curtime < tmin || curtime > tmax ){
    std::cout<<"<PBeamBunch::Fill> Time = "<<curtime
	     <<" out of range "<<tmin<<" to "<<tmax<<std::endl;
    return;
  }
    
  // 
  double psd = 0.0;
  if ( TMath::Abs(ql) > 1e-16 ) psd = (ql-qs)/ql;
  /// PLACE A ROUGH CUT ON PSD VS QL HERE !!!!
  /// BUT ONLY FOR PMT CHANNELS!
  if ( channel==0 || channel==1 || channel==2 || 
       channel==3 || channel==4 || channel==5 || 
       channel==6 || channel==8 || channel==9 ){
    //    if ( ql > 2000.0 && psd > 0.3 ){
    if ( psd > 0.3 ){
      hqsql[channel]->Fill( ql, qs );
      hpsdql[channel]->Fill( ql, psd );
      hevpers[channel]->Fill( curtime * secperns - tmins ); 
      hevs->Fill( curtime * secperns - tmins ); 
    } else if ( ql < 2000 && psd > 0.3 ){
      hevsgamma->Fill( curtime * secperns - tmins ); 
    }
  } else {
    // fill other channels normally
      hqsql[channel]->Fill( ql, qs );
      hpsdql[channel]->Fill( ql, psd );
      hevpers[channel]->Fill( curtime * secperns - tmins );     

  }

  return;
}


