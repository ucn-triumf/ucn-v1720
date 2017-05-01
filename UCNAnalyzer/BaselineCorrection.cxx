// .cxx file for BaselineCorrection methods
#include "BaselineCorrection.h"
//
TF1 *BaselineCorrection::fChargeL(TTree * tin, int chan){

  // fit ranges - set to avoid overflow
  cmin = 2000;
  cmax = 40000;
  bmin = 1800;
  bmax = 2200;

  // set up fit  
  sprintf(anum,
	  "tChannel == %d && tChargeL< %f && tChargeL>%f",
	  chan,
	  cmax,
	  cmin);

  // data to be fitted
  tc1 = new TCanvas();
  tc1->cd();
  hChargeL = new TH2D("hChargeL","hChargeL",
		      cmax-cmin+1,cmin,cmax,
		      bmax-bmin+1,bmin,bmax);
  tin->Draw("tBaseline:tChargeL>>hChargeL",anum);

  // profile
  pChargeL = hChargeL->ProfileX("e");

  // fit for channel
  fCL = new TF1("fCL","pol1",cmin,cmax);
  pChargeL->Fit(fCL);

  return fCL;
}

//
TF1 *BaselineCorrection::fChargeS(TTree * tin, int chan){

  // fit ranges - set to avoid overflow
  cmin = 2000;
  cmax = 40000;
  bmin = 1800;
  bmax = 2200;
  sprintf(anum,
	  "tChannel == %d && tChargeS< %f && tChargeS>%f",
	  chan,
	  cmax,
	  cmin);

  // data to be fitted
  tc1 = new TCanvas();
  tc1->cd();
  hChargeS = new TH2D("hChargeS","hChargeS",
		      cmax-cmin+1,cmin,cmax,
		      bmax-bmin+1,bmin,bmax);
  tin->Draw("tBaseline:tChargeS>>hChargeS",anum);

  // profile
  pChargeS = hChargeS->ProfileX("e");

  // fit for channel
  fCS = new TF1("fCS","pol1",cmin,cmax);
  pChargeS->Fit(fCS);
  
  return fCS;
}
