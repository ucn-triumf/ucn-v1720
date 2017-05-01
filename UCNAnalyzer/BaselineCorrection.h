// header for baseline correction

#ifndef _BaselineCorrection_h
#define _BaselineCorrection_h

#include "TMath.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TH2D.h"
#include "TTree.h"
#include "TProfile.h"

class BaselineCorrection {

 private:

  // fit ranges
  double cmin;
  double cmax;
  double bmin;
  double bmax;

  char anum[100];

  // data to be fitted
  TCanvas *tc1;
  TH2D *hChargeL;
  TH2D *hChargeS;

  // profile
  TProfile *pChargeL;
  TProfile *pChargeS;

  // functions
  TF1* fCL;
  TF1* fCS;

 public:

  BaselineCorrection(void){};
  ~BaselineCorrection(void){};

  TF1 *fChargeS(TTree * tin, int chan);
  TF1 *fChargeL(TTree * tin, int chan);

};
#endif
