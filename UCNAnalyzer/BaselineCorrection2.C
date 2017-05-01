#include "TMath.h"

TF1 *fChargeL(TTree * tin, int chan){

  // fit ranges - set to avoid overflow
  double cmin = 2000;
  double cmax = 40000;
  double bmin = 1800;
  double bmax = 2200;

  // // open root file
  // TFile* fin = new TFile(infilename,"read");
  // TTree* tin = (TTree*) fin->Get("tUCN");
  char anum[100];
  sprintf(anum,
	  "tChannel == %d && tChargeL< %f && tChargeL>%f",
	  chan,
	  cmax,
	  cmin);

  // data to be fitted
  TCanvas *tc1 = new TCanvas();
  tc1->cd();
  TH2D *hChargeL = new TH2D("hChargeL","hChargeL",
			    cmax-cmin+1,cmin,cmax,
			    bmax-bmin+1,bmin,bmax);
  tin->Draw("tBaseline:tChargeL>>hChargeL",anum);

  // profile
  TProfile *pChargeL = hChargeL->ProfileX("e");

  // fit for channel
  fCL = new TF1("fCL","pol1",cmin,cmax);
  pChargeL->Fit(fCL);

  return fCL;
}

TF1 *fChargeS(TTree * tin, int chan){

  // fit ranges - set to avoid overflow
  double cmin = 2000;
  double cmax = 40000;
  double bmin = 1800;
  double bmax = 2200;
  char anum[100];
  sprintf(anum,
	  "tChannel == %d && tChargeS< %f && tChargeS>%f",
	  chan,
	  cmax,
	  cmin);

  // data to be fitted
  TCanvas *tc1 = new TCanvas();
  tc1->cd();
  TH2D *hChargeS = new TH2D("hChargeS","hChargeS",
			    cmax-cmin+1,cmin,cmax,
			    bmax-bmin+1,bmin,bmax);
  tin->Draw("tBaseline:tChargeS>>hChargeS",anum);

  // profile
  TProfile *pChargeS = hChargeS->ProfileX("e");

  // fit for channel
  fCS = new TF1("fCS","pol1",cmin,cmax);
  pChargeS->Fit(fCS);

  return fCS;
}

BaselineCorrection (char *infilename = "output00000045_0000.root", int chan = 0){

  gStyle->SetOptFit(1111);
  char anum[100];

  // open root file
  TFile* fin = new TFile(infilename,"read");
  TTree* tin = (TTree*) fin->Get("tUCN");

  // data to be fitted
  TF1* fCL = fChargeL((TTree*)tin, chan);
  TF1* fCS = fChargeS((TTree*)tin, chan);

  // adjusted 2D plot before and after fit
  double cmin = 2000;
  double cmax = 40000;
  double bmin = 1800;
  double bmax = 2200;
  sprintf(anum,
	  "tChannel == %d && tChargeS< %f && tChargeS>%f",
	  chan,
	  cmax,
	  cmin);

  // before fit
  TCanvas *tc2 = new TCanvas();
  tc2->cd();
  TH2D *hChargeLB = new TH2D("hChargeLB","hChargeLB",
			    cmax-cmin+1,cmin,cmax,
			    bmax-bmin+1,bmin,bmax);
  tin->Draw("tBaseline:tChargeL>>hChargeLB",anum);

  // after fit
  TCanvas *tc3 = new TCanvas();
  tc3->cd();
  TH2D *hChargeLA = new TH2D("hChargeLA","hChargeLA",
			     cmax-cmin+1,cmin,cmax,
			     bmax-bmin+1,bmin,bmax);
  Double_t corrL = 1+abs(fCL->GetParameter(1));
  Double_t corrB = fCL->GetParameter(1);
  UShort_t tChargeL;
  UShort_t tBaseline;
  UShort_t tChannel;
  tin->SetBranchAddress("tChargeL",&tChargeL);
  tin->SetBranchAddress("tBaseline",&tBaseline);
  tin->SetBranchAddress("tChannel",&tChannel);
  Double_t  n = tin->GetEntries();
  for (int i=0;i<n;i++) {
    tin->GetEntry(i);
    if(tChannel == chan){
      hChargeLA->Fill(corrL*tChargeL,
		      -corrB*tChargeL+tBaseline);
    }
  }
  hChargeLA->Draw();

  // flat fit
  TCanvas *tc4 = new TCanvas();
  tc4->cd();
  TH2D *hChargeLF = new TH2D("hChargeLF","hChargeLF",
			     cmax-cmin+1,cmin,cmax,
			     bmax-bmin+1,bmin,bmax);
  corrL = 50; // number of Charge long bins
  corrB = fCL->GetParameter(0);
  for (int i=0;i<n;i++) {
    tin->GetEntry(i);
    if(tChannel == chan){
      hChargeLF->Fill(tChargeL + abs(corrB-tBaseline)*corrL,
		      corrB);
    }
  }
  hChargeLF->Draw();

  return;
}


