// CorrectedFit.C
// 1st order fit to event energy spectra for specified pmt channel
// Background is assumed expo
// Signal is assumed gaus
// Same as SimpleFit but with additional Baseline correction
// To run:
// .L BaselineCorrection.C
// .x CorrectedFit.C

#include "TMath.h"
#include "TAxis.h"

TF1 *total, *sig, *bg;

CorrectedFit(char *infilename = "output00000045_0000.root", int chan = 9){

  gStyle->SetOptFit(1111);

  // type of baseline correction fit
  // 0 == regular adjusted fit 
  // 1 == flat fit
  int fit = 1;
  Double_t binL = 50; // number of 4ns bins in charge long
  Double_t binS = 12; // number of 4ns bins in charge short

  // open root file
  TFile* fin = new TFile(infilename,"read");
  TTree* tin = (TTree*) fin->Get("tUCN");
  UShort_t tChargeL;
  UShort_t tChargeS;
  UShort_t tBaseline;
  tin->SetBranchAddress("tChargeL",&tChargeL);
  tin->SetBranchAddress("tChargeS",&tChargeS);
  tin->SetBranchAddress("tBaseline",&tBaseline);
  char anum[100];

  // Charge long fit --------------------------------------------
  // ------------------------------------------------------------
     
  TCanvas *tc1 = new TCanvas();
  tc1->cd();
  
  // charge range
  double xmin = 540; 
  double xmax = 9000.;

  // fitting range for charge signal
  double xminS = 3500;
  double xmaxS = xmax;

  // fitting range for charge background
  double xminB = xmin;
  double xmaxB = 1500;

  // data to correct
  TH1D* hinC= new TH1D("hinC",
		       "Event Energy Spectrum;Total Charge (ADC);Counts",
		       100,xmin,xmax);
  sprintf(anum,
	  "tChannel == %d && tChargeL< %f && tChargeL>%f",
	  chan,
	  xmax,
	  xmin);
  tin->Draw("tChargeL>>hinC",anum);
  
  // add baseline correction
  TF1 *fCL = fChargeL((TTree*)tin,chan);
  Double_t slopeL = abs(fCL->GetParameter(1));
  Double_t baseL = fCL->GetParameter(0);
  double n = hinC->GetXaxis()->GetNbins();
  TH1D *houtC = new TH1D("houtC",
			 "Event Energy Spectrum;Total Charge (ADC);Counts",
			 100,
			 xmin,
			 xmax);
  if (fit == 0)  // adjust according to regular Baseline correction
    for (int i=0;i<n;i++)
      houtC->Fill((1+slopeL)*hinC->GetXaxis()->GetBinCenter(i),
		  hinC->GetBinContent(i));
  else { // adjust with flat Baseline correction
    n = tin->GetEntries();
    for (int i=0;i<n;i++) {
      tin->GetEntry(i);
      houtC->Fill(tChargeL + abs(baseL-tBaseline)*binL);
    }
  }

  tc1->cd();
  houtC->Draw();

  // for signal and background functions
  sigC = new TF1("sigC","gaus",xminS,xmaxS);
  sigC->SetLineColor(kBlue);
  houtC->Fit(sigC,"R");
  bgC = new TF1("bgC","expo",xminB,xmaxB);
  bgC->SetLineColor(kRed);
  houtC->Fit(bgC,"R+");

  // for full tChargeL fit
  totalC =  new TF1("totalC","expo(0)+gaus(2)",xmin,xmax);
  totalC->SetParameters(bgC->GetParameter(0),
  		       bgC->GetParameter(1),
   		       sigC->GetParameter(0),
   		       sigC->GetParameter(1),
   		       sigC->GetParameter(2));
  totalC->SetParNames("A","Slope","B","x_{avg}","\sigma");
  totalC->SetTitle("Event Energy Spectrum;Total Charge (ADC);Counts");
  totalC->Draw();
  houtC->Draw("sames"); 
  sigC->Draw("same");
  bgC->Draw("same");
  // Charge long fit end ----------------------------------------
  // ------------------------------------------------------------

  // PSD fit ----------------------------------------------------
  // ------------------------------------------------------------
          
  TCanvas *tc2 = new TCanvas();
  tc2->cd();

  // PSD range
  xmin = -0.5; 
  xmax = 1.;

  // fitting range for charge signal
  xminS = 0.4;
  xmaxS = xmax;

  // fitting range for charge background
  xminB = xmin;
  xmaxB = 0.0;

  // data to correct
  TH1D* hinP = new TH1D("hinP",
			"Pulse Shape Spectrum;PSD;Counts",
			100,xmin,xmax);
  sprintf(anum,
	  "tChannel == %d && tPSD< %f && tPSD>%f",
	  chan,
	  xmax,
	  xmin);
  tin->Draw("tPSD>>hinP",anum);

  // add baseline correction
  // already have charge long correction
  // still need charge short correction
  TF1 *fCS = fChargeS((TTree*)tin,chan);
  Double_t slopeS = abs(fCS->GetParameter(1));
  Double_t baseS = fCS->GetParameter(0);
  n = tin->GetEntries();
  TH1D *houtP = new TH1D("houtP",
			 "Pulse Shape Spectrum;PSD;Counts",
			 100,xmin,xmax);
  if (fit == 0) // adjust according to regular Baseline correction
    for (int i=0;i<n;i++) {
      tin->GetEntry(i);
      houtP->Fill(1 - (1+slopeS)*(double)tChargeS/((1+slopeL)*(double)tChargeL));
    }
  else // adjust with flat Baseline correction
    for (int i=0;i<n;i++) {
      tin->GetEntry(i);
      houtP->Fill(1 - (tChargeS+abs(baseS-tBaseline)*binS)/(tChargeL+abs(baseL-tBaseline)*binL));
    }
  
  tc2->cd();
  houtP->Draw();

  // for signal and background functions
  sigP = new TF1("sigP","gaus",xminS,xmaxS);
  sigP->SetLineColor(kBlue);
  houtP->Fit(sigP,"R");
  sigP->SetParameters(sigP->GetParameter(0),
		      sigP->GetParameter(1),
		      sigP->GetParameter(2));
  houtP->Fit(sigP,"R");
  bgP = new TF1("bgP","gaus",xminB,xmaxB);
  bgP->SetLineColor(kRed);
  houtP->Fit(bgP,"R");

  // for full tChargeL fit
  totalP =  new TF1("totalP","gaus(0)+gaus(3)",xmin,xmax);
  totalP->SetParameters(bgP->GetParameter(0),
			bgP->GetParameter(1),
			bgP->GetParameter(2),
			sigP->GetParameter(0),
			sigP->GetParameter(1),
			sigP->GetParameter(2));
  totalP->SetParNames("A","Slope","B","x_{avg}","\sigma");
  totalP->SetTitle("Pulse Shape Spectrum;PSD;Counts");
  totalP->Draw();
  houtP->Draw("same");
  sigP->Draw("same");
  bgP->Draw("same");
  // PSD fit ----------------------------------------------------
  // ------------------------------------------------------------

}
