const double QMEANEQ = 7201.53; //5.26067e+03;//5000.0;
const double QSIGEQ = 1009.59;//9.75788e+02;

double mygaus( double * xx, double * pp ){
  double xv = xx[0];
  double N  = pp[0];
  double mu = pp[1];
  double sig = pp[2];

  double retval = N * TMath::Exp( -0.5 * (xv-mu) * (xv-mu) / sig / sig );

  return retval;
}

void dataforfit(char * infilename = "output00000054_0000.root"){

  std::cout<<"Hello World"<<std::endl;
  //gROOT->SetStyle("Plain");
  //gStyle->SetOptFit(1111);
  //gStyle->SetTitle(0);
  //gStyle->SetOptStat(0);
  // open file and trees to be read
  if (!( infilename && (*infilename) )) return; 
  TFile* fin = new TFile(infilename,"read");
  TTree* din = (TTree*) fin->Get("tUCN");
  if (!din) { return; }

  std::cout<<"Files opened... got tree"<<std::endl;
  
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
  
  char foutname[100];
  sprintf( foutname,"dataforfit_%s",infilename);
  std::cout<<"Open output file: "<<foutname<<std::endl;
  TFile * fout = new TFile( foutname, "recreate" );

  float qs;
  float ql;
  TTree * tout = new TTree("tdata","tdata");
  tout->Branch("QS",&qs,"QS/F");
  tout->Branch("QL",&ql,"QL/F");

  const int NCH=9;
  char aname[100];
  char atitle[200];
  TH1D* hql[NCH];
  TH1D* hqlr[NCH];
  TH2D* hpsdqlr[NCH];
  TH2D* hpsdqlrsum = new TH2D("hpsdqlrsum","All Equalized; CH%d Q_{L} (ADC); PSD", 150, 0.0, 15000.0, 40,-1.,1.); 
  for (int i=0; i<NCH; i++){
    sprintf(aname,"hql%02d",i);
    sprintf(atitle,"Channel %d Cut PSD>0.2; CH%d Q_{L} (ADC); Count",i,i);
    hql[i] = new TH1D(aname,atitle, 150, 0.0, 15000.0);
    sprintf(aname,"hqlr%02d",i);
    sprintf(atitle,"Channel %d Equalized Cut PSD>0.2; CH%d Q_{L} (ADC); Count",i,i);
    hqlr[i] = new TH1D(aname,atitle, 150, 0.0, 15000.0);
    sprintf(aname,"hpsdqlr%02d",i);
    sprintf(atitle,"Channel %d Equalized; CH%d Q_{L} (ADC); PSD",i,i);
    hpsdqlr[i] = new TH2D(aname,atitle, 150, 0.0, 15000.0, 40,-1.,1.);
  }

  // tree for runtime event data
  din->SetBranchAddress("tEntry",    &tEntry);
  din->SetBranchAddress("tTimeE",    &tTimeE);
  din->SetBranchAddress("tChannel",  &tChannel);
  din->SetBranchAddress("tPSD",      &tPSD);
  din->SetBranchAddress("tChargeL",  &tChargeL);
  din->SetBranchAddress("tChargeS",  &tChargeS);
  din->SetBranchAddress("tBaseline", &tBaseline);
  din->SetBranchAddress("tLength",   &tLength);

  std::cout<<"Set Branch addresses!"<<std::endl;
  

  // First pass, fill ql histograms to find 
  // mean and sigma of neutron peak for equalization
  // of channels
  ULong64_t nentries= din->GetEntries();
  //nentries=200000;
  for (ULong64_t j=0; j<=nentries; j++){
    if (j%100000==0) std::cout<<j<<" / "<<nentries<<std::endl;
    din->GetEvent(j);
    int ch = tChannel;
    if (tChannel==7) ch=-1;
    if (tChannel==8||tChannel==9) ch=tChannel-1;
    if (ch>=0 && ch<=NCH) {
      double psd = -2.0;
      if (tChargeL != 0 ) psd = (double(tChargeL)-double(tChargeS))/double(tChargeL);
      if ( psd > 0.2 ) 
	hql[ch]->Fill( tChargeL );
    }
  }

  std::cout<<"Histograms were filled!"<<std::endl;
  
  // Now Fit them
  double amean[NCH];
  double asig[NCH];
  TF1* afit[NCH];
  for (int ich=0; ich<NCH; ich++){
    sprintf(aname,"fit%d",ich);
    afit[ich] = new TF1( aname, mygaus, 0., 15000.0, 3);
    afit[ich]->SetParameters(hql[ich]->GetMaximum(), hql[ich]->GetMean(), hql[ich]->GetRMS());

    hql[ich]->Fit( afit[ich], "Q" );
    amean[ich] = afit[ich]->GetParameter(1);
    asig[ich] = afit[ich]->GetParameter(2);
    std::cout<<"Fit1 Ch"<<ich<<" mean="<<amean[ich]<<" sig="<<asig[ich]<<std::endl;


    hql[ich]->Fit( afit[ich],"Q","", 
		   amean[ich]-2.0*asig[ich],
		   amean[ich]+2.0*asig[ich] );
    amean[ich] = afit[ich]->GetParameter(1);
    asig[ich] = afit[ich]->GetParameter(2);
    std::cout<<"Fit2 Ch"<<ich<<" mean="<<amean[ich]<<" sig="<<asig[ich]<<std::endl;

    hql[ich]->Fit( afit[ich],"Q","", 
		   amean[ich]-1.0*asig[ich],
		   amean[ich]+1.0*asig[ich] );
    amean[ich] = afit[ich]->GetParameter(1);
    asig[ich] = afit[ich]->GetParameter(2);
    std::cout<<"Fit3 Ch"<<ich<<" mean="<<amean[ich]<<" sig="<<asig[ich]<<std::endl;
  }



 
  int order[NCH] = { 1, 2, 3, 0, 8, 4, 7, 6, 5 };

  TCanvas* c1 = new TCanvas();
  c1->Divide(3,3);
  for (int i=0; i<NCH; i++){
    c1->cd( i+1 );
    c1->GetPad( i+1 )->SetLogz();
    hql[ order[i] ]->Draw();
  }


  // Now second pass over data, fill equalized histos
  for (ULong64_t j=0; j<=nentries; j++){
    if (j%100000==0) std::cout<<j<<" / "<<nentries<<std::endl;
    din->GetEvent(j);
    int ch = tChannel;
    if (tChannel==7) ch=-1;
    if (tChannel==8||tChannel==9) ch=tChannel-1;
    if (ch>=0 && ch<=NCH) {
      double qlcor = double(tChargeL)* QMEANEQ / amean[ch];
      double qscor = double(tChargeS)* QMEANEQ / amean[ch];
      double psdcor = -2.0;
      if (qlcor != 0 ) psdcor = ( qlcor - qscor)/qlcor;

      // std::cout<<" Q="<<tChargeL<<" Qcor="<<qcor<<" QMEANEQ="<<QMEANEQ<<" amean="<<amean[ch]<<std::endl;
      if ( psdcor > 0.2 ) {
	hqlr[ch]->Fill( qlcor );
      }
      hpsdqlr[ch]->Fill( qlcor, psdcor );
      hpsdqlrsum->Fill( qlcor, psdcor );
      qs = qscor;
      ql = qlcor;
      tout->Fill();
    }
  }


  
  // Second Fit to check equalization them
  double ameanr[NCH];
  double asigr[NCH];
  TF1* afitr[NCH];
  for (int ich=0; ich<NCH; ich++){
    sprintf(aname,"fitr%d",ich);
    afitr[ich] = new TF1( aname, mygaus, 0., 15000.0, 3);
    afitr[ich]->SetParameters(hqlr[ich]->GetMaximum(), hqlr[ich]->GetMean(), hqlr[ich]->GetRMS());

    hqlr[ich]->Fit( afitr[ich], "Q" );
    ameanr[ich] = afitr[ich]->GetParameter(1);
    asigr[ich] = afitr[ich]->GetParameter(2);
    std::cout<<"Fit1 Ch"<<ich<<" mean="<<ameanr[ich]<<" sig="<<asigr[ich]<<std::endl;


    hqlr[ich]->Fit( afitr[ich],"Q","", 
		   ameanr[ich]-2.0*asigr[ich],
		   ameanr[ich]+2.0*asigr[ich] );
    ameanr[ich] = afitr[ich]->GetParameter(1);
    asigr[ich] = afitr[ich]->GetParameter(2);
    std::cout<<"Fit2 Ch"<<ich<<" mean="<<ameanr[ich]<<" sig="<<asigr[ich]<<std::endl;

    hqlr[ich]->Fit( afitr[ich],"Q","", 
		   ameanr[ich]-1.0*asigr[ich],
		   ameanr[ich]+1.0*asigr[ich] );
    ameanr[ich] = afitr[ich]->GetParameter(1);
    asigr[ich] = afitr[ich]->GetParameter(2);
    std::cout<<"Fit3 Ch"<<ich<<" mean="<<ameanr[ich]<<" sig="<<asigr[ich]<<std::endl;
  }



  TCanvas* c2 = new TCanvas();
  c2->Divide(3,3);
  for (int i=0; i<NCH; i++){
    c2->cd( i+1 );
    c2->GetPad( i+1 )->SetLogz();
    hqlr[ order[i] ]->Draw();
  }


  TCanvas* c3 = new TCanvas();
  c3->Divide(3,3);
  for (int i=0; i<NCH; i++){
    c3->cd( i+1 );
    c3->GetPad( i+1 )->SetLogz();
    hpsdqlr[ order[i] ]->Draw("colz");
  }

  TCanvas* c4 = new TCanvas();
  c4->GetPad(0)->SetLogz();
  hpsdqlrsum->Draw("colz");

  fout->Write();

  std::cout<<"Done!"<<std::endl;
}
