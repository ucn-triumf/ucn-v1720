#include "TROOT.h"
#include "TStyle.h"
#include "TUCNAnaManager.h"
#include "TF1.h"
#include <cmath>
#include <fstream>
#include <iostream>

TUCNAnaManager::TUCNAnaManager(int run, int time){

  //fUCNEvent = NULL;
  gStyle->SetOptFit(1111);

  // tree for run transitions
  tRunTran = new TTree("tRunTran", "RunTransitions");
  tRunTran->Branch("tRunNum", &tRunNum, "tRunNum/I" );
  tRunTran->Branch("tTime", &tTime, "tTime/I" );
  tRunTran->Branch("tEntry",&tEntry, "tEntry/l" );

  tRunNum = run;
  tTime = time;
  tEntry=0;
  
  // tree for runtime event data
  tUCN=new TTree("tUCN","Event");
  tUCN->Branch("tEntry",    &tEntry,    "tEntry/l");
  tUCN->Branch("tTimeE",    &tTimeE,    "tTimeE/l");
  tUCN->Branch("tTimeStamp",    &tTimeStamp,    "tTimeStamp/l");
  tUCN->Branch("tChannel",  &tChannel,  "tChannel/s");
  tUCN->Branch("tPSD",      &tPSD,      "tPSD/f");
  tUCN->Branch("tChargeL",  &tChargeL,  "tChargeL/s");
  tUCN->Branch("tChargeS",  &tChargeS,  "tChargeS/s");
  tUCN->Branch("tBaseline", &tBaseline, "tBaseline/s");
  tUCN->Branch("tLength",   &tLength,   "tLength/s");
  tUCN->Branch("tPulse",     tPulse,    "tPulse[tLength]/s");


  verbose = 1;
  for (int i = 0; i < 16; i++)
    {
      prevEvent[i] = 0;
      currEvent[i] = 0;  //currEvent = {0}; //May 19, 2017 by A. E. Sikora
      base[i]      = 0;  //base      = {0};
      //tSVolt     = {0};
      tMVolt[i]    = 0;  //tMVolt    = {0};
      tMCurr[i]    = 0;  //tMCurr    = {0};
    }
  //tChStat   = {0};
  //tMTemp    = {0};
    

  subTotEvent = 0;
  subTotEventH = 0;
  nEvents = 0;
  //protonPulse == 10;
  //refChan = (int)ichan/PSD_MAXNCHAN;
  //
  //refChan = 12;
  refChan=14;//unused one
}

TUCNAnaManager::~TUCNAnaManager(){

  //delete fPulseFinder;
  //delete fPulseFitter;

}

void TUCNAnaManager::EndRun(int run, int time){
  //tRunNum = run;
  //tTime = time;
  tRunTran->Fill();
}

#include <sys/time.h>

long int total_pulse_cumul = 0;
uint32_t LastTime;  

