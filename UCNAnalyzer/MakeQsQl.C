/ ---------------------------------------------------------------
// Macro for outputting various slow control values over time.
// Used for large files
// Uncomment 2D histogram lines of particular variable to see that 
// particular histogram.  Uncommenting more than that can cause
// root to crash from the large file (~GB). (output588.root)
// ---------------------------------------------------------------
#include "TROOT.h"
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
#include "THStack.h"
#include <stdio.h>
#include <iostream>
#include "TVConstants.h"

void MakeQsQl( char *infilename = "output00604.root") 
{
  gROOT->SetStyle("Plain");
  gStyle->SetOptFit(1111);
  gStyle->SetTitle(0);
  gStyle->SetOptStat(0);
  // open file and trees to be read
  if (!( infilename && (*infilename) )) return; 
  TFile* fin = new TFile(infilename,"read");
  TTree* din = (TTree*) fin->Get("tUCN");
  if (!din) { return; }
  
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
  
  char foutname[100];
  sprintf( foutname,"ql_vs_qs_%s",infilename);
  TFile * fout = new TFile( foutname, "recreate" );
  TH2D* hqlqs = new TH2D("hqlqs","Short vs Long gate charge", 300, 0.0, 15000.0, 200, 0.0, 10000.0);
  
  // tree for runtime event data
  din->SetBranchAddress("tEntry",    &tEntry);
  din->SetBranchAddress("tTimeE",    &tTimeE);
  din->SetBranchAddress("tChannel",  &tChannel);
  din->SetBranchAddress("tPSD",      &tPSD);
  din->SetBranchAddress("tChargeL",  &tChargeL);
  din->SetBranchAddress("tChargeS",  &tChargeS);
  din->SetBranchAddress("tBaseline", &tBaseline);
  din->SetBranchAddress("tLength",   &tLength);
  for (ULong64_t j=0; j<= din->GetEntries(); j++){
    din->GetEvent(j);
    int ch = tChannel;
    if (ch != 7)
      continue;  // skip anything that isn't channel 7
    hqlqs->Fill( tChargeL, tChargeS );
  }
  
  fout->Write();
  TCanvas* c1 = new TCanvas();
  c1->GetPad(0)->SetLogz();
  hqlqs->Draw("colz");
  
}
