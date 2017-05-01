#include "TMath.h"

TF1 *total, *sig, *bg;
 
double finter(double *x, double*par) {
   return TMath::Abs(sig->EvalPar(x,par) - bg->EvalPar(x,par));
}

FitShortRange(char *infilename = "output00000045_0000.root", int chan = 3){

  gStyle->SetOptFit(1111);

  double xmin = 600; 
  double xmax = 5000.;

  double expConst = 12.;
  double expSlope = -0.003;
  double constNoise = 0.;
  double gausEstMax = 100000;
  double gausEstX = 6000; 
  double gausEstSig = 1000.; 
  double gauslEstMax = 1000.;
  double gauslEstX = 3000;
  double gauslEstSig = 400;
  char anum[100];
  TCanvas *tc1 = new TCanvas();
  tc1->cd();

  // open root file
  TFile* fin = new TFile(infilename,"read");
  TTree* tin = (TTree*) fin->Get("tUCN");
  TH1D* hin1= new TH1D("hin1","hin1",100,xmin,xmax);
  sprintf(anum,"tChannel == %d && tChargeL< %f && tChargeL>%f",chan,xmax,xmin);
  tin->Draw("tChargeL>>hin1",anum);

  // for full tChargeL fit
  total =  new TF1("total","expo(0)+gaus(2)",xmin,xmax);
  total->SetParameters(expConst,expSlope,
   		       gausEstMax,gausEstX,gausEstSig);
  hin1->Fit(total,"");

  sfit = new TF1("sfit","expo(0)+gaus(2)+gaus(5)",xmin,xmax);
  sfit->SetParameter(0,total->GetParameter(0));
  sfit->SetParameter(1,total->GetParameter(1));
  sfit->SetParameter(2,total->GetParameter(2));
  sfit->SetParameter(3,total->GetParameter(3));
  sfit->SetParameter(4,total->GetParameter(4));
  hin1->Fit(sfit,"");
  
  // for signal and background functions
  sig = new TF1("b","gaus(0)+gaus(3)",xmin,xmax);
  sig->SetParameters(sfit->GetParameter(2),
  		     sfit->GetParameter(3),
  		     sfit->GetParameter(4),
  		     sfit->GetParameter(5),
  		     sfit->GetParameter(6),
  		     sfit->GetParameter(7));
  sig->SetLineColor(kBlue);
  sig->Draw("same");
  bg = new TF1("bg","expo(0)",xmin,xmax);
  bg->SetParameters(sfit->GetParameter(0),
  		    sfit->GetParameter(1));
  bg->SetLineColor(kRed);
  bg->Draw("same");

  // area where the two functions overlap
  TF1 *fint = new TF1("fint",finter,xmin,3000.,0);
  double xint = fint->GetMinimumX();
  //fint->Draw("lsame");
  //TMarker *m = new TMarker(xint,sig->Eval(xint),24);
  //m->SetMarkerColor(kGreen);
  //m->SetMarkerSize(3);
  //m->Draw();
  //printf("xint=%g\n",xint);
  double A123 = sig->Integral(xmin,xmax);
  std::cout << "Area 1+2+3 = " << A123 << std::endl;
  double A12 = sig->Integral(xmin,xint) + bg->Integral(xint,xmax);
  std::cout << "Area 1+2 = " << A12 << std::endl;
  std::cout << "Area 3 = " << A123 - A12 << std::endl;


  // pile up fit
  TF1* lf = new TF1("lf","gaus",xmin,xmax);
  lf->SetParameters(sfit->GetParameter(5),
  		    sfit->GetParameter(6),
  		    sfit->GetParameter(7));
  lf->SetParError(0,sfit->GetParError(5));
  lf->SetParError(1,sfit->GetParError(6));
  lf->SetParError(2,sfit->GetParError(7));
  lf->SetLineColor(kGreen);
  lf->Draw("same");
  TF1* mf = new TF1("mf","gaus",xmin,xmax);
  mf->SetParameters(sfit->GetParameter(2),
  		    sfit->GetParameter(3),
  		    sfit->GetParameter(4));
  mf->SetParError(0,sfit->GetParError(2));
  mf->SetParError(1,sfit->GetParError(3));
  mf->SetParError(2,sfit->GetParError(4));
  mf->SetLineColor(kGreen);
  mf->Draw("same");

  double sigTot =  sig->Integral(xmin,xmax);
  std::cout << "signal : " << sigTot << std::endl;

  double bgTot = bg->Integral(xmin,xmax);
  std::cout << "background : " << bgTot << std::endl;

  std::cout << "purity : " << sigTot / (sigTot+bgTot) << std::endl;
  std::cout << "contamination : " << bgTot / (sigTot+bgTot) << std:: endl;

  // resolution
  double res = mf->GetParameter(2) + 2 * TMath::Sqrt(TMath::Log(2));
  double rese = mf->GetParError(2) + 2 * TMath::Sqrt(TMath::Log(2));
  double maxx = mf->GetMaximumX(3000.,xmax);
  std::cout << "1 UCN detected resolution : " << res/maxx << " +/- " << rese/maxx << "%"<< std::endl;
  double resl = lf->GetParameter(2) + 2 * TMath::Sqrt(TMath::Log(2));
  double resel = lf->GetParError(2) + 2 * TMath::Sqrt(TMath::Log(2));
  double maxxl = lf->GetMaximumX(xmin,xmax);
  std::cout << resl/maxx << " +/- " << resel/maxxl << "% for lower fit "<< std::endl;


}
