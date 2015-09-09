#include "TROOT.h"
#include "TStyle.h"
#include "TUCNAnaViewer.h"
#include "TF1.h"
#include <cmath>

TUCNAnaViewer::TUCNAnaViewer(){

  //fUCNEvent = NULL;


  verbose = 0;

  // event histograms for runtime window
  fV1720QSQLHistograms = new TV1720QSQLHistograms();
  fV1720QSQLHistograms->DisableAutoUpdate();

  fV1720PSDQLHistograms = new TV1720PSDQLHistograms();
  fV1720PSDQLHistograms->DisableAutoUpdate();


}

TUCNAnaViewer::~TUCNAnaViewer(){

}


// Analyze Data and Save to TTree
int TUCNAnaViewer::FindAndFitPulses(TDataContainer& dataContainer){

  // Loop over all the data, fill the histograms and and find histograms.
  TMidasEvent sample = dataContainer.GetMidasEvent();

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

  // loop over the all data for one event and organize said data
  for(sample.IterateBank32(&bank,&pdata);bank!=NULL&&pdata!=NULL;
      sample.IterateBank32(&bank,&pdata)) {
    
    sscanf(&bank->fName[3],"%1d", &iboard);
    if ( iboard<0 || iboard >= NDPPBOARDS){
      if(verbose)
	std::cout<<"<TUCNAnaViewer> bank="<<iboard<<" but MAXBOARDS= "<<NDPPBOARDS<<std::endl;
      continue;
    } 
    if(verbose)
      std::cout<<"<TUCNAnaViewer> board="<<iboard<<std::endl;
    
    fDPP[iboard].Init( pdata );
    
  } // end looping through banks

  // fill the histograms with the events
  nEvents = fDPP[0].GetNEvents();

  // grab each subevent
  for (iboard = 0; iboard<NDPPBOARDS; iboard++) {
    for (ichan = 0; ichan < PSD_MAXNCHAN;ichan++) {
      for (isubev = 0;isubev<nEvents;isubev++) {	

	b  = fDPP[iboard].GetPSD( isubev, ichan );
	//wf = fDPP[iboard].GetWaveform( isubev, ichan );

	if ( b==NULL) 
	  continue;

  	tChargeL  = b->ChargeLong;
  	tChargeS  = b->ChargeShort;
  	tPSD      = ((Float_t)(tChargeL)-(Float_t)(tChargeS))/((Float_t)(tChargeL));

	// waveform information
	//for (int i=0;i<tLength;i++) 
	//  tPulse[i] = wf[i];
	
	// fill histograms
	fV1720QSQLHistograms->UpdateHistogram(iboard, ichan, tChargeS, tChargeL);
	fV1720PSDQLHistograms->UpdateHistogram(iboard, ichan, tPSD, (Float_t)tChargeL);
      }
    }
  } // end filling tree

  return 1;  

} // end FindAndFitPulses


int TUCNAnaViewer::ProcessMidasEvent(TDataContainer& dataContainer){
  
  // Find and fit V1720 pulses; this also fills the V1720 waveforms.
  int counter = FindAndFitPulses(dataContainer);
  
  return counter;
  
}
