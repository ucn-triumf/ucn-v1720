
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


// Only fit in a particular range:
double g_psdmin = -0.25;
double g_psdmax = 1.0;
double g_qlmin = 200.0;
double g_qlmax = 9000.0;

int g_bpsdmin = 16;
int g_bpsdmax = 40;
int g_bqlmin  =3; 
int g_bqlmax  =90; 


// PDFs as histograms:
TH2D * gPsig;
TH2D * gPretrig;
TH2D * gPbg;

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
  for (int i=0; i<npar; i++) negLogL += fabs(par[i]);

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
    int ibin=gPsig->FindBin( gQL, psd ); 
    sumNP += fabs(par[0])*gPsig->GetBinContent( ibin );
    sumNP += fabs(par[1])*gPretrig->GetBinContent( ibin );
    sumNP += fabs(par[2])*gPbg->GetBinContent( ibin );
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
		 <<" pdf1="<<gPsig->GetBinContent(ibin)
		 <<" pdf2="<<gPretrig->GetBinContent(ibin)
		 <<" pdf3="<<gPbg->GetBinContent(ibin)
		 <<" p0="<<par[0]
		 <<" p1="<<par[1]
		 <<" p2="<<par[2]
		 << std::endl;
      }
      negLogL += sumNP*sumNP;
    } else {
	negLogL -= std::log( sumNP );
    }
  }

  if (counter%5==0) std::cout<<"LL counter="<<counter<<" -LL="<<negLogL<<std::endl;


  ff = negLogL;

  return;
}

