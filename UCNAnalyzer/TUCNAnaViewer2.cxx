 #include "TROOT.h"
#include "TStyle.h"
#include "TUCNAnaViewer2.h"
#include "TF1.h"
#include <cmath>
#include <thread>
#include <iostream>
#include <fstream>

bool moved = true;

TUCNAnaViewer2::TUCNAnaViewer2(){

  fOut = NULL;
  hCurr = NULL;
  hPrev = NULL;

}


TUCNAnaViewer2::~TUCNAnaViewer2(){
}

void TUCNAnaViewer2::BeginRun( int run ){
  // set up a new output file:
  // check if output file already exists?
  if (  fOut != NULL){ 
    // file exits! Clean up!
    fOut->Write();
    fOut->Close();
    fOut = NULL; // is this a memory leak?
  }

  // build file name
  char fname[200];
  sprintf( fname, "ucnanaviewer2_%06d.root", run );
  fOut = new TFile( fname, "recreate" );

  // reset histograms (ie. they disappear when TFile is closed)
  hCurr = NULL;
  hPrev = NULL;
}

void TUCNAnaViewer2::EndRun( int run ){
 // check if output file already exists?
  if (  fOut != NULL ){ 
    // file exits! Clean up!
    fOut->Write();
    fOut->Close();
    fOut = NULL; // is this a memory leak?
  }
}


