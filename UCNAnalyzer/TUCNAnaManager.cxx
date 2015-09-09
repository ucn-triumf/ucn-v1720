#include "TROOT.h"
#include "TStyle.h"
#include "TUCNAnaManager.h"
#include "TF1.h"
#include <cmath>

TUCNAnaManager::TUCNAnaManager(){

  //fUCNEvent = NULL;
  gStyle->SetOptFit(1111);

  // tree for runtime event data
  tUCN=new TTree("tUCN","Event");
  tUCN->Branch("tEntry",    &tEntry,    "tEntry/l");
  tUCN->Branch("tTimeE",    &tTimeE,    "tTimeE/l");
  tUCN->Branch("tChannel",  &tChannel,  "tChannel/s");
  tUCN->Branch("tPSD",      &tPSD,      "tPSD/f");
  tUCN->Branch("tChargeL",  &tChargeL,  "tChargeL/s");
  tUCN->Branch("tChargeS",  &tChargeS,  "tChargeS/s");
  tUCN->Branch("tBaseline", &tBaseline, "tBaseline/s");
  tUCN->Branch("tLength",   &tLength,   "tLength/s");
  // tUCN->Branch("tPulse",     tPulse,    "tPulse[tLength]/s");
  tEntry=0;

  // tree for slow control data
  tSlow = new TTree("tSlow", "Slow");
  tSlow->Branch("tTemp1",   &tTemp1,   "tTemp1/f");
  tSlow->Branch("tTemp2",   &tTemp2,   "tTemp2/f");
  tSlow->Branch("tPress",   &tPress,   "tPress/f");
  //tSlow->Branch("tHV",      &tHV,      "tHV/f");
  tSlow->Branch("tTimeS",   &tTimeS,   "tTimeS/l");
  tSlow->Branch("tPSDIndex",&tPSDIndex,"tPSDIndex/l");


  // tree for HV monitor data
  tHV = new TTree("tHV","High Voltage Monitor");
  //tHV->Branch("tSVolt", tSVolt, "tSVolt[24]/f");  
  tHV->Branch("tMVolt", tMVolt, "tMVolt[24]/f");  
  tHV->Branch("tMCurr", tMCurr, "tMCurr[24]/f");  
  //tHV->Branch("tChStat",tChStat,"tChStat[24]/O");  
  //tHV->Branch("tMTemp", tMTemp, "tMTemp[24]/f");
  tHV->Branch("tTimeH", &tTimeH, "tTimeH/l");
  tHV->Branch("tEventH",&tEventH,"tEventH/l");
  //hvEvent = new HVEvent();
  //tHV->Branch("HVEvent",&hvEvent,160000,0);

  verbose = 0;

  prevEvent = {0};
  currEvent = {0};
  base      = {0};
  //tSVolt    = {0};
  tMVolt    = {0};
  tMCurr    = {0};
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


// Analyze Data and Save to TTree
int TUCNAnaManager::FindAndFitPulses(TDataContainer& dataContainer){

  // Loop over all the data, fill the histograms and and find histograms.
  TMidasEvent sample = dataContainer.GetMidasEvent();
  int eventID;

  // output bank block information
  if(verbose) {
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
    for (iboard = 0; iboard<NDPPBOARDS; iboard++) {
      for (ichan = 0; ichan < PSD_MAXNCHAN;ichan++) {
	for (isubev = 0;isubev< fDPP[iboard].GetNWaves(ichan) ;isubev++) {
	  b  = fDPP[iboard].GetPSD( isubev, ichan );
	  //wf = fDPP[iboard].GetWaveform( isubev, ichan );
	  
	  if ( b==NULL) {
	    if(verbose)
	      std::cout<<"NULL PSD Info board="<<iboard<<" ch="<<ichan
		       <<" ev="<<isubev<<std::endl;  
	    continue;
	  }
	  
	  
	  // event information
	  tEvent    = isubev;
	  tChannel  = iboard * 8 + ichan;
	  // account for time loop
	  currEvent[tChannel] = (ULong64_t)(b->TimeTag);
	  if((currEvent[tChannel]+base[tChannel])<prevEvent[tChannel])
	    base[tChannel] += 4294967295;

	  // check if still behind synchronization clock and adjust
	  if ((currEvent[ichan]+base[ichan])<currEvent[refChan]+base[refChan]){
	    while ((currEvent[ichan]+base[ichan])<currEvent[refChan]+base[refChan]){
	      base[ichan] += 4294967295;
	    }
	  }
	  prevEvent[tChannel] = currEvent[tChannel] + base[tChannel];
	  // time stamps are in 4ns intervals from start of run
	  tTimeE = prevEvent[tChannel]*4;
	  tChargeL  = b->ChargeLong;
	  tChargeS  = b->ChargeShort;
	  tPSD      = ((Float_t)(tChargeL)-(Float_t)(tChargeS))/((Float_t)(tChargeL));
	  tBaseline = b->Baseline;
	  tLength   = b->Length;
	  
	  // waveform information
	  //for (int i=0;i<tLength;i++) 
	  //  tPulse[i] = wf[i];
	  
	  // fill event tree
	  tUCN->Fill();
	  tEntry++;
	  subTotEvent++;
	  subTotEventH++;
	}
      }
    } // end filling event tree tUCN

    // check for HV monitor event
  } else if (sample.GetEventId() == 0x001e) {
    
    sample.Print();
    void *ptr;
    int size;

    // time stamp of read
    tTimeH = sample.GetTimeStamp();
    //hvEvent->SetTime((ULong_t)tTimeH);
    // // set voltage per channel
    // size = sample.LocateBank(NULL,"DMND",&ptr);
    // if (ptr) {
    //   cout<<"Set Voltage"<<endl;

    //   p = (uint16_t*)ptr;
    //   for (int i=0;i<tLength;i++) {
    // 	tSVolt[i] = ((float*)p)[i];
    // 	if(verbose)
    // 	  cout<<p[i]<<" ";
    //   }
    //   if(verbose)
    // 	cout<<endl;

    //   cout<<((float*)p)[0];
    // }

    // Measured voltage per channel
    size = sample.LocateBank(NULL,"MSRD",&ptr);
    if (ptr) {
      
      cout<<"Measured Voltage"<<endl;

      p = (uint16_t*)ptr;
      ps = (float*)ptr;
      //std::copy(ps,ps+(sizeof(ps)/sizeof(*ps)), tMVolt);
      //hvEvent->SetVolt(ps);
      for (int i=0;i<HVCHANNELS;i++) {
	tMVolt[i] = ps[i];
      //if(verbose)
        //cout<<tMVolt[i] << " " << ps[i]<<" ";
      //}
      //if(verbose)
      }
      //cout<<endl;
    }

    // Measured current
    size = sample.LocateBank(NULL,"CRNT",&ptr);
    if(ptr) {
 
      //
      p = (uint16_t*)ptr;
      ps = (float*)ptr;
      std::copy(ps,ps+(sizeof(ps)/sizeof(*ps)), tMCurr);
      //hvEvent->SetCurr(ps);
      for (int i=0;i<tLength;i++) 
	tMCurr[i] = ps[i];
      
      //
      //if(verbose) {
      //cout<<"Measured Current"<<endl;
      //for (int i=0;i<tLength;i++)
      //  cout<<ps[i]<<" "<<tMCurr[i]<<" ";
      //cout<<endl;
	//}
    }

    // // Channel status
    // size = sample.LocateBank(NULL,"STAT",&ptr);
    // if(ptr) {
      
    //   //      
    //   p = (uint16_t*)ptr;
    //   for (int i=0;i<tLength;i++) 
    // 	tChStat[i] = ((float*)p)[i];

    //   //
    //   if(verbose) {
    // 	cout<<"Channel Status"<<endl;
    // 	for (int i=0;i<tLength;i++) 
    // 	  cout<<((float*)p)[i]<<" ";
    // 	cout<<endl;
    //   }
    // }

    // // Temperature of each channel
    // size = sample.LocateBank(NULL,"TPTR",&ptr);
    // if(ptr) {
      
    //   // Fill array with temperature measurements
    //   p = (uint16_t*)ptr;
    //   float *ps = (float*)ptr;
    //   // for (int i=0;i<tLength;i++) 
    //   // 	tMTemp[i] = ps[i];
	
    //   // if verbose, output the measured temperature
    //   //if(verbose) {
    // 	cout<<"Temperature"<<endl;
    // 	std::copy(ps,ps+(sizeof(ps)/sizeof(*ps)), tMTemp);
    // 	for (int i=0;i<tLength;i++)
    // 	  cout<<tMTemp[i]<<" ";
    //   	cout<<endl;
    // 	// }

    // }

    // number of events between reads
    tEventH = subTotEventH;
    //hvEvent->SetEvent(subTotEventH);

    //
    tHV->Fill();

    // reset subtotal counter
    subTotEventH = 0;

    // if reaches here, slow control event
  } else {
    // if smaller bank, then slow control data
    sample.IterateBank(&bank2,&pdata);
    tTimeS  = (ULong_t)sample.GetTimeStamp(); // in unix time
    tTemp1  = ((float*)pdata)[0];
    tPress  = ((float*)pdata)[1];
    tTemp2  = ((float*)pdata)[2];
    //tHV     = ((float*)pdata)[3];
    tPSDIndex = subTotEvent;//tEntry;
    tSlow->Fill();

    // reset subtotal counter
    subTotEvent = 0;
  }
  return 1;  

} // end FindAndFitPulses


int TUCNAnaManager::ProcessMidasEvent(TDataContainer& dataContainer){
  
  // Find and fit V1720 pulses; this also fills the V1720 waveforms.
  int counter = FindAndFitPulses(dataContainer);
  
  return counter;
  
}
