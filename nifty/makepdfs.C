// makepdfs.C
// root script to make template fit to data from simulation files
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <math.h>

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TCanvas.h"

#include "ini.h"
#include "INIReader.h"

using namespace std;

INIReader gConfig("makepdfs.ini");

const double QMEANEQ = gConfig.GetReal("fitparameter","meanq",7201.53);//7201.53; //5.26067e+03;//5000.0;
const double QSIGEQ =  gConfig.GetReal("fitparameter","sigq",1009.59);//1009.59;//9.75788e+02;
const double PMEANEQ = gConfig.GetReal("fitparameter","meanp",0.577457);//7201.53; //5.26067e+03;//5000.0;
const double PSIGEQ =  gConfig.GetReal("fitparameter","sigp",0.0730255);//1009.59;//9.75788e+02;

// script takes simulation file pdfs
void makepdfs(){

  // simulation files to make template comparison
  //TFile * finbg  = new TFile("ucnrateN0.0G100000.0T10.0Run00.root","read");    // background only simulation
  std::string strfname = gConfig.Get("input","bgfilename","output00000063_0000.root");
  TFile * finbg  = new TFile((const char *)strfname.c_str(),"read"); // file for deriving background
  strfname = gConfig.Get("input","sigfilename","ucnrateN100000.0G0.0T10.0Run00.root");
  TFile * finsig = new TFile((const char *)strfname.c_str(),"read"); // signal only simulation
  strfname = gConfig.Get("input","toyfilename","ucnrateN50000.0G50000.0T10.0Run00.root");
  TFile * fintoy = new TFile((const char *)strfname.c_str(),"read"); // signal and background
  
  // TTrees of corresponding simulation files
  //TTree*tbg=(TTree*)finbg->Get("TPSD");   // background only simulation
  TTree*tbg=(TTree*)finbg->Get("tUCN");   // for background derivation
  TTree*tsig=(TTree*)finsig->Get("TPSD"); // signal only simualtion
  TTree*ttoy=(TTree*)fintoy->Get("TPSD"); // signal and background

  std::cout << "Data loaded" << std::endl;
  
  // output fit file
  strfname = gConfig.Get("output","filename","PDFs.root");
  TFile * fout = new TFile((const char *)strfname.c_str(),"recreate");

  // histograms used for making pdfs
  // background/light leak: 0 n, 0 g
  TH2D* hbg_psdql = new TH2D("hbg_psdql","Background ; QL; PSD",150,0.,17000.0,40,-1., 1.);
  // signal: 1 n, 0 g
  TH2D* hsig_psdql = new TH2D("hsig_psdql","Signal ; QL; PSD",150,0.,17000.0,40,-1., 1.);
  // retrigger: 0 n, 0 g
  TH2D* hretrig_psdql = new TH2D("hretrig_psdql","Retrigger ; QL; PSD",150,0.,17000.0,40,-1., 1.);
  // late light: 0 n, 0 g
  TH2D* hlate_psdql = new TH2D("hlate_psdql","Late Light ; QL; PSD",150,0.,17000.0,40,-1., 1.);
  // signal pile-up: 2+ n, 0 g
  TH2D* hsigpile_psdql = new TH2D("hsigpile_psdql","Signal Pile-up ; QL; PSD",150,0.,17000.0,40,-1., 1.);
  // gamma background: 0 n, 1 g
  TH2D* hgbg_psdql = new TH2D("hgbg_psdql","Gamma Background ; QL; PSD",150,0.,17000.0,40,-1., 1.);
  // gamma pile-up: 0 n, 2+ g
  TH2D* hgbgpile_psdql = new TH2D("hgbgpile_psdql","Gamma Pile-up ; QL; PSD",150,0.,17000.0,40,-1., 1.);
  // signal and gamma: 1 n, 1 g
  TH2D* hsiggbg_psdql = new TH2D("hsiggbg_psdql","Signal + Gamma ; QL; PSD",150,0.,17000.0,40,-1., 1.);
  // signal pile-up and gamma: 2+ n, 1 g
  TH2D* hsigpilegbg_psdql = new TH2D("hsigpilegbg_psdql","Signal Pile-up + Gamma ; QL; PSD",150,0.,17000.0,40,-1., 1.);
  // signal and gamma pile-up: 1 n, 2+ g
  TH2D* hsiggbgpile_psdql = new TH2D("hsiggbgpile_psdql","Signal + Gamma Pile-up; QL; PSD",150,0.,17000.0,40,-1., 1.);
  // both signal and gamma pile-up: 2+ n, 2+ g
  // TH2D* hpile_psdql = new TH2D("hpile_psdql","Both Signal + Gamma Pile-up; QL; PSD",150,0.,17000.0,40,-1., 1.);

  TH2D* htoy_psdql = new TH2D("htoy_psdql","Toy ; QL; PSD",150,0.,17000.0,40,-1., 1.);


  TH2D* hretrig1_psdql = new TH2D("hretrig1_psdql","Retrig ; QL; PSD",150,0.,17000.0,40,-1., 1.);
  TH2D* hretrig2_psdql = new TH2D("hretrig2_psdql","Retrig ; QL; PSD",150,0.,17000.0,40,-1., 1.);
  TH2D* hretrig3_psdql = new TH2D("hretrig3_psdql","Retrig ; QL; PSD",150,0.,17000.0,40,-1., 1.);
  TH2D* hretrig4_psdql = new TH2D("hretrig4_psdql","Retrig ; QL; PSD",150,0.,17000.0,40,-1., 1.);

  TH2D* hsig1_psdql = new TH2D("hsig1_psdql","Signal ; QL; PSD",150,0.,17000.0,40,-1., 1.);
  TH2D* hsig2_psdql = new TH2D("hsig2_psdql","Signal ; QL; PSD",150,0.,17000.0,40,-1., 1.);
  TH2D* hsig3_psdql = new TH2D("hsig3_psdql","Signal ; QL; PSD",150,0.,17000.0,40,-1., 1.);
  TH2D* hsig4_psdql = new TH2D("hsig4_psdql","Signal ; QL; PSD",150,0.,17000.0,40,-1., 1.);

  
  TH1D* hdeltat = new TH1D("hdeltat","dt; #Delta t (ns)",10000,0.,50000.);

  TH2D* hqlvsdt = new TH2D("hqlvsdt"," ; #Delta t(ns); QL", 
			   1000,0.,50000.,
			   100, 0.,20000.);//->Fill( t-tlast , qs );

  TH2D* hqlvsdt0n = new TH2D("hqlvsdt0n","No true neutron ; #Delta t(ns); QL", 
			   1000,0.,50000.,
			   100, 0.,20000.);//->Fill( t-tlast , qs );

  TH2D* hqsvsdt = new TH2D("hqsvsdt","PSD>0.85 ; #Delta t(ns); QS", 
			   1000,0.,50000.,
			   70, 0.,7000.);//->Fill( t-tlast , qs );
  TH2D* hqsvsdtall = new TH2D("hqsvsdtall","All events ; #Delta t(ns); QS", 
			   1000,0.,50000.,
			   70, 0.,7000.);//->Fill( t-tlast , qs );

  std::cout << "Objects initialized" << std::endl;

  // float t;
  // float qs;
  // float ql;
  // float bl;
  // tbg->SetBranchAddress("T",&t);
  // tbg->SetBranchAddress("QS",&qs);
  // tbg->SetBranchAddress("QL",&ql);
  // tbg->SetBranchAddress("BL",&bl);

  // // Fill background histogram
  //  Long64_t nentries = tbg->GetEntries();
  //  for (Long64_t ientry=0; ientry<nentries; ientry++){
  //    tbg->GetEntry(ientry);
  //    double psd = -0.999;
  //    if ( ql!=0 ) psd = ( ql - qs ) / ql;
  //    hbg_psdql->Fill( ql, psd );
  //  }

  std::cout << "Make background PDF" << std::endl;

  // use pulses with GV closed to make background pdf
  // first find pulses in file
  UShort_t tChargeL;
  UShort_t tChargeS;
  UShort_t tChannel;
  ULong64_t tTimeE;
  tbg->SetBranchAddress("tChargeL",&tChargeL);
  tbg->SetBranchAddress("tChargeS",&tChargeS);
  tbg->SetBranchAddress("tChannel",&tChannel);
  tbg->SetBranchAddress("tTimeE",&tTimeE);
  std::vector<Long64_t> pulseT; // vector of pulse times
  Double_t  n = tbg->GetEntries();
  tbg->GetEntry(0);
  Double_t eTally = 0;
  ULong64_t tTally = tTimeE;
  Double_t maxRate = gConfig.GetReal("background","max",10000.0); 
  Double_t minRate = gConfig.GetReal("background","min",0.0);
  for (int i=0;i<n;i++) {
    tbg->GetEntry(i);
    
    // get events from centre channel
    if(tChannel != 9) continue;

    // check tally time
    if (tTally > tTimeE) continue;

    // tally rate for second
    eTally++;

    // check if pulse time has GV open or closed
    // based on rate
    if (tTimeE>tTally+2e9){

      // if large, GV open
      // skip ahead 200s
      if(eTally>maxRate*2)
	tTally+=200*1e9;
      // if between rates, GV pulse found
      else if(eTally < maxRate*2 && eTally>minRate*2) {
	// add pulse time to list of pulses
	pulseT.push_back(tTimeE);
	// push ahead a few seconds to prevent double counting
	tTally+=4*1e9;
      }
      
      // reset for next second
      tTally += 2e9;
      eTally = 0;
    }
  }

  std::cout << "GV close pulses found " << pulseT.size() << std::endl;

  // fill background histogram with events from 
  // no pulse
  // use fits from dataforfit to normalize charge
  strfname = gConfig.Get("input","bgfitfile","output00000054_0000.txt");
  ifstream fitin;
  fitin.open((const char *)strfname.c_str());
  const int NCH=9;
  double ameanq[NCH];
  double ameanp[NCH];
  string line; // for parsing through file
  double num;
  getline (fitin,line);
  for (int ich=0; ich<NCH; ich++){
    fitin >> num;
    fitin >> ameanq[ich];
    fitin >> num;
    fitin >> ameanp[ich];
    fitin >> num;
  }
  int j=0;
  Long64_t nentries = tbg->GetEntries();
  for (Long64_t ientry=0; ientry<nentries; ientry++){
    tbg->GetEntry(ientry);

    // Check for data channel
    if( tChannel == 7 || tChannel > 9) continue;

    // Check for timing relative to GV closed pulse
    if (tTimeE < pulseT[j] + 10e9) continue; 

    // Passed cuts, add corrections
    int ch = tChannel;
    if (tChannel==8||tChannel==9) ch=tChannel-1;
    double qlcor = double(tChargeL)* QMEANEQ / ameanq[ch];
    double qscor = double(tChargeS)* QMEANEQ / ameanq[ch];
    double psd = (double( tChargeL) - double(tChargeS))/double(tChargeL);
    double psdcor = psd * PMEANEQ / ameanp[ch];

    // Fill
    // adjust charge according to channel
    // based on fit from dataforfit
    hbg_psdql->Fill( qlcor, psdcor );

    // update reference pulse time if necessary
    if (tTimeE > pulseT[j]+270*1e9)
      j++;
  }

  std::cout << "Background histogram filled" << std::endl;

  std::cout << "Make signal and retrigger histograms"<< std::endl;

  // Fill the signal histogram and retrigger histogram
  // and pile up histogram
  float t;
  float qs;
  float ql;
  float bl;
  tsig->SetBranchAddress("T",&t);
  tsig->SetBranchAddress("QS",&qs);
  tsig->SetBranchAddress("QL",&ql);
  tsig->SetBranchAddress("BL",&bl);
  int tMCNn; // number of true neutrons in QL
  int tRTRn; // 0 within short gate; 1 retrigger event
  int tMCNg; // number of true gammas in QL
  int tRTRg; // 0 within short gate; 1 retrigger event
  tsig->SetBranchAddress("MCNn",&tMCNn);
  tsig->SetBranchAddress("RTRn",&tRTRn);
  tsig->SetBranchAddress("MCNg",&tMCNg);
  tsig->SetBranchAddress("RTRg",&tRTRg);
  nentries = tsig->GetEntries();
  float tlast=0.0;
  for (Long64_t ientry=0; ientry<nentries; ientry++){
    tsig->GetEntry(ientry);
    if(ql<0 || qs<0) continue;

    if (ientry==0) tlast =t;

    // compute psd
    double psd = ( ql - qs ) / ql;

    if (ientry!=0) {
      
      // check signal type
      //      if (tMCNn==1 && tRTRn == 0) // correctly timed event
      //	hsig_psdql->Fill( ql, psd );
      // if (tMCNn==1) // correctly timed event
      // 	hsig_psdql->Fill( ql, psd );
      double qlcor = double(ql)* QMEANEQ / 6886.;
      double qscor = double(qs)* QMEANEQ / 6886.;
      double psdcor = psd* PMEANEQ / 0.5441;


      if( tMCNn==1 && tMCNg==0 )                   // if pure signal
	hsig_psdql->Fill( qlcor, psdcor );       
      else if ( tMCNn==0 && tMCNg==0 && tRTRn > 1) // if late light
	hlate_psdql->Fill( qlcor, psdcor ); // no gammas, no neutrons, but neutron w/in 150ns before PSD
      else if ( tMCNn==0 && tMCNg==0 )             // if retrigger
	hretrig_psdql->Fill(qlcor, psdcor);
      else if (tMCNn>1 && tMCNg==0)                // if signal pile-up
	hsigpile_psdql->Fill( qlcor, psdcor );  
      else if (tMCNn==0 && tMCNg == 1)             // if gamma background
	hgbg_psdql->Fill( qlcor, psdcor ); 
      else if (tMCNn==0 && tMCNg>1)                // if gamma pile-up
	hgbgpile_psdql->Fill( qlcor, psdcor );
      else if( tMCNn==1 && tMCNg==1 )              // if neutron and gamma
	hsiggbg_psdql->Fill( qlcor, psdcor );
      else if( tMCNn>1 && tMCNg==1 )               // if neutron pile-up and gamma
	hsigpilegbg_psdql->Fill( qlcor, psdcor );
      else// if( tMCNn==1 && tMCNg>1 )               // if neutron and gamma pile-up
	hsiggbgpile_psdql->Fill( qlcor, psdcor );
      //else                                         // if both neutron and gamma pile-up
	//hpile_psdql->Fill( qlcor, psdcor );

      // if (tMCNn==0 && t-tlast>400.0) // retrigger (no neutron in psd window)
      // 	hretrig_psdql->Fill( ql, psd );
      
      // if ( t-tlast < 6000. && ql < 2000.0 ){
      // 	// call this a retrigger!
      // 	hretrig_psdql->Fill( ql, psd );
      // } else {
      // 	// call this signal!
      // 	hsig_psdql->Fill( ql, psd );
      // }


      hqsvsdtall->Fill( t-tlast , qs );
      hqlvsdt->Fill( t-tlast , ql );
      if (tMCNn==0)
	hqlvsdt0n->Fill( t-tlast , ql );

      if ( psd > 0.85 ){
	hdeltat->Fill( t-tlast );
	hqsvsdt->Fill( t-tlast , qs );
      }


      if ( t-tlast < 2000. && ql < 2000.0 ){
	hretrig1_psdql->Fill( ql, psd );
      } else {
	hsig1_psdql->Fill( ql, psd );
      }
      if ( t-tlast < 3000. && ql < 2000.0 ){
	hretrig2_psdql->Fill( ql, psd );
      } else {
	hsig2_psdql->Fill( ql, psd );
      }
      if ( t-tlast < 4000. && ql < 2000.0 ){
	hretrig3_psdql->Fill( ql, psd );
      } else {
	hsig3_psdql->Fill( ql, psd );
      }
      if ( t-tlast < 6000. && ql < 2000.0 ){
	hretrig4_psdql->Fill( ql, psd );
      } else {
	hsig4_psdql->Fill( ql, psd );
      }


    }
    tlast = t;
    
  }

  std::cout << "Signal and retrigger histograms filled" << std::endl;

  // // Fill the toy signal+background histogram 
  // ttoy->SetBranchAddress("T",&t);
  // ttoy->SetBranchAddress("QS",&qs);
  // ttoy->SetBranchAddress("QL",&ql);
  // ttoy->SetBranchAddress("BL",&bl);
  // nentries = ttoy->GetEntries();
  // for (Long64_t ientry=0; ientry<nentries; ientry++){
  //   ttoy->GetEntry(ientry);
  //   double psd = -0.999;
  //   if ( ql!=0 ) psd = ( ql - qs ) / ql;
  //   htoy_psdql->Fill( ql, psd );    
  // }

  // TCanvas* tc=new TCanvas();
  // tc->Divide(2,3);
  // tc->cd(1);
  // hsig_psdql->Draw("colz");
  // tc->cd(2);
  // hbg_psdql->Draw("colz");
  // tc->cd(3);
  // hretrig_psdql->Draw("colz");
  // tc->cd(4);
  // hpile_psdql->Draw("colz");
  // tc->cd(5);
  // htoy_psdql->Draw("colz");
  


  // TCanvas* tc2=new TCanvas();
  // tc2->Divide(2,2);
  // tc2->cd(1);
  // hretrig1_psdql->Draw("colz");
  // hretrig1_psdql->SetMaximum(50000.0);
  // tc2->cd(2);
  // hretrig2_psdql->Draw("colz");
  // hretrig2_psdql->SetMaximum(50000.0);
  // tc2->cd(3);
  // hretrig3_psdql->Draw("colz");
  // hretrig3_psdql->SetMaximum(50000.0);
  // tc2->cd(4);
  // hretrig4_psdql->Draw("colz");
  // hretrig4_psdql->SetMaximum(50000.0);


  // TCanvas* tc3=new TCanvas();
  // tc3->Divide(2,2);
  // tc3->cd(1);
  // hsig1_psdql->Draw("colz");
  // hsig1_psdql->SetMaximum(50000.0);
  // tc3->cd(2);
  // hsig2_psdql->Draw("colz");
  // hsig2_psdql->SetMaximum(50000.0);
  // tc3->cd(3);
  // hsig3_psdql->Draw("colz");
  // hsig3_psdql->SetMaximum(50000.0);
  // tc3->cd(4);
  // hsig4_psdql->Draw("colz");
  // hsig4_psdql->SetMaximum(50000.0);


  // TCanvas * tc4=new TCanvas();
  // tc4->Divide(1,2);
  // tc4->cd(1);
  // hqsvsdt->Draw("colz");
  // tc4->cd(2);
  // hqsvsdtall->Draw("colz");

  fout->Write();
}

int main(int argc, char * argv[]) {

  if (gConfig.ParseError() < 0) {
    std::cout << "Can't load 'makepdfs.ini'\n";
    return 1;
  }

  makepdfs();

  return 0;
}
