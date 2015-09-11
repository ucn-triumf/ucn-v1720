
#include "TTOFAnalysis.h"

TTOFAnalysis::TTOFAnalysis( int arun, TTree *atUCN, TUCNTimeWindows * tws ){
  TDirectory* topdir=gDirectory;  
  // make a new directory to hold the time of flight histogram
  std::cout<<"Making directory TTOF"<<std::endl;
  tofdir = topdir->mkdir("TTOF");
  tofdir->cd();

  // make the TOF histograms
  hTOF = new TH1D("hTOF","Time of Flight; Time (s); Count per 0.005s",200,0.,1.0);
  hTOFvsPBt = new TH2D("hTOFvsPBt","Time of Flight vs proton beam time; Proton Time (s); TOF (s)",200,0.,300.,200,0.,1.0);
  
  hSpeed = new TH1D("hSpeed","UCN Speed; Speed (m/s); Count per 0.05m/s",200,0.,10.);

  hTOFvsPBP0 = new TH2D("hTOFvsPBP0","PMTs 0-6 Time of Flight vs Proton beam bunch; Beam Bunch Number; TOF (s)", tws->NBunches(), 0., float(tws->NBunches()), 100,0.,1.0);
  hTOFvsPBP1 = new TH2D("hTOFvsPBP1","PMTs 7,8 Time of Flight vs Proton beam bunch; Beam Bunch Number; TOF (s)", tws->NBunches(), 0., float(tws->NBunches()), 100,0.,1.0);
  // expected time between chopper signals is:
  double dtavg=1.0/CHOPFREQ;
  double dtdt=0.001;
  hChopDt=new TH1D("hChopDt","Time between chopper signals; #Delta t (s); Count",
		   200,dtavg-dtdt,dtavg+dtdt);

  hTOFch = new TH2D("hTOFch","Time of Flight; PMT Number; TOF (s)",9,0.,9.,200,0.,1.0);

  base = 0;
  CalcTOF( arun, atUCN, tws );
  topdir->cd();


  return;
}

