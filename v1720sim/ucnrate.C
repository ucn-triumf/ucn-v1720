
#include <TROOT.h>
#include <TF1.h>
#include <TH1D.h>
#include <TRandom.h>
#include <TMath.h>
#include <TDirectory.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TTree.h>
#include <TFile.h>

#include "V1720DigitizerMC.h"
#include "PMTSignal.h"

// Make an instance of the simulation class available globally
V1720DigitizerMC g1720mc; // use default parameters

// Make a truth tree to keep track of what was added to the wavetrain
TTree * ttrue;
float ttT;     // pulse time 
float ttBL;    // pulse true baseline
float ttAmp;   // pulse amplitude (mV)
float ttFall;  // pulse fall time (ns)
int   ttType;  // 0==scint 1=cerenkov
int   verbose; // control level of output

// Variables used to make wavetrain
PMTSignal * sig;  //scintillation signal
PMTSignal * csig; //cerenkov signal
TF1 * fsig;
TF1 * ascint;
TF1 * cfsig;
TF1 * acerenkov;
TH1D* hscope;
double nexpected;
int ncurrent;
double aSampRate;
double xmax;
int nbins;
double adelay;
double curnpe;  
int ibinpeak;

// Used in main
double ttt;
V1720PSDResult * p;

// simwavetrain -------------------------------------------------------
// Simulate a wavetrain for a given rate of 
// scints and cerenkovs (Hz).  Make the wavetrain
// with a length of count time seconds.
// Arguments:
//
//   nrate      -- rate of scintillation events (Hz)
//   grate      -- rate of cerenkov events (Hz)
//   count_time -- length of wavetrain to simulate
//   kDraw      -- draw segment?
//
// Assume: scintillations are about 50pe. and cerenkovs are about 20 pe.
//         single. pe are gaussians width 6.5ns amplitude 20+-20 gaussian threshold 5mVboth 
//         noise is about 2 mV.
// Return a root histogram with the wavetrain in it.
// Takes the digitization 
//
// B. Jamieson, L. Rebenitsch (2013-2014)
// --------------------------------------------------------------------
TH1D* simwavetrain( double nrate = 1000000.0, double grate = 0.0, double count_time = 0.1, bool kDraw = false){
  static bool first = true;

  // if first event for entire wavetrain, initialize TTree structures
  if ( first==true ){
    if(verbose)
      std::cout<<"First time, make tree"<<std::endl;
    ttrue = new TTree("TruePSD","TruePSD");
    ttrue->Branch("T",&ttT,"T/F");
    ttrue->Branch("Baseline",&ttBL,"Baseline/F");
    ttrue->Branch("Amp",&ttAmp,"Amp/F");
    ttrue->Branch("TFall",&ttFall,"TFall/F");
    ttrue->Branch("Type",&ttType,"Type/I");
    first=false;
  }

  sig = new PMTSignal();
  fsig = sig->GetSimpleSignal();
  fsig->SetNpx(4000);

  csig = new PMTSignal();
  csig->SetCerenkov(true);
  //csig->SetRiseTime(15.0);
  cfsig = csig->GetSimpleSignal();
  cfsig->SetNpx(4000);
  
  // How to simulate a random signal of a given rate?
  // In any given time interval the probability of having a new signal
  // should be equal.  And the number of signals should follow a 
  // Poisson distribution.
  nexpected = nrate * count_time;
  ncurrent  = gRandom->Poisson( nexpected );
  if (verbose) std::cout<<" expect "<<nexpected<<" simulating "<<ncurrent<<" scintillation,  in "<<count_time<<" seconds"<<std::endl;

  // set title based on rates being measured
  char atitle[256];
  sprintf(atitle,"scintillation at %4.1f kHz, cerenkov at %4.1f kHz;time (ns); signal (mV)",
	  nrate/1000.0, grate/1000.0);
  if(verbose) std::cout<<"Simulating "<<atitle<<std::endl;

  // figure out how many bins we need (time axis is in ns)!
  aSampRate = g1720mc.GetSampRate();
  xmax = count_time * 1000000000.0;
  nbins = int(xmax / aSampRate);
  if (verbose) std::cout<<"sample rate="<<aSampRate<<" tMax = "<<xmax<<" ns, and nbins="<<nbins<<std::endl;

  // check if histogram exists, and delete it if it exists
  hscope = (TH1D*)gDirectory->Get("hscope");
  if (hscope) delete hscope;
  
  // initialize scope trace
  hscope = new TH1D("hscope", atitle, nbins, 0.0, xmax);
  //if(verbose) std::cout<<" hscope = "<<hscope<<std::endl;

  // scint signal pulse height distribution?
  // based on average of 50 p.e. collected per scint
  double ScintNPEAvg = 50;

  // simulate some noise
  double noiselevel = 4.0; // mV
  for (int ibin=1; ibin<=hscope->GetNbinsX(); ibin++){
    hscope->SetBinContent(ibin, gRandom->Gaus(0.0,noiselevel) );
  }

  // simulate the scints
  // only do for pulses that are above threshold?
  for (int i=0; i<int(ncurrent); i++){
    adelay = gRandom->Uniform(0.0, xmax);
    sig->SetTime(adelay);
    curnpe = gRandom->Poisson( ScintNPEAvg );  
    ascint = sig->GetSignal( curnpe );
    ttT = adelay;
    ttAmp = ascint->GetMaximum();
    ttFall = sig->GetFallTime();
    ttType = 0;
    // only loop over +- 100 bins of peak
    ibinpeak = int( adelay / aSampRate );

    ttBL = hscope->GetBinContent( ibinpeak );
    ttrue->Fill();

    // fill scopetrace with simulated scint signal
    for (int ibin=std::max<int>(1,ibinpeak-100); ibin<=std::min<int>(ibinpeak+1000,hscope->GetNbinsX()); ibin++){
      //if (ibin%100000==0) std::cout<<"\t"<<ibin<<std::endl;
      ttt = hscope->GetBinCenter(ibin);
      hscope->Fill(ttt, ascint->Eval(ttt) );
    }
  }



  // Now simulate the cerenkov events
  nexpected = grate * count_time;
  ncurrent  = gRandom->Poisson( nexpected );
  if (verbose) std::cout<<" Cerenkov expect "<<nexpected<<" simulating "<<ncurrent<<" cerenkov events,  in "<<count_time<<" seconds"<<std::endl;

  // cerenkov signal pulse height distribution?
  // based on average of 15 p.e. collected per cerenkov
  double CerenkovNPEAvg = 5;

  // simulate the cerenkov events
  // only do for pulses that are above threshold?
  for (int i=0; i<int(ncurrent); i++){
    adelay = gRandom->Uniform(0.0, xmax);
    csig->SetTime(adelay);
    curnpe = gRandom->Exp( CerenkovNPEAvg );  
    acerenkov = csig->GetSignal( curnpe );
    ttT = adelay;
    ttAmp = acerenkov->GetMaximum();
    ttFall = csig->GetFallTime();
    ttType = 1;
    // only loop over +- 100 bins of peak
    ibinpeak = int( adelay / aSampRate );

    ttBL = hscope->GetBinContent( ibinpeak );
    ttrue->Fill();

    // fill scopetrace with simulated scint signal
    for (int ibin=std::max<int>(1,ibinpeak-100); ibin<=std::min<int>(ibinpeak+1000,hscope->GetNbinsX()); ibin++){
      //if (ibin%100000==0) std::cout<<"\t"<<ibin<<std::endl;
      ttt = hscope->GetBinCenter(ibin);
      hscope->Fill(ttt, acerenkov->Eval(ttt) );
    }
  }

  //free up memory
  delete csig;   
  delete sig;    

  return hscope; // return simulated scope trace
}
// end simwavetrain ---------------------------------------------------


