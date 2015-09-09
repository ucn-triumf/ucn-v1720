// ---------------------------------------------------------------
// Macro for finding the average rate of events vs HV over several
// runs.  Macro averages for total and for each channel
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

void RateAndHV() {

  // read in text file of data files to be read
  // find number of entries
  // std::ifstream f("runsHV.txt");
  // char c;
  // int j = 0;
  // while (f.get(c))
  //   if (c == '\n')
  //       ++j;
  // f.close();
  gROOT->SetStyle("Plain");
  gStyle->SetPadColor(kWhite);
  gStyle->SetOptFit(1111);

  const int m = 6;

  // read in text file of data files to be read
  ifstream fList;
  fList.open("runsHV.txt");

  ULong64_t tTimeE;
  UShort_t tChannel;
  UShort_t tChargeL;
  Double_t timeTot;
  Double_t time1,time2;
  Double_t eventTot,eventTot0,eventTot7;
  Double_t chargeTot;
  Double_t aveRate;
  Double_t aveCharge;
  Double_t *aveRateCh = new Double_t[PSD_MAXNCHAN];
  Float_t tMVolt[HVCHANNELS];
  Float_t tMCurr[HVCHANNELS];
  Float_t volt;

  string dir = "/home/midas/online/NeutronDaq/UCNAnalyzer/";

  //std::cout << m << std::endl;
  Double_t *rate = new Double_t[m];
  Double_t *charge = new Double_t[m];
  rate = {0};
  charge = {0};

  TGraph *hvRate = new TGraph(m);
  TGraph *hvRateCh = new TGraph[PSD_MAXNCHAN];
  TGraph *hvCharge = new TGraph(m);

  for (int i=0;!fList.eof();i++) {
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

    TTree *hin = (TTree*) fin->Get("tHV");
    hin->SetBranchAddress("tMVolt",tMVolt);
    hin->SetBranchAddress("tMCurr",tMCurr);

    //TH1D * rate = TH1D();
    //uin->Draw("tTimeE>>rate");
    //    eventTot = rate->Integrate();
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
