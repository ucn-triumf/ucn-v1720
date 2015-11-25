
#include "PMTSignal.h"

double signal(double *x, double *p){
  // x[0] = time (ns)
  // p[0] = amplitude (mV)
  // p[1] = delay (ns)
  // p[2] = tauup rise time (ns)
  // p[3] = taudown fall time (ns)
  // p[4] = fraction of singal with long fall time
  // p[5] = long fall time (ns)
  //
  // f(x)=amp*(1-exp(-(x-delay)/tauup))
  // g(x)=amp*(exp(-(x-delay-5*tauup)/taudown))
  
  double fx = p[0] * ( 1.0 - TMath::Exp( -( x[0]-p[1]) / p[2] ) );
  double gx = p[0] * ( (1.0-p[4]) * TMath::Exp( -(x[0]-p[1]-5.0*p[2]) / p[3] ) +
		       p[4] * TMath::Exp( -(x[0]-p[1]-5.0*p[2]) / p[5] ) );

  double startdown = p[1]+5.0*p[2];
  

  double retval = 0.0;
  if (x[0]>=p[1] && x[0]<=startdown) retval= fx; // rise part
  else if ( x[0] > startdown ) retval= gx;  // fall part
  
  // std::cout<<"startdown = "<<startdown
  // 	   <<" fTime="<<p[1]
  // 	   <<" t="<<x[0]
  // 	   <<" retval="<<retval<<std::endl;
  return retval;
}

double signalgaussian(double *x, double *p){
  return p[0] * TMath::Exp( -(x[0]-p[1])*(x[0]-p[1]) /2.0 /p[2] /p[2] );
}

PMTSignal::~PMTSignal(){
  // cleanup memory!
  if (fSimpleSignal!=NULL) delete fSimpleSignal;
  if (fSignal!=NULL) delete fSignal;
  if (fPhoton[0]!=NULL){
    for (int i=0; i<PMTSignal_MaxPhotons; i++){
      delete fPhoton[i];
    }
  }
}

void PMTSignal::Init(){
  fNPhotons = 0;

  // initialize simple function signal used for wavetrain
  if (fSimpleSignal!=NULL) delete fSimpleSignal;
  if (fIsCerenkov){
    // for Cerenkov signal, use a simple gaussian
    fSimpleSignal = new TF1("SimpleSignal",signalgaussian,fTime-10*fRise,fTime+10*fRise,3);

    fSimpleSignal->SetParameters( fAmplitude, // p[0] = amplitude (mV)
				  fTime,      // p[1] = delay (ns)
				  fRise);      // p[2] = tauup rise time (ns)
				  //				  fBaseline); // p[6] = baseline;
    

  } else{
    fSimpleSignal = new TF1("SimpleSignal",signal,fTime-3*fFall,fTime+6*fLong,6);

    fSimpleSignal->SetParameters( fAmplitude, // p[0] = amplitude (mV)
				  fTime,      // p[1] = delay (ns)
				  fRise,      // p[2] = tauup rise time (ns)
				  fFall,      // p[3] = taudown fall time (ns)
				  fLongFrac,  // p[4] = fraction of singal with long fall time
				  fLong  );   // p[5] = long fall time (ns)
    //fBaseline); // p[6] = baseline;
  }

  // print settings
  if (0){
  std::cout<<"amplitude="<<fAmplitude
	   <<" time="<<fTime
	   <<" rise="<<fRise
	   <<" fall="<<fFall
	   <<" longfrac="<<fLongFrac
	   <<" long="<<fLong
	   <<" bl="<<fBaseline
	   <<std::endl;
  }
  // TF1 * f = new TF1("f",fptr,&MyFunction::Evaluate,0,1,npar,"MyFunction","Evaluate");  
  if (fSignal!=NULL) delete fSignal;
  fSignal = new TF1("PMTSignal_Current",
		    this, 
		    &PMTSignal::CurrentSignal,
		    -3*fFall + fTime,
		    6*fLong + fTime,0,
		    "PMTSignal",
		    "CurrentSignal"); 
  fSignal->SetNpx(4000);
  
  // initialize signal function with many gaussians
  char aname[50];
  if ( fPhoton[0] == NULL ){
    for (int i=0; i<PMTSignal_MaxPhotons; i++){
      sprintf(aname,"PSg%03d",i);
      fPhoton[i] = new TF1(aname, "[0]*exp( -(x-[1])*(x-[1])/2.0/[2]/[2])", -3*fFall+fTime,6*fLong+fTime);
      fPhoton[i]->SetNpx(4000);
      fPhoton[i]->SetParameters(fAmplitude,0.0,fRise);
    }
  }
}

// method
TF1* PMTSignal::GetSignal( int aNPhotons ) {
  fNPhotons = aNPhotons;
  double curamp;
  for(int i=0; i<aNPhotons; i++) {
    fPhoton[i]->SetParameter(1, fSimpleSignal->GetRandom());
    do {
      curamp = gRandom->Gaus( fAmplitude, fAmplitude );
    } while (curamp<5.0);
    fPhoton[i]->SetParameter(0, curamp);
  }
  //fSignal->SetNpx(4000);
  return fSignal;
}

// current function
double PMTSignal::CurrentSignal( double * x, double *p ){
  // no parameters.
  // x[0] is time
  //std::cout<<" nphotons="<<fNPhotons<<" baseline="<<fBaseline<<std::endl;
  double retval = fBaseline;
  for (int i=0; i<fNPhotons; i++){
    retval += fPhoton[i]->Eval( x[0] );
    // std::cout<<" i="<<i
    // 	     <<" retval="<<retval
    // 	     <<" Ampl.="<<fPhoton[i]->GetParameter(0)
    // 	     <<" Mean="<<fPhoton[i]->GetParameter(1)
    // 	     <<" Sigma="<<fPhoton[i]->GetParameter(2)
    // 	     <<" x[i]="<<x[i]
    // 	     <<" fP[i]="<<  fPhoton[i]->Eval( x[0] )
    // 	     <<std::endl;
  }
  
  return retval;
}