// main ---------------------------------------------------------------
// Main function to simulate signal and apply PSD analysis to said
// signal.  Has 4 arguments for determining type and length of signal.
//
//   runNum      -- run number
//   scintRate -- rate of scints in signal (Hz)
//   cerenkovRate   -- rate of cerenkovs in signal (Hz)
//   totalTime   -- length of signal (0.1s)
//
// After compiling, run simulation from script
// $ ./sim.sh
//
// Lori Rebenitsch
// Oct. 29, 2014
// --------------------------------------------------------------------
int main( int argc, const char* argv[]){

  verbose=true;

  char filename[256];
  if ( argc != 5 ){
    std::cout<<"Usage: "<<std::endl;
    std::cout<<" ucnrage NumRuns scintRate(Hz) cerenkovRate(Hz) totalTime(num 0.1s intervals) \n"<<std::endl;
    return 0;
  }

  int    runNum      = atoi(argv[1]);
  double scintRate = atof(argv[2]);
  double cerenkovRate   = atof(argv[3]);
  double totalTime   = atof(argv[4]);

  sprintf(filename,"ucnrateN%.1fG%.1fT%.1fRun%02d.root",scintRate,cerenkovRate,totalTime/10.0,runNum);
  
  TFile * fout = new TFile((const char *)filename,"recreate");
    
  TTree * tout = new TTree("TPSD","TPSD");
  float tT;
  float tQS;
  float tQL;
  float tBL;
  tout->Branch("T", &tT, "T/F");
  tout->Branch("QS",&tQS,"QS/F");
  tout->Branch("QL",&tQL,"QL/F");
  tout->Branch("BL",&tBL,"BL/F");


  for(int j=0; j<runNum; j++) {
    
    TH1D * hanalog;
    
    std::vector< V1720PSDResult > * psd;
    
    // simulate 10s of data (0.1 sec at a time, so 100 times
    for (int i=0; i < totalTime; i++){

      std::cout<<"  Simulating "<< float(i)*0.1<<" second of "<<totalTime*0.1<<" s"<<std::endl;
      if(verbose) {
	std::cout<<"==================================================="<<std::endl;
	std::cout<<"  Simulating "<< float(i)*0.1<<" second of "<<totalTime*0.1<<" s"<<std::endl;
	std::cout<<"==================================================="<<std::endl;
      }
      
      // start with all scints at 1000 kHz 
      // simulate 0.1 s long wavetrains at a time
      // takes a lot of memory, so cant do a full 10s at a time
      hanalog = simwavetrain( scintRate, cerenkovRate, 0.1, false ); // simulate 0.1 s segment of wavetrain
      psd     = g1720mc.DoPSDAnalysis( hanalog );             // analyze segment of wavetrain
      
      // save segment simulation information to file
      for (int j=0; j< psd->size(); j++){
	p = &(psd->at(j));
	tT = p->fTime;
	tQS = p->fQS;
	tQL = p->fQL;
	tBL = p->fBL;
	tout->Fill();
      }
    } // end simulating wavetrain
    //hanalog->Scale(-1.0);
    // uncomment below if you want histograms of what parts of the pulses
    // are used for the short and long gate charge sums
    TH1D * hshort = g1720mc.GetShortGateHisto();
    TH1D * hlong = g1720mc.GetLongGateHisto();
    
    // write data to file and close
    fout->Write();
    //fout->Close();
  }

  fout->Close();
  return 0;
}
// end main -----------------------------------------------------------
