void GetPSDOffsets2(char *infilename = "output00000094.root"){

  gStyle->SetOptFit(1111);

  TFile* fin = new TFile(infilename,"read");
  TTree* tin = (TTree*) fin->Get("tUCN");
  
  // PSD
  const int NCH=9;
  TCanvas*tc=new TCanvas();
  tc->Divide(3,3);
  TH1D* hpsd[NCH];
  TF1* fgaus[NCH];
  char aname[100];
  char anum[100];
  for (int ich=0; ich<9; ich++){
    sprintf(aname,"ch%dpsd",ich);
    hpsd[ich] = new TH1D(aname,aname,100,0.,1.0);
    sprintf(aname,"tPSD>>ch%dpsd",ich);
    sprintf(anum,"tChannel == %d && tChargeL-tChargeS>800.0 && tPSD<1.0 && tChargeL>2500.0 && tChargeS>500",ich);
    tc->cd(ich+1);
    cout<<aname<<endl;
    tin->Draw(aname,anum,"");
    sprintf(aname,"tf%d",ich);
    fgaus[ich]=new TF1(aname,"gaus",0.,1.);
    fgaus[ich]->SetParameters(1200,0.55,0.1);
    hpsd[ich]->Fit(fgaus[ich]);
    double mean = fgaus[ich]->GetParameter(1);
    double sig  = fgaus[ich]->GetParameter(2);
    hpsd[ich]->Fit(fgaus[ich],"","",mean-2.0*sig, mean+2.0*sig);

  }

  cout<<"Channel  MeanPSD   SigmaPSD  "<<std::endl;
  for (int i=0; i<NCH; i++){
    double mean = fgaus[i]->GetParameter(1);
    double meane = fgaus[i]->GetParError(1);
    double sig  = fgaus[i]->GetParameter(2);
    double sige = fgaus[i]->GetParError(2);
    cout<<i<<" "
  	<<mean<<" +- "<<meane<<" "
  	<<sig<<" +- "<<sige<<std::endl;

  }

  // Charge Long
  TCanvas*tc2=new TCanvas();
  tc2->Divide(3,3);
  TH1D* hchl[NCH];
  TF1* fgaus[NCH];
  char aname[100];
  char anum[100];
  for (int ich=0; ich<NCH; ich++){
    sprintf(aname,"ch%dchl",ich);
    hchl[ich] = new TH1D(aname,aname,100,0.,12000.);
    sprintf(aname,"tChargeL>>ch%dchl",ich);
    sprintf(anum,"tChannel == %d && tChargeL-tChargeS>800.0 && tChargeL>2500 && tPSD<1.0&& tChargeS>500",ich);
    tc2->cd(ich+1);
    cout<<aname<<endl;
    tin->Draw(aname,anum,"");
    sprintf(aname,"tf%d",ich);
    fgaus[ich]=new TF1(aname,"gaus",0.,1.);
    fgaus[ich]->SetParameters(1200,6000,1000);
    hchl[ich]->Fit(fgaus[ich]);
    double mean = fgaus[ich]->GetParameter(1);
    double sig  = fgaus[ich]->GetParameter(2);
    hchl[ich]->Fit(fgaus[ich],"","",mean-2.0*sig, mean+2.0*sig);

  }

  cout<<"Channel  MeanCHL   SigmaCHL  "<<std::endl;
  for (int i=0; i<NCH; i++){
    double mean = fgaus[i]->GetParameter(1);
    double meane = fgaus[i]->GetParError(1);
    double sig  = fgaus[i]->GetParameter(2);
    double sige = fgaus[i]->GetParError(2);
    cout<<i<<" "
  	<<mean<<" +- "<<meane<<" "
  	<<sig<<" +- "<<sige<<std::endl;
  }


  // Baseline
  TCanvas*tc4=new TCanvas();
  tc4->Divide(3,3);
  TH1D* hbsl1[NCH];
  TF1* fgaus[NCH];
  char aname[100];
  char anum[100];
  for (int ich=0; ich<NCH; ich++){
    sprintf(aname,"ch%dbsl",ich);
    hbsl1[ich] = new TH1D(aname,aname,100,1900.,2100.);
    sprintf(aname,"tBaseline>>ch%dbsl",ich);
    sprintf(anum,"tChannel == %d && tChargeL-tChargeS>800.0 && tChargeL>2500.0 && tPSD<1.0&& tChargeS>500",ich);
    tc4->cd(ich+1);
    cout<<aname<<endl;
    tin->Draw(aname,anum,"");
    sprintf(aname,"tf%d",ich);
    fgaus[ich]=new TF1(aname,"gaus",0.,1.);
    fgaus[ich]->SetParameters(50000,2000,10);
    hbsl1[ich]->Fit(fgaus[ich]);
    double mean = fgaus[ich]->GetParameter(1);
    double sig  = fgaus[ich]->GetParameter(2);
    hbsl1[ich]->Fit(fgaus[ich],"","",mean-2.0*sig, mean+2.0*sig);

  }

  cout<<"Channel  MeanBSL   SigmaBSL  "<<std::endl;
  for (int i=0; i<NCH; i++){
    double mean = fgaus[i]->GetParameter(1);
    double meane = fgaus[i]->GetParError(1);
    double sig  = fgaus[i]->GetParameter(2);
    double sige = fgaus[i]->GetParError(2);
    cout<<i<<" "
  	<<mean<<" +- "<<meane<<" "
  	<<sig<<" +- "<<sige<<std::endl;

  }

}
