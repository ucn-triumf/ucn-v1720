// ---------------------------------------------------------------
// Macro for finding the average rate of events 
// Macro averages for total and for each channel
// ---------------------------------------------------------------

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TVConstants.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

void Rate(char * outfilename="Rate.root") {
  // make output file
  // The plan will be to make one histogram per channel for every 10 min of running
  // 10 min * 60 sec = 600 bin histograms
  TFile * fout = new TFile(outfilename,"recreate");

  // counter of number of 10 minute intervals processed (keep track for each channel!)
  int n10min[PSD_MAXNCHAN*NDPPBOARDS];
  n10min= {0};

  // number of channels to look for
  const int nchtot = 10;

  // read in text file of data files to be read
  // find number of entries
  // read in text file of data files to be read
  ifstream fList;
  fList.open("rateforruns.txt");

  ULong64_t tTimeE;
  UShort_t tChannel;
  UShort_t tChargeL;
  UShort_t tChargeS;

  Double_t time1,time2; // first and last time in current file... in case needed

  ULong64_t time, nextTime,sec;
  ULong64_t prevTime[PSD_MAXNCHAN*NDPPBOARDS];
  ULong64_t currTime[PSD_MAXNCHAN*NDPPBOARDS];
  ULong64_t base    [PSD_MAXNCHAN*NDPPBOARDS];

  // length of a second in clock ticks:
  sec = 250000000;

  // directory with root files (already converted using UCNAnalyzer).
  string dir = "/home/midas/online/NeutronDaq/UCNAnalyzer/";

  //std::cout << m << std::endl;
  Double_t *rate = new Double_t[nchtot];
  rate = {0};

  for (int i=0;!fList.eof();i++) {
    // Initialize variables for calculating rate per channel
    time      = 0;	
    nextTime = 0;
    prevTime = {0};
    currTime = {0};
    base      = {0};

    string name;
    getline(fList,name);
    if(fList.eof())
      break;
    std::cout << name << std::endl;
    const char *infilename = name.c_str();
    TFile* fin = new TFile(infilename,"READ");
    
    TTree* uin = (TTree*) fin->Get("tUCN");
    uin->SetBranchAddress("tTimeE",&tTimeE);
    uin->SetBranchAddress("tChannel",&tChannel);
    uin->SetBranchAddress("tChargeL",&tChargeL);
    uin->SetBranchAddress("tChargeS",&tChargeS);

    eventTot = (Double_t)uin->GetEntries();
    uin->GetEvent(0);
    time1 = tTimeE;
    uin->GetEvent(eventTot-1);
    time2 = tTimeE;

    //eventTot0=0;
    //eventTot7=0;
    chargeTot=0;
    for(int j=0;j<eventTot;j++) {
      uin->GetEvent(j);
      chargeTot += tChargeL;
      //std::cout << tChannel << " ";
      //if(tChannel==0)
      //eventTot[0]++;
      //if(tChannel==7)
      //eventTot7++;
    }

    //std::cout << eventTot << " " << time1 << " " << time2 << " "
    //	      << eventTot0 << " " <<eventTot7<<std::endl;
    aveRate  = eventTot / ((Double_t)(time2-time1)*4.0*1.0e-9 / 10);
    aveCharge = chargeTot / eventTot;
    //for (int j=0;j<PSD_MAXNCHAN;j++)
    //aveRate[j] = eventTot[j] / ((Double_t)(time2-time1)*4.0*1.0e-9 / 10);
    //rate[i] = aveRate;
 
    hin->GetEvent(0);
    volt = tMVolt[2];
    //charge[i] = volt;

    //hvRate->SetPoint(i,aveRate,volt);
    hvRate->SetPoint(i,volt,aveRate);
    hvCharge->SetPoint(i,volt,aveCharge);
    //for (int j=0;j<MAXNCHAN; j++)
    //hvRate[j]->SetPoint(i,volt,aveRate[j]);
    //std::cout << tMVolt[0] << " " <<  tMVolt[1] << " " << tMVolt[2] << std::endl;
    //std::cout << tMCurr[0] << " " <<  tMCurr[1] << " " << tMCurr[2] << std::endl;

    std::cout << aveRate <<" " << volt<<std::endl;
  }

  TCanvas *c1 = new TCanvas();
  c1->cd();
  hvRate->SetTitle("Event Rate vs HV Setting;HV Setting (V);Event Rate (Hz)");
  //hvRate->GetXaxis()->SetTitle("Event Rate (Hz)");
  //hvRate->GetYaxis()->SetTitle("HV Setting (V)");
  hvRate->Draw("a*");

  TCanvas *c2 = new TCanvas();
  c2->cd();
  hvCharge->SetTitle("Average Charge vs HV Setting;HV Setting (V);Average Charge per Event (ADC)");
  //hvRate->GetXaxis()->SetTitle("Event Rate (Hz)");
  //hvRate->GetYaxis()->SetTitle("HV Setting (V)");
  hvCharge->Draw("a*");

  //TCanvas *c2 = new TCanvas();
  //c2->cd();
  //hvRate[0]->SetTitle("Event Rate vs HV Setting (Channel 0);HV Setting (V);Event Rate (Hz)");
  //hvRate[0]->Draw("a*");

  //TCanvas *c3 = new TCanvas();
  //c3->cd();
  //hvRate[7]->SetTitle("Event Rate vs HV Setting (Channel 7);HV Setting (V);Event Rate (Hz)");
  //hvRate[7]->Draw("a*");

}
