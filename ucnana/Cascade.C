#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TGraph.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#include "CascadeTOF.h"


void Cascade(){

  TFile * outfile = new TFile("Cascade.root","recreate");
  TDirectory*topdir = outfile.GetDirectory("/");
  TDirectory*curdir=topdir;
  std::string curfile;
  ifstream infiles;
  infiles.open("infiles.txt");
  int lastdate = 0;
  while (infiles){
    infiles>>curfile;
    CascadeTOF ctof( curfile.c_str() );
    int curdate = ctof.GetDate();
    if (curdate!=lastdate){
      lastdate = curdate;
      std::stringstream ss;
      ss << curdate;
      curdir = topdir->mkdir( ss.str().c_str() );
      curdir->cd();
      ctof.GetHist()->SetDirectory( curdir );
      ctof.GetHist0()->SetDirectory( curdir );
    }
      
  }
  infiles.close();
  outfile->Write();
  outfile->Close();

  //TCanvas *c=new TCanvas();
  //c->Divide(1,2);
  //CascadeTOF *ctof = new CascadeTOF("T140815_0058_2");
  //c->cd(1);
  //ctof->GetHist0()->Draw();

  //c->cd(2);
  //ctof->GetHist()->Draw();

}
