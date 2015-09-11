
#include "TTimeOffsets.h"
#include <strings.h>
#include <iostream>
#include <fstream>

TTimeOffsets * TTimeOffsets::instance = NULL;


// Constructor is private, only called once
// from the Get method
TTimeOffsets::TTimeOffsets(){
  
  // Clear memory
  bzero( TOff, sizeof( TTOffArray_struct ) * TTOFFSETS_MAXNRUNS );

  // Read in the time offsets
  ifstream fin;
  fin.open("timecorrectionbyrun.txt");
  
  // keep track of min and max run number
  fMinRun = TTOFFSETS_MAXNRUNS;
  fMaxRun = 0;
  int arunnum;
  Long64_t curtoffs[16];
  for (int i=0; !fin.eof(); i++){
    fin >> arunnum;
    if ( arunnum > fMaxRun ) fMaxRun = arunnum;
    if ( arunnum < fMinRun ) fMinRun = arunnum;
    for (int j=0; j< NDPPBOARDS*PSD_MAXNCHAN; j++ ){
      if ( arunnum>=0 && arunnum<5000 ){
	fin >> TOff[ arunnum ].toffset[j]; 
      } else {
	fin >> curtoffs[j];
      }
    }
  }

  // Print time offsets
  for (int i=fMinRun; i<=fMaxRun; i++){
    std::cout<<" Run "<<i<<" toffsets=";
    for (int j=0; j<NDPPBOARDS*PSD_MAXNCHAN; j++){
      std::cout<<" "<<TOff[ i ].toffset[ j ];
    }
    std::cout<<std::endl;
  }

  return;
}

// Method to get a pointer to the TTimeOffsets static object
TTimeOffsets* TTimeOffsets::Get(){
  if ( instance == NULL ){
    instance = new TTimeOffsets();
  }
  return instance;
}
