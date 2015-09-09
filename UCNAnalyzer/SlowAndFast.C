// ---------------------------------------------------------------
// Macro for outputting various slow control values over time.
// Used for large files
// Uncomment 2D histogram lines of particular variable to see that 
// particular histogram.  Uncommenting more than that can cause
// root to crash from the large file (~GB). (output588.root)
// ---------------------------------------------------------------
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TLegend.h"
#include "TH2.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "THStack.h"
#include <stdio.h>
#include <iostream>
#include "TVConstants.h"

void SlowAndFast(char *infilename = "output00604.root") {
  gROOT->SetStyle("Plain");
  gStyle->SetOptFit(1111);
  gStyle->SetTitle(0);
  gStyle->SetOptStat(0);


  // open file and trees to be read
  if (!( infilename && (*infilename) )) return; 
  TFile* fin = new TFile(infilename,"read");
  TTree* sin = (TTree*) fin->Get("tSlow");
  TTree* din = (TTree*) fin->Get("tUCN");
  TTree* hin = (TTree*) fin->Get("tHV");
  if (!sin) { delete sin; return;}
  if (!din) { return; }

  // branches of tSlow
  Float_t tTemp1;
  Float_t tTemp2;
  Float_t tPress;
  //Float_t tHV;
  ULong64_t tTimeS;
  ULong64_t tPSDIndex;
 

  // tree for slow control data
  sin->SetBranchAddress("tTemp1",   &tTemp1 );
  sin->SetBranchAddress("tTemp2",   &tTemp2 );
  sin->SetBranchAddress("tPress",   &tPress );
  //sin->SetBranchAddress("tHV",      &tHV );
  sin->SetBranchAddress("tTimeS",   &tTimeS );
  sin->SetBranchAddress("tPSDIndex",&tPSDIndex );

  // branches of tHV
  Float_t   tMVolt[HVCHANNELS];
  Float_t   tMCurr[HVCHANNELS];
  ULong64_t tTimeH;
  ULong64_t tEventH;

  hin->SetBranchAddress("tMVolt",   tMVolt );
  hin->SetBranchAddress("tMCurr",   tMCurr );
  hin->SetBranchAddress("tTimeH", &tTimeH );
  hin->SetBranchAddress("tEventH",&tEventH );

  // branches of tUCN
  ULong64_t tEntry;
  ULong64_t tTimeE;
  UShort_t tChannel;
  UShort_t tLength;
  //Short_t tPulse[1000];
  Float_t tPSD;
  UShort_t tChargeL;
  UShort_t tChargeS;
  UShort_t tBaseline;

  // tree for runtime event data
  din->SetBranchAddress("tEntry",    &tEntry);
  din->SetBranchAddress("tTimeE",    &tTimeE);
  din->SetBranchAddress("tChannel",  &tChannel);
  din->SetBranchAddress("tPSD",      &tPSD);
  din->SetBranchAddress("tChargeL",  &tChargeL);
  din->SetBranchAddress("tChargeS",  &tChargeS);
  din->SetBranchAddress("tBaseline", &tBaseline);
  din->SetBranchAddress("tLength",   &tLength);

  // title
  //TCanvas*tc=new TCanvas();

  const Long64_t m = sin->GetEntries();
  //Long64_t m=60;
  if (!m) { delete fin; return; }

  const int NCH=8;
  Double_t *event = new Double_t [m];

  Double_t *press = new Double_t [m];

  Double_t *temp1 = new Double_t [m];

  Double_t *temp2 = new Double_t [m];

  //Double_t *hv    = new Double_t [m];

  Double_t *timeS = new Double_t [m];

  Double_t **Qaa   = new Double_t*[NCH]; // Average signal charge
  for ( int i=0; i<NCH; i++) Qaa[i] = new Double_t [m];

  Double_t **Qar   = new Double_t*[NCH]; // rms signal charge
  for ( int i=0; i<NCH; i++) Qar[i] = new Double_t [m];

  Double_t **Qsum2  = new Double_t*[NCH]; // sum squared charge for current reading
  for ( int i=0; i<NCH; i++) Qsum2[i] = new Double_t [m];

  Double_t **Qsum   = new Double_t*[NCH];
  for ( int i=0; i<NCH; i++) Qsum[i] = new Double_t [m];

  Long64_t **Nsignal = new Long64_t*[NCH];
  for ( int i=0; i<NCH; i++) Nsignal[i] = new Long64_t [m];

  Long64_t **Nnoise= new Long64_t*[NCH];
  for ( int i=0; i<NCH; i++) Nnoise[i] = new Long64_t [m];

  Long64_t **Nll= new Long64_t*[NCH];
  for ( int i=0; i<NCH; i++) Nll[i] = new Long64_t [m];


  ULong64_t **DQTime = new ULong64_t*[NCH]; 
  for ( int i=0; i<NCH; i++) DQTime[i] = new ULong64_t [m];

  const Long64_t n = hin->GetEntries();
  if (!n) { delete hin; return; }

  Double_t *hv    = new Double_t [n];

  Double_t *curr  = new Double_t [n];

  Double_t *timeH = new Double_t [n];

  for(int i=0;i<n;i++) {

    hin->GetEvent(i);
    hv[i]    = (Double_t)tMVolt[2];
    curr[i]  = (Double_t)tMCurr[2];
    timeH[i] = (Double_t)tTimeH;
  }

  ULong64_t prevS = 0;
  int imc[ NCH ];
  for(int i=0;i<m;i++) {
    sin->GetEvent(i);
    if (i%20==0) cout<<"Slow Control event number "<<i<<" / "<<m<<endl;

    // fill event rate
    // Due to slow control timing at the beginning of a run, the first event 
    // often contains 2 slow control reads instead of 1.  Average these reads.
    if (i==0) {
      timeS[i] = tTimeS;
      event[i] = tPSDIndex/2.;
    }
    else {
      timeS[i] = tTimeS;
      event[i] = float( tPSDIndex );
    }
  

    // fill slow control variables
    press[i] = tPress;  
    temp1[i] = tTemp1;
    temp2[i] = tTemp2;
    //hv[i]    = tHV;

    ULong64_t firstevtime[NCH];
    for (int ch =0; ch<NCH; ch++){
      imc[ ch ] = 0; // count pulses in each channel during one slow control read
      Qaa   [ch][i]=0.0; // Average signal charge
      Qar   [ch][i]=0.0; // rms signal charge
      Qsum2 [ch][i]=0.0; // sum squared signal charge for current s.c. reading
      Qsum  [ch][i]=0.0; // sum signal charge for current s.c. reading
      Nsignal[ch][i]=0.0;
      Nnoise[ch][i]=0.0;
      Nll[ch][i]=0.0;
      firstevtime[ch]=0;
    }
    //cout <<"one for"<<endl;
    for (ULong64_t j=prevS; j<= prevS + tPSDIndex; j++){
      din->GetEvent(j);
      int ch = tChannel;
      if (ch != 0)
	continue;
      if (imc[ch]==0) firstevtime[ch] = tTimeE;
      DQTime[ch][i] = tTimeE - firstevtime[ch];
      
      // we input a different signal to channel 7
      // so it has different cut values for noise vs signal
      if ( ch == 7 ){
	// For pulser data:
	//if ( tChargeL > 5000.0 && tChargeL < 14000.0 ){
	// For signal runs:
	if ( tChargeL - tChargeS > -50.0 &&
	     tChargeL > 800.0 && tChargeL < 15000.0 &&
	     tChargeS > 400.0 && tChargeS < 10000.0 ) {
	  
	  // signal
	  Nsignal[ch][i]++;
	  Qsum[ch][i] += tChargeL;
	  Qsum2[ch][i] += tChargeL*tChargeL;
	  
	  // For pulser runs:
	  //}else if ( tChargeL <=5000.0 ) {
	  // For signal runs:
	} else if ( tChargeL-tChargeS>-50.0 &&
		    tChargeL<800.0 &&
		    tChargeS<400.0 ){
	  // noise/gamma
	  Nnoise[ch][i]++;
	}
	if ( tChargeL-tChargeS<50.0 && 
	     tChargeL>800.0 &&
	     tChargeL<15000.0 &&
	     tChargeS>600.0 &&
	     tChargeS<10000.0 ) {
	  Nll[ch][i]++;

	}


      } else {

	// For pulser data:
	//if ( tChargeL > 5000.0 && tChargeL < 14000.0 ){
	// For signal runs:
	if ( tChargeL - tChargeS > -50.0 &&
	     tChargeL > 800.0 && tChargeL < 15000.0 &&
	     tChargeS > 300.0 && tChargeS < 10000.0 ) {
	  
	  // signal
	  Nsignal[ch][i]++;
	  Qsum[ch][i] += tChargeL;
	  Qsum2[ch][i] += tChargeL*tChargeL;
	  
	  // For pulser runs:
	  //}else if ( tChargeL <=5000.0 ) {
	  // For signal runs:
	} else if ( tChargeL-tChargeS>-50.0 &&
		    tChargeL<800.0 &&
		    tChargeS<300.0 ){
	  // noise/gamma
	  Nnoise[ch][i]++;
	}
	if ( tChargeL-tChargeS<50.0 && 
	     tChargeL>800.0 &&
	     tChargeL<15000.0 &&
	     tChargeS>300.0 &&
	     tChargeS<10000.0 ) {
	  Nll[ch][i]++;

	}
      }

      //cout <<"one fill"<<endl;
      imc[ch] ++;

    }
    // calculate average and rms
    for (int ch=0; ch<NCH; ch++){
      if ( Nsignal[ch][i] > 0 )
	Qaa[ch][i] = Qsum[ch][i] / float(Nsignal[ch][i]) ;
      else
	Qaa[ch][i] = 0.0;

      if (Nsignal[ch][i]>1 ){
	Qar[ch][i] = ( Qsum2[ch][i] - Nsignal[ch][i]*Qaa[ch][i]*Qaa[ch][i] )/ float(Nsignal[ch][i]-1);
	if ( Qar[ch][i] > 0.0 ) Qar[ch][i] = sqrt( Qar[ch][i] );
	else {
	  Qar[ch][i]=0.0;
	}
      } else {
	Qar[ch][i]=0.0;
      }
      if (i==100) std::cout<<"ch="<<ch
			   <<" Nsig="<<Nsignal[ch][i]
			   <<" Nnoise="<<Nnoise[ch][i]
			   <<" Qaa="<<Qaa[ch][i]
			   <<" Qar="<<Qar[ch][i]
			   <<" DQTime="<<DQTime[ch][i]
			   <<std::endl;

    }
    prevS += tPSDIndex;
  }

  TFile* fout=new TFile("SlowAndFast.root","recreate");


  int firstnonzero=m;
  for (int i=0; i<m; i++){
    if (DQTime[0][i]>0){ 
      firstnonzero=i;
      break;
    }
  } 
  cout<<"First non zero = "<<firstnonzero<<endl;

  TH1D* hPress = new TH1D("hPress",";Day Hour:Minute; Pressure (Torr)", m-firstnonzero, timeS[firstnonzero], timeS[m-1]);
  TH1D* hT1    = new TH1D("hT1",";Day Hour:Minute; Temperature (degC)", m-firstnonzero, timeS[firstnonzero], timeS[m-1]);

  int firstnonzeroH=n;
  for (int i=0; i<n; i++){
    if (DQTime[0][i]>0){ 
      firstnonzeroH=i;
      break;
    }
  } 
  TH1D* hHV   = new TH1D("hHV",  ";Day Hour:Minute; Voltage (V)",  n-firstnonzeroH, timeH[firstnonzeroH], timeH[n-1]);
  TH1D* hCurr = new TH1D("hCurr",";Day Hour:Minute; Current (mA)", n-firstnonzeroH, timeH[firstnonzeroH], timeH[n-1]);

  hPress->GetXaxis()->SetTimeFormat("%H:%M");
  hT1   ->GetXaxis()->SetTimeFormat("%H:%M");
  hHV   ->GetXaxis()->SetTimeFormat("%H:%M");
  hCurr ->GetXaxis()->SetTimeFormat("%H:%M");

  hPress->GetXaxis()->SetTimeDisplay(1);
  hT1   ->GetXaxis()->SetTimeDisplay(1);
  hHV   ->GetXaxis()->SetTimeDisplay(1);
  hCurr ->GetXaxis()->SetTimeDisplay(1);
  
  hPress->SetLineWidth(2);
  hT1   ->SetLineWidth(2);
  hHV   ->SetLineWidth(2);
  hCurr ->SetLineWidth(2);

  TH1D** hQa =new TH1D* [NCH];
  TH1D** hQr =new TH1D* [NCH];
  TH1D** hArate=new TH1D* [NCH];
  TH1D** hNrate=new TH1D* [NCH];
  TH1D** hNll  =new TH1D* [NCH];

  char aname[256];
  char atitle[256];
  TDirectory * chdirs[NCH];
  for (int i=0; i<NCH; i++){
    fout->cd();
    sprintf(aname,"CH%02d",i);
    chdirs[i] = (TDirectory*) fout->mkdir(aname);

    sprintf(aname, "hQa%02d", i);
    sprintf(atitle," Channel %02d ;Day Hour:Minute; CH%02d Q_{signal} ",i,i);
    hQa[i] = new TH1D(aname,atitle, m-firstnonzero, timeS[firstnonzero], timeS[m-1]);

    sprintf(aname, "hQr%02d", i);
    sprintf(atitle," Channel %02d ;Day Hour:Minute; CH%02d Q_{signal}^{RMS} ",i,i);
    hQr[i] = new TH1D(aname,atitle, m-firstnonzero, timeS[firstnonzero], timeS[m-1]);

    sprintf(aname, "hArate%02d", i);
    sprintf(atitle," Channel %02d ;Day Hour:Minute; CH%02d signal rate (Hz) ",i,i);
    hArate[i] = new TH1D(aname,atitle, m-firstnonzero, timeS[firstnonzero], timeS[m-1]);

    sprintf(aname, "hNrate%02d", i);
    sprintf(atitle," Channel %02d ;Day Hour:Minute; CH%02d noise rate (Hz)",i,i);
    hNrate[i] = new TH1D(aname,atitle, m-firstnonzero, timeS[firstnonzero], timeS[m-1]);

    sprintf(aname, "hNll%02d", i);
    sprintf(atitle," Channel %02d ;Day Hour:Minute; CH%02d ll rate (Hz)",i,i);
    hNll[i] = new TH1D(aname,atitle, m-firstnonzero, timeS[firstnonzero], timeS[m-1]);
  

    hQa[i]->GetXaxis()->SetTimeFormat("%H:%M");
    hQr[i]->GetXaxis()->SetTimeFormat("%H:%M");
    hArate[i]->GetXaxis()->SetTimeFormat("%H:%M");
    hNrate[i]->GetXaxis()->SetTimeFormat("%H:%M");
    hNll[i]->GetXaxis()->SetTimeFormat("%H:%M");
    
    hQa[i]->GetXaxis()->SetTimeDisplay(1);
    hQr[i]->GetXaxis()->SetTimeDisplay(1);
    hArate[i]->GetXaxis()->SetTimeDisplay(1);
    hNrate[i]->GetXaxis()->SetTimeDisplay(1);
    hNll[i]->GetXaxis()->SetTimeDisplay(1);

    hQa[i]->SetLineWidth(2);
    hQr[i]->SetLineWidth(2);
    hArate[i]->SetLineWidth(2);
    hNrate[i]->SetLineWidth(2);
    hNll[i]->SetLineWidth(2);

  }
 
  for (int i=firstnonzero; i<m; i++){
    hPress->SetBinContent( i+1, 100.0*press[i] ); hPress->SetBinError( i+1, 0.01);
    hT1->SetBinContent( i+1, temp1[i] ); hT1->SetBinError( i+1, 0.03 );
  }
  for (int i=firstnonzero; i<n; i++ ){
    hHV->SetBinContent( i+1, hv[i] ); hHV->SetBinError( i+1, 0.005 );
    hCurr->SetBinContent( i+1, curr[i] ); hCurr->SetBinError(i+1, 0.005);
  }

  for (int ch=0; ch<NCH; ch++) {
    for (int i=firstnonzero; i<m; i++){
      hQa[ch]->SetBinContent( i+1, Qaa[ch][i] );
      if ( Nsignal[ch][i]>0 ) hQa[ch]->SetBinError( i+1, Qar[ch][i]/sqrt( double(Nsignal[ch][i]) ) );
      else hQa[ch]->SetBinError( i+1,0.0 );
      hQr[ch]->SetBinContent( i+1, Qar[ch][i] ); 
      if ( Nsignal[ch][i]>0 ) hQr[ch]->SetBinError( i+1, Qar[ch][i]/sqrt( double(Nsignal[ch][i]) ) );
      else hQr[ch]->SetBinError( i+1, 0.0 );
      
      double arate = double(Nsignal[ch][i]) / ( double(DQTime[ch][i]) * 4.0 * 1.0e-9 );
      hArate[ch]->SetBinContent( i+1, arate ); 
      if (Nsignal[ch][i]>0 ) hArate[ch]->SetBinError( i+1, arate / sqrt(double(Nsignal[ch][i]))  );
      double nrate = double(Nnoise[ch][i]) / ( double(DQTime[ch][i]) * 4.0 * 1.0e-9 );
      hNrate[ch]->SetBinContent( i+1, nrate ); 
      if ( Nnoise[ch][i]>0 ) hNrate[ch]->SetBinError( i+1, nrate/sqrt(double(Nnoise[ch][i])) );

      double lrate = double(Nll[ch][i]) / ( double(DQTime[ch][i]) * 4.0 * 1.0e-9 );
      hNll[ch]->SetBinContent( i+1, lrate ); 
      if ( Nll[ch][i]>0 ) hNll[ch]->SetBinError( i+1, lrate/sqrt(double(Nll[ch][i])) );

      //cout<<"Arate i="<< hArate->GetBinContent(i+1) << " " << Nsignal[ch][i]<<endl;
      if ( 0&& Nnoise[ch][i]>0 && Nsignal[ch][i]>0 )
	std::cout<<"DQTime = "<<DQTime[ch][i]<<" = "<<double(DQTime[ch][i])
		 <<" arate="<<arate<<" +-" << arate / sqrt(double(Nsignal[ch][i]))
		 <<" nrate="<<nrate<<" +-" << nrate / sqrt(double(Nnoise[ch][i]))
		 <<std::endl;
    }
  }
  fout->Write();
  
  TCanvas * tcsc = new TCanvas();
  tcsc->Divide(1,4);
  tcsc->cd(1); hPress->Draw("e1");
  tcsc->cd(2); hT1->Draw("e1");
  tcsc->cd(3); hHV->Draw("e1");
  tcsc->cd(4); hCurr->Draw("e1");
  
  int cols[NCH] = { kRed, kGreen, kBlue, kOrange, kMagenta, kCyan+2, kViolet, kBlack};
  TLegend *tl=new TLegend(0.85,0.5,0.98,0.98);
  tl->SetFillColor(kWhite);

  
  const int chmin=7; // set to first channel to draw
  const int chmax=8; // set to one more than last channel to draw

  TCanvas * tcQa = new TCanvas();
  tcQa->cd();
  double amin=9999999999999999999999.0;
  double amax=-9999999999999999999999999.0;
  for (int ch=chmin; ch<chmax; ch++){
    if ( hQa[ch]->GetMinimum()<amin ) amin = hQa[ch]->GetMinimum();
    if ( hQa[ch]->GetMaximum()>amax ) amax = hQa[ch]->GetMaximum();
    sprintf(aname,"ch%d",ch);
    tl->AddEntry( hQa[ch], aname, "lp");
    hQa[ch]->SetLineColor(cols[ch]);
    hQa[ch]->SetMarkerColor(cols[ch]);
    if (ch==chmin) hQa[ch]->Draw("e1");
    else hQa[ch]->Draw("e1same");
 }
 hQa[chmin]->SetMinimum(amin*0.95);
 hQa[chmin]->SetMaximum(amax*1.05);
 tl->Draw();

 TCanvas * tcQr = new TCanvas();
 tcQr->cd();
 amin=9999999999999999999999.0;
 amax=-9999999999999999999999999.0;
 for (int ch=chmin; ch<chmax; ch++){
   if ( hQr[ch]->GetMinimum()<amin ) amin = hQr[ch]->GetMinimum();
   if ( hQr[ch]->GetMaximum()>amax ) amax = hQr[ch]->GetMaximum();
   hQr[ch]->SetLineColor(cols[ch]);
   hQr[ch]->SetMarkerColor(cols[ch]);
   if (ch==chmin) hQr[ch]->Draw("e1");
   else hQr[ch]->Draw("e1same");
 }
 hQr[chmin]->SetMinimum(amin*0.95);
 hQr[chmin]->SetMaximum(amax*1.05);
 tl->Draw();


 TCanvas * tcAR = new TCanvas();
 tcAR->cd();
 amin=9999999999999999999999.0;
 amax=-9999999999999999999999999.0;
 for (int ch=chmin; ch<chmax; ch++){
   if ( hArate[ch]->GetMinimum()<amin ) amin = hArate[ch]->GetMinimum();
   if ( hArate[ch]->GetMaximum()>amax ) amax = hArate[ch]->GetMaximum();
   hArate[ch]->SetLineColor(cols[ch]);
   hArate[ch]->SetMarkerColor(cols[ch]);
   if (ch==chmin) hArate[ch]->Draw("e1");
   else hArate[ch]->Draw("e1same");
 }
 hArate[chmin]->SetMinimum(amin*0.95);
 hArate[chmin]->SetMaximum(amax*1.05);
 tl->Draw();

 TCanvas * tcNR = new TCanvas();
 tcNR->cd();
 amin=9999999999999999999999.0;
 amax=-9999999999999999999999999.0;
 for (int ch=chmin; ch<chmax; ch++){
   if ( hNrate[ch]->GetMinimum()<amin ) amin = hNrate[ch]->GetMinimum();
   if ( hNrate[ch]->GetMaximum()>amax ) amax = hNrate[ch]->GetMaximum();
   hNrate[ch]->SetLineColor(cols[ch]);
   hNrate[ch]->SetMarkerColor(cols[ch]);
   if (ch==chmin) hNrate[ch]->Draw("e1");
   else hNrate[ch]->Draw("e1same");
 }
 hNrate[chmin]->SetMinimum(amin*0.95);
 hNrate[chmin]->SetMaximum(amax*1.05);
 tl->Draw();


 TCanvas * tcLR = new TCanvas();
 tcLR->cd();
 amin=9999999999999999999999.0;
 amax=-9999999999999999999999999.0;
 for (int ch=chmin; ch<chmax; ch++){
   if ( hNll[ch]->GetMinimum()<amin ) amin = hNll[ch]->GetMinimum();
   if ( hNll[ch]->GetMaximum()>amax ) amax = hNll[ch]->GetMaximum();
   hNll[ch]->SetLineColor(cols[ch]);
   hNll[ch]->SetMarkerColor(cols[ch]);
   if (ch==chmin) hNll[ch]->Draw("e1");
   else hNll[ch]->Draw("e1same");
 }
 hNll[chmin]->SetMinimum(amin*0.95);
 hNll[chmin]->SetMaximum(amax*1.05);
 tl->Draw();



 return;
}
