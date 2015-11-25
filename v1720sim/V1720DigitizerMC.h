
#ifndef V1720DigitizerMC_h_
#define V1720DigitizerMC_h_

#include <vector>
#include <TH1D.h>


/// V1720PSDResult
/// Class to store results for a single PSD pulse
class V1720PSDResult {
 public:
  V1720PSDResult( double aTime, double aQS, double aQL, double aBL ){
    fTime = aTime;
    fQS   = aQS;
    fQL   = aQL;
    fBL   = aBL;
    return;
  }

  float fTime; //< Pulse time
  float fQS;   //< Pulse short gate integrated charge (ADC)
  float fQL;   //< Pulse long gate integrated charge (ADC)
  float fBL;   //< Pulse baseline (ADC)
};


/// V1720DigitizerMC
/// Class to simulate the operation of a CAEN V1720 digitizer.
/// Initialize an instance with the digitizer settings.
/// Analyze a wavetrain stored in a TH1D you provide it to get the
/// PSD results as a vector of PSD results using the DoPSDAnalysis
/// method of the class.
///
/// *** nb.  Assumes POSITIVE going pulses! ***
///          at some point above was changed from negative... not sure when!
///
/// Author:  Blair Jamieson (May 2014)
class V1720DigitizerMC {
 private:
  // digitizer parameters
  double fSampRate;  //< digitizer sampling rate (ns/sample)
  int    fNBLSample; //< number of samples for baseline averaging
  double fShortGate; //< length of short gate (ns) 
  double fLongGate;  //< length of long gate (ns)
  double fThreshold; //< threshold (ADC)
  double fADCpermV;  //< ADC counts per mV conversion
  int    fBaseline;  //< Baseline (-9999= use number of samples, otherwise value is baseline in ADC counts)
  double fTrigHold;  //< Trigger Holdoff (ns) to discount double events
  double fGateOff;   //< Gate Offset (ns) to align short/long gates before trigger signal

  // variables used in PSD analysis
  long ans;         //< number of short gate bins
  long anl;         //< number of long gate bins
  long ang;         //< number of gate offset bins
  long ant;         //< number of trigger holdoff bins
  double athres;    //< threshold in mV
  long ibin;        //< bin number
  double cursample; //< sample to be analyzed
  double aTime;     //< time of trigger
  double aQS;       //< charge of short gate
  double aQL;       //< charge of long gate
  double aBL;       //< charge of baseline

  // variables used in baseline calculation
  std::vector<double> aVals; //values used in average
  long iavgd; 
  double asum;
  long ibasefixtil;
  double abase;

  // variables used in long/short gate histograms
  V1720PSDResult * thepsd;

  // working variables for the digitizer algorithms
  TH1D* fhInputPulse; //< pointer to input histogram to run PSD on
  TH1D* fhBaseline;   //< histogram to store the current baseline
  TH1D* fhShortGate;  //< histogram showing the short gates
  TH1D* fhLongGate;   //< histogram showing the long gates
  
  /// vectors of pulse shape results filled when
  /// the PSD algorithm is called for an input pulse train
  std::vector< V1720PSDResult > fPSD;

  /// method to make baseline histogram from input histogram
  void MakeBaselineHisto();

  int verbose;

 public:

  // constructor
  V1720DigitizerMC(double aSampRate  = 4.0,   // ns per sample
		   int    aNBLSample = 32,    // number of samples for baseline
		   double aShortGate = 40.0 , // in ns
		   double aLongGate  = 200.0, // in ns
		   double aThreshold = 125.0, //in ADC eg. 300ADC~150mV!
		   double aADCpermV  = 4096.0 / 2000.0, // eg 2ADC ~ 1mV
		   int    aBaseline  = 0, 
		   double aTrigHold  = 350.0, // hold off in ns
		   double aGateOff   = 8.0 ); // gate offset in ns

  // Setters
  void SetSampRate ( double aSampRate ) { fSampRate  = aSampRate;  };
  void SetNBLSample( int aNBLSample )   { fNBLSample = aNBLSample; };
  void SetShortGate( double aShortGate ){ fShortGate = aShortGate; };
  void SetLongGate ( double aLongGate ) { fLongGate  = aLongGate;  };
  void SetThreshold( double aThreshold ){ fThreshold = aThreshold; };
  void SetADCpermV ( double aADCpermV ) { fADCpermV  = aADCpermV;  };
  void SetBaseline ( int aBaseline )    { fBaseline  = aBaseline;  };
  void SetTrigHold ( double aTrigHold ) { fTrigHold  = aTrigHold;  };
  void SetGateOff  ( double aGateOff )  { fGateOff   = aGateOff;   }

  // Getters
  double GetSampRate()     { return fSampRate;  };
  int    GetNBLSample()    { return fNBLSample; };
  double GetShortGate()    { return fShortGate; };
  double GetLongGate()     { return fLongGate;  };
  double GetThresholdADC() { return fThreshold; };
  double GetThresholdmV()  { return fThreshold / fADCpermV; };
  double GetADCpermV()     { return fADCpermV;  }; 
  int    GetBaselineADC()  { return fBaseline;  };
  double GetTrigHold()     { return fTrigHold;  };
  double GetGateOff()      { return fGateOff;   };

  /// Main methods for pulse shape analysis
  /// Deletes any existing PSD results
  /// The input histogram is assumed to be binned so that each bin is
  /// one sample length wide, and that it is in units of mV
  std::vector< V1720PSDResult > * DoPSDAnalysis( TH1D* aInputHisto );

  /// Get PSD Results
  long GetNPulses(){ return fPSD.size(); };
  V1720PSDResult * GetPulse( long ipulse ){ 
    if ( ipulse>=0 && ipulse<fPSD.size() ) return &(fPSD[ipulse]);
    else return NULL;
  }

  /// Get Baseline histogram
  TH1D * GetBaselineHisto(){ return fhBaseline ; };
  /// Get Short gate histogram (only generated if requested)
  /// Only works if DoPSDAnalysis was already called on an input histogram
  TH1D * GetShortGateHisto();
  /// Get Long gate histogram (only generated if requested)
  /// Only works if DoPSDAnalysis was already called on an input histogram
  TH1D * GetLongGateHisto();

};
  
  
#endif
