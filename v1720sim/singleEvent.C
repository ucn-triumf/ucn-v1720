// ------------------------------------------------------
// generate single signal
// 
// to make:
//    make
//    make singleEvent
//
// to run:
//    singleEvent <# of neutron events> <# of gamma events> <# of signals to simulate>
// ------------------------------------------------------
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
#include <TGraph.h>

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
double peak1;
double peak2;

// Used in main
double ttt;
V1720PSDResult * p;

// simWave ------------------------------------------------------------
// Simulate a wave for a given events of 
// scints and cerenkovs (Hz).  Make the wavetrain
// with a length of count time seconds.
// Arguments:
//
//   nsig      -- number of scintillation events 
//   nbg       -- number of cerenkov events
//   length    -- length of signal (ns)
//
// Assume: scintillations are about 50pe. and cerenkovs are about 20 pe.
//         single. pe are gaussians width 6.5ns amplitude 20+-20 gaussian threshold 5mVboth 
//         noise is about 2 mV.
// Return a root histogram with the wavetrain in it.
// Takes the digitization 
//
// B. Jamieson, L. Rebenitsch (2013-2014)
// --------------------------------------------------------------------
TH1D* simWave( int nsig = 1, int nbg = 0, int length = 600){
  static bool first = true;

  // set title based on rates being measured
  char atitle[256];
  sprintf(atitle,"scintillation of %d events, cerenkov of %d events;time (ns); signal (mV)",
	  nsig, nbg);
  if(verbose) std::cout<<"Simulating "<<atitle<<std::endl;

  // check if histogram exists, and delete it if it exists
  hscope = (TH1D*)gDirectory->Get("hscope");
  if (hscope) delete hscope;
  
  // initialize scope trace
  nbins = length;
  hscope = new TH1D("hscope", atitle, nbins+1, 0.0, length);

  // scint signal pulse height distribution?
  // based on average of 50 p.e. collected per scint
  double ScintNPEAvg = 50;

  // for double events
  // range of second pulse
  double minrange = 0.0;
  double maxrange = 300.0;

  // cerenkov signal pulse height distribution?
  // based on average of 15 p.e. collected per cerenkov
  //double CerenkovNPEAvg = 5;
  double CerenkovNPEAvg = 100;

  // simulate some noise
  double noiselevel = 4.0; // mV
  for (int ibin=1; ibin<=hscope->GetNbinsX(); ibin++){
    hscope->SetBinContent(ibin, gRandom->Gaus(0.0,noiselevel) );
  }

  // simulate the scints
  // only do for pulses that are above threshold?

  if(nsig > 0) {

    // create new signal
    sig = new PMTSignal();
    fsig = sig->GetSimpleSignal();
    fsig->SetNpx(4000);
    
    for (int i=0; i<nsig; i++){
      
      // if first event set to 1/4 position
      if (i == 0) {
	adelay = length / 4;
	peak1 = adelay;
      }
      // else, place event in randomized range
      else {
	adelay = gRandom->Uniform(peak1+minrange, peak1+maxrange);
	peak2 = adelay;
      }
      sig->SetTime(adelay);
      curnpe = gRandom->Poisson( ScintNPEAvg );  
      ascint = sig->GetSignal( curnpe );
      // only loop over +- 100 bins of peak
      ibinpeak = int( adelay / aSampRate );
      
      // fill scopetrace with simulated scint signal
      for (int ibin=1; ibin<=nbins; ibin++){
	ttt = hscope->GetBinCenter(ibin);
	hscope->Fill(ttt, ascint->Eval(ttt) );
      }
    }
  } // end adding scintillation signal

  // simulate the cerenkov events
  if(nbg > 0) {

    // create new Cerenkhov signal
    csig = new PMTSignal();
    csig->SetCerenkov(true);

    cfsig = csig->GetSimpleSignal();
    cfsig->SetNpx(4000);

    // only do for pulses that are above threshold?
    for (int i=0; i<nbg; i++){

      // if first event set to 1/4 position
      if (i == 0) {
	adelay = length / 4;
	peak1 = adelay;
      }
      // else, place event in randomized range
      else {
	adelay = gRandom->Uniform(peak1+minrange, peak1+maxrange);
	peak2 = adelay;
      }
      csig->SetTime(adelay);
      curnpe = gRandom->Exp( CerenkovNPEAvg );  
      acerenkov = csig->GetSignal( curnpe );
      
      // fill scopetrace with simulated scint signal
      for (int ibin=1; ibin<=nbins; ibin++){
	ttt = hscope->GetBinCenter(ibin);
	hscope->Fill(ttt, acerenkov->Eval(ttt) );
      }
    }
  } // end adding Cerenkhov events
  
  //free up memory
  delete csig;   
  delete sig;    

  return hscope; // return simulated scope trace
}
// end simWave --------------------------------------------------------


