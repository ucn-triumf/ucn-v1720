
#include "TPulserAnalysis.h"

TPulserAnalysis::TPulserAnalysis( int arun, TTree *atUCN, TUCNTimeWindows * tws ){
  // reserve space
  int thechan=-1;
  for (int ich=0; ich<TPA_NCHAN; ich++){
    vPulseT[ich].reserve( 1000000 );
    for (int jch=ich; jch<TPA_NCHAN; jch++){
      thechan++;
      vPulseDT[thechan].reserve( 1000000 );
    }
  }

  TDirectory* topdir=gDirectory;  
  // make a new directory to hold the time of flight histogram
  std::cout<<"Making directory PulserAnalysis"<<std::endl;
  tpadir = topdir->mkdir("PulserAnalysis");
  base = 0;
  topdir->cd();
  CalcPulser( arun, atUCN, tws );
  
  
  return;
}

void TPulserAnalysis::CalcPulser( int arun, TTree *atUCN, TUCNTimeWindows * tws ){
  TDirectory * topdir=gDirectory;
  std::cout<<"<TPulserAnalysus::CalcPulser> for run "<<arun<<std::endl;
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
  
  ULong64_t time2=base;
  // loop over the TTree and save pulser signal times
  // due to horrible unsortedness of times of events
  // when comparing to other channels.
  for( ULong64_t j=0; j<eventTot; j++ ) {
    atUCN->GetEvent(j);
    if ( j%100000 ==0 ) printf("(TPulserAnalysis-pass 1) Load event % 20lldu of % 20lldu  t = % 20lldu base=% 20lldu\n",j,eventTot,tTimeE,base);
    // loop over pulser channels
    for (int i=0; i<TPA_NCHAN; i++){
      if ( tChannel == TPA_CHANS[i] ){
	if ( tChargeL > TPA_QLMIN[i] &&
	     tChargeL < TPA_QLMAX[i] ){
	  tTimeE += base +  TTimeOffsets::Get()->Offset( arun, tChannel, tTimeE+base );
	  double ts = tTimeE * PBNSTOSEC; // time of current event in sec.
	  if ( j%10000==0) std::cout<<"         "<<j<<" tChannel="<<tChannel<<" QL="<<tChargeL<<std::endl;
	  vPulseT[i].push_back( ts );
	  //if (i==1) std::cout<<"tChannel="<<TPA_CHANS[i]<<" QL="<<tChargeL<<" t="<<ts<<std::endl;
	}
      }
    }
  }
  
  // To make this general, we want to make sure that if the pulsers
  // start at different times, and end at different times, that we
  // only look over a time period when we have all of the pulser
  // signals
  // Latest time of first pulse:
  int    ilatefirst=-1;
  int    minlen=999999999999;
  double tlatefirst=0.0;
  for (int i=0; i<TPA_NCHAN; i++){
    if ( vPulseT[i].size()>0 ){
      if ( vPulseT[i].at(0) > tlatefirst ){
	ilatefirst = i;
	tlatefirst = vPulseT[i].at(0);
      }
      if ( vPulseT[i].size()<minlen ) minlen = vPulseT[i].size();
    }
  }
  
  if (ilatefirst<0){
    std::cout<<"<TPulserAnalysis> one or more pulser signals not found!"<<std::endl;
    return;
  }
  
  // offset into pulser times array for each
  // channel, to account for any difference in 
  // time offsets
  double dtmax = 1.0/TPA_PULSERFREQ; // period of pulser 
  int ifirst[ TPA_NCHAN ] = {0, 0, 0};
  int gotoffsets=0;
  for (int ich=0; ich< TPA_NCHAN; ich++){
    if ( ich == ilatefirst) continue;
    for (int i=0; i<minlen; i++){
      if ( tlatefirst-vPulseT[ich].at(i) < dtmax ) {
	ifirst[ ich ] = i;
	break;
      }
    }
  }

  for (int ich=0; ich<TPA_NCHAN; ich++)
    std::cout<<"ifirst["<<ich<<"] = "<<ifirst[ich]
	     <<" tlatefirst = "<<tlatefirst
	     <<" vPulseT.size="<<vPulseT[ich].size()
	     <<std::endl;

  
  // now find last entry to look at
  int npulser = vPulseT[0].size()-ifirst[0];
  for (int ich=1; ich<TPA_NCHAN; ich++){
    if ( vPulseT[ich].size()-ifirst[ich] < npulser )
      npulser = vPulseT[ich].size()-ifirst[ich];
  }
  std::cout<<"npulser="<<npulser<<std::endl;


  // now we are ready to loop over pulser times
  for (int i=0; i<npulser; i++){
    if (i%10000==0) std::cout<<"<TPulserAnalysis::CalcPulser> Filling Dt vector i="<<i<<" of "<<npulser<<std::endl;
    int thechan = -1;
    for (int ich=0; ich<TPA_NCHAN; ich++ ){
      for (int jch=ich; jch<TPA_NCHAN; jch++ ){
	thechan++;
	if ( ich==jch ){
	  // compare in same array
	  //if (ich==0){
	  //    std::cout<<"i="<<i<<" ich="<<ich<<" jch="<<jch
	  // 		       <<" thechan="<<thechan<<std::endl;
	  //   std::cout<<"  i+first="<<i+ifirst[ich]
	  //  		       <<"  size="<<vPulseDT[ich].size()<<std::endl;
	  //   std::cout<< "  cond 1="<< (i+ifirst[ich] > 1)
	  //		      << "  cond 2="<< (i+ifirst[ich]<vPulseDT[ich].size())
	  //		      << "  overall="
	  //		      << (i+ifirst[ich] > 1 && i+ifirst[ich]<vPulseDT[ich].size())
	  //		      <<std::endl;
	  //    
	  //}
	  if ( i+ifirst[ich] > 1 && i+ifirst[ich]<vPulseT[ich].size() ) {
	    double curdt = 
	      vPulseT[ich].at(i+ifirst[ich]) -
	      vPulseT[jch].at(i+ifirst[jch]-1);
	    //	    if( ich==1 ) std::cout<<"ich="<<ich<<" thechan="<<thechan
	    //			  <<" curdt="<<curdt
	    //				  <<" t="<< vPulseT[ich].at(i+ifirst[ich]) 
	    //				  <<" t-1="<<      vPulseT[jch].at(i+ifirst[jch]-1)
	    //				  <<std::endl;

	    vPulseDT[ thechan ].push_back( curdt );
	  }
	} else {
	  double curdt = 
	    vPulseT[ich].at(i+ifirst[ich]) -
	    vPulseT[jch].at(i+ifirst[jch]);
	  //	  std::cout<<"i="<<i<<" ich="<<ich<<" jch="<<jch<<" thechan="<<thechan<<" curdt="<<curdt<<std::endl;
	  vPulseDT[ thechan ].push_back( curdt );
	}
      }
    }
  }

  std::cout<<"<TPulserAnalysis::CalcPulser> Filling Histograms..."<<std::endl;
  
  // make the pulser time difference histograms
  char aname[100];
  char atitle[100];
  int ich=-1;
  for (int i=0; i<TPA_NCHAN; i++ ){
    for (int j=i; j<TPA_NCHAN; j++) {
      ich++;
      if (base==0){
	double dtmean, dtrms;
	this->GetMeanRMS( vPulseDT[ich], dtmean, dtrms  );
	std::cout<<"<TPulserAnalysis::After GetMeanRMS>  mean="<<dtmean<<" rms="<<dtrms<<std::endl;
	tpadir->cd();
	sprintf(aname,"hdtch%02dch%02d",TPA_CHANS[i],TPA_CHANS[j]);
	sprintf(atitle,"#Delta t Ch %02d - Ch %02d; #Delta t (s); Count",TPA_CHANS[i],TPA_CHANS[j]);

	// For same channel comparison, we are comparing subsequent pulses,
	// so expect time difference to match the pulser period
	// For different channel comparisons, we are comparing the same pulse,
	// so we expect no time difference (if there is no relative 
	// time drift between channels).
	//if (i==j) 
	//hDT[ ich ] = new TH1D( aname, atitle, 100,  dt-dtdt, dt+dtdt );
	//else
	hDT[ ich ] = new TH1D( aname, atitle, 100, dtmean-5.0*dtrms, dtmean+5.0*dtrms );
	
	sprintf(aname,"hDTvsPBPch%02dch%02d",TPA_CHANS[i],TPA_CHANS[j]);
	sprintf(atitle,"#Delta t Ch %02d - Ch %02d vs Proton beam bunch; Proton Beam Bunch; #Delta t (s)");
	//if (i==j){
	//	hDTvsPBP[ ich ] = new TH2D( aname, atitle, 
	//				    tws->NBunches(), 0., float( tws->NBunches()),
	//				    100, dt-dtdt, dt+dtdt);
	//} else {
	std::cout<<"ich="<<ich
		 <<" making "<<aname
		 <<" Nx="<<tws->NBunches()
		 <<" xmin="<<0.0<<" xmax="<<float(tws->NBunches())
		 <<" Ny = 100 ymin="<<dtmean-5.0*dtrms<<" ymax="<<dtmean+5.0*dtrms
		 <<std::endl;
	hDTvsPBP[ ich ] = new TH2D( aname, atitle, 
				    tws->NBunches(), 0.0, float( tws->NBunches()),
				    100, dtmean-5.0*dtrms, dtmean+5.0*dtrms);
	topdir->cd();

      }
      // fill the histograms
      for (int idt=0; idt<vPulseDT[ ich ].size(); idt++){
	double curdt = vPulseDT[ich].at(idt);
	// decide which proton beam bunch number
	// this time is approximate
	int ibunch = tws->GetBunchNum( vPulseT[i].at(idt) );
	hDT[ich]->Fill( curdt );
	hDTvsPBP[ich]->Fill( float(ibunch), curdt ); 
      }
    }
  }     

  base = time2; // end of this file... set base to end time.
  // this is set in main analysis loop?
  return;
}