// Analyze Data and Save to TTree
int TUCNAnaManager::FindAndFitPulses(TDataContainer& dataContainer){

  // Loop over all the data, fill the histograms and and find histograms.
  TMidasEvent sample = dataContainer.GetMidasEvent();
  //int eventID;

  

  // output bank block information
  if(verbose) {
    printf("time: %d\n",sample.GetTimeStamp());
    printf("event id: %d\n",sample.GetEventId());
    printf("trigger mask: %d\n",sample.GetTriggerMask());
    printf("serial number: %d\n",sample.GetSerialNumber());
    printf("time: %d\n",sample.GetTimeStamp());
    printf("data size: %d\n",sample.GetDataSize());
    
    // got event, now to iterate through the banks
    printf("  bank list:\n");
    printf(sample.GetBankList());
    printf("\n");

  }

  if ( strstr( sample.GetBankList(), "W2\0" ) == NULL ) {
    printf("Bank list did not contain v1720 digitizer bank\n");
    return 0;
  }


  // ignore event id's that we don't know about:
  std::cout<<"Event id: "<<sample.GetEventId()<<std::endl;;
  //if ( sample.GetEventId() != 1 ) return 0;

  TMidas_BANK32 * bank = NULL;
  TMidas_BANK *bank2 = NULL;
  char * pdata = sample.GetData();
  // subTotEvent = 0;
  // subTotEventH = 0;
  // nEvents = 0;
  // loop over the all data for one event and organize said data
  if(sample.IsBank32()) {

    /// We have some new waveforms, make sure to clear
    /// the previous event info so it doesnt get
    /// used again in case this board is not in this event
    /// ClearWaves Added Aug.14,2015
    for (iboard = 0; iboard<NDPPBOARDS; iboard++) {
      fDPP[iboard].ClearWaves();
    }
    
    for(sample.IterateBank32(&bank,&pdata);bank!=NULL&&pdata!=NULL;
	sample.IterateBank32(&bank,&pdata)) {

      // trigger event
      sscanf(&bank->fName[3],"%1d", &iboard);
      if ( iboard<0 || iboard >= NDPPBOARDS){
	if(verbose)
	  std::cout<<"<TUCNAnaManager> bank="<<iboard
		   <<" but MAXBOARDS= "<<NDPPBOARDS<<std::endl;
	continue;
      } 
      if(verbose)
	std::cout<<"<TUCNAnaManager> board="<<iboard<<std::endl;
      
      fDPP[iboard].Init( pdata );

      ///********************************************************
      ///******************************************************
      ///********************************************************
      ///******************************************************
      /// NOTE TO SELF! Aug. 14, 2015
      /// MAYBE THIS FOR LOOP SHOULD NOT END.  INSTEAD
      /// OF LOOPING OVER iboard GET RID OF THE END OF THIS 
      /// LOOP.
      ///
      /// ie. WHAT HAPPENS IF ONLY FIND ONE OF iboard==0 or 1
      /// THEN WE TRY TO LOOP OVER BOTH BOARDS NEXT, AND WHAT
      /// IS LEFT IN THE STRUTURE FOR THE BOARD THAT WAS NOT READ?
      ///********************************************************
      ///******************************************************
      ///********************************************************
      ///******************************************************


      ///

    } // end looping through banks
    
    // grab each subevent
    //EventCounter.open("EventCounter.txt");

    int haveCh[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    long int total_pulses = 0;
    for (iboard = 0; iboard<NDPPBOARDS; iboard++) {
      for (ichan = 0; ichan < PSD_MAXNCHAN;ichan++) {
	for (isubev = 0;isubev< fDPP[iboard].GetNWaves(ichan) ;isubev++) {
	  b  = fDPP[iboard].GetPSD( isubev, ichan );
	  
	  if ( b==NULL) {
	    if(verbose)
	      std::cout<<"NULL PSD Info board="<<iboard<<" ch="<<ichan
		       <<" ev="<<isubev<<std::endl;  
	    continue;
	  }
	  total_pulses++;
	  //continue;
	  // totalPulses++;
	  //if (ichan==valveChan && valveOpen == 0)
	  // valveOpen = 1;
	  //else if (ichan == valveChan && valveOpen == 1)
	  // valveOpen = 0;
	  //if (valveOpen == 1)
	  //  valveOpenEvents++;
	  //if (ichan == valveChan)
	  //  EventCounter<<totalPulses<<"\t"<<valveOpenEvents<<"\n";
	  
	  // event information
	  tEvent    = isubev;
	  tChannel  = iboard * 8 + ichan;
	  // account for time loop
	  currEvent[tChannel] = (ULong64_t)(b->TimeTag);
	  if((currEvent[tChannel]+base[tChannel])<prevEvent[tChannel])
	    base[tChannel] += 4294967295;

	  if(tChannel < 16)
	    haveCh[tChannel] = 1;
	  
	  // check if still behind synchronization clock and adjust
	  if ((currEvent[ichan]+base[ichan])<currEvent[refChan]+base[refChan]){
	    while ((currEvent[ichan]+base[ichan])<currEvent[refChan]+base[refChan]){
	      base[ichan] += 4294967295;
	    }
	  }
	  prevEvent[tChannel] = currEvent[tChannel] + base[tChannel];
	  // time stamps are in 4ns intervals from start of run
	  tTimeE = prevEvent[tChannel]*4;
	  tTimeStamp = sample.GetTimeStamp();
	  tChargeL  = b->ChargeLong;
	  tChargeS  = b->ChargeShort;
	  tPSD      = ((Float_t)(tChargeL)-(Float_t)(tChargeS))/((Float_t)(tChargeL));
	  tBaseline = b->Baseline;
	  tLength   = b->Length;

	  bzero( tPulse, 1000 );
	  if ( tLength > 0 ){
	    wf = fDPP[iboard].GetWaveform( isubev, ichan );
	    memcpy( tPulse, wf, tLength*sizeof(uint16_t) );
	  }
	  
	  // fill event tree
	  tUCN->Fill();
	  tTime = sample.GetTimeStamp();
	  if (tEntry==0) tRunTran->Fill();
	     
	  tEntry++;
	  subTotEvent++;
	  subTotEventH++;
	}
      }
    } // end filling event tree tUCN
    //EventCounter<<totalPulses<<"\t"<<valveOpenEvents<<"\n";
    //EventCounter.close();


    static bool myfirsttime = true;
    if(myfirsttime){
      LastTime = sample.GetTimeStamp();
      myfirsttime = false;
    }

    total_pulse_cumul += 	total_pulses;			
    double dtime = (double)( sample.GetTimeStamp() - LastTime);

    if(dtime > 10.0){
      std::cout << "Total pulse rate " << ((double) total_pulse_cumul) / ((double)dtime *1000.0) << " kHz" <<
	" " << total_pulse_cumul << " " << dtime << std::endl;
      total_pulse_cumul = 0;
      LastTime = sample.GetTimeStamp();


    }


    
    
  } 
  return 1;  

} // end FindAndFitPulses


int TUCNAnaManager::ProcessMidasEvent(TDataContainer& dataContainer){
  
  // Find and fit V1720 pulses; this also fills the V1720 waveforms.
  int counter = FindAndFitPulses(dataContainer);
  
  return counter;
  
}
