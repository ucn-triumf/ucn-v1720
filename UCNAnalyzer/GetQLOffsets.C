void GetQLOffsets(char *infilename = "output00582.root"){

  gStyle->SetOptFit(1111);

  TFile* fin = new TFile(infilename,"read");
  TTree* tin = (TTree*) fin->Get("tUCN");
  
  // PSD
  const int NCH=9;
  TCanvas*tc=new TCanvas();
  tc->Divide(2,4);
  TH1D* hpsd[NCH];
  TF1* fgaus[NCH];
  char aname[100];
  char anum[100];
  for (int ich=0; ich<9; ich++){
    sprintf(aname,"ch%dpsd",ich);
    hpsd[ich] = new TH1D(aname,aname,100,0.,1.0);
    sprintf(aname,"tPSD>>ch%dpsd",ich);
    sprintf(anum,"tChannel == %d",ich);
    tc->cd(ich+1);
    cout<<aname<<endl;
    tin->Draw(aname,anum,"",100000);
    sprintf(aname,"tf%d",ich);
    fgaus[ich]=new TF1(aname,"gaus",0.,1.);
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

  // Charge Long, Charge Short, Baseline
  TCanvas*tc1=new TCanvas();
  tc1->Divide(2,4);
  TH1D* hchl[NCH];
  TH1D* hchs[NCH];
  TH1D* hbsl[NCH];
  TH2D* hfr[NCH];
  TF1* fgaus[NCH];
  TF1* fgaus1[NCH];
  TF1* fgaus2[NCH];
  char aname[100];
  char anum[100];
  for (int ich=0; ich<NCH; ich++){
    tc1->cd(ich+1);
    hfr[ich] = new TH2D("hfr","Charge Gates",100,0.,3500.,100,0.,16000000.);
    hfr[ich]->SetStats(0);
    hfr[ich]->Draw();

    sprintf(aname,"ch%dchl",ich);
    hchl[ich] = new TH1D(aname,aname,100,0.,3500.);
    hchl[ich]->SetLineColor(kRed);
    sprintf(aname,"tChargeL>>ch%dchl",ich);
    sprintf(anum,"tChannel == %d",ich);
    cout<<aname<<endl;
    tin->Draw(aname,anum,"same",100000);
    sprintf(aname,"tf%d",ich);
    fgaus[ich]=new TF1(aname,"gaus",0.,1.);

    sprintf(aname,"ch%dchs",ich);
    hchs[ich] = new TH1D(aname,aname,100,0.,3500.);
    hchs[ich]->SetLineColor(kBlue);
    sprintf(aname,"tChargeS>>ch%dchs",ich);
    sprintf(anum,"tChannel == %d",ich);
    cout<<aname<<endl;
    tin->Draw(aname,anum,"same",100000);
    sprintf(aname,"tf%d",ich);
    fgaus1[ich]=new TF1(aname,"gaus",0.,1.);

    sprintf(aname,"ch%dbsl",ich);
    hbsl[ich] = new TH1D(aname,aname,100,0.,3500.);
    hbsl[ich]->SetLineColor(kGreen);
    sprintf(aname,"tBaseline>>ch%dbsl",ich);
    sprintf(anum,"tChannel == %d",ich);
    cout<<aname<<endl;
    tin->Draw(aname,anum,"same",100000);
    sprintf(aname,"tf%d",ich);
    fgaus[ich]=new TF1(aname,"gaus",0.,1.);

  }

  // Charge Long
  TCanvas*tc2=new TCanvas();
  tc2->Divide(2,4);
  TH1D* hchl1[NCH];
  TF1* fgaus[NCH];
  char aname[100];
  char anum[100];
  for (int ich=0; ich<NCH; ich++){
    sprintf(aname,"ch%dchl",ich);
    hchl1[ich] = new TH1D(aname,aname,100,0.,3000.);
    sprintf(aname,"tChargeL>>ch%dchl",ich);
    sprintf(anum,"tChannel == %d",ich);
    tc2->cd(ich+1);
    cout<<aname<<endl;
    tin->Draw(aname,anum,"",100000);
    sprintf(aname,"tf%d",ich);
    fgaus[ich]=new TF1(aname,"gaus",0.,1.);
    hchl1[ich]->Fit(fgaus[ich]);
    double mean = fgaus[ich]->GetParameter(1);
    double sig  = fgaus[ich]->GetParameter(2);
    hchl1[ich]->Fit(fgaus[ich],"","",mean-2.0*sig, mean+2.0*sig);

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

  // Charge Short
  TCanvas*tc3=new TCanvas();
  tc3->Divide(2,4);
  TH1D* hchs1[NCH];
  TF1* fgaus[NCH];
  char aname[100];
  char anum[100];
  for (int ich=0; ich<NCH; ich++){
    sprintf(aname,"ch%dchs",ich);
    hchs1[ich] = new TH1D(aname,aname,100,0.,3000.);
    sprintf(aname,"tChargeS>>ch%dchs",ich);
    sprintf(anum,"tChannel == %d",ich);
    tc2->cd(ich+1);
    cout<<aname<<endl;
    tin->Draw(aname,anum,"",100000);
    sprintf(aname,"tf%d",ich);
    fgaus[ich]=new TF1(aname,"gaus",0.,1.);
    hchs1[ich]->Fit(fgaus[ich]);
    double mean = fgaus[ich]->GetParameter(1);
    double sig  = fgaus[ich]->GetParameter(2);
    hchs1[ich]->Fit(fgaus[ich],"","",mean-2.0*sig, mean+2.0*sig);

  }

  cout<<"Channel  MeanCHS   SigmaCHS  "<<std::endl;
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
  tc4->Divide(2,4);
  TH1D* hbsl1[NCH];
  TF1* fgaus[NCH];
  char aname[100];
  char anum[100];
  for (int ich=0; ich<NCH; ich++){
    sprintf(aname,"ch%dbsl",ich);
    hbsl1[ich] = new TH1D(aname,aname,100,1900.,2100.);
    sprintf(aname,"tBaseline>>ch%dbsl",ich);
    sprintf(anum,"tChannel == %d",ich);
    tc4->cd(ich+1);
    cout<<aname<<endl;
    tin->Draw(aname,anum,"",100000);
    sprintf(aname,"tf%d",ich);
    fgaus[ich]=new TF1(aname,"gaus",0.,1.);
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
