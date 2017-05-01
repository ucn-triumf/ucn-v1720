#include "TMath.h"

TF1 *total, *sig, *bg;
 
double finter(double *x, double*par) {
   return TMath::Abs(sig->EvalPar(x,par) - bg->EvalPar(x,par));
}

DrawFun(char *infilename = "output00000045_0000.root"){

  gStyle->SetOptFit(1111);

  double xmin = 540.; 
  //double xmax = 11000.; // for channels 0, 1 and 2
  //double xmax = 9000.; // for channel 3 and 9
  double xmax = 11000.;
  int chan = 0;
  //double gaushXmin = 8500.; // for channel 0, 1 and 3 9
  //double gaushXmin = 7000.; // for channel 2
  double gaushXmin =8500; 
  double gausEstMax = 170000.; // for channels 0, 1 and 2 9
  //double gaus1EstMax = 30000.; // for channel 3
  //double gausEstX = 6500.;
  double gausEstX = 4874.; // for channels 0,1, 2 and 3
  //double gausEstSig = 980.;
  double gausEstSig = 712.2; // for channels 0,1 2 and 3
  double gauslEstMax = 945.8;
  double gauslEstX = 3290.;
  //double gauslEstX = 3000.;
  double gauslEstSig = 2479.;
  char anum[100];
  TCanvas *tc1 = new TCanvas();
  tc1->cd();

  TFile* fin = new TFile(infilename,"read");
  TTree* tin = (TTree*) fin->Get("tUCN");
  TH1D* hin1= new TH1D("hin1","hin1",100,xmin,xmax);
  sprintf(anum,"tChannel == %d && tChargeL< %f && tChargeL>%f",chan,xmax,xmin);
  //tin->Draw("tChargeL>>hin1","tChannel==1  && tChargeL<13000. && tChargeL>xmin");
  tin->Draw("tChargeL>>hin1",anum);

  // for full tChargeL fit
  TF1 *bh = new TF1("bh","gaus",gaushXmin,xmax);
  total =  new TF1("total","expo(0)+gaus(2)+gaus(5)+gaus(8)+pol2(11)",xmin,xmax);
  hin1->Fit(bh,"R");
  total->SetParameters(10.7,-0.002752,
		       gausEstMax,gausEstX,gausEstSig,
		       gauslEstMax,gauslEstX,gauslEstSig, 
		       129.2,8503,1333);
  hin1->Fit(total,"");
  total->SetParameter(8,bh->GetParameter(0));
  total->SetParameter(9,bh->GetParameter(1));
  total->SetParameter(10,bh->GetParameter(2));
  hin1->Fit(total,"");

  // for signal and background functions
  sig = new TF1("b","gaus(0)+gaus(3)+gaus(6)",xmin,xmax);
  sig->SetParameters(total->GetParameter(2),
  		     total->GetParameter(3),
  		     total->GetParameter(4),
  		     total->GetParameter(5),
  		     total->GetParameter(6),
  		     total->GetParameter(7),
  		     total->GetParameter(8),
  		     total->GetParameter(9),
  		     total->GetParameter(10));
  sig->SetLineColor(kBlue);
  sig->Draw("same");
  bg = new TF1("bg","expo(0)+pol2(2)",xmin,xmax);
  bg->SetParameters(total->GetParameter(0),
  		    total->GetParameter(1),
  		    total->GetParameter(11),
		    total->GetParameter(12),
		    total->GetParameter(13));
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
  TF1* hf = new TF1("hf","gaus",xmin,xmax); 
  hf->SetParameters(total->GetParameter(5),
  		    total->GetParameter(6),
  		    total->GetParameter(7));
  hf->SetParError(0,total->GetParError(5));
  hf->SetParError(1,total->GetParError(6));
  hf->SetParError(2,total->GetParError(7));
  hf->SetLineColor(kGreen);
  hf->Draw("same");
  TF1* lf = new TF1("lf","gaus",xmin,xmax);
  lf->SetParameters(total->GetParameter(8),
  		    total->GetParameter(9),
  		    total->GetParameter(10));
  lf->SetParError(0,total->GetParError(8));
  lf->SetParError(1,total->GetParError(9));
  lf->SetParError(2,total->GetParError(10));
  lf->SetLineColor(kGreen);
  lf->Draw("same");
  TF1* mf = new TF1("mf","gaus",xmin,xmax);
  mf->SetParameters(total->GetParameter(2),
  		    total->GetParameter(3),
  		    total->GetParameter(4));
  mf->SetParError(0,total->GetParError(2));
  mf->SetParError(1,total->GetParError(3));
  mf->SetParError(2,total->GetParError(4));
  mf->SetLineColor(kGreen);
  mf->Draw("same");

  TCanvas *tc2 = new TCanvas();
  tc2->cd();

  // TF1* e = new TF1("e","1.-expo(0)/(gaus(2)+gaus(5))",xmin,4000.);
  // e->SetParameters(9.171,-0.002211,3395000.,8065,1302,-116400000,90590,8973);
  // e->Draw();

  TF1* s = new TF1("s","1.-(expo(0))/(gaus(2)+gaus(5))",xmin,4000.);
  s->SetParameters(10.89,-0.003062,179000,4660,810.7,456.6,3863.,3073.);
  s->Draw();

  TCanvas *tc3 = new TCanvas();
  tc3->cd();

  TF1* e = new TF1("e","(gaus(2)+gaus(5))/(expo(0)+gaus(2)+gaus(5) +pol0(8))",xmin,4000.);
  e->SetParameters(10.89,-0.003062,179000,4660,810.7,456.6,3863.,3073.,12.37);
  e->Draw();

  TF1* b = new TF1("b","(expo(0)+pol0(8))/(expo(0)+gaus(2)+gaus(5) +pol0(8))",xmin,4000.);
  b->SetParameters(10.89,-0.003062,179000,4660,810.7,456.6,3863.,3073.,12.37);
  b->SetMarkerColor(kBlue);
  b->Draw("same");

  // TCanvas *tc4 = new TCanvas();
  // tc4->cd();

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
  double resh = lf->GetParameter(2) + 2 * TMath::Sqrt(TMath::Log(2));
  double reseh = lf->GetParError(2) + 2 * TMath::Sqrt(TMath::Log(2));
  double maxxh = lf->GetMaximumX(xmin,xmax);
  double resl = hf->GetParameter(2) + 2 * TMath::Sqrt(TMath::Log(2));
  double resel = hf->GetParError(2) + 2 * TMath::Sqrt(TMath::Log(2));
  double maxxl = hf->GetMaximumX(3000.,xmax);
  std::cout << "2 UCN detected resolution : " << std::endl;
  std::cout << resh/maxxh << " +/- " << reseh/maxxh << "% for higher fit "<< std::endl;
  std::cout << resl/maxx << " +/- " << resel/maxxl << "% for lower fit "<< std::endl;


}
