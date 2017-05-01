#include "TUCNTimeWindows.h"
#include <iostream>

TUCNTimeWindows::TUCNTimeWindows( int arun, TTree * atUCN, TTree* atRunTran ){
  // initialize the time window structures.
  tstart = 0;
  base = 0;
  time1 = 0;
  time2 = 0;
  nfileschecked = 0;
  vBeamTimes.reserve(1000);
  vStartTimes.reserve(1000);
  vEndTimes.reserve(1000);
  twhists.reserve(100);
  vRunStartTimes.reserve(1000);

  TDirectory* topdir=gDirectory;  
  twdir= topdir->mkdir("UCNTimeWindowCheck");
  // make a directory to store histograms
  twdir->cd();
    
  hdtwin=new TH1D("hdtwin","Time between proton beam signals",100, PBEAMTIMESEP-5.0, PBEAMTIMESEP+5.0);
  CalcTimes( arun, atUCN, atRunTran );

  topdir->cd();
}

void TUCNTimeWindows::CalcTimes( int arun, TTree * atUCN, TTree* atRunTran ){
  // Get the unix timestamp for the start of the run
  Int_t tRunnum;
  Int_t tRunstarttime;
  ULong64_t tEntry;
  atRunTran->SetBranchAddress("tRunNum",&tRunnum);
  atRunTran->SetBranchAddress("tTime",&tRunstarttime);
  atRunTran->SetBranchAddress("tEntry",&tEntry);
  atRunTran->GetEvent(0);

  // Set up TTree pointer to these variables
  ULong64_t tTimeE;
  UShort_t tChannel;
  UShort_t tChargeL;
  UShort_t tChargeS;

  atUCN->SetBranchAddress("tTimeE",&tTimeE);
  atUCN->SetBranchAddress("tChannel",&tChannel);
  atUCN->SetBranchAddress("tChargeL",&tChargeL);
  atUCN->SetBranchAddress("tChargeS",&tChargeS);

  TDirectory* topdir=gDirectory;  
  twdir->cd();
  // save current base offset to reset on second loop
  // over the TTree
  ULong64_t prevbase = base;

  ULong64_t eventTot = atUCN->GetEntries();

  // Loop over events just to get earliest and latest times
  atUCN->GetEvent(0);
  time1 = tTimeE + base + TTimeOffsets::Get()->Offset( arun, tChannel, tTimeE+base );
  atUCN->GetEvent( eventTot - 1 );
  time2 = tTimeE+base +  TTimeOffsets::Get()->Offset( arun, tChannel, tTimeE+base );
  if ( nfileschecked == 0) tstart = time1;

  std::cout<<"<TUCNTimeWindows> First Pass over TTree"<<std::endl;
  for( ULong64_t j=0; j<eventTot; j++ ) {
    atUCN->GetEvent(j);
    tTimeE+=base +  TTimeOffsets::Get()->Offset( arun, tChannel, tTimeE+base );
    //if ( j%100000 ==0 ) printf("Load event % 20lldu of % 20lldu  t = % 20lldu base=% 20lldu\n",j,eventTot,tTimeE,base);
    if ( j%100000 ==0 ) std::cout<<"."<<std::flush;
    
    if ( tTimeE < tstart ) tstart = tTimeE;
    if ( tTimeE < time1 ) time1 = tTimeE;
    if ( tTimeE > time2 ) time2 = tTimeE;
  }
  std::cout<<std::endl;
  
  int nbinstotal = int( 10.0*((time2-time1)*PBNSTOSEC) );
  double time1s = time1*PBNSTOSEC;
  double time2s = time2*PBNSTOSEC;

  // need to reset base and loop over tree again
  base = prevbase;

  // make a histogram for the current file
  char aname[100];
  char atitle[100];
  sprintf(aname,"UCNTW%04d",nfileschecked);
  sprintf(atitle,"Rate / 0.1 sec file %d; Time (s); Count/0.1sec",nfileschecked);
  
  std::cout << "<TUCNTimeWindows> Build histogram with " << nbinstotal 
	    << " bins from t="<< time1s <<" to t="<< time2s << std::endl;
  TH1D* curhist = new TH1D( aname, atitle, nbinstotal, time1s, time2s  );
  twhists.push_back( curhist );

  sprintf(aname,"UCNRT%04d",nfileschecked);
  sprintf(atitle,"Rate / 0.1 sec file %d; Clock Time (s); Count/0.1sec",nfileschecked);
  
  std::cout << "<TUCNTimeWindows> Build histogram with " << nbinstotal 
	    << " bins from t="<< time1s+tRunstarttime <<" to t="<< time2s+tRunstarttime << std::endl;
  TH1D* curhistrt = new TH1D( aname, atitle, nbinstotal, 
			      time1s+double(tRunstarttime), time2s+double(tRunstarttime)  );
  twhistsrt.push_back( curhistrt );
  curhistrt->GetXaxis()->SetTimeDisplay(kTRUE);
  // force root to use the standard unix time stamp instead of the root one
  curhistrt->GetXaxis()->SetTimeFormat("%m/%d %H:%M%F1970-01-01 00:00:00s0");

  if ( nfileschecked == 0){
    for (int iboard=0; iboard<NDPPBOARDS; iboard++){
      for (int ich=0; ich<PSD_MAXNCHAN; ich++){
	int ichan = iboard*PSD_MAXNCHAN+ich;
	sprintf( aname, "twhistch%02d", ichan );
	sprintf( atitle, "Event Time Board%d-Ch%d Global Channel %02d; Time (s); Count / 0.1s", iboard, ich, ichan);
	twhistch[ ichan ] = new TH1D( aname, atitle, nbinstotal, time1s, time2s  );
      }
    }
  }

  // fill current time windows histogram
  std::cout<<"<TUCNTimeWindows> Filling histograms loop over TTree"<<std::endl;
  for( ULong64_t j=0; j<eventTot; j++ ) {
    atUCN->GetEvent(j);
    tTimeE+=base +  TTimeOffsets::Get()->Offset( arun, tChannel, tTimeE+base );
    if ( j%100000 ==0 ) std::cout<<"."<<std::flush;
    //if ( j%100000 ==0 ) printf("(2nd pass) Load event % 20lldu of % 20lldu  t = % 20lldu base=% 20lldu\n",j,eventTot,tTimeE,base);
    curhist->Fill( tTimeE*PBNSTOSEC );
    curhistrt->Fill( tTimeE*PBNSTOSEC + double(tRunstarttime) );
    twhistch[ tChannel ]->Fill( tTimeE*PBNSTOSEC );
  }
  std::cout<<std::endl;

  // need to reset base and loop over tree again
  base = prevbase;
  
  // Now the hard part, find the start times.
  // We do this one of two ways:
  // 
  // Method 1) Look for the proton beam signal:
  if (PBUSEPBSIGNAL == true) {
    // No need to fill time window histogram.  Just loop through the
    // file looking for the proton beam signal.
    std::cout<<"<TUCNTimeWindows> Find proton beam signal times"<<std::endl;
    for( ULong64_t j=0; j<eventTot; j++ ) {
      atUCN->GetEvent(j);
      //if ( j%100000 ==0 ) printf("(3rd pass) Load event % 20lldu of % 20lldu  t = % 20lldu base=% 20lldu\n",j,eventTot,tTimeE,base);
      if ( tChannel != PBCHAN ) continue;
      if ( tChargeL < PBQLMIN ) continue;
      if ( tChargeL > PBQLMAX ) continue;
      tTimeE+=base +  TTimeOffsets::Get()->Offset( arun, tChannel, tTimeE+base );
      
      double mpv = tTimeE*PBNSTOSEC - 6.45; // arbitrarily add a 6.45 s offset to when to make window.
      double twminwin = mpv;
      double twmaxwin = mpv+PBEAMTIMESEP;
      
      std::cout<<"<TUCNTimeWindows>  Add time window for pulse at "<<mpv
	       <<" from "<<twminwin<<" to "<<twmaxwin<<std::endl;
      vBeamTimes.push_back( mpv );
      vStartTimes.push_back( twminwin );
      vEndTimes.push_back( twmaxwin );
      isGVclosed.push_back( false ); // assume open!
      // save run start time.  
      vRunStartTimes.push_back( tRunstarttime );
      

      //      curhist->Fill( tTimeE*PBNSTOSEC );
    }
    
    
  } else {
    // Method 2) do a rate analysis

    // We know there should be a peak every PBEAMTIMESEP
    // so look for highest point in 1.1* this length initial
    // to find the first bunch.  
    int maxbin=0;
    double maxval=0.0;
    int nbins = int( PBEAMTIMESEP*12 );
    for (int ibin=1; ibin<nbins; ibin++){
      if ( curhist->GetBinContent( ibin ) > maxval ){
	maxval = curhist->GetBinContent(ibin);
	maxbin = ibin;
      }
    }
    
    double maxval0 = maxval;
    
    // Fit peak to Landaun
    std::cout<<"Fit peak 0 near t="<<curhist->GetBinCenter(maxbin)<<std::endl;
    int pknum=0;
    double xmin = curhist->GetBinCenter(maxbin-10);
    double xmax = curhist->GetBinCenter(maxbin+10);
    sprintf(aname,"ldau%04d_%04d",nfileschecked,pknum);
    TF1* tf = new TF1(aname,"gaus", xmin, xmax );
    tf->SetParameters( maxval, curhist->GetBinCenter( maxbin ), 1.5 );
    
    curhist->Fit( tf, "Q", "", xmin, xmax );
    
    // Now define the first bunch as 14s before the peak,
    // until PBEAMTIMESEP - 14 s after the peak.
    double mpv = tf->GetParameter(1);
    double twminwin = mpv - 14.0;
    double twmaxwin = mpv - 14.0 + PBEAMTIMESEP;
    std::cout<<"<TUCNTimeWindows>  Add time window for pulse at "<<mpv
	     <<" from "<<twminwin<<" to "<<twmaxwin<<std::endl;
    vBeamTimes.push_back( mpv );
    vStartTimes.push_back( twminwin );
    vEndTimes.push_back( twmaxwin );
    isGVclosed.push_back( false );
    // save run start time.  
    vRunStartTimes.push_back( tRunstarttime );
    

    // guess number of time windows by first and last times
    int guessntwin = (time2s-time1s)/PBEAMTIMESEP;
    
    // loop over PBEAMTIMESEP second sections after this first one and
    // repeat the process of finding peaks
    for (int iwin=1; iwin<guessntwin; iwin++ ){
      if ( mpv + PBEAMTIMESEP > time2s ) break;
      maxbin=0;
      maxval=0.0;
      nbins = int( PBEAMTIMESEP*10 );
      int binstart = (mpv-14.0+PBEAMTIMESEP)*10;
      std::cout<<"Search for maxval between "
	       << curhist->GetBinCenter( binstart) <<" and "
	       << curhist->GetBinCenter( TMath::Min( binstart+nbins, nbinstotal) )
	       <<std::endl;
      for (int ibin= binstart; 
	   ibin< TMath::Min( binstart+nbins, nbinstotal); 
	   ibin++ ) {
	if ( curhist->GetBinContent( ibin ) > maxval ){
	  maxval = curhist->GetBinContent(ibin);
	  maxbin = ibin;
	}
      }
      
      // Fit peak to Landaun
      pknum++;
      std::cout<<"Fit peak "<<pknum<<" near t="<<curhist->GetBinCenter(maxbin)<<std::endl;
      xmin = curhist->GetBinCenter(maxbin-10.);
      xmax = curhist->GetBinCenter(maxbin+10.);
      sprintf(aname,"ldau%04d_%04d",nfileschecked,pknum);
      tf = new TF1(aname,"gaus", xmin, xmax );
      tf->SetParameters( maxval, curhist->GetBinCenter( maxbin ), 1.5 );
      curhist->Fit( tf, "Q+", "", xmin, xmax );
      
      // Now define the first bunch as 10s before the peak,
      // until PBEAMTIMESEP - 10 s after the peak.
      mpv = tf->GetParameter(1);
      if ( maxval < 0.1*maxval0 ){
	mpv = GetPBeamTime( pknum-1 )+PBEAMTIMESEP; 
	isGVclosed.push_back( true );
      } else {
	isGVclosed.push_back( false );
      }
      
      twminwin = mpv - 14.0;
      twmaxwin = mpv - 14.0 + PBEAMTIMESEP;
      std::cout<<"<TUCNTimeWindows>  Add time window for pulse at "<<mpv
	       <<" from "<<twminwin<<" to "<<twmaxwin<<std::endl;
      vBeamTimes.push_back( mpv );
      vStartTimes.push_back( twminwin );
      vEndTimes.push_back( twmaxwin ); 
      // save run start time.  
      vRunStartTimes.push_back( tRunstarttime );

    }
  }

  for (int ipulse=1; ipulse<vBeamTimes.size(); ipulse++){
    hdtwin->Fill( vBeamTimes.at(ipulse) - vBeamTimes.at(ipulse-1) );
  }
  nfileschecked++;
  topdir->cd();
  base = time2; // end of this file... set base to end time.
  return;
}

void TUCNTimeWindows::Print(){
  
  std::cout<<"<TUCNTimeWindows> Found "<< NBunches()
	   <<" proton beam pulses"<<std::endl;

  for (int ipulse=0; ipulse<vBeamTimes.size(); ipulse++){

    std::cout<<"  Pulse "<<ipulse
	     <<" at "<< GetPBeamTime( ipulse ) << "s "
	     <<" from "<< GetStartTime( ipulse ) << "s "
	     <<" to "<< GetEndTime( ipulse ) << "s "
	     <<std::endl;
  }
  return;
}