// main ---------------------------------------------------------------
// Main function to simulate signal and apply PSD analysis to said
// signal.  Has 4 arguments for determining type and length of signal.
//
//   numScint -- number of scintillator events
//   numCeren -- number of Cerenkhov events
//
//
// Lori Rebenitsch
// Oct. 29, 2014
// --------------------------------------------------------------------
int main( int argc, const char* argv[]){

  verbose=true;

  if ( argc != 4 ){
    std::cout<<"Usage: "<<std::endl;
    std::cout<<" singleEvent numScint numCeren numSig\n"<<std::endl;
    return 0;
  }

  char filename[256];
  sprintf(filename,"singleBg.root");
  TFile *fout = new TFile((const char *)filename,"recreate");
  TTree *tout = new TTree("TPSD","TPSD");

  float tT;
  float tQS;
  float tQL;
  float tBL;
  float tTR;
  float tR;
  float tB;
  tout->Branch("T", &tT, "T/F");
  tout->Branch("QS",&tQS,"QS/F");
  tout->Branch("QL",&tQL,"QL/F");
  tout->Branch("BL",&tBL,"BL/F");
  tout->Branch("TR",&tTR,"TR/F");
  tout->Branch("R", &tR, "R/F");
  tout->Branch("B", &tB, "B/F");

  // signal specifications
  int numScint = atoi(argv[1]);
  int numCeren = atoi(argv[2]);
  int numSimul = atoi(argv[3]);

  // simulate signal to analyze
  int tLength = (int)4*g1720mc.GetLongGate(); // in ns
  V1720PSDResult  *psd;
  TH1D *hanalog;
  for(int j=0;j<numSimul;j++) {
    hanalog = simWave( numScint, numCeren, tLength );

    for (int i=0; i<3*(int)g1720mc.GetLongGate(); i++) {
      
      // analyze signal at position, i
      psd = g1720mc.GetPSDforTrigger( hanalog , i); 
      
      // fill TTree
      tT = psd->fTime;
      tQS = psd->fQS;
      tQL = psd->fQL;
      tBL = psd->fBL;

      //if(tQL<0 || tQS<0) continue;

      // find if correct trigger
      bool trig = g1720mc.GoodTrigger( hanalog, i );
      if (trig == true)
	tTR = 1;
      else 
	tTR = 0;

      // get position of peak pulse relative to gate position
      if (numScint == 1) {

	tB = 0;

	// peak in qs range
	if((i < peak1) && (i + g1720mc.GetShortGate() > peak1)) {
	  if( i + g1720mc.GetShortGate()/2 > peak1 ) // within first 1/2 of short gate
	    tR = 1;
	  else  // within remaining short gate
	    tR = 0.5;
	}
	// peak in ql range 
	else if((i < peak1) && (i + g1720mc.GetLongGate() > peak1)) {
	  if(i + g1720mc.GetLongGate()/4 > peak1) // within first 1/4 of long gate
	    tR = 0;
	  else // within remaining long gate
	    tR = -0.5;
	}
	// out of charge gate range
	else if (i<peak1) { // before peak
	  tR = -1;
	}
	else { // after peak
	  tR = -2;
	}
      } else {
	
	tB = peak2 - peak1;
	
	// 1st peak in qs range
	if((i < peak1) && (i + g1720mc.GetShortGate() > peak1)) {
	  if( i + g1720mc.GetShortGate()/2 > peak1 ) // within first 1/2 of short gate
	    tR = 2;
	  else  // within remaining short gate
	    tR = 1.5;
	}
	// 1st peak in ql range within first 1/4 of long gate
	else if((i < peak1) && (i + g1720mc.GetLongGate()/4 > peak1))
	  tR = 1;
	// 2nd peak in qs range
	else if((i < peak2) && (i + g1720mc.GetShortGate() > peak2)) {
	  if( i + g1720mc.GetShortGate()/2 > peak2 ) // within first 1/2 of short gate
	    tR = 0.5;
	  else  // within remaining short gate
	    tR = 0;
	}
	// 2nd peak in ql range within first 1/4 of long gate
	else if((i + g1720mc.GetLongGate()/4 > peak1) &&
		(i < peak2) && (i + g1720mc.GetLongGate()/4 > peak2)) 
	  tR = -0.5;
	// within remaining long gates
	else if ((i + g1720mc.GetLongGate() > peak2))
	  tR = -1;
	// out of charge gate range
	else if (i<peak1)  // before first peak
	  tR = -1.5;
	else // after peaks
	  tR = -2;
      }
      
      if (psd != NULL) delete psd;
      
      tout->Fill();

    } // end filling tree
  } // end simulating signals

  // Write data to file and close
  fout->Write();
  fout->Close();

  return 0;
}
// end main -----------------------------------------------------------