// main ---------------------------------------------------------------
// Blair Jamieson, Dec. 2015
int main( int argc, const char* argv[]){

  if ( argc != 3 ){
    std::cout<<"Usage: "<<std::endl;
    std::cout<<" nifty pdfs.root data.root\n"<<std::endl;
    return 0;
  }

  gSystem->Load("libTree.so");

  const char * mc_fname = argv[1];
  const char * dt_fname = argv[2];
  std::cout<<"MC File: "<<mc_fname<<std::endl;
  std::cout<<"DT File: "<<dt_fname<<std::endl;

  TFile * fmc = new TFile( mc_fname, "read" );
  std::cout<<"fmc pointer = "<< fmc <<std::endl;
  if (fmc == NULL){
    std::cout<<"Failed to open "<<mc_fname<<std::endl;
    return 0;
  }
  if (fmc->IsOpen()==kFALSE){
    std::cout<<"Problem opening mc "<<mc_fname<<std::endl;
    return 0;
  }
  
  gPsig = (TH2D*)fmc->Get("hsig_psdql");
  gPretrig = (TH2D*)fmc->Get("hretrig_psdql");
  gPbg = (TH2D*)fmc->Get("hbg_psdql");

  // smooth data to try to make up for poor statistics
  // remove these lines when more data is available
  // for the PDFs
  //gPsig->Smooth();
  //gPretrig->Smooth();
  //gPbg->Smooth();
   
  // normalize the PDFs.
  double nmcsig = gPsig->Integral( g_bqlmin, g_bqlmax, g_bpsdmin, g_bpsdmax );
  gPsig->Scale( 1.0 / nmcsig );
  double nmcret = gPretrig->Integral(  g_bqlmin, g_bqlmax, g_bpsdmin, g_bpsdmax );
  gPretrig->Scale( 1.0 / nmcret );
  double nmcbg = gPbg->Integral(  g_bqlmin, g_bqlmax, g_bpsdmin, g_bpsdmax );
  gPbg->Scale( 1.0 / nmcbg );

  TFile * fdt = new TFile( dt_fname, "read" );
  std::cout<<"fdt pointer = "<< fdt <<std::endl;
  if (fdt == NULL){
    std::cout<<"Failed to open "<<dt_fname<<std::endl;
    return 0;
  }
  if ( fdt->IsOpen()==kFALSE){
    std::cout<<"Problem opening dt "<<dt_fname<<std::endl;
    return 0;
  }
  fdt->ls();
  std::cout<<"About to get TTree"<<std::endl;
  g_gData = (TTree*)fdt->Get( "tdata" );
  std::cout<<" TTree pointer = "<<g_gData<<std::endl;
  if (g_gData==NULL){
    std::cout<<"Problem getting tdata TTree from "<<dt_fname<<std::endl;
    return 0;

  } else {
    std::cout<<"g_gData="<<g_gData<<std::endl;
  }

  g_gData->SetBranchAddress( "QS", &gQS );
  g_gData->SetBranchAddress( "QL", &gQL );


  gDt = (TH2D*)gPsig->Clone( "hdata" );
  gDt->SetName("hdata");
  gDt->Clear();
  gNdata = g_gData->GetEntries();
  for (int i=0; i<gNdata; i++){
    g_gData->GetEntry(i);
    float psd = 0.0;
    if ( gQL != 0.0 ) psd = (gQL-gQS)/gQL;
    gDt->Fill( gQL, psd);
  }


  int npars = 3;

  double arglist[10];
  int ierflg[10];
  TMinuit * minuit = new TMinuit( npars );
  double level = 0;
  minuit->mnexcm("SET PRINT", &level, 1, ierflg[9]);
  double width=300;
  minuit->mnexcm("SET WIDTH", &width, 1, ierflg[9]);
  double fcnerror=0.5;
  minuit->mnexcm("SET ERR", &fcnerror, 1, ierflg[9]);
  
  minuit->SetFCN( NiftyLikelihood );

  minuit->mncler();
  
  minuit->mnparm( 0, "Signal", 450000.0, 0.01, 0, 0, ierflg[9] );
  minuit->mnparm( 1, "Retrig", 400000.0, 0.01, 0, 0, ierflg[9] );
  minuit->mnparm( 2, "Backgr", 400000.0, 0.01, 0, 0, ierflg[9] );

  arglist[0] = 500; // max of 5000 iterations
  arglist[1] = 1;
  std::cout<<"before migrad g_gData="<<g_gData<<std::endl;
  minuit->mnexcm("MIGRAD",arglist,2,ierflg[0]);
  std::cout<<"after migrad g_gData="<<g_gData<<std::endl;

  // get parameters from this run
  double apars[3];
  double aparerrs[3];
  for (int i=0; i<npars; i++){
    minuit->GetParameter(i,apars[i],aparerrs[i] );
  }
    
  // re-initialize minuit to rerun
  // need to do this to eliminate dependence on the initial conditions
  // which is inherent to Levenberg Marquardt method used by Minuit
  minuit->mncler();

  minuit->mnparm( 0, "Signal", apars[0], 0.01, apars[0]-5.0*aparerrs[0], apars[0]+5.0*aparerrs[0], ierflg[9] );
  minuit->mnparm( 1, "Retrig", apars[1], 0.01, apars[1]-5.0*aparerrs[1], apars[1]+5.0*aparerrs[1], ierflg[9] );
  minuit->mnparm( 2, "Backgr", apars[2], 0.01, apars[2]-5.0*aparerrs[2], apars[2]+5.0*aparerrs[2], ierflg[9] );

  arglist[0] = 5000; // max of 5000 iterations
  arglist[1] = 2;
  minuit->mnexcm("MIGRAD",arglist,2,ierflg[1]);
  arglist[0] = -1;
  arglist[1] = -1;
  minuit->mnexcm("MINOS",arglist,0,ierflg[2]);
  minuit->mnexcm("SHOW COR",arglist,0,ierflg[3]);

  for (int i=0; i<npars; i++){
    minuit->GetParameter(i,apars[i],aparerrs[i] );
  }

  //scale final pdfs by results
  gPsig->Scale( apars[0] );
  gPretrig->Scale( apars[1] );
  gPbg->Scale( apars[2] );

  TFile * fout=new TFile("nifty.root","recreate");
  fout->cd();
  gPsig->Write();
  gPretrig->Write();
  gPbg->Write();
  //gDt->SetName("hdata");
  gDt->Write("hdata");
  fout->Close();
  

  return 0;
}
// end main -----------------------------------------------------------