void TTOFAnalysis::CalcTOF( int arun, TTree *atUCN, TUCNTimeWindows * tws ){

  int ntws = tws->NBunches();

  // Set up TTree pointer to these variables
  ULong64_t tTimeE;
  UShort_t tChannel;
  UShort_t tChargeL;
  UShort_t tChargeS;

  atUCN->SetBranchAddress("tTimeE",&tTimeE);
  atUCN->SetBranchAddress("tChannel",&tChannel);
  atUCN->SetBranchAddress("tChargeL",&tChargeL);
  atUCN->SetBranchAddress("tChargeS",&tChargeS);

  ULong64_t eventTot = atUCN->GetEntries();

  // keep track of time of last chopper signal
  double tchop = -1.0;

  ULong64_t time2=base;
  // loop over the TTree and save chopper signal times
  // due to horrible unsortedness of times of events
  // when comparing to other channels.
  for( ULong64_t j=0; j<eventTot; j++ ) {
    atUCN->GetEvent(j);
    if ( j%100000 ==0 ) printf("(TTOFAnalysis-pass 1) Load event % 20lldu of % 20lldu  t = % 20lldu base=% 20lldu\n",j,eventTot,tTimeE,base);
    tTimeE += base +  TTimeOffsets::Get()->Offset( arun, tChannel, tTimeE+base );
    double ts = tTimeE * PBNSTOSEC; // time of current event in sec.
    
    // Reset chopper time, each time we see its signal
    if ( tChannel == TOFCHAN){
      if (tchop>0.0 && (ts-tchop)<0.3 ){
	std::cout<<"   FOUND CHOPPER SIGNAL AT t="<<ts<<std::endl;
	std::cout<<"    IGNORING SPURIOUS CHOPPER TRIGGER"<<std::endl;
      } else {
	tchop = ts;
	vChops.push_back( tchop );
      }
    }
  }

  // Loop over chopper times
  for (int ichop=1; ichop<vChops.size(); ichop++){
    hChopDt->Fill(  vChops.at(ichop) - vChops.at(ichop-1) );
  }
  
  // loop over the TTree and fill the time of flight histogram
  
  for( ULong64_t j=0; j<eventTot; j++ ) {
    atUCN->GetEvent(j);
    if ( j%100000 ==0 ) printf("(TTOFAnalysis) Load event % 20lldu of % 20lldu  t = % 20lldu base=% 20lldu\n",j,eventTot,tTimeE,base);
    tTimeE += base + TTimeOffsets::Get()->Offset( arun, tChannel, tTimeE+base ) ;
    if ( tTimeE > time2 )  time2 = tTimeE;
    double ts = tTimeE * PBNSTOSEC; // time of current event in sec.

    int ibunch = tws->GetBunchNum( ts ); // proton pulse number
    
    // skip events with times outside of ucn time windows
    if ( ibunch<0 ) continue;
    
    double pbtime = tws->GetPBeamTime( ibunch ); // proton bunch time
    // std::cout<<" pbtime = "<<pbtime<<" ts-pbtime="<<ts-pbtime<<std::endl;
    
    // skip events with proton bunch time outside
    // of 44 to 144 sec after proton pulse
    if ( (ts-pbtime) < 44.0 || (ts-pbtime) > 144.0 ) continue;

    // find closest chopper time prior to this neutron
    tchop = GetChopperTime( ts );

    // skip events until we see the first chopper signal
    if ( tchop<0.0 ) continue;
    
    // Calculate the psd and place cuts on ql, psd, and pmt channel
    double psd =0.0;
    if ( TMath::Abs( tChargeL ) > 1.0e-16 ) {
      //std::cout<<" calculate psd QL="<<tChargeL
      //	       <<" QS="<<tChargeS
      //	       <<" QL-QS="<<tChargeL-tChargeS
      //	       <<" psd="<< double(tChargeL-tChargeS)/double(tChargeL)
      //	       <<std::endl;
      psd = double(tChargeL-tChargeS)/double(tChargeL); 
    } else {
      std::cout<<" not calculating psd QL="<<tChargeL
	       <<" abs(QL)="<< TMath::Abs(tChargeL)<<std::endl;
    }
    /// PLACE A ROUGH CUT ON PSD VS QL HERE !!!!
    /// BUT ONLY FOR PMT CHANNELS!
    //std::cout<<"Chan="<<tChannel<<" QL="<<tChargeL<<" psd="<<psd<<std::endl;
    if ( tChannel==0 || tChannel==1 || tChannel==2 || 
	 tChannel==3 || tChannel==4 || tChannel==5 || 
	 tChannel==6 || tChannel==8 || tChannel==9 ){
      if ( tChargeL > 2000.0 && psd > 0.3 ){
	//std::cout<<"FILL HISTO ts="<<ts<<" tchop="<<tchop<<" tof="<<ts-tchop<<std::endl;
	double thetof= ts-tchop;
	hTOF->Fill( thetof );
	if (tChannel<7) hTOFch->Fill( float(tChannel), thetof );
	else hTOFch->Fill( float(tChannel-1), thetof ); 
	hTOFvsPBt->Fill( ts-pbtime , thetof );
       
	if (tChannel<7) hTOFvsPBP0->Fill( ibunch, thetof );
	else hTOFvsPBP1->Fill( ibunch, thetof );
	if (thetof>0.0) hSpeed->Fill( GUIDELEN/thetof );
      }
    }
  }
  
  base = time2; // end of this file... set base to end time.
  // this is set in main analysis loop?
  return;
}

double TTOFAnalysis::GetChopperTime( double aTime ){

  double choptime = -1.0;

  // Use std::lower_bound algorithm of STL
  // Returns an iterator pointing to the first element 
  // that is not less than (i.e. greater or equal to) value. 
  std::vector<double>::iterator it = 
    std::lower_bound( vChops.begin(), vChops.end(), aTime );

  if ( it != vChops.begin() ){
    // *it is the first time in the sorted array that is right after
    // aTime, so look at the previous entry to see if it is within 2s
    double curchopt = *(--it);
    if ( aTime-curchopt > 0.0 ){
      if ( aTime-curchopt < 1.0/CHOPFREQ ){
	choptime = curchopt;
      }
    }
  }

  return choptime;
}

void TTOFAnalysis::Print(){
  
  return;
}
