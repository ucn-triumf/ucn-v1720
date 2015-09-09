// ---------------------------------------------------------------
// Macro for outputting various gate values over time.
// Used for large files
// Uncomment 2D histogram lines of particular variable to see that 
// particular histogram.  Uncommenting more than that can cause
// root to crash from the large file (~GB). (output588.root)
// ---------------------------------------------------------------
#include "TFile.h"
#include "TTree.h"
#include "TLegend.h"
#include "TH2.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include <stdio.h>
#include <iostream>

void GetTimeCompDAQ(char *infilename = "output00604.root") {

  // set draw style attributes
  gROOT->SetStyle("Plain");
  gStyle->SetOptFit(1111);

  // open file and tree to be read
  if (!( infilename && (*infilename) )) return; 
  TFile* fin = new TFile(infilename,"read");
  TTree* uin = (TTree*) fin->Get("tUCN");
  if (!uin) { delete fin; return; }

  // branches of tUCN
  ULong64_t tEntry;
  ULong64_t tTimeE;
  UShort_t tChannel;
  UShort_t tLength;
  //Short_t tPulse[1000];
  Float_t tPSD;
  UShort_t tChargeL;
  UShort_t tChargeS;
  UShort_t tBaseline;

  // tree for runtime event data
  uin->SetBranchAddress("tEntry",    &tEntry);
  uin->SetBranchAddress("tTimeE",    &tTimeE);
  uin->SetBranchAddress("tChannel",  &tChannel);
  uin->SetBranchAddress("tPSD",      &tPSD);
  uin->SetBranchAddress("tChargeL",  &tChargeL);
  uin->SetBranchAddress("tChargeS",  &tChargeS);
  uin->SetBranchAddress("tBaseline", &tBaseline);
  uin->SetBranchAddress("tLength",   &tLength);

  TCanvas*tc=new TCanvas();

  // histograms for plotting
  uin->Draw("tChargeL:tTimeE>>hChargeL",  "tPSD<1&&tPSD>0","");
  hChargeL->SetLineColor(kBlue);
  hChargeL->SetMarkerColor(kBlue);
 
  uin->Draw("tChargeS:tTimeE>>hChargeS",  "tPSD<1&&tPSD>0","");
  hChargeS->SetLineColor(kRed);
  hChargeS->SetMarkerColor(kRed);
  
  uin->Draw("tBaseline:tTimeE>>hBaseline","tPSD<1&&tPSD>0","");
  hBaseline->SetLineColor(kGreen+3);
  hBaseline->SetMarkerColor(kGreen+3);

  uin->Draw("tPSD:tTimeE>>hPSD",          "tPSD<1&&tPSD>0","");
  hPSD->SetLineColor(kOrange);
  hPSD->SetMarkerColor(kOrange);

  THStack *hs = new THStack();
  hs->Add(hChargeL);
  hs->Add(hChargeS);
  hs->Add(hBaseline);
  hs->Add(hPSD);

  hs->SetTitle("DAQ Variables over Course of Run;Time (ns);Charge (ADC)");
  hs->Draw("nostack");
  

  TLegend *l1 = tc->BuildLegend();
  l1->SetFillColor(kWhite);

  TCanvas*tc1=new TCanvas();
  tc1->Divide(2,3);

  tc1->cd(1);
  uin->Draw("tChargeL:tChargeS", "tPSD<1&&tPSD>0","");

  tc1->cd(2);
  uin->Draw("tChargeL:tBaseline","tPSD<1&&tPSD>0","");

  tc1->cd(3);
  uin->Draw("tChargeL:tPSD",     "tPSD<1&&tPSD>0","");

  tc1->cd(4);
  uin->Draw("tChargeS:tBaseline","tPSD<1&&tPSD>0","");

  tc1->cd(5);
  uin->Draw("tChargeS:tPSD",     "tPSD<1&&tPSD>0","");

  tc1->cd(6);
  uin->Draw("tBaseline:tPSD",    "tPSD<1&&tPSD>0","");


  // clear memory
  delete fin; // automatically deletes "uin", too

  return;
}
