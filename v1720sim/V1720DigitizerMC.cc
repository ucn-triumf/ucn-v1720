
#include <iostream>
#include "V1720DigitizerMC.h"

//====================================================================================
V1720DigitizerMC::V1720DigitizerMC( 
				   double aSampRate, 
				   int    aNBLSample, 
				   double aShortGate, 
				   double aLongGate, 
				   double aThreshold,
				   double aADCpermV,
				   int    aBaseline,
				   double aTrigHold,
				   double aGateOff ) {					
  fSampRate  =  aSampRate;
  fNBLSample =  aNBLSample;
  fShortGate =  aShortGate;
  fLongGate  =  aLongGate;
  fThreshold =  aThreshold;
  fADCpermV  =  aADCpermV;
  fBaseline  =  aBaseline;
  fTrigHold  =  aTrigHold;
  fGateOff   =  aGateOff;

  // initialize storage
  fhBaseline = NULL;
  fhShortGate = NULL;
  fhLongGate = NULL;
  fPSD.reserve( 10000 );

  verbose = 1;
}

//====================================================================================
std::vector< V1720PSDResult > * V1720DigitizerMC::DoPSDAnalysis( TH1D * aInputHisto ){
  if (verbose) std::cout<<"V1720DigitizerMC::DoPSDAnalysis()"<<std::endl;
  fhInputPulse = aInputHisto;
  MakeBaselineHisto();
  fPSD.clear();

  // find number of bins for gate values
  ans = long( fShortGate / fSampRate ); // number of short gate bins
  anl = long( fLongGate  / fSampRate ); // number of long gate bins
  ang = long( fGateOff   / fSampRate ); // number of gate offset bins
  ant = long( fTrigHold  / fSampRate ); // number of trigger holdoff bins

  // threshold in mV
  athres = fThreshold / fADCpermV;
  std::cout<<"threshold is "<<athres<<" mV or "<<fThreshold<<" ADC"<<std::endl;

  // loop over input histogram to make pulses
  ibin = 1;
  long presample = 0.0;
  
  while ( ibin < fhInputPulse->GetNbinsX() ){
    //if (ibin%1000==0) std::cout<<"ANA "<<ibin<<" / "<< fhInputPulse->GetNbinsX()<<std::endl;
    cursample =  fhInputPulse->GetBinContent(ibin) - fhBaseline->GetBinContent(ibin); // in mV

    int iter=0;
    // if signal goes below threshold
    if ( cursample > athres ){

      // create a new PSD result

      // short gate is from ibin to ibin + ans - ang
      // long gate is from ibin to ibin + anl - ang
      aTime = fhInputPulse->GetBinCenter( ibin );
      aQS = 0.0;
      aQL = 0.0;
      aBL = fhBaseline->GetBinContent(ibin);
      for (long i=ibin-ang; i<std::min<long>( ibin+anl-ang, fhInputPulse->GetNbinsX() ); i++){
	//cursample =  ( fhBaseline->GetBinContent(i)- fhInputPulse->GetBinContent(i)) * fADCpermV;
	cursample =  fhInputPulse->GetBinContent(i) - fhBaseline->GetBinContent(i);
	if ( i < ibin+ans ) aQS += cursample; 
	aQL += cursample;

	// std::cout << "bin " << i 
	// 	  << " current sample " << cursample  
	// 	  << " baseline " << aBL 
	// 	  << std::endl;
      }
      fPSD.push_back( V1720PSDResult( aTime, aQS, aQL, aBL ) );
      ibin+=ant;
    }
    ibin ++;          
  } 

  return &fPSD;
}

