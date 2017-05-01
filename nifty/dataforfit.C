// dataforfit.C
// root script for fitting data according to simulated data template
// 
#include "ini.h"
#include "INIReader.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TCanvas.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

INIReader gConfig("dataforfit.ini");

const double QMEANEQ = gConfig.GetReal("fitparameter","meanq",7201.53);
const double QSIGEQ =  gConfig.GetReal("fitparameter","sigq",1009.59);
const double PMEANEQ = gConfig.GetReal("fitparameter","meanp",0.577457);
const double PSIGEQ =  gConfig.GetReal("fitparameter","sigp",0.0730255);
int cut = gConfig.GetReal("input","cut",0);

double mygaus( double * xx, double * pp ){
  double xv = xx[0];
  double N  = pp[0];
  double mu = pp[1];
  double sig = pp[2];

  double retval = N * TMath::Exp( -0.5 * (xv-mu) * (xv-mu) / sig / sig );

  return retval;
}

void dataforfit(){//char * infilename = "output00000054_0000.root"){


  //gROOT->SetStyle("Plain");
  //gStyle->SetOptFit(1111);
  //gStyle->SetTitle(0);
  //gStyle->SetOptStat(0);
  // open file and trees to be read
  //if (!( infilename && (*infilename) )) return; 

  // load in data file
  std::string strfname = gConfig.Get("input","filename","output00000054_0000.root");
  TFile * fin = new TFile((const char *)strfname.c_str(),"read");
  //TFile* fin = new TFile(infilename,"read");
  TTree* din = (TTree*) fin->Get("tUCN");
  if (!din) { return; }

  std::cout<<"Files opened... got tree"<<std::endl;
  
  // branches of tUCN
  ULong64_t tEntry;
  ULong64_t tTimeE;
  UShort_t tChannel;
  UShort_t tLength;
  //Short_t tPulse[1000];
  Float_t tPSD;
  UShort_t tChargeL;
  UShort_t tChargeS;
  UShort_t tBaseline;
  
  // create output file for fitted data
  char foutname[100];
  sprintf( foutname,"dataforfit_%s",(const char *)strfname.c_str());
  std::cout<<"Open output file: "<<foutname<<std::endl;
  TFile * fout = new TFile( foutname, "recreate" );

  float qs;
  float ql;
  TTree * tout = new TTree("tdata","tdata");
  tout->Branch("QS",&qs,"QS/F");
  tout->Branch("QL",&ql,"QL/F");

  const int NCH=9;
  char aname[100];
  char atitle[200];

  // fit structures for ql
  TH1D* hql[NCH];
  TH1D* hqlr[NCH];
  TH2D* hpsdqlr[NCH];
  TH2D* hpsdqlrsum = new TH2D("hpsdqlrsum","All Equalized; CH%d Q_{L} (ADC); PSD", 150, 0.0, 17000.0, 40,-1.,1.); 
  for (int i=0; i<NCH; i++){
    sprintf(aname,"hql%02d",i);
    sprintf(atitle,"Channel %d Cut PSD>0.2; CH%d Q_{L} (ADC); Count",i,i);
    hql[i] = new TH1D(aname,atitle, 150, 0.0, 17000.0);
    sprintf(aname,"hqlr%02d",i);
    sprintf(atitle,"Channel %d Equalized Cut PSD>0.2; CH%d Q_{L} (ADC); Count",i,i);
    hqlr[i] = new TH1D(aname,atitle, 150, 0.0, 17000.0);
    sprintf(aname,"hpsdqlr%02d",i);
    sprintf(atitle,"Channel %d Equalized; CH%d Q_{L} (ADC); PSD",i,i);
    hpsdqlr[i] = new TH2D(aname,atitle, 150, 0.0, 17000.0, 40,-1.,1.);
  }

  // fit structures for psd
  TH1D* hpsd[NCH];
  TH1D* hpsdr[NCH];
  for (int i=0; i<NCH; i++){
    sprintf(aname,"hpsd%02d",i);
    sprintf(atitle,"Channel %d Cut PSD>0.2; CH%d PSD (ADC); Count",i,i);
    hpsd[i] = new TH1D(aname,atitle, 150, -2.0, 1.0);
    sprintf(aname,"hpsdr%02d",i);
    sprintf(atitle,"Channel %d Equalized Cut PSD>0.2; CH%d PSD (ADC); Count",i,i);
    hpsdr[i] = new TH1D(aname,atitle, 150, 0.0, 17000.0);
  }

  // tree for runtime event data
  din->SetBranchAddress("tEntry",    &tEntry);
  din->SetBranchAddress("tTimeE",    &tTimeE);
  din->SetBranchAddress("tChannel",  &tChannel);
  din->SetBranchAddress("tPSD",      &tPSD);
  din->SetBranchAddress("tChargeL",  &tChargeL);
  din->SetBranchAddress("tChargeS",  &tChargeS);
  din->SetBranchAddress("tBaseline", &tBaseline);
  din->SetBranchAddress("tLength",   &tLength);

  std::cout<<"Set Branch addresses!"<<std::endl;

  // if cut == 1, cut on pulse time
  // if cut == 0, no cut
  ULong64_t nentries= din->GetEntries();
  std::vector<Long64_t> pulseT; // vector of pulse times
  if (cut) {
    din->GetEntry(0);
    Double_t eTally = 0;
    ULong64_t tTally = tTimeE;
    Double_t maxRate = gConfig.GetReal("background","max",0.0);
    for (int i=0;i<nentries;i++) {
      din->GetEntry(i);
      
      // get events from centre channel
      if(tChannel != 9) continue;
      
      // check tally time
      if (tTally > tTimeE) continue;
      
      // tally rate for second
      eTally++;
      
      // check if pulse time has GV open or closed
      // based on rate
      if (tTimeE>tTally+2e9){
	
	// if large, GV open
	// add pulse time to list of pulses
	if(eTally>maxRate*2){
	  pulseT.push_back(tTimeE);
	  // push ahead to prevent double counting
	  tTally+=280*1e9;
	}
	
	// reset for next second
	tTally += 2e9;
	eTally = 0;
      }
    }
  }
  std::cout << "GV open pulses found " << pulseT.size() << std::endl;

  // First pass, fill ql histograms to find 
  // mean and sigma of neutron peak for equalization
  // of channels
  int ch;
  int k=0;
  for (ULong64_t j=0; j<=nentries; j++){
    if (j%1000000==0) std::cout<<j<<" / "<<nentries<<std::endl;
    din->GetEvent(j);
  
    // if cutting on pulse time
    if(cut && tTimeE < pulseT[k]) continue;

    ch = tChannel;
    if (tChannel==7) ch=-1;
    if (tChannel==8||tChannel==9) ch=tChannel-1;
    if (ch>=0 && ch<=NCH) {
      double psd = -2.0;
      if (tChargeL != 0 ) psd = (double(tChargeL)-double(tChargeS))/double(tChargeL);
      if ( psd > 0.2 ) {
	hql[ch]->Fill( tChargeL );	
	hpsd[ch]->Fill( psd );
      }
    }

    // update reference pulse time if necessary
    if (cut && tTimeE > pulseT[k]+270*1e9)
      k++;
  }

  std::cout<<"Histograms were filled!"<<std::endl;
  
  // Now Fit them
  // specifically find the signal height for each channel
  double ameanq[NCH];
  double asigq[NCH];
  TF1* afitq[NCH];
  double ameanp[NCH];
  double asigp[NCH];
  TF1* afitp[NCH];
  for (int ich=0; ich<NCH; ich++){

    // ql
    sprintf(aname,"fit%d",ich);
    afitq[ich] = new TF1( aname, mygaus, 0., 17000.0, 3);
    afitq[ich]->SetParameters(hql[ich]->GetMaximum(), hql[ich]->GetMean(), hql[ich]->GetRMS());

    // psd
    afitp[ich] = new TF1( aname, mygaus, -2., 1.0, 3);
    afitp[ich]->SetParameters(hpsd[ich]->GetMaximum(), hpsd[ich]->GetMean(), hpsd[ich]->GetRMS());

    // first fit attempt

    // ql
    hql[ich]->Fit( afitq[ich], "Q" );
    ameanq[ich] = afitq[ich]->GetParameter(1);
    asigq[ich] = afitq[ich]->GetParameter(2);
    std::cout<<"Fit1 Ch"<<ich<<" mean="<<ameanq[ich]<<" sig="<<asigq[ich]<<std::endl;

    // psd
    hpsd[ich]->Fit( afitp[ich], "P" );
    ameanp[ich] = afitp[ich]->GetParameter(1);
    asigp[ich] = afitp[ich]->GetParameter(2);
    std::cout<<"Fit1 Ch"<<ich<<" mean="<<ameanp[ich]<<" sig="<<asigp[ich]<<std::endl;

    // second fit attempt

    // ql
    hql[ich]->Fit( afitq[ich],"Q","", 
		   ameanq[ich]-2.0*asigq[ich],
		   ameanq[ich]+2.0*asigq[ich] );
    ameanq[ich] = afitq[ich]->GetParameter(1);
    asigq[ich] = afitq[ich]->GetParameter(2);
    std::cout<<"Fit2 Ch"<<ich<<" mean="<<ameanq[ich]<<" sig="<<asigq[ich]<<std::endl;

    // psd
    hpsd[ich]->Fit( afitp[ich],"P","", 
		   ameanp[ich]-2.0*asigp[ich],
		   ameanp[ich]+2.0*asigp[ich] );
    ameanp[ich] = afitp[ich]->GetParameter(1);
    asigp[ich] = afitp[ich]->GetParameter(2);
    std::cout<<"Fit2 Ch"<<ich<<" mean="<<ameanp[ich]<<" sig="<<asigp[ich]<<std::endl;

    // final fit attempt

    // ql
    hql[ich]->Fit( afitq[ich],"Q","", 
		   ameanq[ich]-1.0*asigq[ich],
		   ameanq[ich]+1.0*asigq[ich] );
    ameanq[ich] = afitq[ich]->GetParameter(1);
    asigq[ich] = afitq[ich]->GetParameter(2);
    std::cout<<"Fit3 Ch"<<ich<<" mean="<<ameanq[ich]<<" sig="<<asigq[ich]<<std::endl;

    // psd
    hpsd[ich]->Fit( afitp[ich],"P","", 
		   ameanp[ich]-1.0*asigp[ich],
		   ameanp[ich]+1.0*asigp[ich] );
    ameanp[ich] = afitp[ich]->GetParameter(1);
    asigp[ich] = afitp[ich]->GetParameter(2);
    std::cout<<"Fit3 Ch"<<ich<<" mean="<<ameanp[ich]<<" sig="<<asigp[ich]<<std::endl;
  }



  // draw channels plus fit
  int order[NCH] = { 1, 2, 3, 0, 8, 4, 7, 6, 5 };

  TCanvas* c1 = new TCanvas();
  c1->Divide(3,3);
  for (int i=0; i<NCH; i++){
    c1->cd( i+1 );
    c1->GetPad( i+1 )->SetLogz();
    hql[ order[i] ]->Draw();
  }

  TH1D* hqlforgammas = new TH1D("hqlforgammas","Q_L (ADC) for gammas (-0.2 lt psd lt 0.05); Q_L(ADC); count", 150, 0.0, 17000.0);



  // Now second pass over data, fill equalized histos
  // use previous fit to normalize the out the charge variation
  // over the 9 channels
  ULong64_t theLastT[9]={0,0,0,0,0,0,0,0,0};
  k=0;
  for (ULong64_t j=0; j<=nentries; j++){
    if (j%100000==0) std::cout<<j<<" / "<<nentries<<std::endl;
    din->GetEvent(j);
    if ( j==0 ) {
      for (int i=0; i<9; i++ ) theLastT[i] = tTimeE;
    }

    // if cutting on pulse time
    if(cut && tTimeE < pulseT[k]+5e9) continue;

    ch = tChannel;
    if (tChannel==7) ch=-1;
    if (tChannel==8||tChannel==9) ch=tChannel-1;
    if (ch>=0 && ch<=NCH) {
      double qlcor = double(tChargeL)* QMEANEQ / ameanq[ch];
      double qscor = double(tChargeS)* QMEANEQ / ameanq[ch];
      double psd = (double( tChargeL) - double(tChargeS))/double(tChargeL);
      //if (qlcor != 0 ) psdcor = ( qlcor - qscor)/qlcor;
      double psdcor = -2.0;
      if (qlcor != 0 && qscor !=0) psdcor = psd * PMEANEQ / ameanp[ch];

      //std::cout<<" Q="<<tChargeL<<" Qcor="<<qlcor<<" QMEANEQ="<<QMEANEQ<<" amean="<<ameanq[ch]<<std::endl;
      //std::cout<<" P="<<psd<<" Pcor="<<psdcor<<" PMEANEQ="<<PMEANEQ<<" amean="<<ameanp[ch]<<std::endl;
      if ( psdcor > 0.2 ) {
	hqlr[ch]->Fill( qlcor );
	hpsdr[ch]->Fill( psdcor );
      }
     
      hpsdqlr[ch]->Fill( qlcor, psdcor );
      hpsdqlrsum->Fill( qlcor, psdcor );
      qs = qscor;
      ql = qlcor;
      
      ULong64_t deltat = tTimeE - theLastT[ ch ] ;
      if (psdcor > -0.15 && psdcor < 0.05 && deltat > 5000.0 ) {
	hqlforgammas->Fill( ql );
      }
      //std::cout << "fill" << std::endl;
      theLastT[ch] = tTimeE;
      tout->Fill();

      // update reference pulse time if necessary
      if (cut && tTimeE > pulseT[k]+270*1e9)
	k++;
    }
  }


  
  // Second Fit to check equalization them
  // Fit over the now normalized data
  double ameanqr[NCH];
  double asigqr[NCH];
  TF1* afitqr[NCH];
  double ameanpr[NCH];
  double asigpr[NCH];
  TF1* afitpr[NCH];
  for (int ich=0; ich<NCH; ich++){
    sprintf(aname,"fitr%d",ich);

    // ql
    afitqr[ich] = new TF1( aname, mygaus, 0., 17000.0, 3);
    afitqr[ich]->SetParameters(hqlr[ich]->GetMaximum(), hqlr[ich]->GetMean(), hqlr[ich]->GetRMS());

    // psd
    afitpr[ich] = new TF1( aname, mygaus, -2., 1.0, 3);
    afitpr[ich]->SetParameters(hpsdr[ich]->GetMaximum(), hpsdr[ich]->GetMean(), hpsdr[ich]->GetRMS());

    // first fit attempt

    // ql
    hqlr[ich]->Fit( afitqr[ich], "Q" );
    ameanqr[ich] = afitqr[ich]->GetParameter(1);
    asigqr[ich] = afitqr[ich]->GetParameter(2);
    std::cout<<"Fit1 Ch"<<ich<<" mean="<<ameanqr[ich]<<" sig="<<asigqr[ich]<<std::endl;

    // psd
    hpsdr[ich]->Fit( afitpr[ich], "P" );
    ameanpr[ich] = afitpr[ich]->GetParameter(1);
    asigpr[ich] = afitpr[ich]->GetParameter(2);
    std::cout<<"Fit1 Ch"<<ich<<" mean="<<ameanpr[ich]<<" sig="<<asigpr[ich]<<std::endl;

    // second fit attempt

    // ql
    hqlr[ich]->Fit( afitqr[ich],"Q","", 
		   ameanqr[ich]-2.0*asigqr[ich],
		   ameanqr[ich]+2.0*asigqr[ich] );
    ameanqr[ich] = afitqr[ich]->GetParameter(1);
    asigqr[ich] = afitqr[ich]->GetParameter(2);
    std::cout<<"Fit2 Ch"<<ich<<" mean="<<ameanqr[ich]<<" sig="<<asigqr[ich]<<std::endl;

    // psd
    hpsdr[ich]->Fit( afitpr[ich],"P","", 
    		    ameanpr[ich]-2.0*asigpr[ich],
    		    ameanpr[ich]+2.0*asigpr[ich] );
    ameanpr[ich] = afitpr[ich]->GetParameter(1);
    asigpr[ich] = afitpr[ich]->GetParameter(2);
    std::cout<<"Fit2 Ch"<<ich<<" mean="<<ameanpr[ich]<<" sig="<<asigpr[ich]<<std::endl;

    // last fit attempt

    // ql
    hqlr[ich]->Fit( afitqr[ich],"Q","", 
		   ameanqr[ich]-1.0*asigqr[ich],
		   ameanqr[ich]+1.0*asigqr[ich] );
    ameanqr[ich] = afitqr[ich]->GetParameter(1);
    asigqr[ich] = afitqr[ich]->GetParameter(2);
    std::cout<<"Fit3 Ch"<<ich<<" mean="<<ameanqr[ich]<<" sig="<<asigqr[ich]<<std::endl;

    // psd
    hpsdr[ich]->Fit( afitpr[ich],"P","", 
    		   ameanpr[ich]-1.0*asigpr[ich],
    		   ameanpr[ich]+1.0*asigpr[ich] );
    ameanpr[ich] = afitpr[ich]->GetParameter(1);
    asigpr[ich] = afitpr[ich]->GetParameter(2);
    std::cout<<"Fit3 Ch"<<ich<<" mean="<<ameanpr[ich]<<" sig="<<asigpr[ich]<<std::endl;
  }



  TCanvas* c2 = new TCanvas();
  c2->Divide(3,3);
  for (int i=0; i<NCH; i++){
    c2->cd( i+1 );
    c2->GetPad( i+1 )->SetLogz();
    hqlr[ order[i] ]->Draw();
  }


  TCanvas* c3 = new TCanvas();
  c3->Divide(3,3);
  for (int i=0; i<NCH; i++){
    c3->cd( i+1 );
    c3->GetPad( i+1 )->SetLogz();
    hpsdqlr[ order[i] ]->Draw("colz");
  }

  TCanvas* c4 = new TCanvas();
  c4->GetPad(0)->SetLogz();
  hpsdqlrsum->Draw("colz");

  TCanvas* c5 = new TCanvas();
  hqlforgammas->Draw();

  fout->Write();

  // save fit parameters to text file
  strfname = gConfig.Get("output","fitfilename","output00000054_0000.txt");
  ofstream fitout;
  fitout.open((const char *)strfname.c_str());
  fitout << "Fit parameters for " << strfname << std::endl;
  for (int ich=0; ich<NCH; ich++){
    fitout << ich << " " << ameanq[ich] << " " << asigq[ich]
	   << " " << ameanp[ich] << " " << asigp[ich] << std::endl;
  }
  fitout.close();

  std::cout<<"Done!"<<std::endl;
}

int main(int argc, char * argv[]) {

  if (gConfig.ParseError() < 0) {
    std::cout << "Can't load 'dataforfit.ini'\n";
    return 1;
  }
  
  dataforfit();

  return 0;
}
