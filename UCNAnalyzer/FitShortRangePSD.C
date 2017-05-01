#include "TMath.h"

TF1 *total, *sig, *bg;
 
double finter(double *x, double*par) {
   return TMath::Abs(sig->EvalPar(x,par) - bg->EvalPar(x,par));
}

FitShortRangePSD(char *infilename = "output00000045_0000.root", int chan = 0){

  gStyle->SetOptFit(1111);

  double xmin = -0.2; 
  double xmax = 0.75;

  // double expConst = 12.;
  // double expSlope = -0.003;
  // //double constNoise = 0.;
  double gausEstMax = 100000;
  double gausEstX = 0.6; 
  double gausEstSig = 0.06; 
  double gauslEstMax = 10000.;
  double gauslEstX = -0.05;
  double gauslEstSig = 0.05;
  char anum[100];
  TCanvas *tc1 = new TCanvas();
  tc1->cd();

  // open root file
  TFile* fin = new TFile(infilename,"read");
  TTree* tin = (TTree*) fin->Get("tUCN");
  TH1D* hin1= new TH1D("hin1","hin1",100,xmin,xmax);
  sprintf(anum,"tChannel == %d && tPSD< %f && tPSD>%f",chan,xmax,xmin);
  tin->Draw("tPSD>>hin1",anum);

  // for full tPSD fit
  total =  new TF1("total","gaus(0)+gaus(3)+gaus(6)",xmin,xmax);
  total->SetParameters(gausEstMax,gausEstX,gausEstSig,
    		       gauslEstMax,gauslEstX,gauslEstSig,
		       gauslEstMax/100.,gauslEstX,gauslEstSig*100.);
  hin1->Fit(total,"");

  gausEstMax  = total->GetParameter(2);
  gausEstX    = total->GetParameter(3); 
  gausEstSig  = total->GetParameter(4); 
  gauslEstMax = total->GetParameter(5);
  gauslEstX   = total->GetParameter(6);
  gauslEstSig = total->GetParameter(7);
  total->SetParameters(gausEstMax,gausEstX,gausEstSig,
   		       gauslEstMax,gauslEstX,gauslEstSig,
		       gauslEstMax/100.,gauslEstX,gauslEstSig*100.);
  hin1->Fit(total,"");

  // for signal and background functions
  sig = new TF1("b","gaus(0)+gaus(3)",xmin,xmax);
  sig->SetParameters(total->GetParameter(2),
  		     total->GetParameter(3),
  		     total->GetParameter(4),
  		     total->GetParameter(5),
  		     total->GetParameter(6),
  		     total->GetParameter(7));
  sig->SetLineColor(kBlue);
  sig->Draw("same");
  bg = new TF1("bg","expo(0)",xmin,xmax);
  bg->SetParameters(total->GetParameter(0),
  		    total->GetParameter(1));
  bg->SetLineColor(kRed);
  bg->Draw("same");

  // // area where the two functions overlap
  // TF1 *fint = new TF1("fint",finter,xmin,3000.,0);
  // double xint = fint->GetMinimumX();
  // //fint->Draw("lsame");
  // //TMarker *m = new TMarker(xint,sig->Eval(xint),24);
  // //m->SetMarkerColor(kGreen);
  // //m->SetMarkerSize(3);
  // //m->Draw();
  // //printf("xint=%g\n",xint);
  // double A123 = sig->Integral(xmin,xmax);
  // std::cout << "Area 1+2+3 = " << A123 << std::endl;
  // double A12 = sig->Integral(xmin,xint) + bg->Integral(xint,xmax);
  // std::cout << "Area 1+2 = " << A12 << std::endl;
  // std::cout << "Area 3 = " << A123 - A12 << std::endl;


  // // pile up fit
  // TF1* lf = new TF1("lf","gaus",xmin,xmax);
  // lf->SetParameters(total->GetParameter(5),
  // 		    total->GetParameter(6),
  // 		    total->GetParameter(7));
  // lf->SetParError(0,total->GetParError(5));
  // lf->SetParError(1,total->GetParError(6));
  // lf->SetParError(2,total->GetParError(7));
  // lf->SetLineColor(kGreen);
  // lf->Draw("same");
  // TF1* mf = new TF1("mf","gaus",xmin,xmax);
  // mf->SetParameters(total->GetParameter(2),
  // 		    total->GetParameter(3),
  // 		    total->GetParameter(4));
  // mf->SetParError(0,total->GetParError(2));
  // mf->SetParError(1,total->GetParError(3));
  // mf->SetParError(2,total->GetParError(4));
  // mf->SetLineColor(kGreen);
  // mf->Draw("same");

  // double sigTot =  sig->Integral(xmin,xmax);
  // std::cout << "signal : " << sigTot << std::endl;

  // double bgTot = bg->Integral(xmin,xmax);
  // std::cout << "background : " << bgTot << std::endl;

  // std::cout << "purity : " << sigTot / (sigTot+bgTot) << std::endl;
  // std::cout << "contamination : " << bgTot / (sigTot+bgTot) << std:: endl;

  // // resolution
  // double res = mf->GetParameter(2) + 2 * TMath::Sqrt(TMath::Log(2));
  // double rese = mf->GetParError(2) + 2 * TMath::Sqrt(TMath::Log(2));
  // double maxx = mf->GetMaximumX(3000.,xmax);
  // std::cout << "1 UCN detected resolution : " << res/maxx << " +/- " << rese/maxx << "%"<< std::endl;
  // double resl = lf->GetParameter(2) + 2 * TMath::Sqrt(TMath::Log(2));
  // double resel = lf->GetParError(2) + 2 * TMath::Sqrt(TMath::Log(2));
  // double maxxl = lf->GetMaximumX(xmin,xmax);
  // std::cout << resl/maxx << " +/- " << resel/maxxl << "% for lower fit "<< std::endl;


}