//====================================================================================
void V1720DigitizerMC::MakeBaselineHisto(){	
	
  if(verbose) std::cout<<"V1720DigitizerMC::MakeBaselineHisto()"<<std::endl;

  // we will clean up the baseline histo if it exists
  // also cleanup the shortgate and long gate histos
  if ( fhBaseline != NULL ){
    // don't delete it, just clear it
    fhBaseline->Clear();
    if( fhLongGate != NULL ) fhLongGate->Clear();
    if (fhShortGate != NULL ) fhShortGate->Clear();
  } else {
    // make a new histogram of same length as input histo
    fhBaseline = new TH1D( "V1720DigitizerMC_fhBaseline","baseline",
			   fhInputPulse->GetNbinsX(), 
			   fhInputPulse->GetXaxis()->GetXmin(),
			   fhInputPulse->GetXaxis()->GetXmax() );
  }

  // if fBaseling != -9999, baseline value is set in ADC counts
  if ( fBaseline != -9999){
    for ( long ibin=1; ibin <= fhInputPulse->GetNbinsX(); ibin++) 
      fhBaseline->SetBinContent(ibin, fBaseline);
    return;
  }

  // loop over input histogram bins and calculate average of last fNBLSample bins as the baseline
  // keep the baseline fixed for length of trigger holdoff gate 
  aVals.push_back( fhInputPulse->GetBinContent(1) ); //values used in average
  asum = aVals[0];
  iavgd=1;
  //ibasefixtil = 0;
  ang = long( fGateOff   / fSampRate ); // number of gate offset bins
  ant = long( fTrigHold  / fSampRate ); // number of trigger holdoff bins

  // threshold in mV
  athres = fThreshold / fADCpermV;
  for ( long ibin=1; ibin <= fhInputPulse->GetNbinsX(); ibin++){
    
    // find current baseline average
    if (ibin<fNBLSample) abase = asum / float(ibin);
    else abase = asum / float(fNBLSample);

    // see if we would start another pulse
    cursample = fhInputPulse->GetBinContent(ibin) - abase;

    // if current sample crosses threshold, fix baseline to current 
    // average for duration of trigger holdoff.
    if ( cursample > athres ) {
      //std::cout << "event" << std::endl;
      // increment ibin in the for loop
      for ( int i=ibin; ibin<i+ant;ibin++)
	fhBaseline->SetBinContent(ibin,abase);
	//ibasefixtil = ibin + ant; // length to fix baseline value
    }
    // if not, update baseline average and save to histogram
    else {
      double sample = fhInputPulse->GetBinContent( ibin );
      if ( ibin > fNBLSample ) {
	asum -= aVals[ iavgd%fNBLSample ];
	aVals[ iavgd%fNBLSample ] = sample;
      } else  {
	aVals.push_back(sample );
      }
      iavgd++;
      asum += sample;

      fhBaseline->SetBinContent( ibin, abase );
    }
    
    // // set current baseline value in histogram
    // fhBaseline->SetBinContent( ibin, abase );

    // // if ibin is past fixed bin setting (past trigger holdoff),
    // // update baseline average to next bin setting
    // if ( ibin > fNBLSample && ibin > ibasefixtil ) {
    //   asum -= aVals[ iavgd%fNBLSample ];
    //   aVals[ iavgd%fNBLSample ] = fhInputPulse->GetBinContent( ibin );
    //   iavgd++;
    //   asum += fhInputPulse->GetBinContent( ibin );
    // } else if (ibin <= fNBLSample && ibin > ibasefixtil) {
    //   aVals.push_back( fhInputPulse->GetBinContent( ibin ) );
    //   iavgd++;
    //   asum += fhInputPulse->GetBinContent( ibin );
    // }

    //std::cout << "baseline " << abase << std::endl;
  }
  std::cout << "baseline " << abase << std::endl;
  return;
}
					   

//====================================================================================
TH1D * V1720DigitizerMC::GetShortGateHisto(){

  if(verbose)std::cout<<"V1720DigitizerMC::GetShortGateHisto()"<<std::endl;

  if ( fhShortGate == NULL ){
    fhShortGate = new TH1D( "V1720DigitizerMC_fhShortGate","short gate",
			 fhInputPulse->GetNbinsX(), 
			 fhInputPulse->GetXaxis()->GetXmin(),
			 fhInputPulse->GetXaxis()->GetXmax() );
  }

    // loop over the psd results
    for (long ipsd = 0 ; ipsd < fPSD.size() ; ipsd ++ ){
      thepsd = &(fPSD[ipsd]);
      ibin = long( thepsd->fTime / fSampRate );
      ans = long( fShortGate / fSampRate ); // number of short gate bins
      for (long i = ibin; i < ibin+ans; i++ ){
	fhShortGate->SetBinContent(i, fhInputPulse->GetBinContent(i));
      }
    }
  
  return fhShortGate;
}


//====================================================================================
TH1D * V1720DigitizerMC::GetLongGateHisto(){

  if(verbose)std::cout<<"V1720DigitizerMC::GetLongGateHisto()"<<std::endl;

  if ( fhLongGate == NULL ){
    fhLongGate = new TH1D( "V1720DigitizerMC_fhLongGate","long gate",
			 fhInputPulse->GetNbinsX(), 
			 fhInputPulse->GetXaxis()->GetXmin(),
			 fhInputPulse->GetXaxis()->GetXmax() );
  } 
    // loop over the psd results
    for (long ipsd = 0 ; ipsd < fPSD.size() ; ipsd ++ ){
      thepsd = &(fPSD[ipsd]);
      ibin = long( thepsd->fTime / fSampRate );
      anl = long( fLongGate / fSampRate ); // number of long gate bins
      for (long i = ibin; i < ibin+anl; i++ ){
	fhLongGate->SetBinContent(i, fhInputPulse->GetBinContent(i));
      }
    }
  
  return fhLongGate;
}

