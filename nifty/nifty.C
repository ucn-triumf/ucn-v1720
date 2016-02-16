
#include <TROOT.h>
#include <TSystem.h>
#include <TF1.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TRandom.h>
#include <TMath.h>
#include <TDirectory.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TTree.h>
#include <TFile.h>
#include <TMinuit.h>
#include "ini.h"
#include "INIReader.h"

INIReader gConfig("nifty.ini");

// Only fit in a particular range:
double g_psdmin = gConfig.GetReal("fitparameter","psdmin",-0.75);
double g_psdmax = gConfig.GetReal("fitparameter","psdmin",-0.75);
double g_qlmin =  gConfig.GetReal("fitparameter","qlmin",200.0);
double g_qlmax = gConfig.GetReal("fitparameter","qlmax",15000.0);

int g_bpsdmin = 16;
int g_bpsdmax = 40;
int g_bqlmin  =3; 
int g_bqlmax  =90; 

const int g_npdfs = 10;
const int g_cols[g_npdfs] = { kOrange, kYellow+1, kYellow+3, kSpring-7, kGreen+2, kRed, kPink+8, kViolet+7, kBlue, kCyan+3};

TH2D* g_hpdf[g_npdfs];
TH2D* g_hdt;

// PDFs as histograms:
TH2D * gPbg;
TH2D * gPgbg;
TH2D * gPgbgpile;
TH2D * gPlate;
TH2D * gPretrig;
TH2D * gPsig;
TH2D * gPsiggbg;
TH2D * gPsiggbgpile;
TH2D * gPsigpile;
TH2D * gPsiggpilegbg;
TH2D * gPpile;

// Data as a TTree:
TTree * g_gData;
int gNdata;
float gQS;
float gQL;
// For now lets do binned likelihood fit
// only need unbinned if we have continuous PDFs rather than binned PDFs.
TH2D * gDt;

