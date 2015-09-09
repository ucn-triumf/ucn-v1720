// script for finding crosstalk between all nine channels
#include <vector>
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include <stdio.h>
#include <iostream>
#include "TStyle.h"
#include "TROOT.h"
#include <cmath>

// output00000083.root -> neutron run
// output00000094.root -> alpha run
void Crosstalk (char*infilename = "output00000103.root", int refChan = 8) {

  gStyle->SetOptFit(1111);

  const int NCH=9; // number of channels

  // set up file to read
  TFile* fin = new TFile(infilename,"read");
  TTree* tin = (TTree*) fin->Get("tUCN");

  ULong64_t tTimeE;
  ULong64_t div=4;  // should be a multiple of 4
  Long64_t delay = 1000;
  UShort_t tChannel;
  UShort_t tChargeL;
  UShort_t tChargeS;
  Float_t tPSD;
  ULong64_t *countE = new ULong64_t[NCH];
  ULong64_t *countC = new ULong64_t[NCH];
  for (int i=0;i<NCH;i++) {
    countE[i] = 0;
    countC[i] = 0;
  }

  tin->SetBranchAddress("tTimeE",&tTimeE);
  tin->SetBranchAddress("tChannel",&tChannel);
  tin->SetBranchAddress("tChargeL",&tChargeL);
  tin->SetBranchAddress("tChargeS",&tChargeS);
  tin->SetBranchAddress("tPSD",&tPSD);

  // find histogram range
  tin->GetEvent(0);
  ULong64_t t0 = tTimeE;                          // first time in file
  std::cout <<"First time in file="<< t0 << " ";
  Long64_t n = tin->GetEntries();                // number of entries in file
  tin->GetEvent(n-1);
  const ULong64_t t1 = tTimeE;                   // last time in file
  ULong64_t t =  5000000000/2;                     // buffer size in ns
  //             2147483647
  std::cout <<" Time at end of first buffer="<< t0+t << " Last time in file="<<t1<<std::endl;
  //ULong64_t t = 10*60*1000000000;
  //const ULong64_t t1 = t0 + t;

  // get time stamps
  //TCanvas *tc=new TCanvas();
  //tc->cd();

 // initialize 1D histograms
  TCanvas *tc1=new TCanvas();
  tc1->Divide(3,3);
  TH1D *hTime[NCH];
  char aname[100];
  char anum[100];
  for (int y=0;y<NCH;y++) {
    
    if (y!=refChan) {
      sprintf(aname,"Ch%d;Time Difference (ns);Count",y);
      hTime[y] = new TH1D(aname,aname,2*delay/div+1,-delay,delay);
    }
    else {
      sprintf(aname,"Reference Ch%d;Time Difference (ns);Count",y);
      hTime[y] = new TH1D(aname,aname,2*delay/div+1,-delay,delay);
    }
  } // end for

  // find 2d histogram of channel vs event time and project 
  // channel slices onto time axis
  std::cout << "Begin fill 2d hist" << std::endl;
  //ULong64_t tBins = (t0 + t1)/div + 1;
  ULong64_t tBins = t/div + 1;                       // number of time bins (4ns wide)
  std::cout <<"Number of time bins is = "<< tBins << " as int "<< (Int_t)tBins<< std::endl;
  //TH2I *hCross = new TH2I ("hCross","hCross",NCH,0,NCH,
  //tBins, t0,t0+t);

  int** iCross = new int*[NCH];
  for (int i=0;i<NCH;i++)
    iCross[i] = new int[tBins];

  // read data file
  ULong64_t beginTime = t0;
  ULong64_t beginEvent = 0;
  ULong64_t nextTime = t0 + t;
  ULong64_t nextBeginEvent = 0;
  tin->GetEvent(0);
  
  ULong64_t frac_to_read = 1; // 1/frac_to_read is fraction of file to read in
  ULong64_t numbufreads = (t1-t0)/t + 1; // number of buffers to read in full file

  ULong64_t maxbufsize = 16*1024*10*2; // maximum size of a buffer read from the digitizer

  ULong64_t ntCount = 0;
  int lap = 0;

  for(ULong64_t lp=0;lp<=numbufreads/frac_to_read;lp++) {

    std::cout << "data portion " << lp << " of " << numbufreads
	      << ", percent file read " << float(lp)/float(numbufreads) * 100.0 << "%"<< std::endl;
    // loop through file and save events to 2d hist
    // stop looping after we are maxbufsize waveforms after seeing
    // a time later than our buffer time
    for (ULong64_t k=beginEvent; ntCount < maxbufsize && k<n;k++) {
      //std::cout << "*"<<k;
      // get event
      tin->GetEvent(k);

      if (tChannel >= NCH || tChannel < 0)
	continue;

      if(tTimeE > nextTime + t * maxbufsize) {
	std::cout << "lapped at " << tTimeE << " from " << nextTime << std::endl;
	lap++;
      }

      // check if we found first event with time > nextTime
      if ( tTimeE > nextTime || ntCount > 0 ) {
	if (ntCount == 0) nextBeginEvent = k;
	ntCount++;
      }

      // check if this time is within our buffer time
      //std::cout<<tTimeE<< " " << nextTime << " " << beginTime;
      if ( tTimeE > nextTime || tTimeE < beginTime) continue;
      
      // with cuts
      if (tChargeL-tChargeS<800 || tChargeL < 2500 || tChargeS < 500 || tPSD > 1.0 || tPSD < 0.0)
	continue;
      
      // if event found, increment in hist
      if (beginTime > tTimeE) continue;
      ULong64_t curtbin = (tTimeE - beginTime)/div; 
      iCross[ tChannel ][ curtbin ]++;
      countE[tChannel]++;
      //std::cout<<curtbin << " ";

    } // end for

    beginTime = nextTime;
    nextTime = beginTime + t;
    beginEvent = nextBeginEvent;
    ntCount = 0;

    //std::cout <<"! " <<std::endl<<std::endl;
    // compare time delay for found events in reference channel
    for (int j=0;j<tBins;j++){
      //std::cout<<"%";
      // find event
      if(iCross[refChan][j] == 0)
	continue;
      //std::cout<<"*";
      // find events after ref channel event for 1000 ns
      for (int m=TMath::Max((ULong64_t)0,(ULong64_t)(j-delay));m<TMath::Min((ULong64_t)(j+delay),(ULong64_t)t1);m++) {
	for (int l=0;l<NCH;l++)
	  // if gets to here, potential cross talk event
	  if (l!=refChan && iCross[l][m]>0) {
	    if (m-j<-delay || m-j>delay) {
	      std::cout << "channel " << l << " count error of time " << m-j << std::endl;
	      continue;
	    }
	    hTime[l]->Fill(m-j,iCross[l][m]);
	    hTime[refChan]->Fill(m-j,iCross[l][m]);
	    countC[l]+=iCross[l][m];
	    countC[refChan]+=iCross[l][m];
	    //std::cout<<"#";
	  }
      }
    } // end outer for

    // reset 2d hist
    for (int i=0;i<NCH;i++)
      for (int j=0;j<tBins;j++)
	iCross[i][j] = 0;

  } // end reading and analyzing file

  // plot result
  tc1->cd(1);
  hTime[7]->Draw();
  tc1->cd(2);
  hTime[0]->Draw();
  tc1->cd(3);
  hTime[1]->Draw();
  tc1->cd(4);
  hTime[6]->Draw();
  tc1->cd(5);
  hTime[8]->Draw();
  tc1->cd(6);
  hTime[2]->Draw();
  tc1->cd(7);
  hTime[5]->Draw();
  tc1->cd(8);
  hTime[4]->Draw();
  tc1->cd(9);
  hTime[3]->Draw();

  for(int i=0;i<NCH;i++)
    std::cout << countE[i] << " total events and " << countC[i] << " possible crosstalk events in channel " << i 
	      << ", " << double (countC[i])/double(countE[i])*100. << "% crosstalk"<<std::endl;

  // coincidence
  double coince = 0.000002*countE[refChan];
  for(int i=0;i<NCH;i++)
    if( i!=refChan)
      std::cout << "Channel " << i << " has " << coince * countE[i]/(4*3600) << " coincidence events"<<std::endl;
  
  std::cout << "Lapped " << lap << " times" << std::endl;

  delete [] countE;
  delete [] countC;

  for (int i=0;i<NCH;i++)
    delete []iCross[i];
  delete [] iCross;
} // end Crosstalk

int main(){

  Crosstalk();
  return 1;
}
