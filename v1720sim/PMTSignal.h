
#ifndef _PMTSignal_h_
#define _PMTSignal_h_

#include <TF1.h>
#include <TMath.h>
#include <TRandom.h>

const int PMTSignal_MaxPhotons = 1000;

// function for basic signal
double signal( double * x, double *p );


/// PMTSignal
/// Class to simulate the signal from PMT
///
/// B. Jamieson, L. Rebenitsch (Fall 2014)
class PMTSignal {


 public:

  // destructor!
  ~PMTSignal();

  // constructor
  PMTSignal(){
    fIsCerenkov = false;
    fBaseline  = 0.0;   // mV (not used currently)
    fAmplitude = 25.0; // mV of single p.e. assume 100% uncertainty with 5mV thres.
    fTime      = 0.0;   // ns
    fFall      = 40.0;  // ns
    fRise      = 6.4;   // ns
    fLongFrac  = 0.01;   // fraction of signal
    fLong      = 2000.0; // ns
    fSimpleSignal = NULL;
    fSignal = NULL;
    fPhoton[0] = NULL;

    Init();
  }

  PMTSignal( double aBaseline, double aAmplitude, double aTime, double aFall, double aRise,
	     double aLongFrac, double aLong) {
    fIsCerenkov = false;
    fBaseline=aBaseline;
    fAmplitude=aAmplitude;
    fTime=aTime;
    fFall=aFall;
    fRise=aRise;
    fLongFrac=aLongFrac;
    fLong=aLong;
    fSimpleSignal = NULL;
    fSignal = NULL;
    fPhoton[0] = NULL;
    Init();
  }
  

  TF1* GetSignal ( int aNPhotons );
  double CurrentSignal( double * x, double * p);

  // Setters

  // Simple signal
  void SetBaseline  ( double aBaseline )  { fBaseline  = aBaseline; Init();  };
  void SetAmplitude ( double aAmplitude ) { fAmplitude = aAmplitude; Init();};
  void SetTime      ( double aTime )      { fTime      = aTime;  Init();    };
  void SetFallTime  ( double aFall )      { fFall      = aFall;  Init();    };
  void SetRiseTime  ( double aRise )      { fRise      = aRise;  Init();    };
  void SetLongFrac  ( double aLongFrac )  { fLongFrac  = aLongFrac; Init(); };
  void SetLongTime  ( double aLong )      { fLong      = aLong; Init();     };

  // Set Signal Type
  // By default assumes it is a scintillation signal
  // set flag true to do cerenkov signal, in that case
  // the signal is assumed to be a gaussian, so only
  // uses the fTime, fRiseTime parameters.
  void SetCerenkov( bool iscerenkov=false){ fIsCerenkov = iscerenkov; Init(); }

  // Noisy signal
  //void SetWidth ( double aWidth ) { fWidth = aWidth; };
  //void SetMean  ( double aMean )  { fMean = aMean; };
  //void SetTail  ( double aTail)   { fTail = aTail; };

  // Getters

  // Simple signal
  double GetBaseline ()  { return fBaseline;  };
  double GetAmplitude () { return fAmplitude; };
  double GetTime ()      { return fTime;      };
  double GetFallTime ()  { return fFall;      };
  double GetRiseTime ()  { return fRise;      };
  double GetLongFrac ()  { return fLongFrac;  };
  double GetLongTime ()  { return fLong;      };

  TF1 * GetSimpleSignal(){ return fSimpleSignal; };

 private:

  // flag to set this as a cerenkov signal
  // otherwise assume that it is a scintillation.
  bool   fIsCerenkov;

  // signal parameters
  double fBaseline;    // baseline for signal (mV)
  double fAmplitude;   // amplitude of single pe (mV)  
  double fTime;        // nominal time for peak of pulse (ns)
  double fFall;        // fall time for signal (ns)
  double fRise;        // sigma of single pe (ns)
  double fLongFrac;    // fraction of signal with a long fall time
  double fLong;        // long fall time (ns)

  // noisy signal parameters
  int fNPhotons;       // number of photons for current waveform
  TF1* fPhoton[PMTSignal_MaxPhotons];   // Individual photon functions

  // Initialize signal structures
  void Init(); // initialization 

  // Signal functions
  TF1* fSimpleSignal; // simple waveform
  TF1* fSignal;       // signal built from many gaussians.



};


#endif