// Likelihood function
void NiftyLikelihood( int &npar, double * gin, double &ff, double *par, int iflag){
  static int counter=0;
  counter++;
  
  double negLogL = 0.0;
  for (int i=0; i<g_npdfs; i++) negLogL += fabs(par[i]);

  // loop over the histogram bins
  //for (int i=1; i<= gDt->GetNbinsX(); i++){
  //  for (int j=1; j<= gDt->GetNbinsY(); j++){
  //    double ndata = gDt->GetBinContent( i, j );
  //    if (ndata<=0) continue; // bins with zero entries add nothing to likelihood
  for (int i=0; i< gNdata; i++){
    //std::cout<<"i="<<i1<<"/"<<gNdata<<std::endl;
    if (g_gData==NULL) {
      std::cout<<"g_gData is NULL"<<std::endl;
      ff=negLogL;
      return;
    }
    //    std::cout<<g_gData->GetEntries()<<std::endl;
    g_gData->GetEntry(i);
    
    float psd = 0.0;
    if ( gQL != 0.0 ) psd = (gQL-gQS)/gQL;

    if ( psd < g_psdmin || psd > g_psdmax ||
	 gQL < g_qlmin || gQL > g_qlmax ) continue;
    
    double sumNP = 0.0;
    int ibin=g_hpdf[5]->FindBin( gQL, psd ); 
    //std::cout<<"count="<<counter<<" ibin="<<ibin<<std::endl;
    sumNP += fabs(par[0])*g_hpdf[0]->GetBinContent( ibin );
    //std::cout<<"pdf0 sumNP="<<sumNP<<std::endl;
    sumNP += fabs(par[1])*g_hpdf[1]->GetBinContent( ibin );
    //std::cout<<"pdf1 sumNP="<<sumNP<<std::endl;
    sumNP += fabs(par[2])*g_hpdf[2]->GetBinContent( ibin );
    //std::cout<<"pdf2 sumNP="<<sumNP<<std::endl;
    sumNP += fabs(par[3])*g_hpdf[3]->GetBinContent( ibin );
    //std::cout<<"pdf3 sumNP="<<sumNP<<std::endl;
    sumNP += fabs(par[4])*g_hpdf[4]->GetBinContent( ibin );
    //std::cout<<"pdf4 sumNP="<<sumNP<<std::endl;
    sumNP += fabs(par[5])*g_hpdf[5]->GetBinContent( ibin );
    //std::cout<<"pdf5 sumNP="<<sumNP<<std::endl;
    sumNP += fabs(par[6])*g_hpdf[6]->GetBinContent( ibin );
    //std::cout<<"pdf6 sumNP="<<sumNP<<std::endl;
    sumNP += fabs(par[7])*g_hpdf[7]->GetBinContent( ibin );
    //std::cout<<"pdf7 sumNP="<<sumNP<<std::endl;
    sumNP += fabs(par[8])*g_hpdf[8]->GetBinContent( ibin );
    //std::cout<<"pdf8 sumNP="<<sumNP<<std::endl;
    sumNP += fabs(par[9])*g_hpdf[9]->GetBinContent( ibin );
    //std::cout<<"pdf9 sumNP="<<sumNP<<std::endl;
    //sumNP += fabs(par[10])*g_hpdf[10]->GetBinContent( ibin );
    //std::cout<<"pdf10 sumNP="<<sumNP<<std::endl;


    //sumNP *= ndata;
    if (sumNP<=0){
      // don't want this to happen, it means all
      // PDFs have zero probability where we actuall
      // do have some data!
      if (false){
	std::cout<<"sumNP="<<sumNP
		 <<" ibin="<<ibin
		 <<" QL="<< gQL
		 <<" psd="<<psd
		 <<" pdf1="<<g_hpdf[0]->GetBinContent(ibin)
		 <<" pdf2="<<g_hpdf[1]->GetBinContent(ibin)
		 <<" pdf3="<<g_hpdf[2]->GetBinContent(ibin)
		 <<" pdf4="<<g_hpdf[3]->GetBinContent(ibin)
		 <<" pdf5="<<g_hpdf[4]->GetBinContent(ibin)
		 <<" pdf6="<<g_hpdf[5]->GetBinContent(ibin)
		 <<" pdf7="<<g_hpdf[6]->GetBinContent(ibin)
		 <<" pdf8="<<g_hpdf[7]->GetBinContent(ibin)
		 <<" pdf9="<<g_hpdf[8]->GetBinContent(ibin)
		 <<" pdf10="<<g_hpdf[9]->GetBinContent(ibin)
	  //<<" pdf11="<<g_hpdf[10]->GetBinContent(ibin)
		 <<" p0="<<par[0]
		 <<" p1="<<par[1]
		 <<" p2="<<par[2]
		 <<" p3="<<par[3]
		 <<" p4="<<par[4]
		 <<" p5="<<par[5]
		 <<" p6="<<par[6]
		 <<" p7="<<par[7]
		 <<" p8="<<par[8]
		 <<" p9="<<par[9]
	  //<<" p10="<<par[10]
		 << std::endl;
      }
      negLogL += sumNP*sumNP;
    } else {
	negLogL -= std::log( sumNP );
    }
  }

  if (counter%100==0) std::cout<<"LL counter="<<counter<<" -LL="<<negLogL<<std::endl;


  ff = negLogL;

  return;
}

