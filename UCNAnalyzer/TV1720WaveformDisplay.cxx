#include "TV1720WaveformDisplay.h"
#include "TH1D.h"
#include "TF1.h"

#include "TCanvas.h"
#include "TTree.h"

#include "TDirectory.h"

#include "time.h"




/// Reset the histograms for this canvas
TV1720QLQL::TV1720QLQL(){

  SetNumberChannelsInGroup(PSD_MAXNCHAN);
  SetGroupName("Module");
  SetChannelName("Channel");
    
  CreateHistograms();
}


void TV1720QLQL::CreateHistograms(){
  
  // check if we already have histograms.
  char tname[100];
  sprintf(tname,"V1720QLQL_%i_%i",0,0);
  
  TH2F *tmp = (TH2F*)gDirectory->Get(tname);
  if (tmp) return;

  // Otherwise make histograms
  clear();

  for(int iBoard=0; iBoard<NDPPBOARDS; iBoard++){// Loop over V1720 boards        
    for(int i = 0; i < PSD_MAXNCHAN; i++){ // loop over 8 channels

      char name[100];
      char title[100];
      sprintf(name,"V1720QLQL_%i_%i",iBoard,i);

      sprintf(title,"V1720 Gate Values for module = %i, channel=%i",iBoard,i);	

      TH2F *tmp = new TH2F(name,title,320,-20,40000,320,-20,30000);
      tmp->SetDrawOption("colz");
      tmp->SetXTitle("Q Long Calculated");
      tmp->SetYTitle("Q Long From Board");
      push_back(tmp);
    }
  }

}


/// Update the histograms for this canvas 
void TV1720QLQL::UpdateHistogram(int board, int chan, uint16_t QLCal[], uint16_t QLBoard[], int nEvents){

  int index = board*PSD_MAXNCHAN + chan;
  for (int i=0; i<nEvents; i++)
    {
      GetHistogram(index)->Fill(QLBoard[i],QLCal[i]);     
    }
}



/// Take actions at begin run
void TV1720QLQL::BeginRun(int transition,int run,int time){

  CreateHistograms();
}

/// Take actions at end run  
void TV1720QLQL::EndRun(int transition,int run,int time){

}


///////////////////////////////////////////////////////////
TV1720QSQS::TV1720QSQS(){

  SetNumberChannelsInGroup(PSD_MAXNCHAN);
  SetGroupName("Module");
  SetChannelName("Channel");
    
  CreateHistograms();
}


void TV1720QSQS::CreateHistograms(){
  
  // check if we already have histograms.
  char tname[100];
  sprintf(tname,"V1720QSQS_%i_%i",0,0);
  
  TH2F *tmp = (TH2F*)gDirectory->Get(tname);
  if (tmp) return;

  // Otherwise make histograms
  clear();

  for(int iBoard=0; iBoard<NDPPBOARDS; iBoard++){// Loop over V1720 boards        
    for(int i = 0; i < PSD_MAXNCHAN; i++){ // loop over 8 channels

      char name[100];
      char title[100];
      sprintf(name,"V1720QSQS_%i_%i",iBoard,i);

      sprintf(title,"V1720 Gate Values for module = %i, channel=%i",iBoard,i);	

      TH2F *tmp = new TH2F(name,title,320,-20,40000,320,-20,30000);
      tmp->SetDrawOption("colz");
      tmp->SetXTitle("Q Short Calculated");
      tmp->SetYTitle("Q Short From Board");
      push_back(tmp);
    }
  }

}


/// Update the histograms for this canvas 
void TV1720QSQS::UpdateHistogram(int board, int chan, uint16_t QSCal[], uint16_t QSBoard[], int nEvents){

  int index = board*PSD_MAXNCHAN + chan;
  for (int i=0; i<nEvents; i++)
    {
      GetHistogram(index)->Fill(QSBoard[i],QSCal[i]);     
    }
}



/// Take actions at begin run
void TV1720QSQS::BeginRun(int transition,int run,int time){

  CreateHistograms();
}

/// Take actions at end run  
void TV1720QSQS::EndRun(int transition,int run,int time){

}





