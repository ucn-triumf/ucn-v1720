// ---------------------------------------------------------------
// Macro for outputting various gate values over time.
// Used for large files
// Uncomment 2D histogram lines of particular variable to see that 
// particular histogram.  Uncommenting more than that can cause
// root to crash from the large file (~GB). (output588.root)
// ---------------------------------------------------------------
#include "TROOT.h"
#include "THStack.h"
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

void GetTimeComp(char *infilename = "output00604.root") {

  // set draw style attributes
  gROOT->SetStyle("Plain");
  gStyle->SetPadColor(kWhite);
  gStyle->SetOptFit(1111);
  //gStyle->SetEndErrorSize(3);
  //gStyle->SetErrorX(1.);

  // open file and trees to be read
  if (!( infilename && (*infilename) )) return; 
  TFile* fin = new TFile(infilename,"read");
  TTree* uin = (TTree*) fin->Get("tUCN");
  if (!uin) { delete fin; return; }
  TTree* sin = (TTree*) fin->Get("tSlow");

  //TFile* fout = new TFile("output.root","recreate");

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

  // branches of tSlow
  Float_t tTemp1;
  Float_t tTemp2;
  Float_t tPress;
  Float_t tHV;
  ULong64_t tTimeS;
  ULong64_t tPSDIndex;

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
  sin->SetBranchAddress("tTemp1",   &tTemp1 );
  sin->SetBranchAddress("tTemp2",   &tTemp2 );
  sin->SetBranchAddress("tPress",   &tPress );
  sin->SetBranchAddress("tHV",      &tHV );
  sin->SetBranchAddress("tTimeS",   &tTimeS );
  sin->SetBranchAddress("tPSDIndex",&tPSDIndex );

  // Number of events over the course of the run----------------------
  TCanvas*tc=new TCanvas();
  tc->Divide(2,1);
  
  // event rate from DAQ point of view
  // loop over slow control tree and get values' time dependence into hist
  tc->cd(1);
  if (!uin) { delete fin; return; } // "tUCN" not found in "fin"

  TH1D *rateE = new TH1D("rateE","rateE",100,0.,100.);
  uin->Draw("tTimeE>>rateE");
  rateE->Draw("e");
  rateE->Print();
  // TH1D *charge = new TH1D();
  // uin->Draw("tChargeL>>charge");

  // // find begin and end of run in time
  // const Long64_t n = uin->GetEntries();
  // if (!n) { delete fin; return; } 
  // uin->GetEvent(0);
  // Double_t minTime = tTimeE/1000000.;
  // uin->GetEvent(n);
  // Double_t maxTime = tTimeE/1000000.;

  // // Total events
  // TH1D *rateE = new TH1D("rateE","Event Rate;Event;Time (ms)",200,minTime,maxTime);
  // uin->Draw("tTimeE>>rateE");

  // TH1D* hChargeL = new TH1D("hChargeL", "Charge Long;Charge (ADC);Time (ms)",
  // 			    200,minTime,maxTime);
  // uin->Draw("tChargeL>>hChargeL","tPSD>0&&tPSD<1","e");
  // hChargeL->SetLineColor(kBlue);
  // hChargeL->SetMarkerColor(kBlue);

  // TH1D* hChargeS = new TH1D();
  // uin->Draw("tChargeS>>hChargeS");
  // hChargeS->SetLineColor(kRed);
  // hChargeS->SetMarkerColor(kRed);
  
  // TH1D* hBaseline = new TH1D();
  // uin->Draw("tBaseline>>hBaseline");
  // hBaseline->SetLineColor(kGreen+3);
  // hBaseline->SetMarkerColor(kGreen+3);

  // TH1D* hPSD      = new TH1D();
  // uin->Draw("tPSD>>hPSD");
  // hPSD->SetLineColor(kOrange);
  // hPSD->SetMarkerColor(kOrange);

  // tc->cd(1);
  // rateE->Draw("e1");


  // // Partial events
  // TH1D* hChargeLoT  = new TH1D();
  // uin->Draw("tChargeL>>hChargeLoT","tCharge>tBaseline","");
  // hChargeLoT ->SetLineColor(kBlue);
  // hChargeLoT ->SetMarkerColor(kBlue);

  // TH1D* hChargeLuT  = new TH1D("hChargeLuT", "Charge Long less than Baseline" ,
  // 			       200,minTime,maxTime);
  // hChargeLuT ->SetLineColor(kBlue+3);
  // hChargeLuT ->SetMarkerColor(kBlue+3);
  // TH1D* hChargeSoT  = new TH1D("hChargeSoT", "Charge Short greater than Baseline",
  // 			       200,minTime,maxTime);
  // hChargeSoT ->SetLineColor(kRed);
  // hChargeSoT ->SetMarkerColor(kRed);
  // TH1D* hChargeSuT  = new TH1D("hChargeSuT", "Charge Short greater than Baseline",
  // 			       200,minTime,maxTime);
  // hChargeSuT ->SetLineColor(kRed+3);
  // hChargeSuT ->SetMarkerColor(kRed+3);




  // loop over slow control tree and get values' time dependence into hist
  tc->cd(2);
  if (!sin) { delete fin; return; } // "tSlow" not found in "fin"
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
      event[i] = tPSDIndex;
      eventT  += tPSDIndex;
    }
    prevS += tPSDIndex;

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
  //gEventS->SetMarkerStyle(21);
  gEventS->SetTitle("Event Rate Over Time (Slow)");
  gEventS->GetXaxis()->SetTitle("Time of Run");
  gEventS->GetYaxis()->SetTitle("Events");
  gEventS->Draw("AL");
  
  // -----------------------------------------------------------------

  // Gate values over the course of the run --------------------------
  TCanvas*tc1=new TCanvas();
  tc1->cd();

  // THStack *hs = new THStack();
  // hs->Add(hChargeL);
  // hs->Add(hChargeS);
  // hs->Add(hBaseline);

  // //hs->GetXaxis()->SetTitle("Time (ms)");
  // //hs->GetYaxis()->SetTitle("Charge (ADC)");
  // hs->Draw();
  // //tc1->SetLogy();
  // TLegend *l1 = tc1->BuildLegend();
  // l1->SetFillColor(kWhite);
  const Long64_t n = uin->GetEntries();
  if (!n) { delete fin; return; }

  //find begin and end of run in time
  uin->GetEvent(0);
  Double_t minTime = tTimeE/1000000.;
  uin->GetEvent(n);
  Double_t maxTime = tTimeE/1000000.;

  // Total events
  TH1D* hChargeL  = new TH1D("hChargeL", "Charge Long" ,200,minTime,maxTime);
  hChargeL ->SetLineColor(kBlue);
  hChargeL ->SetMarkerColor(kBlue);
  TH1D* hChargeS  = new TH1D("hChargeS", "Charge Short",200,minTime,maxTime);
  hChargeS ->SetLineColor(kRed);
  hChargeS ->SetMarkerColor(kRed);
  TH1D* hBaseline = new TH1D("hBaseline","Baseline",    200,minTime,maxTime);
  hBaseline->SetLineColor(kGreen+3);
  hBaseline->SetMarkerColor(kGreen+3);
  TH1D* hPSD      = new TH1D("hPSD",     "PSD",         200,minTime,maxTime);
  hPSD     ->SetLineColor(kOrange);
  hPSD     ->SetMarkerColor(kOrange);

  // Partial events
  TH1D* hChargeLoT  = new TH1D("hChargeLoT", "Charge Long greater than Baseline" ,
  			       200,minTime,maxTime);
  hChargeLoT ->SetLineColor(kBlue);
  hChargeLoT ->SetMarkerColor(kBlue);
  TH1D* hChargeLuT  = new TH1D("hChargeLuT", "Charge Long less than Baseline" ,
  			       200,minTime,maxTime);
  hChargeLuT ->SetLineColor(kBlue+3);
  hChargeLuT ->SetMarkerColor(kBlue+3);
  TH1D* hChargeSoT  = new TH1D("hChargeSoT", "Charge Short greater than Baseline",
  			       200,minTime,maxTime);
  hChargeSoT ->SetLineColor(kRed);
  hChargeSoT ->SetMarkerColor(kRed);
  TH1D* hChargeSuT  = new TH1D("hChargeSuT", "Charge Short greater than Baseline",
  			       200,minTime,maxTime);
  hChargeSuT ->SetLineColor(kRed+3);
  hChargeSuT ->SetMarkerColor(kRed+3);

  for(int i=0;i<n;i++) {
    uin->GetEvent(i);
    if(tPSD>0 && tPSD<1.) {
      hChargeL ->Fill(tTimeE/1000000.,tChargeL);
      hChargeS ->Fill(tTimeE/1000000.,tChargeS);
      hBaseline->Fill(tTimeE/1000000.,tBaseline);
      hPSD     ->Fill(tTimeE/1000000.,tPSD*10);

  //     if(tChargeL<tBaseline)
  // 	hChargeLuT->Fill(tTimeE/1000000.,tChargeL);
  //     else
  // 	hChargeLoT->Fill(tTimeE/1000000.,tChargeL);

  //     if(tChargeS<tBaseline)
  // 	hChargeSuT->Fill(tTimeE/1000000.,tChargeS);
  //     else
  // 	hChargeSoT->Fill(tTimeE/1000000.,tChargeS);
    }
  //   //std::cout << timeE[i] << " " << chargeL[i] << " " << chargeS[i] << " "
  //   //	      << base[i] << " " << psd[i] << std::endl;
  }

  hChargeL->Sumw2();

  THStack *hs = new THStack("hs","hs");
  hs->Add(hChargeL);
  hs->Add(hChargeS);
  hs->Add(hBaseline);
  hs->Add(hPSD);
  
  //hs->GetXaxis()->SetTitle("Time (ms)");
  //hs->GetYaxis()->SetTitle("Charge (ADC)");
  hs->Draw("nostack");
  tc1->SetLogy();
  TLegend *l1 = tc1->BuildLegend();
  l1->SetFillColor(kWhite);

  TCanvas *tc1_1 = new TCanvas();
  tc1_1->cd();

  // THStack *hs1 = new THStack();
  // hs1->Add(hChargeLoT);
  // hs1->Add(hChargeLuT);
  // hs1->Add(hChargeSoT);
  // hs1->Add(hChargeSuT);

  // hs1->Draw("nostack");

  // uin->SetMarkerColor(kRed);
  // uin->Draw("tChargeL:tTimeE","tPSD<1&&tPSD>0","");

  // TCanvas*tc2=new TCanvas();
  // tc2->cd();
  // uin->SetMarkerColor(kBlue);
  // uin->Draw("tChargeS:tTimeE","tPSD<1&&tPSD>0","same");

  // TCanvas*tc3=new TCanvas();
  // tc3->cd();
  // uin->SetMarkerColor(kGreen);
  // uin->Draw("tBaseline:tTimeE","tPSD<1&&tPSD>0","same");

  // tc1->cd(1);
  // tc1->BuildLegend();

  // TCanvas*tc4=new TCanvas();
  // tc4->cd();
  // tc1->cd(2);
  // uin->SetMarkerColor(kBlack);
  // uin->Draw("tPSD:tTimeE","tPSD<1&&tPSD>0","same");
  // TLegend *l1_1 = tc1_1->BuildLegend();
  // l1_1->SetFillColor(kWhite);
  // tc1->cd();
  // tc1->BuildLegend();
  // -----------------------------------------------------------------

  // // Slow control values over the course of the run ------------------
  TCanvas*tc2=new TCanvas();
  tc2->cd();

  // normalize slow control variables
  TGraph *gEventSNorm = new TGraph((TGraph&)gEventS);
  for (int i=0;i<m;i++) {
    gEventSNorm->GetY()[i] /= eventT;
    press[i]               /= pressT;  
    temp1[i]               /= temp1T;
    temp2[i]               /= temp2T;
    hv[i]                  /= hvT;
  }

  // create graphs for plotting
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

  gEventSNorm->SetFillColor(kWhite);
  gPress     ->SetFillColor(kWhite);
  gTemp1     ->SetFillColor(kWhite);
  gTemp2     ->SetFillColor(kWhite);
  gHV        ->SetFillColor(kWhite);

  // plot all together
  TMultiGraph *mgS = new TMultiGraph();
  mgS->Add(gEventSNorm);
  mgS->Add(gPress);
  mgS->Add(gTemp1);
  mgS->Add(gTemp2);
  mgS->Add(gHV);

  // set title and axes
  mgS->SetTitle("Normalized Slow Control Values Over Course of Run;Time (Unix);");
  // mgS->GetXaxis()->SetTitle("Time (Unix)");
  mgS->Draw("alp");

  TLegend *l2 = tc2->BuildLegend();
  l2->SetFillColor(kWhite);
 
  //gr->DrawCopy("A*");
  // TH2D* hfr = new TH2D("hfr","Slow Control",100,minTime*1.,maxTime*1.,100,0.,10.);
  // hfr->SetStats(0);
  // hfr->Draw();

  //TLegend*tl=new TLegend(0.05,0.7,0.45,0.95);
  //tl->SetFillColor(kWhite);

  // normalize histograms
  // hEventsE->Scale(1./hEventsE->Integral());
  // hP      ->Scale(1./hP->Integral());
  // hT1     ->Scale(1./hT1->Integral());
  // hT2     ->Scale(1./hT2->Integral());
  // hHV     ->Scale(1./hHV->Integral());
  //hEventsE->Draw();
  //tl->AddEntry(hEventsE,"Event Rate (number of events/200000)","l");

  // //TH2D* hP = new TH2D("hP","Slow Control",100,1406048000.,1406063000.,100,0.,10.);
  // hP->SetMarkerColor(kGreen);
  // hP->SetMarkerStyle(7);
  // sin->Draw("tPress:tTimeS>>hP");
  // //tl->AddEntry("hP","Pressure (Atm)","p");
  // TH2D* hT1 = new TH2D("hT1","Slow Control",100,1406048000.,1406063000.,100,0.,10.);
  // hT1->SetMarkerColor(kRed);
  // hT1->SetMarkerStyle(7);
  // sin->Draw("tTemp1:tTimeS>>hT1");
  // //tl->AddEntry("hT1","Temperature 1 (C/5)","p");
  // TH2D* hT2 = new TH2D("hT2","Slow Control",100,1406048000.,1406063000.,100,0.,10.);
  // hT2->SetMarkerColor(kBlue);
  // hT2->SetMarkerStyle(7);
  // sin->Draw("tTemp2:tTimeS>>hT2");
  // //tl->AddEntry(hT2,"Temperature 2 (C/5)","p");
  // TH2D* hHV = new TH2D("hHV","Slow Control",100,1406048000.,1406063000.,100,0.,10.);
  // hHV->SetMarkerColor(kBlack);
  // hHV->SetMarkerStyle(7);
  // sin->Draw("tHV*-1:tTimeS>>hHV");
  // //tl->AddEntry(hHV,"Voltage (-V)","p");

  // TH2D* hfr = new TH2D("hfr","Slow Control",100,minTime,maxTime,100,0.,1.);
  // hfr->SetStats(0);
  // hfr->Draw();
  // hEventsE->Draw();
  // hP->Draw("same");
  // hT1->Draw("same");
  // hT2->Draw("same");
  // hHV->Draw("same");
  //tl->Draw();
  //tc2->BuildLegend();
  //fout->Write();
  // -----------------------------------------------------------------

  // fout->Close();
  delete fin; // automatically deletes "uin", too
  //delete[] entry;
  //delete[] timeE;
  //delete[] chargeL;
  //delete[] chargeS;
  //delete[] base;
  //delete[] psd;
  delete[] event;
  delete[] press;
  delete[] temp1;
  delete[] temp2;
  delete[] hv;
  delete[] timeS;
  return;
}