// main ---------------------------------------------------------------
// Blair Jamieson, Dec. 2015
int main( int argc, const char* argv[]){

  if (gConfig.ParseError() < 0) {
    std::cout << "Can't load 'ucnrate.ini'\n";
    return 1;
  }
  // if ( argc != 3 ){
  //   std::cout<<"Usage: "<<std::endl;
  //   std::cout<<" nifty pdfs.root data.root\n"<<std::endl;
  //   return 0;
  // }

  // gSystem->Load("libTree.so");

  // const char * mc_fname = argv[1];
  // const char * dt_fname = argv[2];
  // std::cout<<"MC File: "<<mc_fname<<std::endl;
  // std::cout<<"DT File: "<<dt_fname<<std::endl;
  std::string strfname = gConfig.Get("input","mcfilename","PDFs.root");
  TFile * fmc = new TFile((const char *)strfname.c_str(), "read" );
  std::cout<<"fmc pointer = "<< fmc <<std::endl;
  if (fmc == NULL){
    std::cout<<"Failed to open "<<strfname<<std::endl;
    return 0;
  }
  if (fmc->IsOpen()==kFALSE){
    std::cout<<"Problem opening mc "<<strfname<<std::endl;
    return 0;
  }
  
  // pull histograms
  std::cout << "Pulling pdfs for fit" << std::endl;
  g_hpdf[0]  = (TH2D*)fmc->Get("hbg_psdql");     
  g_hpdf[1]  = (TH2D*)fmc->Get("hgbg_psdql"); 
  g_hpdf[2]  = (TH2D*)fmc->Get("hgbgpile_psdql");
  g_hpdf[4]  = (TH2D*)fmc->Get("hlate_psdql");     
  g_hpdf[3]  = (TH2D*)fmc->Get("hretrig_psdql"); 
  g_hpdf[5]  = (TH2D*)fmc->Get("hsig_psdql");    
  g_hpdf[6]  = (TH2D*)fmc->Get("hsiggbg_psdql");      
  g_hpdf[7]  = (TH2D*)fmc->Get("hsiggbgpile_psdql");
  g_hpdf[8]  = (TH2D*)fmc->Get("hsigpile_psdql");
  g_hpdf[9]  = (TH2D*)fmc->Get("hsiggbgpile_psdql");
  //g_hpdf[10] = (TH2D*)fmc->Get("hpile_psdql");

  // gPbg         = (TH2D*)fmc->Get("hbg_psdql");
  // gPgbg        = (TH2D*)fmc->Get("hgbg_psdql");
  // gPgbgpile    = (TH2D*)fmc->Get("hgbgpile_psdql");
  // gPlate       = (TH2D*)fmc->Get("hlate_psdql");
  // gPretrig     = (TH2D*)fmc->Get("hretrig_psdql");
  // gPsig        = (TH2D*)fmc->Get("hsig_psdql");
  // gPsiggbg     = (TH2D*)fmc->Get("hsigbg_psdql");
  // gPsiggbgpile = (TH2D*)fmc->Get("hsiggbgpile_psdql");
  // gPsigpile    = (TH2D*)fmc->Get("hsigpile_psdql");
  // gPsigpilegbg = (TH2D*)fmc->Get("hsigpilegbg_psdql");
  // gPpile       = (TH2D*)fmc->Get("hpile_psdql");


  // smooth data to try to make up for poor statistics
  // remove these lines when more data is available
  // for the PDFs
  //gPsig->Smooth();
  //gPretrig->Smooth();
  //gPbg->Smooth();
   
  // normalize the PDFs.
  std::cout << "Normalizing" << std::endl;
  std::cout << "Scale pdfs" << std::endl;
  double sf;
  for (int i=0; i<g_npdfs; i++){
    sf = g_hpdf[i]->Integral("width");
    std::cout<<"PDF "<<i<<" sf="<<sf<<std::endl;
    if (sf != 0) 
      g_hpdf[i]->Scale( 1.0/ sf );
  }
 

  // double nmcsig = gPsig->Integral( g_bqlmin, g_bqlmax, g_bpsdmin, g_bpsdmax );
  // gPsig->Scale( 1.0 / nmcsig );
  // double nmcret = gPretrig->Integral(  g_bqlmin, g_bqlmax, g_bpsdmin, g_bpsdmax );
  // gPretrig->Scale( 1.0 / nmcret );
  // double nmcpil = gPpile->Integral(  g_bqlmin, g_bqlmax, g_bpsdmin, g_bpsdmax );
  // gPpile->Scale( 1.0 / nmcpil );
  // double nmcbg = gPbg->Integral(  g_bqlmin, g_bqlmax, g_bpsdmin, g_bpsdmax );
  // gPbg->Scale( 1.0 / nmcbg );

  // read in data
  std::cout << "Read in data file" << std::endl;
  strfname = gConfig.Get("input","datafilename","dataforfit_output00000054_0000.root");
  TFile * fdt = new TFile( (const char *)strfname.c_str(), "read" );
  std::cout<<"fdt pointer = "<< fdt <<std::endl;
  if (fdt == NULL){
    std::cout<<"Failed to open "<<strfname<<std::endl;
    return 0;
  }
  if ( fdt->IsOpen()==kFALSE){
    std::cout<<"Problem opening dt "<<strfname<<std::endl;
    return 0;
  }
  fdt->ls();
  std::cout<<"About to get TTree"<<std::endl;
  g_gData = (TTree*)fdt->Get( "tdata" );
  std::cout<<" TTree pointer = "<<g_gData<<std::endl;
  if (g_gData==NULL){
    std::cout<<"Problem getting tdata TTree from "<<strfname<<std::endl;
    return 0;

  } else {
    std::cout<<"g_gData="<<g_gData<<std::endl;
  }

  g_gData->SetBranchAddress( "QS", &gQS );
  g_gData->SetBranchAddress( "QL", &gQL );


  gDt = (TH2D*)g_hpdf[5]->Clone( "hdata" );
  gDt->SetName("hdata");
  gDt->Clear();
  gNdata = g_gData->GetEntries();
  for (int i=0; i<gNdata; i++){
    g_gData->GetEntry(i);
    float psd = 0.0;
    if ( gQL != 0.0 ) psd = (gQL-gQS)/gQL;
    gDt->Fill( gQL, psd);
  }


  //int npars = 4;
  std::cout << "set up minuit" << std::endl;
  double arglist[16];
  int ierflg[16];
  TMinuit * minuit = new TMinuit( g_npdfs );
  double level = 0;
  minuit->mnexcm("SET PRINT", &level, 1, ierflg[15]);
  double width=300;
  minuit->mnexcm("SET WIDTH", &width, 1, ierflg[15]);
  double fcnerror=0.5;
  minuit->mnexcm("SET ERR", &fcnerror, 1, ierflg[15]);
  
  std::cout <<"Set likelihood function" << std::endl;
  minuit->SetFCN( NiftyLikelihood );

  minuit->mncler();
  
  minuit->mnparm( 0, "", 2.5e6, 0.01, 0, 0, ierflg[16] );
  minuit->mnparm( 1, "", 1.85e6, 0.01, 0, 0, ierflg[16] );
  minuit->mnparm( 2, "", 2.0e6, 0.01, 0, 0, ierflg[16] );
  minuit->mnparm( 3, "", 6.0e5, 0.01, 0, 0, ierflg[16] );
  minuit->mnparm( 4, "", 1.7e6, 0.01, 0, 0, ierflg[16] );
  minuit->mnparm( 5, "", 7.73e7, 0.01, 0, 0, ierflg[16] );
  minuit->mnparm( 6, "", 1.7e7, 0.01, 0, 0, ierflg[16] );
  minuit->mnparm( 7, "", 1.7e5, 0.01, 0, 0, ierflg[16] );
  minuit->mnparm( 8, "", 9.0e4, 0.01, 0, 0, ierflg[16] );
  minuit->mnparm( 9, "", 1.7e0, 0.01, 0, 0, ierflg[16] );
  //minuit->mnparm( 10, "",5.7e2, 0.01, 0, 0, ierflg[16] );

  arglist[0] = 50; // max of 5000 iterations
  arglist[1] = 1;
  std::cout<<"before migrad g_gData="<<g_gData<<std::endl;
  minuit->mnexcm("MIGRAD",arglist,2,ierflg[0]);
  std::cout<<"after migrad g_gData="<<g_gData<<std::endl;

  // get parameters from this run
  double apars[g_npdfs];
  double aparerrs[g_npdfs];
  for (int i=0; i<g_npdfs; i++){
    minuit->GetParameter(i,apars[i],aparerrs[i] );
  }
    
  // re-initialize minuit to rerun
  // need to do this to eliminate dependence on the initial conditions
  // which is inherent to Levenberg Marquardt method used by Minuit
  minuit->mncler();

  minuit->mnparm( 0, "", apars[0], 0.01, apars[0]-5.0*aparerrs[0], apars[0]+5.0*aparerrs[0], ierflg[16] );
  minuit->mnparm( 1, "", apars[1], 0.01, apars[1]-5.0*aparerrs[1], apars[1]+5.0*aparerrs[1], ierflg[16] );
  minuit->mnparm( 2, "", apars[2], 0.01, apars[2]-5.0*aparerrs[2], apars[2]+5.0*aparerrs[2], ierflg[16] );
  minuit->mnparm( 3, "", apars[3], 0.01, apars[3]-5.0*aparerrs[3], apars[3]+5.0*aparerrs[3], ierflg[16] );
  minuit->mnparm( 4, "", apars[4], 0.01, apars[4]-5.0*aparerrs[4], apars[4]+5.0*aparerrs[4], ierflg[16] );
  minuit->mnparm( 5, "", apars[5], 0.01, apars[5]-5.0*aparerrs[5], apars[5]+5.0*aparerrs[5], ierflg[16] );
  minuit->mnparm( 6, "", apars[6], 0.01, apars[6]-5.0*aparerrs[6], apars[6]+5.0*aparerrs[6], ierflg[16] );
  minuit->mnparm( 7, "", apars[7], 0.01, apars[7]-5.0*aparerrs[7], apars[7]+5.0*aparerrs[7], ierflg[16] );
  minuit->mnparm( 8, "", apars[8], 0.01, apars[8]-5.0*aparerrs[8], apars[8]+5.0*aparerrs[8], ierflg[16] );
  minuit->mnparm( 9, "", apars[9], 0.01, apars[9]-5.0*aparerrs[9], apars[9]+5.0*aparerrs[9], ierflg[16] );
  //minuit->mnparm( 2, "", apars[10], 0.01, apars[10]-5.0*aparerrs[10], apars[10]+5.0*aparerrs[10], ierflg[16] );
  

  // minuit->mnparm( 0, "Signal", apars[0], 0.01, apars[0]-5.0*aparerrs[0], apars[0]+5.0*aparerrs[0], ierflg[9] );
  // minuit->mnparm( 1, "Retrig", apars[1], 0.01, apars[1]-5.0*aparerrs[1], apars[1]+5.0*aparerrs[1], ierflg[9] );
  // minuit->mnparm( 2, "Pile-up",apars[2], 0.01, apars[2]-5.0*aparerrs[2], apars[2]+5.0*aparerrs[2], ierflg[9] );
  // minuit->mnparm( 3, "Backgr", apars[3], 0.01, apars[3]-5.0*aparerrs[3], apars[3]+5.0*aparerrs[3], ierflg[9] );

  arglist[0] = 50; // max of 5000 iterations
  arglist[1] = 2;
  minuit->mnexcm("MIGRAD",arglist,2,ierflg[1]);
  arglist[0] = -1;
  arglist[1] = -1;
  minuit->mnexcm("MINOS",arglist,0,ierflg[2]);
  minuit->mnexcm("SHOW COR",arglist,0,ierflg[3]);

  for (int i=0; i<g_npdfs; i++){
    minuit->GetParameter(i,apars[i],aparerrs[i] );
  }

  //scale final pdfs by results
  g_hpdf[0]->Scale(apars[0]);  // kOrange
  g_hpdf[1]->Scale(apars[1]); // kYellow+1
  g_hpdf[2]->Scale(apars[2]);  // kYellow+3
  g_hpdf[3]->Scale(apars[3]);  // kSpring-7
  g_hpdf[4]->Scale(apars[4]);  // kGreen+2
  g_hpdf[5]->Scale(apars[5]); // kRed 
  g_hpdf[6]->Scale(apars[6]);  // kPink+8
  g_hpdf[7]->Scale(apars[7]);  // kViolet+7
  g_hpdf[8]->Scale(apars[8]);  // kBlue
  g_hpdf[9]->Scale(apars[9]);  // kCyan+3
  //g_hpdf[10]->Scale(apars[10]); // kTeal-6

  // gPsig->Scale( apars[0] );
  // gPretrig->Scale( apars[1] );
  // gPpile->Scale( apars[2] );
  // gPbg->Scale( apars[3] );

  strfname = gConfig.Get("output","filename","nifty.root");
  TFile * fout=new TFile((const char *)strfname.c_str(),"recreate");
  fout->cd();

  g_hpdf[0]->Write();  // kOrange
  g_hpdf[1]->Write(); // kYellow+1
  g_hpdf[2]->Write();  // kYellow+3
  g_hpdf[3]->Write();  // kSpring-7
  g_hpdf[4]->Write();  // kGreen+2
  g_hpdf[5]->Write(); // kRed 
  g_hpdf[6]->Write();  // kPink+8
  g_hpdf[7]->Write();  // kViolet+7
  g_hpdf[8]->Write();  // kBlue
  g_hpdf[9]->Write();  // kCyan+3
  //g_hpdf[10]->Write(); // kTeal-6
  // gPsig->Write();
  // gPretrig->Write();
  // gPpile->Write();
  // gPbg->Write();
  //gDt->SetName("hdata");
  gDt->Write("hdata");
  fout->Close();
  

  return 0;
}
// end main -----------------------------------------------------------
