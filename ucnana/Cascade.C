

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
  
  CascadeTOF *ctof = new CascadeTOF("T140815_0058_2");
  ctof->GetHist()->Draw();

}