TV1720Waveform::TV1720Waveform(){

  SetNumberChannelsInGroup(PSD_MAXNCHAN);
  SetGroupName("Module");
  SetChannelName("Channel");
    
  CreateHistograms();
}


void TV1720Waveform::CreateHistograms(){

  // check if we already have histogramss.
  char tname[100];
  sprintf(tname,"V1720_%i",0);

  TH1D *tmp = (TH1D*)gDirectory->Get(tname);
  if (tmp) return;

  //int fWFLength = 1000; // Need a better way of detecting this...
  //int numSamples = fWFLength / nanosecsPerSample;

  //Otherwise make histograms
  clear();


  for(int iBoard=0; iBoard<NDPPBOARDS; iBoard++){
	for(int i = 0; i < PSD_MAXNCHAN; i++){ // loop over 8 channels		
		char name[100];
		char title[100];
		sprintf(name,"V1720_%i_%i",i,iBoard);

		sprintf(title,"V1720 Waveform for channel=%i Board=%i",i,iBoard);	
		
		TH1D *tmp = new TH1D(name, title, 200, 0, 200);
		tmp->SetXTitle("ns");
		tmp->SetYTitle("ADC value");
		
		push_back(tmp);
	}
  }
}


/// Update the histograms for this canvas 
void TV1720Waveform::UpdateHistogram(int board, int chan, uint16_t * wf, int tLength,  char * ctag){

  int index = board*PSD_MAXNCHAN + chan;

  for (int b = 0; b<tLength; b++){
    //std::cout<<*wf<<std::endl;
    GetHistogram(index)->SetBinContent(b,*(wf+b));
    //GetHistogram(index)->Fill(b);
  }
  for (int i = 0; i<tLength; i++)
    {
      GetHistogram(index)->Fill(i);
    }
  GetHistogram(index)->SetTitle(ctag);

}



/// Take actions at begin run
void TV1720Waveform::BeginRun(int transition,int run,int time){

  CreateHistograms();
}

/// Take actions at end run  
void TV1720Waveform::EndRun(int transition,int run,int time){

}







//////////////////////////////////////////////////////////////////
/*TV1720CSQEvNum::TV1720CSQEvNum(){

  SetNumberChannelsInGroup(PSD_MAXNCHAN);
  SetGroupName("Module");
  SetChannelName("Channel");
    
  CreateHistograms();
}


void TV1720CSQEvNum::CreateHistograms(){
  
  // check if we already have histograms.
  char tname[100];
  sprintf(tname,"V1720QLQL_%i_%i",0,0);
  
  TH2F *tmp = (TH2F*)gDirectory->Get(tname);
  if (tmp) return;

  // Otherwise make histograms
  clear();

  for(int iBoard=0; iBoard<NDPPBOARDS; iBoard++){// Loop over V1720 boards        
    for(int i = 0; i < PSD_MAXNCHAN; i++){ // loop over 8 channels

      char name[100];
      char title[100];
      sprintf(name,"V1720QSQS_%i_%i",iBoard,i);

      sprintf(title,"V1720 Gate Values for module = %i, channel=%i",iBoard,i);	

      TH1D *tmp = new TH1D(name,title,320,-20,40000,320,-20,30000);
      tmp->SetDrawOption("colz");
      tmp->SetXTitle("Event Number");
      tmp->SetYTitle("ChargeSBoard/ChargeSCal");
      push_back(tmp);
    }
  }

}


/// Update the histograms for this canvas 
void TV1720CSQEvNum::UpdateHistogram(int board, int chan, float QSDifference[], int nEvents, char * ctag){

  int index = board*PSD_MAXNCHAN + chan;
  for (int i=1; i<nEvents; i++)
    {
      GetHistogram(index)->Fill(QSBoard[i]);     
    }
}



/// Take actions at begin run
void TV1720CSQEvNum::BeginRun(int transition,int run,int time){

  CreateHistograms();
}

/// Take actions at end run  
void TV1720CSQEvNum::EndRun(int transition,int run,int time){

}*/
