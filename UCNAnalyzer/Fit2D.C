// 2DFit.C
// File for fitting tChargeL:tPSD 2D histogram
// Includes basline correction
// To run:
// .L BaselineCorrection.C
// .x 2DFit.C

#include "TF2.h"
#include "TH2.h"
#include "TMath.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TObject.h"

#include <vector>

#include "BaselineCorrection.h"

//using namespace std;

// functions to fit------------------------------------
// ----------------------------------------------------

// function to UCN signal
Double_t fSignal(Double_t *x, Double_t *par) {
  Double_t r1 = Double_t((x[0]-par[1])/par[2]);
  Double_t r2 = Double_t((x[1]-par[3])/par[4]);
  return par[0]*TMath::Exp(-0.5*(r1*r1+r2*r2+2*r1*r2*par[5]));
}

// function to background
Double_t fBackground(Double_t *x, Double_t *par) {
  Double_t r1 = Double_t(x[0]*par[1]);
  Double_t r2 = Double_t((x[1]-par[2])/par[3]);
  return par[0]*TMath::Exp(-0.5*(r2*r2)+r1);
}

// signal + signal2 + background function
Double_t fTotal(Double_t *x, Double_t *par) {
  Double_t *p1 = &par[0];
  Double_t *p2 = &par[6];
  //Double_t *p3 = &par[12];
  Double_t result = fSignal(x,p1) + fBackground(x,p2);
  return result;
}

// 1D projection of signal (tChargeL or PSD)
Double_t fSignal1D(Double_t *x, Double_t *par) {
  Double_t r1 = Double_t(TMath::Sqrt(2.*TMath::Pi())*par[0]*par[1]);
  Double_t r2 = Double_t((x[0]-par[2])/par[3]);
  return r1*TMath::Exp(-0.5*(1-par[4]*par[4])*r2*r2);
}

// 1D projection of tChargeL background
Double_t fBackgroundCL(Double_t *x, Double_t *par){
  Double_t r1 = Double_t(par[2]*x[0]);
  return par[0]*par[1]*TMath::Sqrt(2.*TMath::Pi())*TMath::Exp(r1);
}

// 1D signal + background charge long projection
Double_t fTotalCL(Double_t *x, Double_t *par) {
  Double_t *p1 = &par[0];
  Double_t *p2 = &par[5];
  Double_t result = fSignal1D(x,p1) + fBackgroundCL(x,p2);
  return result;
}

// 1D projection of PSD background
Double_t fBackgroundPSD(Double_t *x, Double_t *par) {
  Double_t r1 = Double_t((x[0]-par[2])/par[3]);
  return (-1.*par[0]/par[1])*TMath::Exp(-0.5*r1*r1);
}

// 1D signal + background PSD projection
Double_t fTotalPSD(Double_t *x, Double_t *par) {
  Double_t *p1 = &par[0];
  Double_t *p2 = &par[5];
  Double_t result = fSignal1D(x,p1) + fBackgroundPSD(x,p2);
  return result;
}

// ----------------------------------------------------

