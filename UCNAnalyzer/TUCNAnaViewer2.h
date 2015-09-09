
#include "TH1D.h"
#include "TV1720Histograms.h"
#include "TDataContainer.hxx"
#include "PulseShapeStruct.h"
#include "TCanvas.h"
#include "TTree.h"

/// This is a class for managing the analysis of UCN data.
/// It creates and fills a TTree of DPP values and a variable
/// length array of the ADC values for individual pulses.
class TUCNAnaViewer2 {
  
 public:
  
  TUCNAnaViewer2();
  ~TUCNAnaViewer2();
  
  /// Processes the midas event, fills histograms, etc.
  int ProcessMidasEvent(TDataContainer& dataContainer);
  int FindAndFitPulses(TDataContainer& dataContainer);

  TH1D *fV1720TimeHistogram;

 private:
   
  // runtime variables
  int index;
  int numBins;
  DPP_Bank_Out_t *b;
  //uint16_t *wf;
  int nEvents,channel,nLoop;
  int iboard, ichan, isubev;
  ULong_t subTotEvent;
  ULong_t subTotEventH;
  ULong64_t time, nextTime,sec;
  ULong64_t prevTime[PSD_MAXNCHAN*NDPPBOARDS];
  ULong64_t currTime[PSD_MAXNCHAN*NDPPBOARDS];
  ULong64_t base    [PSD_MAXNCHAN*NDPPBOARDS];
  ULong64_t sync    [NDPPBOARDS];
  //ULong64_t dtTime[PSD_MAXNCHAN*NDPPBOARDS];
  //ULong64_t tminThisEvent[PSD_MAXNCHAN*NDPPBOARDS];
  //ULong64_t tmaxThisEvent[PSD_MAXNCHAN*NDPPBOARDS];
  uint32_t numEvents,numSamples;
  int      verbose;
  int refChan;
  bool loop;
  bool first;
  DPPBankHandler fDPP[NDPPBOARDS];

};