int TUCNAnaViewer2::ProcessMidasEvent(TDataContainer& dataContainer, char CutChoice, float PSDMax, float PSDMin){

  int nwaveformsprocessed=0;
    
  // Loop over all the data, fill the histograms and and find histograms.
  TMidasEvent sample = dataContainer.GetMidasEvent();
  if ( ! sample.IsBank32() ) return 0;


  std::string banklist(sample.GetBankList());
  //std::cout << banklist << std::endl;
  if(banklist.find("W20") == std::string::npos) return 1;

  // output bank block information
  
  printf("event id: %d\n",sample.GetEventId());
  printf("trigger mask: %d\n",sample.GetTriggerMask());
  printf("serial number: %d\n",sample.GetSerialNumber());
  printf("time: %d\n",sample.GetTimeStamp());
  printf("data size: %d\n",sample.GetDataSize());
    
  // got event, now to iterate through the banks
  printf("  bank list:\n");
  printf(sample.GetBankList()); 
  printf("\n");


  TMidas_BANK32 * bank = NULL;
  char * pdata = sample.GetData();

  /// We have some new waveforms, make sure to clear
  /// the previous event info so it doesnt get
  /// used again in case this board is not in this event
  /// ClearWaves Added Aug.15,2015 ABJ
  for (int iboard = 0; iboard<NDPPBOARDS; iboard++) {
    fDPP[iboard].ClearWaves();
  }
  
  // loop over the all data for one event and organize said data
  bool foundabank=false;
  for( sample.IterateBank32(&bank,&pdata); bank!=NULL&&pdata!=NULL;
       sample.IterateBank32(&bank,&pdata)  ) {
    int ibrd;
 
    sscanf(&bank->fName[3],"%1d", &ibrd);
    if ( ibrd<0 || ibrd >= NDPPBOARDS){
      //      if(verbose)
	std::cout<<"<TUCNAnaViewer2> bank="<<ibrd<<" but MAXBOARDS= "<<NDPPBOARDS<<std::endl;
      continue;
    } 
    //if(verbose)
      std::cout<<"<TUCNAnaViewer2> board="<<ibrd<<std::endl;
    
    foundabank=true;
    std::cout<<"Before pdata Init"<<std::endl;
    fDPP[ibrd].Init( pdata );
    std::cout<<"After pdata Init"<<std::endl;

  } // end looping through banks

  // if slow control event, not part of the event rate
  if ( foundabank == false ) return 0;

  // check if we need to make histograms!
  if ( hCurr == NULL){
    ULong64_t earliestTime = -1; // initialize to largest unsigned integer
    // loop over events to find earliest time
    for (int iboard = 0; iboard<NDPPBOARDS; iboard++) {
      for (int ichan = 0; ichan < PSD_MAXNCHAN;ichan++) {
	int NWaves = fDPP[iboard].GetNWaves(ichan);
	for (int isubev = 0;isubev<NWaves;isubev++) {	
	  DPP_Bank_Out_t * b  = fDPP[iboard].GetPSD( isubev, ichan );
	
	  if ( b==NULL) 
	    continue;

	  ULong64_t curt  =b->TimeTag;
	  if ( curt < earliestTime ) earliestTime = curt;
	}
      }
    }
    // now make histogram for current setting
    hCurr = new UCNRateHistogram( earliestTime );
    hCurr->SetTimeStamp( sample.GetTimeStamp() );

    // initialize time helpers
    for (int iboard = 0; iboard<NDPPBOARDS; iboard++) {
      for (int ichan = 0; ichan < PSD_MAXNCHAN;ichan++) {
	int ich = iboard*8 + ichan;
	tthelp[ ich ].SetChan( ich );
	tthelp[ ich ].SetBankT0( sample.GetTimeStamp() );
	tthelp[ ich ].SetLastTimeTag( earliestTime );
      }
    }	
  }

  
  unsigned long banktime = sample.GetTimeStamp();

  EventCounter.open("EventCounter.txt");
  
  // loop over all waveforms and fill histogram
  for (int iboard = 0; iboard<NDPPBOARDS; iboard++) {
    for (int ichan = 0; ichan < PSD_MAXNCHAN;ichan++) {
      int ich = iboard*8 + ichan;
      int NWaves = fDPP[iboard].GetNWaves(ichan);	 
      nwaveformsprocessed += NWaves;  
      /// used to split up events evenly in each thread
      int NWavesPerThr = NWaves/NCPU;
     

      /// For debugging purposes. Want to have this loop use threading but having some difficulties
      /// have option for no threading so we always have a working version.
      /// threading == 1 then threading
      /// threading == 0 then no threading
      /// June 2017, A. Sikora
      if (threading == 1)
	{
	  Thr1loopstart = 0;
	  Thr2loopstart = NWavesPerThr;
	  Thr3loopstart = NWavesPerThr*2;
	  Thr4loopstart = NWavesPerThr*3;

	  /// If number of CPU cores is not 4 the amount of threads must be updated
	  if(Thr2loopstart && Thr3loopstart && Thr4loopstart){
	    std::cout<<"First start"<<std::endl;
	    first = std::thread(&TUCNAnaViewer2::SubEvLoop, this, hPrev, hCurr, tthelp, ich, 
				ichan, iboard, &banktime, sample, &Thr1loopstart, &Thr2loopstart);
	    std::cout<<"Second start"<<std::endl;
	    second = std::thread(&TUCNAnaViewer2::SubEvLoop, this, hPrev, hCurr, tthelp, ich, 
				 ichan, iboard, &banktime, sample, &Thr2loopstart, &Thr3loopstart);
	    std::cout<<"Third start"<<std::endl;
	    third  = std::thread(&TUCNAnaViewer2::SubEvLoop, this, hPrev, hCurr, tthelp, ich, 
				 ichan, iboard, &banktime, sample, &Thr3loopstart, &NWaves);
	    std::cout<<"Fouth start"<<std::endl;
	     fourth = std::thread(&TUCNAnaViewer2::SubEvLoop, this, hPrev, hCurr, tthelp, ich, 
	    			 ichan, iboard, &banktime, sample, &Thr4loopstart, &NWaves);
	    std::cout<<"loop1 start: "
		     <<0<<"\nloop2 start: "
		     <<Thr2loopstart<<"\nloop3 start: "
		     <<Thr3loopstart<<"\nloop4 start: "
		     <<Thr4loopstart<<"\nloop4 end: "
		     <<NWaves<<std::endl;
	    
	    first.join();
	    std::cout<<"First joined"<<std::endl;
	    second.join();
	    std::cout<<"Second joined"<<std::endl;
	    third.join();
	    std::cout<<"Third joined"<<std::endl;
	    fourth.join();
	    std::cout<<"Fourth joined"<<std::endl;
	  } 
	}

      /// for when threading need not be done (when we want to run the program without it crashing
      /// before debugging of the treading section is complete)
      if (threading == 0)
      {	 
	for (int isubev = 0;isubev<NWaves;isubev++) {
	  DPP_Bank_Out_t * b  = fDPP[iboard].GetPSD( isubev, ichan );
	  if (b->ChargeLong !=0)
	    PSD = (b->ChargeLong - b->ChargeShort)/b->ChargeLong;


	  // check current histogram to see if we increment in that histogram
	  unsigned long timetag = tthelp[ich].GetTime_in_ticks( banktime, b->TimeTag );
	  if ( b==NULL) 
	    continue;
	  if (CutChoice == 'y')
	    if (PSD < PSDMin || PSD > PSDMax)
	      continue;
	  totalPulses++;
	  if (ichan==valveChan && valveOpen == 0)
	    valveOpen = 1;
	  else if (ichan == valveChan && valveOpen == 1)
	    valveOpen = 0;
	  if (valveOpen == 1)
	    valveOpenEvents++;
	  
	  int ibin = hCurr->GetBinForTime( timetag);
	  if ( ibin == -1 ){ 
	    // no, it is in previous (or earlier) histogram
	    if ( hPrev != NULL ) {
	      // put in previous times... if exists
	      ibin = hPrev->GetBinForTime( timetag);
	    }
	    if ( ibin == -1 ){
	      std::cout << "Warning:: found time that is before previous histogram... waveform not counted" <<std::endl;
	      continue; // go to next waveform
	    } else {
	      // Increment in previous histogram
	      hPrev->Get()->AddBinContent( ibin );
	    }
	  } else if ( ibin == -numBins ) {
	    // no, it is in time after current histogram, so:
	    // need a new histogram for times later than current histogram!
	    // 1) save any existing previous histogram
	    if ( hPrev != NULL ) hPrev->Get()->Write();
	    // 2) Current histogram becomes previous one
	    hPrev = hCurr;
	    // 3) Make a new current histogram
	    hCurr = new UCNRateHistogram( hPrev->Get_tmax() );
	    hCurr->SetTimeStamp( sample.GetTimeStamp() );
	    ibin = hCurr->GetBinForTime( timetag );
	    if ( ibin > 0 ){
	      // okay add an entry to the new histogram
	      hCurr->Get()->AddBinContent( ibin );
	    } else {
	      std::cout<<"Warning:: ibin="<<ibin
		       <<" is either before or after prev and current histo"
		       <<std::endl;
	    }
	  } else {
	    // yes, it is in current histogram:
	      hCurr->Get()->AddBinContent( ibin );

	  }
	}
      }
    }
  }
    EventCounter<<totalPulses<<"\t"<<valveOpenEvents<<"\n";
    EventCounter.close();
    return nwaveformsprocessed;
}

  
/// Used to split up the amount of sub evetns being processed at one time using threads
void TUCNAnaViewer2::SubEvLoop(UCNRateHistogram * hPrev, UCNRateHistogram * hCurr, 
			       TTimeHelper tthelp[], int ich, int ichan, int iboard, 
			       unsigned long* banktime, TMidasEvent sample, int* loopstart, 
			       int* loopend)
{ 
  for(int isubev = *loopstart; isubev<*loopend; isubev++)
    {
      
      DPP_Bank_Out_t * b  = fDPP[iboard].GetPSD( isubev, ichan );
      
      if ( b==NULL) 
	return;
      
      // check current histogram to see if we increment in that histogram
      unsigned long timetag = tthelp[ich].GetTime_in_ticks( *banktime, b->TimeTag );
      
      int ibin = hCurr->GetBinForTime( timetag);
      if ( ibin == -1 ){ 
	// no, it is in previous (or earlier) histogram
	if ( hPrev != NULL ) {
	  // put in previous times... if exists
	  ibin = hPrev->GetBinForTime( timetag);
	}
	if ( ibin == -1 ){
	  std::cout << "Warning:: found time that is before previous histogram... waveform not counted" <<std::endl;
	  return; // go to next waveform
	} else {
	  // Increment in previous histogram
	  hPrev->Get()->AddBinContent( ibin );
	}
      } else if ( ibin == -numBins ) {
	// no, it is in time after current histogram, so:
	// need a new histogram for times later than current histogram!
	// 1) save any existing previous histogram
	if ( hPrev != NULL ) hPrev->Get()->Write();
	// 2) Current histogram becomes previous one
	hPrev = hCurr;
	// 3) Make a new current histogram
	hCurr = new UCNRateHistogram( hPrev->Get_tmax() );
	hCurr->SetTimeStamp( sample.GetTimeStamp() );
	ibin = hCurr->GetBinForTime( timetag);
	if ( ibin > 0 ){
	  // okay add an entry to the new histogram
	  hCurr->Get()->AddBinContent( ibin );
	  //std::cout<<"ibin: "<<ibin<<std::endl;
	} else {
	  std::cout<<"Warning:: ibin="<<ibin
		   <<" is either before or after prev and current histo"
		   <<std::endl;
	}
      } else {
	
	// yes, it is in current histogram:
	hCurr->Get()->AddBinContent( ibin );
      }
      
    }
}
