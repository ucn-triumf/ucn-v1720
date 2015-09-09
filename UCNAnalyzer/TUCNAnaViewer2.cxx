#include "TROOT.h"
#include "TStyle.h"
#include "TUCNAnaViewer2.h"
#include "TF1.h"
#include <cmath>

TUCNAnaViewer2::TUCNAnaViewer2(){

  gStyle->SetOptStat(11111111);

  verbose = 0;
  numBins = 3600;

  // one histogram for events over1 time
  fV1720TimeHistogram = new TH1D("v1720Time","Time of Events", numBins+1, 0, numBins);
  fV1720TimeHistogram->SetXTitle("Time (1hr - now in sec)");
  fV1720TimeHistogram->SetYTitle("Events (Hz)");
  fV1720TimeHistogram->SetDrawOption("e1");

  // fill histogram with 0 ?

  // initialize variables for calculating Hz
  time      = 0;
  nextTime = 0;
  prevTime = {0};
  currTime = {0};
  base      = {0};
  //dtTime ={0};
  nEvents   = 0;
  nLoop = 0;
  loop = false;
  first = true;
  sec = 250000000;
  sync = {0};
  refChan = 14;//pick empty channel
}

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

  // grab each subevent
  for (iboard = 0; iboard<NDPPBOARDS; iboard++) {
    for (ichan = 0; ichan < PSD_MAXNCHAN;ichan++) {
      int ich = iboard*8 + ichan;
      
      for (isubev = 0;isubev<fDPP[iboard].GetNWaves(ichan);isubev++) {	
	b  = fDPP[iboard].GetPSD( isubev, ichan );
	
	if ( b==NULL) 
	  continue;
	
	// account for time loop
	currTime[ich] = (ULong64_t)(b->TimeTag);
	if ((currTime[ich]+base[ich])<prevTime[ich])
	  base[ich] += 4294967295;


	// check if still behind synchronization clock and adjust
	//if (iboard==0) refChan=7;
	//else refChan=15;
	if ((currTime[ichan]+base[ichan])<currTime[refChan]+base[refChan]){
	  while ((currTime[ichan]+base[ichan])<currTime[refChan]+base[refChan]){
	    base[ichan] += 4294967295;
	  }
	}

	// check if still behind synchronization clock and adjust
	//if (ich != 7 && ich != 15){
	// if ((currTime[ich]+base[ich])<prevTime[refChan]){
	//  while ((currTime[ich]+base[ich])<prevTime[refChan]){
	//    base[ich] += 4294967295;
	//  }
	// }
	//}
	prevTime[ich] = currTime[ich] + base[ich];
	
	// if first event to be read, set time
	if(first) {
	  time = prevTime[ich];
	  first = false;
	}
	
	// if event time is 1 s past current time, then update time and shift all bins back
	// and rescale the bin so that it is in Hz (in case all dt's are less than 1 sec)
	if(prevTime[ich] > time + sec) {
	  while (prevTime[ich]> time +sec ){
	    time += sec;
	    
	    for (int i =1; i<numBins+1; i++) {
	      fV1720TimeHistogram->SetBinContent(i,fV1720TimeHistogram->GetBinContent(i+1));
	      fV1720TimeHistogram->SetBinError(i,fV1720TimeHistogram->GetBinError(i+1));
	    }
	    //fV1720TimeHistogram->SetBinContent(numBins, 0.0);
	    //fV1720TimeHistogram->SetBinError(numBins, 0.0);
	  }
	}
	
	//if ( ich==7 || ich==15)
	//  fV1720TimeHistogram->Fill(3599 -(int)( (time + sec - prevTime[ich])/sec));

	if (1){
	// find bin that event belongs to relative to time
	if ( b->ChargeLong - b->ChargeShort > -50.0 &&
	     b->ChargeLong > 2000.0 && b->ChargeLong < 160000.0 &&
	     b->ChargeShort > 500.0 && b->ChargeShort < 10000.0 ){
	  
	  //if ( ich == 12 ) {
	  //if((int)( (time + sec - prevTime[ich])/sec)>0)
	    fV1720TimeHistogram->Fill(3599 -(int)( (time + sec - prevTime[ich])/sec));
	    //else 
	    //fV1720TimeHistogram->Fill(3599);
	}
	}
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
