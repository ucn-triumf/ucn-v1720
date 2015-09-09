void QSQLPerCh(char * infilename = "output00604.root"){

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
  sprintf( foutname,"ql_vs_qs_%s",infilename);
  std::cout<<"Open output file: "<<foutname<<std::endl;
  TFile * fout = new TFile( foutname, "recreate" );

  const int NCH=9;
  char aname[100];
  char atitle[200];
  TH2D* hqlqs[NCH];
  for (int i=0; i<NCH; i++){
    sprintf(aname,"hqlqs%02d",i);
    sprintf(atitle,"Channel %d ; CH%d Q_{L} (ADC); CH %dQ_{S} (ADC)",i,i,i);
    hqlqs[i] = new TH2D(aname,atitle, 200, 0.0, 10000.0, 200, 0.0, 3000.0);
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
  
  ULong64_t nentries= din->GetEntries();
  //nentries=200000;
  for (ULong64_t j=0; j<=nentries; j++){
    if (j%10000==0) std::cout<<j<<" / "<<nentries<<std::endl;
    din->GetEvent(j);
    int ch = tChannel;
    if (ch>=0 && ch<=NCH) {
      hqlqs[ch]->Fill( tChargeL, tChargeS );
    }else{
      std::cout<<"WARN: ch="<<ch<<" for entry "<<j<<std::endl;
    }
  }

  std::cout<<"Histograms were filled!"<<std::endl;
 
  fout->Write();
  int order[NCH] = { 1, 2, 3, 0, 8, 4, 7, 6, 5 };

  TCanvas* c1 = new TCanvas();
  c1->Divide(3,3);
  for (int i=0; i<NCH; i++){
    c1->cd( i+1 );
    c1->GetPad( i+1 )->SetLogz();
    hqlqs[ order[i] ]->Draw("colz");
  }
  std::cout<<"Done!"<<std::endl;
}
