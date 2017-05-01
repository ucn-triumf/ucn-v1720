#include "TROOT.h"
#include "TStyle.h"
#include "TUCNAnaViewer2.h"
#include "TF1.h"
#include <cmath>

bool moved = true;

TUCNAnaViewer2::TUCNAnaViewer2(){

  gStyle->SetOptStat(11111111);

  verbose = 1;
  numBins = 3600;

  // one histogram for events over1 time
  fV1720TimeHistogram = new TH1D("v1720Time","Time of Events", numBins+1, 0.0, float(numBins));
  fV1720TimeHistogram->SetXTitle("Time (3600 - now in sec)");
  fV1720TimeHistogram->SetYTitle("Events (Hz)");
  fV1720TimeHistogram->SetDrawOption("e1");

  // fill histogram with 0 ?

  // initialize variables for calculating Hz
  time      = 0;  // time is the second interval of the largest time observed so far....
  nextTime = 0;
  prevTime = {0,0,0,0,
	      0,0,0,0,
	      0,0,0,0,
	      0,0,0,0};
  currTime = {0,0,0,0,
	      0,0,0,0,
	      0,0,0,0,
	      0,0,0,0};
  addedTime = {0,0,0,0,
	       0,0,0,0,
	       0,0,0,0,
	       0,0,0,0};



  //dtTime ={0};
  nEvents   = 0;
  nLoop = 0;
  loop = false;
  first = {true,true,true,true,
	   true,true,true,true,
	   true,true,true,true,
	   true,true,true,true};
  sec = 250000000;
  refChan = 14;//pick empty channel
}

  bool incr17thisev[NDPPBOARDS*PSD_MAXNCHAN] = 
    { false,false,false,false,
      false,false,false,false,
      false,false,false,false,
      false,false,false,false};

TUCNAnaViewer2::~TUCNAnaViewer2(){
}


