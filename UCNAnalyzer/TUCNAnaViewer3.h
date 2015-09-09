
#include "TH1D.h"
#include "TV1720Histograms.h"
#include "TDataContainer.hxx"
#include "PulseShapeStruct.h"
#include "TCanvas.h"
#include "TTree.h"

/// This is a class for managing the analysis of UCN data.
/// It creates and fills a TTree of DPP values and a variable
/// length array of the ADC values for individual pulses.
class TUCNAnaViewer {
  
 public:
  
  TUCNAnaViewer();
  ~TUCNAnaViewer();
  
  /// Processes the midas event, fills histograms, etc.
  int ProcessMidasEvent(TDataContainer& dataContainer);
  
  int FindAndFitPulses(TDataContainer& dataContainer);

  /// Blah, make histograms public; 
  TV1720QSQLHistograms*  fV1720QSQLHistograms;
  TV1720PSDQLHistograms* fV1720PSDQLHistograms;
  
 private:
  
  double fFirstPulseFittedTime;

  // branches
  Short_t tEvent;
  ULong_t  tTime;
  Short_t tChannel;
  Short_t tLength;
  //Short_t tPulse[1000];
  Float_t tPSD;
  Short_t tChargeL;
  Short_t tChargeS;
  Short_t tBaseline;
  
  // runtime variables
  int index;
  DPP_Bank_Out_t *b;
  //uint16_t *wf;
  int nEvents;
  int iboard, ichan, isubev;

  uint32_t numEvents,numSamples;
  int      verbose;
  DPPBankHandler fDPP[NDPPBOARDS];

};