void TPulserAnalysis::GetMeanRMS( std::vector<double> & vec, double &mean, double &rms ){
  
  double sum=0.0;
  double minval=1e200;
  double maxval=-1e200;
  double sum2=0.0;
  int N=vec.size();

  if (N<2){
    mean=0.0;
    rms=1.0;
    return;
  }

  for (int i=0; i<N; i++){
    if ( vec.at(i) > maxval ) maxval = vec.at(i);
    if ( vec.at(i) < minval ) minval = vec.at(i);
    sum += vec.at(i);
    sum2 += vec.at(i)*vec.at(i);
  }


  mean = sum / double( N );
  // rms =  sum ( xi-xav )^2 / (N-1)
  //     =  sum ( xi^2 - 2xi*xav + xav^2 ) / (N-1)
  //     =  (sum2 - 2*sum*mean + N*xav^2 ) / (N-1)
  //     = sum2/(N-1) - 2*mean^2*N/(N-1) + mean^2 * N/(N-1)
  //     = sum2/(N-1) - mean^2*N/(N-1)
  rms = ( sum2/double(N-1) - 
  	    mean*double(N)/double(N-1)  );
  //rms = (sum2 - 2*sum*mean + double(N)*mean*mean )/double(N-1);
  if (rms<0.0) rms = (maxval-minval)/2.0;

  std::cout<<"<TPulserAnalysis::GetMeanRMS> size="<<N
	   <<" mean="<<mean<<" rms="<<rms<<std::endl;
}

void TPulserAnalysis::Print(){
  
  return;
}
