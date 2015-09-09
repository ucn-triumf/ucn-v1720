// ---------------------------------------------------------------
// Macro for outputting various gate values over time.
// Used for more than one file at a time.
// Filenames to be analyzed are placed in text file "runs.txt".
// ---------------------------------------------------------------

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TStyle.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

void GetTimeCompMulti() {

  // read in text file of data files to be read
  //char infilename[100];
  ifstream fList;
  fList.open("runs.txt");

  // DAQ comparisons
  TCanvas *tcCL=new TCanvas();
  TCanvas *tcCS=new TCanvas();
  TCanvas *tcB =new TCanvas();

  // Slow control comparisons
  TCanvas *tcE=new TCanvas(); // Event rate
  TCanvas *tcP=new TCanvas(); // Pressure
  TCanvas *tcT=new TCanvas(); // Temperature
  TCanvas *tcH=new TCanvas(); // HV

  ULong64_t tEntry;
  ULong64_t tTimeE;
  UShort_t tChannel;
  UShort_t tLength;
  //Short_t tPulse[1000];
  Float_t tPSD;
  UShort_t tChargeL;
  UShort_t tChargeS;
  UShort_t tBaseline;
  Float_t tTemp1;
  Float_t tTemp2;
  Float_t tPress;
  Float_t tHV;
  ULong64_t tTimeS;
  ULong64_t tPSDIndex;
  ULong_t nentries;
  ULong64_t maxTime;
  ULong64_t minTime;
  ULong_t prevE;
  string dir = "/home/midas/online/NeutronDaq/UCNAnalyzer/";

  for (int i=0;!fList.eof();i++) {
    // gDirectory->pwd();
    string name;
    getline(fList,name);
    if(fList.eof())
      break;
    std::cout << name << std::endl;
    string fname = dir + name;
    // const char *infilename = fname.c_str();
    const char *infilename = name.c_str();
    // gDirectory->pwd();
    // gStyle->SetOptFit(1111);
    TFile* fin = new TFile(infilename,"READ");
    TTree* uin = (TTree*) fin->Get("tUCN");
    TTree* sin = (TTree*) fin->Get("tSlow");
    //std::cout << infilename << std::endl;
    // gDirectory->pwd();
    // gROOT->cd();
    // gDirectory->pwd();
    // tree for runtime event data
    uin->SetBranchAddress("tEntry",    &tEntry);
    uin->SetBranchAddress("tTimeE",    &tTimeE);
    uin->SetBranchAddress("tChannel",  &tChannel);
    uin->SetBranchAddress("tPSD",      &tPSD);
    uin->SetBranchAddress("tChargeL",  &tChargeL);
    uin->SetBranchAddress("tChargeS",  &tChargeS);
    uin->SetBranchAddress("tBaseline", &tBaseline);
    uin->SetBranchAddress("tLength",   &tLength);
    
    // tree for slow control data
    sin->SetBranchAddress("tTemp1", &tTemp1 );
    sin->SetBranchAddress("tTemp2", &tTemp2 );
    sin->SetBranchAddress("tPress", &tPress );
    sin->SetBranchAddress("tHV",    &tHV );
    sin->SetBranchAddress("tTimeS", &tTimeS );
    sin->SetBranchAddress("tPSDIndex",&tPSDIndex );
  
    nentries = sin->GetEntries();
    sin->GetEvent(nentries);
    maxTime = tTimeS;
    sin->GetEvent(0);
    minTime = tTimeS;
    prevE = 0;
    TH1D *hEventsE = new TH1D("hEventsE","Number of Events", nentries/10,minTime,maxTime );
    for(int j=0;j<nentries;j++) {
      sin->GetEvent(j);
      std::cout << tTimeS << " " << tPSDIndex << " " << tPSDIndex - prevE <<std::endl;
      if(j == 0)
	hEventsE->Fill(tTimeS,tPSDIndex/2.);
      else
	hEventsE->Fill(tTimeS, tPSDIndex);
      prevE += tPSDIndex;
    }

    // if first draw
    if(i==0) {
      //std::cout <<"first loop"<<std::endl;
      tcE->cd();
      uin->Draw("tTimeE");

      tcP->cd();
      hEventsE->Scale(1/200000.);
      hEventsE->DrawCopy();
      sin->Draw("tPress:tTimeS","","same");

      tcH->cd();
      hEventsE->Scale(-1.);
      hEventsE->DrawCopy();
      sin->Draw("tHV:tTimeS","","same");

      tcT->cd();
      hEventsE->Scale(-5.);
      hEventsE->DrawCopy();
      sin->Draw("tTemp1:tTimeS","","same");
      sin->Draw("tTemp2:tTimeS","","same");

    } // end first draw
    else {
      //std::cout << "loop"<< std::endl;
      uin->SetLineColor(i+2);
      sin->SetMarkerColor(i+2);
      hEventsE->SetLineColor(i+2);

      tcE->cd();
      uin->SetMarkerColor(i+2);
      uin->Draw("tTimeE","","same");

      tcP->cd();
      hEventsE->Scale(1/200000.);
      hEventsE->DrawCopy("same");
      sin->Draw("tPress:tTimeS","","same");

      tcH->cd();
      hEventsE->Scale(-1.);
      hEventsE->DrawCopy("same");
      sin->Draw("tHV:tTimeS","","same");

      tcT->cd();
      hEventsE->Scale(-5.);
      hEventsE->DrawCopy("same");
      sin->Draw("tTemp1:tTimeS","","same");
      sin->Draw("tTemp2:tTimeS","","same");

    } // end other draw

  } // end for
}