void Fit2D(char *infilename = "output00000062_0000.root", int chan = 0) {
  
  gStyle->SetOptFit(1111);
 
  char anum[100];

  // timing cut
  // 0 - no timing cut
  // 1 - add timing cut
  int timing = 1;

  // baseline correction
  // 0 - no correction
  // 1 - linear fit correction
  int baseCorr = 1;

  // pulse cut
  // 0 - no cut
  // 1 - cut on full pulse (0,300)s
  // 2 - cut on prepulse (-10,0)s
  // 3 - cut on pulse peak (0,30)s
  // 4 - cut on late pulse (250,300)s
  // 5 - cut on falling pulse (100,150)s
  int pulse = 5;

  // open root file
  TFile* fin = new TFile(infilename,"read");
  TTree* tin = (TTree*) fin->Get("tUCN");

  // 2D fitting --------------------------------------------
  // -------------------------------------------------------

  // data range
  Double_t xmin = 540; 
  Double_t xmax = 12000.;
  Double_t ymin = -0.75;
  Double_t ymax = 1.0;
  Int_t xbin = 200;
  Int_t ybin = 200;

  // bin width
  Double_t xwidth = (xmax - xmin)/Double_t(xbin);
  Double_t ywidth = (ymax - ymin)/Double_t(ybin);

  // timing range - adjusted to remove retrigger events
  ULong64_t delTMin = 5000; // time difference of 4000ns
  ULong64_t tPrev = 0;
  ULong64_t skip = 0; // number of skipped events

  // pulse range
  std::vector<Long64_t> pulseT;
  Long64_t secCon = 40; // factor conversion to ns for pulse time
  Long64_t nsCon = 1000000000; // conversion from s to ns
  Long64_t preT;
  Long64_t postT;
  switch(pulse) {
  case 1:  // cut on pulse (0,300)s
    preT = 0*nsCon;
    postT = 300*nsCon;
    break;
  case 2:  // cut on prepulse (-10,0)s
    preT = -10*nsCon;
    postT = 0*nsCon;
    break;
  case 3:  // cut on pulse peak (0,30)s
    preT = 0*nsCon;
    postT = 30*nsCon;
    break;
  case 4:  // cut on late pulse (250,300)s
    preT = 250*nsCon;
    postT = 300*nsCon;
    break;
  case 5:  // cut on falling pulse (100,150)s
    preT = 100*nsCon;
    postT = 150*nsCon;
    break;
  default: // no cut on pulse
    preT = 0*nsCon;
    postT = 0*nsCon;
    break;
  }

  // 2D data to fit
  // include baseline correction
  TH2D *qlpsd = new TH2D("qlpsd","Event Energy vs Pulse Shape;Total Event Energy (ADC);PSD",
			 xbin,xmin,xmax,
			 ybin,ymin,ymax);
  TH2D *qlqs = new TH2D("qlqs","",xbin,xmin,xmax,ybin,0,2.8);

  // if adding baseline correction
  // baseline correction functions
  BaselineCorrection *base = new BaselineCorrection();
  TF1 *fCL = base->fChargeL((TTree*)tin,chan);
  Double_t corrL = 1+abs(fCL->GetParameter(1));
  TF1 *fCS = base->fChargeS((TTree*)tin,chan);
  Double_t corrS = 1+abs(fCS->GetParameter(1));
  // min charge for correction
  Double_t chargeMinL = 50*1.; // number of 4ns bins * 1 ADC
  Double_t chargeMinS = 10*1.; // number of 4ns bins * 1 ADC
  Double_t corrCL;
  Double_t corrCS;
  

  // fill in data
  UShort_t tChargeL;
  UShort_t tChargeS;
  UShort_t tChannel;
  ULong64_t tTimeE;
  tin->SetBranchAddress("tChargeL",&tChargeL);
  tin->SetBranchAddress("tChargeS",&tChargeS);
  tin->SetBranchAddress("tChannel",&tChannel);
  tin->SetBranchAddress("tTimeE",&tTimeE);
  Double_t  n = tin->GetEntries();

  // if cutting on pulse, find array of pulse start times
  if(pulse>0) {
    for (int i=0;i<n;i++) {
      tin->GetEntry(i);
      
      // get events from channel 10
      if(tChannel != 10)
	continue;
      
      // add pulse time to list of pulses
      pulseT.push_back(tTimeE*secCon);
    }
  }

  // loop over entries in TTree
  for (int i=0, j=0;i<n;i++) {
    tin->GetEntry(i);
    
    // narrow down to specified channel
    if (tChannel != chan)
      continue;
    
    // check for time difference between events (retrigger)
    if(timing==1 && (tTimeE-tPrev)<delTMin) {
      tPrev = tTimeE;
      continue;
    } 
    tPrev = tTimeE;

    // check for placement along beam pulse
    if (pulse > 0) {
     
      // check if reference pulse time needs to be updated
      if(tTimeE > (ULong64_t)(postT+(Long64_t)pulseT[j]))
	j++;

      if(tTimeE < (ULong64_t)(preT+(Long64_t)pulseT[j]))
	continue;
    }

    // baseline correction adjustment
    if( baseCorr==1 ) {

      // charge correction factors
      // based on baseline correction
      corrCL = corrL*tChargeL;
      corrCS = corrS*tChargeS;
      
      qlpsd->Fill(corrCL, 1.-corrCS/corrCL);
    } else
      qlpsd->Fill(tChargeL,1.-Double_t(tChargeS)/tChargeL);

    qlqs->Fill(tChargeL,TMath::Exp(1.-Double_t(tChargeS)/tChargeL));
  } // end get entries

  TH2D *sub = (TH2D*)qlpsd->Clone();
  
  // fit signal - gausx gausy
  // fit range
  Double_t xminS = 3000;
  Double_t xmaxS = 8000;
  Double_t yminS = 0.4;
  Double_t ymaxS = 0.8;
  const Int_t nparS = 6;
  Double_t fSigPar[nparS] = {100000.,5000.,700.,0.5,0.06,0.0};
  TF2 *fSig = new TF2("fSig",fSignal,
		      xminS,xmaxS,yminS,ymaxS,nparS);
  fSig->SetParameters(fSigPar);
  qlpsd->Fit("fSig","0");

  // // fit double signal - gausx gausy
  // // fit range
  // Double_t xminS2 = 9000;
  // Double_t xmaxS2 = xmax;
  // Double_t yminS2 = 0.6;
  // Double_t ymaxS2 = 0.9;
  // const Int_t nparS2 = 6;
  // Double_t fSigPar2[nparS] = {1000.,9000.,1000.,0.9,0.06,0.0};
  // TF2 *fSig2 = new TF2("fSig2",fSignal,
  // 		      xminS2,xmaxS2,yminS2,ymaxS2,nparS2);
  // fSig2->SetParameters(fSigPar2);
  // qlpsd->Fit("fSig2","0");

  // fit background - expox gausy
  // fit range
  Double_t xminB = xmin+200;
  Double_t xmaxB = 3000.;
  Double_t yminB = ymin;
  Double_t ymaxB = 0.3;
  const Int_t nparB = 4;
  Double_t fBgPar[nparB] = {1000000,-0.02,-0.07,0.03};
  TF2 *fBg = new TF2("fBg",fBackground,
  		     xminB,xmaxB,yminB,yminB,nparB);
  fBg->SetParameters(fBgPar);
  qlpsd->Fit("fBg","0");

  // total fit
  const Int_t nparT = 10;
  TF2 *fTot = new TF2("fTot",fTotal,
  		      xmin,xmax,ymin,ymax,nparT);
  Double_t fTotPar[nparT] = {fSig->GetParameter(0),
			     fSig->GetParameter(1),
			     fSig->GetParameter(2),
			     fSig->GetParameter(3),
			     fSig->GetParameter(4),
			     fSig->GetParameter(5),
			     // fSig2->GetParameter(0),
			     // fSig2->GetParameter(1),
			     // fSig2->GetParameter(2),
			     // fSig2->GetParameter(3),
			     // fSig2->GetParameter(4),
			     // fSig2->GetParameter(5),
			     fBg->GetParameter(0),
			     fBg->GetParameter(1),
			     fBg->GetParameter(2),
			     fBg->GetParameter(3)};
  fTot->SetParameters(fTotPar);
  // const char* fTotParName[nparT] = {"Const_{s}",
  // 				    "#mu_{xs}",
  // 				    "#sigma_{xs}",
  // 				    "#mu_{ys}",
  // 				    "#sigma_{ys}",
  // 				    "#rho_{xys}",
  // 				    // "Const_{2s}",
  // 				    // "#mu_{2xs}",
  // 				    // "#sigma_{2xs}",
  // 				    // "#mu_{2ys}",
  // 				    // "#sigma_{2ys}",
  // 				    // "#rho_{2xys}",
  // 				    "Const_{b}",
  // 				    "#lambda_{xb}",
  // 				    "#mu_{yb}",
  // 				    "#sigma_{yb}"};
  fTot->SetParNames("Const_{s}",
		    "#mu_{xs}",
		    "#sigma_{xs}",
		    "#mu_{ys}",
		    "#sigma_{ys}",
		    "#rho_{xys}",
		    "Const_{b}",
		    "#lambda_{xb}",
		    "#mu_{yb}",
		    "#sigma_{yb}");
  qlpsd->SetMarkerStyle(1);
  qlpsd->Fit("fTot");
  fTot->Draw("cont1 same");
  qlpsd->SetMarkerStyle(1);

  // project onto tChargeL axis ----------------------------
  // -------------------------------------------------------
  TCanvas *tc2 = new TCanvas();
  tc2->cd();
  TH1D  *ql = qlpsd->ProjectionX();

  // Signal projection
  const Int_t nparS1 = 5;
  TF1 *fSigCL = new TF1("fSigCL",fSignal1D,
			xminS,xmaxS,nparS1);
  fSigCL->SetParameters(fTot->GetParameter(0)/ywidth,
			fTot->GetParameter(4),
			fTot->GetParameter(1),
			fTot->GetParameter(2),
			fTot->GetParameter(5));

  // Background projection
  const Int_t nparBCL = 3;
  TF1 *fBgCL = new TF1("fBgCL",fBackgroundCL,
		       xminB,xmaxB,nparBCL);
  fBgCL->SetParameters(fTot->GetParameter(6)/ywidth*2.,
		       fTot->GetParameter(9),
		       fTot->GetParameter(7));

  // total charge long projection
  const Int_t nparTCL = 8;
  TF1 *fTotCL = new TF1("fTotCL",fTotalCL,
  		      xmin,xmax,nparTCL);
  fTotCL->SetParameters(fSigCL->GetParameter(0),
			fSigCL->GetParameter(1),
			fSigCL->GetParameter(2),
			fSigCL->GetParameter(3),
			fSigCL->GetParameter(4),
			fBgCL->GetParameter(0),
			fBgCL->GetParameter(1),
			fBgCL->GetParameter(2));
  ql->Draw();
  fTotCL->Draw("same");
  fSigCL->SetLineColor(kBlue);
  fSigCL->Draw("same");
  fBgCL->SetLineColor(kRed);
  fBgCL->Draw("same");

  // -------------------------------------------------------

  // project onto tPSD axis --------------------------------
  // -------------------------------------------------------
  TCanvas *tc3 = new TCanvas();
  tc3->cd();
  TH1D *psd = qlpsd->ProjectionY();

  // Signal projection
  TF1 *fSigPSD = new TF1("fSigPSD",fSignal1D,
			 yminS,ymaxS,nparS1);
  fSigPSD->SetParameters(fTot->GetParameter(0)/xwidth,
			 fTot->GetParameter(2),
			 fTot->GetParameter(3),
			 fTot->GetParameter(4),
			 fTot->GetParameter(5));

  // Background projection
  const Int_t nparBPSD = 4;
  TF1 *fBgPSD = new TF1("fBgPSD",fBackgroundPSD,
		       yminB,ymaxB,nparBPSD);
  fBgPSD->SetParameters(fTot->GetParameter(6)/xwidth/2.,
			fTot->GetParameter(7),
			fTot->GetParameter(8),
			fTot->GetParameter(9));

  // total charge long projection
  const Int_t nparTPSD = 9;
  TF1 *fTotPSD = new TF1("fTotPSD",fTotalPSD,
			 ymin,ymax,nparTPSD);
  fTotPSD->SetParameters(fSigPSD->GetParameter(0),
			 fSigPSD->GetParameter(1),
			 fSigPSD->GetParameter(2),
			 fSigPSD->GetParameter(3),
			 fSigPSD->GetParameter(4),
			 fBgPSD->GetParameter(0),
			 fBgPSD->GetParameter(1),
			 fBgPSD->GetParameter(2),
			 fBgPSD->GetParameter(3));
  psd->Draw();
  fTotPSD->Draw("same");
  fSigPSD->SetLineColor(kBlue);
  fSigPSD->Draw("same");
  fBgPSD->SetLineColor(kRed);
  fBgPSD->Draw("same");
  // -------------------------------------------------------


  // subtract 2D hist and fit
  TCanvas *tc4 = new TCanvas();
  tc4->cd();
  sub->Add(fTot,-1.,"");
  sub->Draw("colz");

  //
  TCanvas *tc5 = new TCanvas();
  tc5->cd();
  qlqs->Draw("colz");
}

int main(){
  Fit2D();

  return 1;
}
