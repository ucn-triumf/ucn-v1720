// ---------------------------------------------------------------
// Macro for finding the average rate of events 
// Macro averages for total and for each channel
// ---------------------------------------------------------------

#include "TROOT.h"
#include "TSystem.h"
#include "TVConstants.h"


#include "PBeamBunch.h"
#include "TUCNTimeWindows.h"
#include "TRateAnalysis.h"
#include "TTOFAnalysis.h"
#include "TTimeOffsets.h"
#include "TPulserAnalysis.h"

#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

const int npmts = 16; // actually just all channels.  only first nine in array are PMTs, rest are other signals
const int pbeamchannel = 10; // hard coded channel in which the proton beam signal is located
const int mod0_1Hzchan = 7;  // hard coded channel in which the 1Hz pulser is connected for digitizer Module 0
const int mod1_1Hzchan = 8;  // hard coded channel in which the 1Hz pulser is connected for digitizer Module 1
const int pmtchannels[npmts] = { 1, 2, 3, 0, 9, 4, 8, 6, 5, 7, 10, 11, 12, 13, 14, 15  }; // channel numbers with PMTs row by row 


// Function to figure out which beam bunch current time is in
PBeamBunch * FindBunch( int &curbunch, ULong64_t tTimeE, std::vector< PBeamBunch* > &pbunches ){
  int prevbunch=curbunch;

  //std::cout<<"initial curbunch="<<curbunch<<"of nbunches="<<pbunches.size()<<" time="<< tTimeE<<std::endl;
  for ( int ibunch=0; ibunch<pbunches.size(); ibunch++ ){
    int inbunch = pbunches.at(ibunch)->CheckTime( tTimeE );
    //std::cout<<" CheckTime for curbunch="<<curbunch<<" is inbunch="<<inbunch<<std::endl;
    if ( inbunch == 0 ) {
      curbunch = ibunch;
      return pbunches.at(curbunch); 
    }
  }

  // didn't find a bunch matching this time
  //std::cout<<"FindBunch failed for time="<<tTimeE<<" prevbunch="<<prevbunch<<std::endl;
  curbunch = prevbunch;
  return NULL;
}


