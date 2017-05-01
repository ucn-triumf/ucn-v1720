// SimpleFit.C
// 1st order fit to event energy spectra for specified pmt channel
// Background is assumed expo
// Signal is assumed gaus

#include "TMath.h"

TF1 *total, *sig, *bg;
 
double finter(double *x, double*par) {
   return TMath::Abs(sig->EvalPar(x,par) - bg->EvalPar(x,par));
}

SimpleFit(char *infilename = "output00000045_0000.root", int chan = 0){

  gStyle->SetOptFit(1111);

  char anum[100];
  TCanvas *tc1 = new TCanvas();
  tc1->cd();

  // open root file
  TFile* fin = new TFile(infilename,"read");
  TTree* tin = (TTree*) fin->Get("tUCN");

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

  // data to fit
  TH1D* hinC= new TH1D("hinC",
		       "Event Energy Spectrum;Total Charge (ADC);Counts",
		       100,xmin,xmax);
  sprintf(anum,
	  "tChannel == %d && tChargeL< %f && tChargeL>%f",
	  chan,
	  xmax,
	  xmin);
  tin->Draw("tChargeL>>hinC",anum);

  // for signal and background functions
  sigC = new TF1("sigC","gaus",xminS,xmaxS);
  sigC->SetLineColor(kBlue);
  hinC->Fit(sigC,"R");
  sigC->SetParameters(sigC->GetParameter(0),
		      sigC->GetParameter(1),
		      sigC->GetParameter(2));
  hinC->Fit(sigC,"R");
  bgC = new TF1("bgC","expo",xminB,xmaxB);
  bgC->SetLineColor(kRed);
  hinC->Fit(bgC,"R");

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
  hinC->Draw("same");
  //totalC->Draw("sames");  
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
  xminS = 0.3;
  xmaxS = xmax;

  // fitting range for charge background
  xminB = xmin;
  xmaxB = 0.2;

  // data to fit
  TH1D* hinP= new TH1D("hinP",
		       "Pulse Shape Spectrum;PSD;Counts",
		       100,xmin,xmax);
  sprintf(anum,
	  "tChannel == %d && tPSD< %f && tPSD>%f",
	  chan,
	  xmax,
	  xmin);
  tin->Draw("tPSD>>hinP",anum);

  // for signal and background functions
  bgP = new TF1("bgP","gaus",xminB,xmaxB);
  bgP->SetLineColor(kRed);
  hinP->Fit(bgP,"R");
  sigP = new TF1("sigP","gaus",xminS,xmaxS);
  sigP->SetLineColor(kBlue);
  hinP->Fit(sigP,"R");
  sigP->SetParameters(sigP->GetParameter(0),
		      sigP->GetParameter(1),
		      sigP->GetParameter(2));
  hinP->Fit(sigP,"R");

  // for full tChargeL fit
  totalP =  new TF1("totalP","gaus(0)+gaus(3)",xmin,xmax);
  totalP->SetParameters(bgP->GetParameter(0),
			bgP->GetParameter(1),
			bgP->GetParameter(2),
			sigP->GetParameter(0),
			sigP->GetParameter(1),
			sigP->GetParameter(2));
  //totalP->SetParNames("A","Slope","B","x_{avg}","\sigma");
  totalP->SetTitle("Pulse Shape Spectrum;PSD;Counts");
  totalP->Draw();
  hinP->Draw("same");
  sigP->Draw("same");
  bgP->Draw("same");
  // PSD fit ----------------------------------------------------
  // ------------------------------------------------------------


  // area where the two functions overlap
  TF1 *fint = new TF1("fint",finter,xmin,3000.,0);
  double xint = fint->GetMinimumX();
  //fint->Draw("lsame");
  //TMarker *m = new TMarker(xint,sig->Eval(xint),24);
  //m->SetMarkerColor(kGreen);
  //m->SetMarkerSize(3);
  //m->Draw();
  //printf("xint=%g\n",xint);
  double A123 = sigC->Integral(xmin,xmax);
  std::cout << "Area 1+2+3 = " << A123 << std::endl;
  double A12 = sigC->Integral(xmin,xint) + bgC->Integral(xint,xmax);
  std::cout << "Area 1+2 = " << A12 << std::endl;
  std::cout << "Area 3 = " << A123 - A12 << std::endl;


  double sigCTot =  sigC->Integral(xmin,xmax);
  std::cout << "signal : " << sigCTot << std::endl;

  double bgCTot = bgC->Integral(xmin,xmax);
  std::cout << "background : " << bgCTot << std::endl;

  std::cout << "purity : " << sigCTot / (sigCTot+bgCTot) << std::endl;
  std::cout << "contamination : " << bgCTot / (sigCTot+bgCTot) << std:: endl;

  // resolution
  double res = sigC->GetParameter(2) + 2 * TMath::Sqrt(TMath::Log(2));
  double rese = sigC->GetParError(2) + 2 * TMath::Sqrt(TMath::Log(2));
  double maxx = sigC->GetMaximumX(3000.,xmax);
  std::cout << "1 UCN detected resolution : " << res/maxx << " +/- " << rese/maxx << "%"<< std::endl;

  double mean = sigC->GetParameter(1);
  double sigCma = sigC->GetParameter(2);
  std::cout<<"\sigma / \mu = " << sigma/mean << std::endl;

}
