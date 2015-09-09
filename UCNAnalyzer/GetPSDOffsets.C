void GetPSDOffsets(char *infilename = "output00000045_0000.root"){

  const int NCH=10;

  gStyle->SetOptFit(1111);

  TFile* fin = new TFile(infilename,"read");
  TTree* tin = (TTree*) fin->Get("tUCN");
  
  Double_t parPSD[6];
  Double_t parCL[9];
  double mean;
  double meane;
  double sig;
  double sige;

  // PSD -------------------------------------------
  TCanvas*tc=new TCanvas();
  tc->Divide(3,3);
  TH1D* hpsd[NCH];
  TF1* fgausPSD1[NCH];
  TF1* fgausPSD2[NCH];
  TF1* ftotalPSD[NCH];
  char aname[100];
  char anum[100];
  for (int ich=0; ich<NCH; ich++){

    if (ich==7) continue;

    sprintf(aname,"ch%dpsd",ich);
    hpsd[ich] = new TH1D(aname,aname,100,-1.,1.0);
    sprintf(aname,"tPSD>>ch%dpsd",ich);
    sprintf(anum,"tChannel == %d",ich);
    if (ich <7)
      tc->cd(ich+1);
    else 
      tc->cd(ich);
    cout<<aname<<endl;
    tin->Draw(aname,anum,"");
    sprintf(aname,"tf%d",ich);

    // fit background and UCN signal
    sprintf(aname,"ch%dpsdb",ich);
    fgausPSD1[ich]=new TF1(aname,"gaus",-1.,-0.1);
    fgausPSD1[ich]->SetLineColor(kRed);
    fgausPSD1[ich]->SetParameters(1000.,-0.1,0.1);
    sprintf(aname,"ch%dpsds",ich);
    fgausPSD2[ich]=new TF1(aname,"gaus",0.5,0.8);
    fgausPSD2[ich]->SetParameters(1000.,0.6,0.06);
    sprintf(aname,"ch%dpsd",ich);
    ftotalPSD[ich]=new TF1(aname,"gaus(0)+gaus(3)",-1.,1.);
    ftotalPSD[ich]->SetLineColor(kBlue);

    hpsd[ich]->Fit(fgausPSD1[ich],"","",-0.2,0.1);
    hpsd[ich]->Fit(fgausPSD2[ich],"");
    mean = fgausPSD1[ich]->GetParameter(1);
    sig  = fgausPSD1[ich]->GetParameter(2);
    hpsd[ich]->Fit(fgausPSD1[ich],"","",mean-2.0*sig, mean+2.0*sig);
    mean = fgausPSD2[ich]->GetParameter(1);
    sig  = fgausPSD2[ich]->GetParameter(2);
    hpsd[ich]->Fit(fgausPSD2[ich],"","",mean-2.0*sig, mean+2.0*sig);

    fgausPSD1[ich]->GetParameters(&parPSD[0]);
    fgausPSD2[ich]->GetParameters(&parPSD[3]);
    ftotalPSD[ich]->SetParameters(parPSD);
    hpsd[ich]->Fit(ftotalPSD[ich],"");
  }

  // show fit parameters for PSD
  cout<<"Channel  MeanPSD   SigmaPSD  "<<std::endl;
  std::cout<<"Fit for background"<<std::endl;
  for (int i=0; i<NCH; i++){

    if (i==7) continue;

    // parameters for background
    mean = fgausPSD1[i]->GetParameter(1);
    meane = fgausPSD1[i]->GetParError(1);
    sig  = fgausPSD1[i]->GetParameter(2);
    sige = fgausPSD1[i]->GetParError(2);
    std::cout<<i<<" "
	     <<mean<<" +- "<<meane<<" "
	     <<sig<<" +- "<<sige<<std::endl;
  }
  std::cout << std::endl;
  std::cout<<"Fit for UCN signal"<<std::endl;
  for (int i=0; i<NCH; i++){

    if (i==7) continue;

    // parameters for UCN signal
    mean = fgausPSD2[i]->GetParameter(1);
    meane = fgausPSD2[i]->GetParError(1);
    sig  = fgausPSD2[i]->GetParameter(2);
    sige = fgausPSD2[i]->GetParError(2);
    std::cout<<i<<" "
	     <<mean<<" +- "<<meane<<" "
	     <<sig<<" +- "<<sige<<std::endl;
  }

  // Charge Long -------------------------------------------
  TCanvas*tc1=new TCanvas();
  tc1->Divide(3,3);
  TH1D* hcl[NCH];
  TF1* fgausCL1[NCH];
  TF1* fgausCL2[NCH];
  TF1* fgausCL3[NCH];
  TF1* ftotalCL[NCH];
  char aname[100];
  char anum[100];
  for (int ich=0; ich<NCH; ich++){

    if (ich==7) 
      continue;

    sprintf(aname,"ch%dcl",ich);
    hcl[ich] = new TH1D(aname,aname,200,0.,18000.);
    sprintf(aname,"tChargeL>>ch%dcl",ich);
    sprintf(anum,"tChannel == %d",ich);

    if (ich<7)
      tc1->cd(ich+1);
    else
      tc1->cd(ich);

    std::cout<<aname<<std::endl;
    tin->Draw(aname,anum,"");
    sprintf(aname,"tf%d",ich);

    // fit background
    sprintf(aname,"ch%dclb",ich);
    fgausCL1[ich]=new TF1(aname,"expo",600.,1800.);
    fgausCL1[ich]->SetLineColor(kRed);
    fgausCL1[ich]->SetParameters(15.,-0.004);

    sprintf(aname,"ch%dcls",ich);
    fgausCL2[ich]=new TF1(aname,"gaus",3000.,18000.);
    fgausCL2[ich]->SetParameters(1000000.,5000.,1000.);
    fgausCL2[ich]->SetLineColor(kBlue);

    sprintf(aname,"ch%dcld",ich);
    fgausCL3[ich]=new TF1(aname,"gaus",9000.,16000.);
    fgausCL3[ich]->SetParameters(1000.,4000.,800.);
    fgausCL3[ich]->SetLineColor(kGreen);

    sprintf(aname,"ch%dcl",ich);
    ftotalCL[ich]=new TF1(aname,"expo(0)+gaus(3)+gaus(6)",500,18000);

    hcl[ich]->Fit(fgausCL1[ich],"");
    hcl[ich]->Fit(fgausCL2[ich],"");
    hcl[ich]->Fit(fgausCL3[ich],"");
    mean = fgausCL1[ich]->GetParameter(1);
    sig  = fgausCL1[ich]->GetParameter(2);
    hcl[ich]->Fit(fgausCL1[ich],"","",mean-2.0*sig, mean+2.0*sig);
    mean = fgausCL2[ich]->GetParameter(1);
    sig  = fgausCL2[ich]->GetParameter(2);
    hcl[ich]->Fit(fgausCL2[ich],"","",mean-2.0*sig, mean+2.0*sig);
    mean = fgausCL3[ich]->GetParameter(1);
    sig  = fgausCL3[ich]->GetParameter(2);
    hcl[ich]->Fit(fgausCL3[ich],"","",mean-2.0*sig, mean+2.0*sig);

    fgausCL1[ich]->GetParameters(&parCL[0]);
    fgausCL2[ich]->GetParameters(&parCL[3]);
    fgausCL2[ich]->GetParameters(&parCL[6]);
    ftotalCL[ich]->SetParameters(parCL);
    hcl[ich]->Fit(ftotalCL[ich],"");
    fgausCL1[ich]->Draw("same");
    fgausCL2[ich]->Draw("same");
    fgausCL3[ich]->Draw("same");

  }

  // show fit parameters for CL
  cout<<"Channel  MeanCL   SigmaCL  "<<std::endl;
  std::cout<<"Fit for background"<<std::endl;
  for (int i=0; i<NCH; i++){

    if (i==7) continue;

    // parameters for background
    mean = fgausCL1[i]->GetParameter(1);
    meane = fgausCL1[i]->GetParError(1);
    sig  = fgausCL1[i]->GetParameter(2);
    sige = fgausCL1[i]->GetParError(2);
    std::cout<<i<<" "
	     <<mean<<" +- "<<meane<<" "
	     <<sig<<" +- "<<sige<<std::endl;
  }
  std::cout << std::endl;
  std::cout<<"Fit for UCN signal"<<std::endl;
  for (int i=0; i<NCH; i++){

    if (i==7) continue;

    // parameters for UCN signal
    mean = fgausCL2[i]->GetParameter(1);
    meane = fgausCL2[i]->GetParError(1);
    sig  = fgausCL2[i]->GetParameter(2);
    sige = fgausCL2[i]->GetParError(2);
    std::cout<<i<<" "
	     <<mean<<" +- "<<meane<<" "
	     <<sig<<" +- "<<sige<<std::endl;
  }
}
