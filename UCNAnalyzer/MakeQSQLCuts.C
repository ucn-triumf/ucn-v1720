void MakeQSQLCuts(char * infilename = "output00604.root"){

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
  sprintf( foutname,"ql_vs_qs_%s",infilename);
  std::cout<<"Open output file: "<<foutname<<std::endl;
  TFile * fout = new TFile( foutname, "recreate" );
  TH2D* hqlqs = new TH2D("hqlqs","Short vs Long gate charge;Charge Long (ADC);Charge Short (ADC)", 400, 0.0, 40000.0, 400, 0.0, 40000.0);

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
    if (ch != 0)
      continue;  // skip anything that isn't channel 7
    if ( tChargeS >300.0 && tChargeL-tChargeS >50.0 )
      hqlqs->Fill( tChargeL, tChargeS );
  }

  std::cout<<"Histogram was filled!"<<std::endl;
 
  fout->Write();
  TCanvas* c1 = new TCanvas();
  c1->GetPad(0)->SetLogz();
  hqlqs->Draw("colz");
  std::cout<<"Done!"<<std::endl;
}