void TWindow(){//char * outfilename="TWindow.root"){

  // Prepare time offests calibration
  TTimeOffsets * ttoff = TTimeOffsets::Get();


  // read in text file of data files to be read
  // find number of entries
  // read in text file of data files to be read
  ifstream fList;
  fList.open("twinforruns.txt");
  if ( fList.is_open() == false ){
    std::cout<<"Could not open file list : twinforruns.txt"<<std::endl;
    return;
  }

  ULong64_t eventTot;

  ULong64_t tTimeE;
  UShort_t tChannel;
  UShort_t tChargeL;
  UShort_t tChargeS;

  ULong64_t time1,time2; // first and last time in current file... in case needed
  ULong64_t base=0;

  // length of a second in ns
  ULong64_t sec = 1000000000;

  // keep a list of the input file names
  std::vector< std::string > infilenames;
  // keep a list of the input run numbers
  std::vector< int > inrunnums;

  // make a ponter for object to find the proton beam time windows
  TUCNTimeWindows * twfinder=NULL;

  TFile * fout=NULL;

  // first pass over the data will just look at the proton beam
  // signal, and count how many cycles there are, and get the length
  // of a cycle
  for (int i=0;!fList.eof();i++) {
    // Initialize variables for calculating rate per channel
    std::string name;
    int runnum;
    fList >> runnum >> name;
    //getline(fList,name);
    if(fList.eof())
      break;

    
    // make output file
    if (fout==NULL){
      char aname[100];
      sprintf(aname,"ucnana%06d.root",runnum);
      fout = new TFile(aname,"recreate");
    }

    infilenames.push_back( name );
    inrunnums.push_back( runnum );
    std::cout << name << std::endl;
    const char *infilename = name.c_str();
    TFile* fin = new TFile(infilename,"READ");
    if (fin == NULL) {
      std::cout<<"could not open file : "<<infilename<<std::endl;
      continue;
    } 
    if ( fin->IsOpen() == false ) {
      std::cout<<"file not open : "<<infilename<<std::endl;
      
      continue;
    }
    fin->ls();
    
    TTree* uin = (TTree*) fin->Get("tUCN");
    TTree* truntime = (TTree*) fin->Get("tRunTran");

    uin->Print();
    truntime->Print();

    fout->cd();

    if ( twfinder==NULL ) twfinder = new TUCNTimeWindows( runnum, uin, truntime );
    else twfinder->CalcTimes( runnum, uin, truntime );
  }

  // Finished first passes over data... print out proton beam bunch info
  twfinder->Print();

  // If no beam bunches were found return
  if ( twfinder->NBunches() <= 0 ) return;

  // build storage for the histograms
  std::vector< PBeamBunch* > pbunches;
  for (int ibunch=0; ibunch<twfinder->NBunches(); ibunch++){
    ULong64_t btmin = ULong64_t( twfinder->GetStartTime( ibunch ) / PBNSTOSEC );
    ULong64_t btmax = ULong64_t( twfinder->GetEndTime( ibunch ) / PBNSTOSEC );
    int tstart = twfinder->GetRunStartTime( ibunch );
    std::cout<<"Build PBeamBunch "<<ibunch<<" from "<<btmin<<" to "<<btmax
	     <<" tstart="<<tstart<<std::endl;
    pbunches.push_back( new PBeamBunch( ibunch, btmin, btmax, tstart ) );
    
  }

  // do another pass over the data to fill the histograms for each 
  // proton beam bunch
  int curbunch = 0;
  base=0;
  for (int i=0; i<infilenames.size(); i++) {
    std::string name = infilenames.at( i );
    int runnum = inrunnums.at(i);
    std::cout << runnum << " " << name << std::endl;
    const char *infilename = name.c_str();
    TFile* fin = new TFile(infilename,"READ");
    
    TTree* uin = (TTree*) fin->Get("tUCN");
    uin->SetBranchAddress("tTimeE",&tTimeE);
    uin->SetBranchAddress("tChannel",&tChannel);
    uin->SetBranchAddress("tChargeL",&tChargeL);
    uin->SetBranchAddress("tChargeS",&tChargeS);

    eventTot = (Double_t)uin->GetEntries();

    uin->GetEvent(0);
    time1 = tTimeE+base+TTimeOffsets::Get()->Offset( runnum, tChannel, tTimeE+base );
    uin->GetEvent(eventTot-1);
    time2 = tTimeE+base+TTimeOffsets::Get()->Offset( runnum, tChannel, tTimeE+base );    

    std::cout<<"Pass over data to fill histograms per proton beam bunches"<<std::endl;
    for(ULong64_t j=0;j<eventTot;j++) {
      uin->GetEvent(j);
      tTimeE+=base+TTimeOffsets::Get()->Offset( runnum, tChannel, tTimeE+base );
      if ( tChannel > npmts-1 ) continue;
      PBeamBunch * bunch = FindBunch( curbunch, tTimeE, pbunches );

      if (j%500000==0){ 
	std::cout<<"."<<std::flush;
      }
      //	int icb=0;
      //	for (int ib=0; ib<pbunches.size(); ib ++){
      //	  if ( pbunches[ib] == bunch ) {
      //	    icb = ib;
      //	    break;
      //	  }
      //	}
	//printf("(2nd pass) Load event % 20lldu of % 20lldu  t = % 20lldu curbunch=%d\n",j,eventTot,tTimeE,  icb );
      //}

      if (bunch != NULL)
	bunch->Fill( tChannel, tTimeE, tChargeS, tChargeL );

    }
    base += time2;
  }
  std::cout<<std::endl;
  // Now we can do some analysis on the results!
  // hell yeah!
  
  // Analysis 1 : Basic rate analysis
  //              Calculate and print rate for each beam bunch
  //              And print whether it is a BG bunch or not
  fout->cd();
  TRateAnalysis * tranny = new TRateAnalysis( twfinder, pbunches );
  tranny->Print();

  // Analysis 2: TOF Analysis, loop over files and fill TOF histogram
  TTOFAnalysis * tofan = NULL;
  for (int i=0; i<infilenames.size(); i++) {
    std::string name = infilenames.at( i );
    int runnum = inrunnums.at(i);
    std::cout << runnum << " " << name << std::endl;
    const char *infilename = name.c_str();
    TFile* fin = new TFile(infilename,"READ");
    
    TTree* uin = (TTree*) fin->Get("tUCN");
    fout->cd();
    if (tofan==NULL) tofan = new TTOFAnalysis( runnum, uin, twfinder );
    else tofan->CalcTOF( runnum, uin, twfinder );
  }

  // Analysis 3: Pulser Analysis, loop over files and fill Pulser histogram
  TPulserAnalysis * pulseran = NULL;
  for (int i=0; i<infilenames.size(); i++) {
    std::string name = infilenames.at( i );
    int runnum = inrunnums.at(i);
    std::cout << runnum << " " << name << std::endl;
    const char *infilename = name.c_str();
    TFile* fin = new TFile(infilename,"READ");
    
    TTree* uin = (TTree*) fin->Get("tUCN");
    fout->cd();
    if (pulseran==NULL) pulseran = new TPulserAnalysis( runnum, uin, twfinder );
    else pulseran->CalcPulser( runnum, uin, twfinder );
  }

  fout->cd();
  fout->Write();
  fout->Close();
  gROOT->cd();
  

}

int main( int argc, const char* argv[] ){
  gSystem->Load("libTree.so");


  TWindow();
  return 0;
}
