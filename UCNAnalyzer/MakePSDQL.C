void MakePSDQL(char * infilename = "output00604.root"){

  std::cout<<"Hello World"<<std::endl;
  gROOT->SetStyle("Plain");
  gStyle->SetOptFit(1111);
  gStyle->SetTitle(0);
  gStyle->SetOptStat(0);
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
  sprintf( foutname,"psd_vs_ql_%s",infilename);
  std::cout<<"Open output file: "<<foutname<<std::endl;
  TFile * fout = new TFile( foutname, "recreate" );
  TH2D* hqlqs = new TH2D("hpsdql","PSD vs Long gate charge;Charge Long (ADC); PSD", 150,0.,15000.0,40,-1., 1.);

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
  
  ULong64_t nentries= din->GetEntries();
  for (ULong64_t j=0; j<=nentries; j++){
    if (j%10000==0) std::cout<<j<<" / "<<nentries<<std::endl;
    din->GetEvent(j);
    int ch = tChannel;
    if (ch > 9 || ch == 7 )
      continue;  // skip anything that isn't a pmt channel
    double psd = -2.0;
    if ( tChargeL != 0 ) psd = ( double(tChargeL) - double(tChargeS) ) / double(tChargeL);

    hpsdql->Fill( double(tChargeL), psd );
  }

  std::cout<<"Histogram was filled!"<<std::endl;
 
  fout->Write();
  TCanvas* c1 = new TCanvas();
  c1->GetPad(0)->SetLogz();
  hpsdql->Draw("colz");
  std::cout<<"Done!"<<std::endl;
}
