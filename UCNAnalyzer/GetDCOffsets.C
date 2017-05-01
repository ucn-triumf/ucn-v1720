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
#include <string>
#include <sstream>


double MyGaus( double * xx,  double * pp ){
  // pp[0] = normalization
  // pp[1] = mean of gaussian
  // pp[2] = sigma of gaussian
  return pp[0] * TMath::Exp( -1.0*(xx[0]-pp[1])*(xx[0]-pp[1])/(2.0 * pp[2]*pp[2] ) );
}


void GetDCOffsets( char* infilename="output00000150_0000.root") {
  // make output file

  TFile * fin = new TFile(infilename,"read");

  ifstream is;
  is.open("BaselineSetValue");
  char * p = new char [30];
  int firstNum,secondNum,thirdNum,fourthNum;

  is.read(p, 36);

  firstNum = *(p+32)-48;
  secondNum = *(p+33)-48;
  thirdNum = *(p+34)-48;
  fourthNum = *(p+35)-48;

  const int NCH=16;
  TH1D* hbaseline[ NCH ];
  TF1* fbl[ NCH ];
  char aname[100];
  char atitle[100];
  int Offset[NCH] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  ofstream offsetFileMod1, offsetFileMod2;
  //ifstream getbsl;
  //string line;
  double baselineSetValue;
  cout<<firstNum<<" "<<secondNum<<" "<<thirdNum<<" "<<fourthNum<<endl;

  baselineSetValue =firstNum*1000 + secondNum*100 + thirdNum*10 + fourthNum;

  for (int i=0; i<NCH; i++){
    sprintf(aname,"hbaseline%02d",i);
    sprintf(atitle," Channel %02d; Q_L (ADC); Count",i);
    hbaseline[i]=new TH1D(aname,atitle,400,0.,4000.0);
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
  
    hbaseline[tChannel]->Fill( tBaseline );
  
  }
  //for (int j = 0; j<1; j++)
  // {
  //   getline(getbsl, line);
  // }
  //baselineSetValue = atoi(line.c_str());


  offsetFileMod1.open("DCOffsetsNeededMod1.txt");
  offsetFileMod2.open("DCOffsetsNeededMod2.txt");
  for (int i=0; i<NCH; i++){
    sprintf(aname,"fbl%02d",i);
    fbl[i] = new TF1(aname, MyGaus, 0., 4000., 3);
    fbl[i]->SetParameters( hbaseline[i]->GetMaximum(),
			   hbaseline[i]->GetMean(),
			   hbaseline[i]->GetRMS() );
    hbaseline[i]->Fit( fbl[i] );
    //if (i ==0) baselineSetValue = hbaseline[i]->GetMean();
    std::cout<<"BaselineSetValue "<<baselineSetValue<<std::endl;
    //std::cout<<"p+32"<<*(p+32)<<"\n"<<"p+33"<<*(p+33)<<"\n"<<"p+34"<<*(p+34)<<"\n"<<"p+35"<<*(p+35);
    // for(int i =0; i<36; i++)
    // {
    //std::cout<<"p+"<<i<<" "<<*(p+i)<<std::endl;
    //}

    //std::cout<<"Baseline "<<hbaseline[i]->GetMean()<<std::endl;

    Offset[i] = 32767.5 + 13.374*(hbaseline[i]->GetMean() - baselineSetValue);
    std::cout<<"Offset needed "<<Offset[i]<<std::endl;
    
    if (i >= 0 && i < 8) offsetFileMod1 << Offset[i] <<"\n";
    if (i >=8 && i<16) offsetFileMod2 << Offset[i] << "\n"; 


  }
  offsetFileMod1.close();
  offsetFileMod2.close();

  TCanvas*c=new TCanvas();
  c->Divide(4,4);
  for (int i=0; i<NCH; i++){

    c->cd(i+1);
    hbaseline[i]->Draw();
  }

}
