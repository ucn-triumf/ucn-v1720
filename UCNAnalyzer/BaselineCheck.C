// ---------------------------------------------------------------
// Macro for plotting baseline vs charge for each channel
// ---------------------------------------------------------------

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TVConstants.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

void BaselineCheck(char * infilename="output00000150_0000.root") {
  // make output file

  TFile * fin = new TFile(infilename,"read");

  const int NCH=16;
  TH2D* hbaseline[ NCH ];
  char aname[100];
  char atitle[100];
  for (int i=0; i<NCH; i++){
    sprintf(aname,"hbaseline%02d",i);
    sprintf(atitle," Channel %02d; Q_L (ADC); Baseline (ADC)",i);
    hbaseline[i]=new TH2D(aname,atitle,150,0.,15000.0,400,1500.,2500.0);
  }

  ULong64_t tTimeE;
  UShort_t tChannel;
  UShort_t tChargeL;
  UShort_t tChargeS;
  UShort_t tBaseline;

    // length of a second in clock ticks:
  sec = 250000000;

  TTree* uin = (TTree*) fin->Get("tUCN");
  uin->SetBranchAddress("tTimeE",&tTimeE);
  uin->SetBranchAddress("tChannel",&tChannel);
  uin->SetBranchAddress("tChargeL",&tChargeL);
  uin->SetBranchAddress("tChargeS",&tChargeS);
  uin->SetBranchAddress("tBaseline",&tBaseline);

  eventTot = (Double_t)uin->GetEntries();
      
  for(int j=0;j<eventTot;j++) {
    uin->GetEvent(j);
  
    hbaseline[tChannel]->Fill( tChargeL, tBaseline );
  
  }

  TCanvas*c=new TCanvas();
  c->Divide(4,4);
  for (int i=0; i<NCH; i++){

    c->cd(i+1);
    hbaseline[i]->Draw("colz");
  }


  TCanvas*c=new TCanvas();
  c->Divide(4,4);
  for (int i=0; i<NCH; i++){

    c->cd(i+1);
    hbaseline[i]->ProjectionY()->Draw();
  }



  

}