// Analyze Data and Save to TTree
int TUCNAnaViewer2::FindAndFitPulses(TDataContainer& dataContainer){

  // Loop over all the data, fill the histograms and and find histograms.
  TMidasEvent sample = dataContainer.GetMidasEvent();
  if ( ! sample.IsBank32() ) return 0;

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
  char * pdata = sample.GetData();

  /// We have some new waveforms, make sure to clear
  /// the previous event info so it doesnt get
  /// used again in case this board is not in this event
  /// ClearWaves Added Aug.15,2015 ABJ
  for (iboard = 0; iboard<NDPPBOARDS; iboard++) {
    fDPP[iboard].ClearWaves();
  }
  
  // loop over the all data for one event and organize said data
  bool foundabank=false;
  for( sample.IterateBank32(&bank,&pdata); bank!=NULL&&pdata!=NULL;
       sample.IterateBank32(&bank,&pdata)  ) {
    
    sscanf(&bank->fName[3],"%1d", &iboard);
    if ( iboard<0 || iboard >= NDPPBOARDS){
      if(verbose)
	std::cout<<"<TUCNAnaViewer2> bank="<<iboard<<" but MAXBOARDS= "<<NDPPBOARDS<<std::endl;
      continue;
    } 
    if(verbose)
      std::cout<<"<TUCNAnaViewer2> board="<<iboard<<std::endl;
    
    foundabank=true;
    fDPP[iboard].Init( pdata );
    
  } // end looping through banks

  // if slow control event, not part of the event rate
  if ( foundabank == false ) return 0;

  // bool incr17thisev[NDPPBOARDS*PSD_MAXNCHAN] = 
  //{ false,false,false,false,
  //  false,false,false,false,
  //  false,false,false,false,
  //  false,false,false,false};

  // grab each subevent
  for (iboard = 0; iboard<NDPPBOARDS; iboard++) {
    for (ichan = 0; ichan < PSD_MAXNCHAN;ichan++) {
      int ich = iboard*8 + ichan;
      int NWaves = fDPP[iboard].GetNWaves(ichan);
      //std::cout<<"iboard="<<iboard<<" ichan="<<ichan<<" ich="<<ich
      //	       <<" NWaves="<<NWaves<<std::endl;
      for (isubev = 0;isubev<NWaves;isubev++) {	
	b  = fDPP[iboard].GetPSD( isubev, ichan );
	
	if ( b==NULL) 
	  continue;
	
	currTime[ich] = (ULong64_t)(b->TimeTag)+ addedTime[ich];
	//currTime[ich] += base[ich];
	if(first[ich]==true){
	  time = currTime[ich] + sec;
	  prevTime[ich] = currTime[ich];
	  first[ich]=false;
	}

	if (prevTime[ich]-currTime[ich]>=4294967000)
	  {
	    addedTime[ich] += 4294967295;
	    currTime[ich] += addedTime[ich];
	  }

	std::cout<<"time(variable) "<<time<<std::endl;
	std::cout<<"currTime "<<currTime[ich]<<std::endl;

	//	if (currTime[ich]<prevTime[ich]) incr17thisev[ich] = false;
	//	if ( currTime[ich] < prevTime[ich] && incr17thisev[ich] ==false) {
	  // std::cout<<"Add 17 second jump for ch="<<ich
	  //		   <<" base[ch]="<<base[ich]
	  //	   <<" curr="<<currTime[ich]
	  //	   <<" prev="<<prevTime[ich]
	  //	   <<std::endl;
	//  incr17thisev[ich] = true;
	// base[ich] += 4294967295;
	  // currTime[ich] += 4294967295;
	//  currTime[ich] = (ULong64_t)(b->TimeTag)+base[ich];
	  // }//
	//	std::cout<<" iboard="<<iboard
	//	 <<" ich="<<ich
	//	 <<" iev="<<isubev<<" / "<<NWaves
	//	 <<" curtime="<<currTime[ich]
	//	 <<" base="<<base[ich]
	//	 <<" prev="<<prevTime[ich]
	//	 <<" time="<<time
	//	 <<std::endl;


	
	// if event time is 1 s past current time, then update time and shift all bins back
	// and rescale the bin so that it is in Hz (in case all dt's are less than 1 sec)
	if(currTime[ich] > time) {
	  while (currTime[ich] > time ){
	    std::cout<<"Move bins... time="<<time<<" curTime["<<ich<<"]="<<currTime[ich]<<std::endl;
	    time += sec;
	    
	    for (int i =1; i<numBins+2; i++) {
	      fV1720TimeHistogram->SetBinContent(i,fV1720TimeHistogram->GetBinContent(i+1));
	      fV1720TimeHistogram->SetBinError(i,fV1720TimeHistogram->GetBinError(i+1));
	    }
	    //fV1720TimeHistogram->SetBinContent(numBins+1, 0.0);
	    //fV1720TimeHistogram->SetBinError(numBins+1, 0.0);
	  }
	  //first[ich] = true;
	  //fV1720TimeHistogram->Fill( 3599.5 - float( (int)(time-1-currTime[ich])/sec) );
	  moved = true;

	}
	
	//if ( ich==7 || ich==15)
	//  fV1720TimeHistogram->Fill(3599 -(int)( (time + sec - prevTime[ich])/sec));

	//if (1){
	// find bin that event belongs to relative to time
	//if (1){
	    // ||( b->ChargeLong - b->ChargeShort > -50.0 &&
	    // b->ChargeLong > 2000.0 && b->ChargeLong < 160000.0 &&
	    //		    b->ChargeShort > 500.0 && b->ChargeShort < 10000.0) ){
	  
	  //if ( ich == 12 ) {
	  //if((int)( (time + sec - prevTime[ich])/sec)>0)
	//std::cout<<"Fill bin "<< 3599.5 - float( (int)(time-1-currTime[ich])/sec) 
	//	 <<" time="<<time
	//	 <<" currTime["<<ich<<"]="<<currTime[ich]
	//	 <<std::endl;
	if (moved = true)
	  {
	    fV1720TimeHistogram->Fill( 3599.5 - float( (int)(time-1-currTime[ich])/sec) );
	    moved = false;
	  }   
	//else 
	    //fV1720TimeHistogram->Fill(3599);
	    //}
	    //	}
	prevTime[ich] = currTime[ich];
      } // end filling histogram
    }
  }

  return 1;  

} // end FindAndFitPulses


int TUCNAnaViewer2::ProcessMidasEvent(TDataContainer& dataContainer){

  // Find and fit V1720 pulses; this also fills the V1720 waveforms.
  int counter = FindAndFitPulses(dataContainer);
  
  return counter;
  
}
