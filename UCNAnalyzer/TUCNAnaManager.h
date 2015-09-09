
#include "TH1D.h"
#include "PressData.hxx"
#include "TV1720Histograms.h"
#include "TDataContainer.hxx"
#include "PulseShapeStruct.h"
#include "TCanvas.h"
#include "TTree.h"
#include <iostream>
//#include "HVEvent.h"

using namespace std;

/// This is a class for managing the analysis of UCN data.
/// It creates and fills a TTree of DPP values and a variable
/// length array of the ADC values for individual pulses.
class TUCNAnaManager {
  
 public:
  
  TUCNAnaManager();
  ~TUCNAnaManager();
  
  /// Processes the midas event, fills histograms, etc.
  int ProcessMidasEvent(TDataContainer& dataContainer);
  int FindAndFitPulses(TDataContainer& dataContainer);
  
 private:
  
  double fFirstPulseFittedTime;

  // output file
  TTree * tUCN;
  TTree * tSlow;
  TTree * tHV;

  // branches

  // tUCN
  ULong_t tEntry;
  ULong_t tEvent;
  ULong64_t tTimeE;
  Short_t tChannel;
  Short_t tLength;
  //Short_t tPulse[1000];
  Float_t tPSD;
  Short_t tChargeL;
  Short_t tChargeS;
  Short_t tBaseline;

  // tSlow
  Float_t tTemp1;
  Float_t tTemp2;
  Float_t tPress;
  ULong_t tTimeS;
  ULong_t tPSDIndex;
  
  // tHV
  // Float_t tSVolt[HVCHANNELS];
  Float_t tMVolt[HVCHANNELS];
  Float_t tMCurr[HVCHANNELS];
  //  Bool_t tChStat[HVCHANNELS];
  //  Float_t tMTemp[HVCHANNELS];
  ULong_t tTimeH;
  ULong_t tEventH;

  //HVEvent *hvEvent;


  // runtime variables
  int index;
  DPP_Bank_Out_t *b;
  //uint16_t *wf;
  uint16_t *p;
  float *ps;
  int nEvents;
  ULong_t isubev;
  Short_t iboard, ichan;
  ULong_t subTotEvent;
  ULong_t subTotEventH;
  ULong64_t prevEvent[PSD_MAXNCHAN*NDPPBOARDS];
  ULong64_t currEvent[PSD_MAXNCHAN*NDPPBOARDS];
  ULong64_t base     [PSD_MAXNCHAN*NDPPBOARDS];
  ULong64_t sync    [NDPPBOARDS];
  int refChan;
  int protonPulse;

  uint32_t numEvents,numSamples;
  int      verbose;
  DPPBankHandler fDPP[NDPPBOARDS];

};
