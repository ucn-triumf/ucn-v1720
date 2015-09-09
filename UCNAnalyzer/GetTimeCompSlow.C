// ---------------------------------------------------------------
// Macro for outputting various slow control values over time.
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

void GetTimeCompSlow(char *infilename = "output00604.root") {

  gROOT->SetStyle("Plain");
  gStyle->SetOptFit(1111);

  // open file and trees to be read
  if (!( infilename && (*infilename) )) return; 
  TFile* fin = new TFile(infilename,"read");
  TTree* sin = (TTree*) fin->Get("tSlow");
  if (!sin) { delete sin; return;}

  // branches of tSlow
  Float_t tTemp1;
  Float_t tTemp2;
  Float_t tPress;
  Float_t tHV;
  ULong64_t tTimeS;
  ULong64_t tPSDIndex;

  // tree for slow control data
  sin->SetBranchAddress("tTemp1",   &tTemp1 );
  sin->SetBranchAddress("tTemp2",   &tTemp2 );
  sin->SetBranchAddress("tPress",   &tPress );
  sin->SetBranchAddress("tHV",      &tHV );
  sin->SetBranchAddress("tTimeS",   &tTimeS );
  sin->SetBranchAddress("tPSDIndex",&tPSDIndex );

  // title
  TCanvas*tc=new TCanvas();

  const Long64_t m = sin->GetEntries();
  if (!m) { delete fin; return; }
  Double_t *event = new Double_t[m];
  Double_t *press = new Double_t[m];
  Double_t *temp1 = new Double_t[m];
  Double_t *temp2 = new Double_t[m];
  Double_t *hv    = new Double_t[m];
  Double_t *timeS = new Double_t[m];
  Double_t eventT=0,pressT=0,temp1T=0,temp2T=0,hvT=0; //  for later normalizing
  Double_t prevS = 0;
  for(int i=0;i<m;i++) {
    sin->GetEvent(i);

    // fill event rate
    // Due to slow control timing at the beginning of a run, the first event 
    // often contains 2 slow control reads instead of 1.  Average these reads.
    if (i==0) {
      timeS[i] = tTimeS;
      event[i] = tPSDIndex/2.;
      eventT  += tPSDIndex/2.;
    }
    else {
      timeS[i] = tTimeS;
      event[i] = tPSDIndex - prevS;
      eventT  += tPSDIndex - prevS;
    }
    prevS = tPSDIndex;

    // fill slow control variables
    press[i] = tPress;  
    pressT  += tPress;  
    temp1[i] = tTemp1;
    temp1T  += tTemp1;
    temp2[i] = tTemp2;
    temp2T  += tTemp2;
    hv[i]    = tHV;
    hvT     += tHV;
  }

  TGraph *gEventS = new TGraph(m,timeS,event);
  gEventS->SetTitle("Event Rate Over Time (Slow)");
  gEventS->GetXaxis()->SetTitle("Time of Run");
  gEventS->GetYaxis()->SetTitle("Events");
 
  // normalize slow control variables
  for (int i=0;i<m;i++) {
    event[i] /= eventT;
    press[i] /= pressT;  
    temp1[i] /= temp1T;
    temp2[i] /= temp2T;
    hv[i]    /= hvT;
  }

  // create graphs for plotting
  TGraph *gEventS = new TGraph(m,timeS,event);
  gEventS->SetTitle("Event Rate Over Time (Slow)");
  TGraph *gPress = new TGraph(m,timeS,press);
  gPress->SetTitle("Pressure");
  TGraph *gTemp1 = new TGraph(m,timeS,temp1);
  gTemp1->SetTitle("Temperature 1");
  TGraph *gTemp2 = new TGraph(m,timeS,temp2);
  gTemp2->SetTitle("Temperature 2");
  TGraph *gHV    = new TGraph(m,timeS,hv);
  gHV   ->SetTitle("HV");

  gPress->SetMarkerColor(kGreen+3);
  gTemp1->SetMarkerColor(kRed);
  gTemp2->SetMarkerColor(kBlue);
  gHV   ->SetMarkerColor(kOrange);

  gPress->SetLineColor(kGreen+3);
  gTemp1->SetLineColor(kRed);
  gTemp2->SetLineColor(kBlue);
  gHV   ->SetLineColor(kOrange);

  gEventS->SetFillColor(kWhite);
  gPress ->SetFillColor(kWhite);
  gTemp1 ->SetFillColor(kWhite);
  gTemp2 ->SetFillColor(kWhite);
  gHV    ->SetFillColor(kWhite);

  // plot all together
  TMultiGraph *mgS = new TMultiGraph();
  mgS->Add(gEventS);
  mgS->Add(gPress);
  mgS->Add(gTemp1);
  mgS->Add(gTemp2);
  mgS->Add(gHV);

  // set title and axes
  mgS->SetTitle("Normalized Slow Control Values Over Course of Run;Time (Unix);");
  mgS->Draw("alp");

  TLegend *l1 = tc->BuildLegend();
  l1->SetFillColor(kWhite);

  // clear memory
  delete fin; // automatically deletes "sin", too
  delete[] event;
  delete[] press;
  delete[] temp1;
  delete[] temp2;
  delete[] hv;
  delete[] timeS;

  return;
}
