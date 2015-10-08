

const double Li6Diam = 75.0 ; //mm
const double CasDiam = 70.0 ; //mm

const double AreaNorm = TMath::Power(CasDiam/Li6Diam,2);

void RateComparer(){

  TFile * fli = new TFile("ucnana000086.root");
  TFile * fca = new TFile("cascade/08/Cascade_small.root");

  const int nbunches = 132;
  TH1D* thli[ nbunches ] ; // li-6 detector histograms
  TH1D* thlis[ nbunches ] ; // li-6 detector histograms, background subtracted
  TH1D* thca[ nbunches ] ; // cascade detector histograms
  TH1D* thratio[ nbunches ]; // li-6 / cascade rate

  // use the last 6 bunches (ones for which there are no cascade runs)
  // that is bunches 140,141,...,145 to get an estimate of the background
  // as the gate valve was closed for these runs.
  TH1D* hlibg;
  char aname[200];  
  for (int i=140; i<146; i++){
    sprintf(aname,"bunch%06d/b%06d_all_hevper",i,i);
    std::cout<<"Getting GV closed bunch "<<i<<" li run histo="<<aname<<std::endl;
    if (i==140) {
      hlibg = (TH1D*)fli->Get(aname);
    } else {
      TH1D* htemp =  (TH1D*)fli->Get(aname);
      hlibg->Add( htemp );
    }
  }
  hlibg->Scale(1.0/6.0);// divide by the six bunches
  double libg = hlibg->Integral();
  double libge = TMath::Sqrt(libg);
  std::cout<<"Li6 gate valve closed BG count="<<libg<<"+-"<<libge<<std::endl;

  int icaadder = 0 ; // increment every time there is a missed cascade bunch
  for (int i=0; i<nbunches; i++){
    sprintf(aname,"bunch%06d/b%06d_all_hevper",i,i);
    std::cout<<"Bunch "<<i<<" li run histo = "<<aname<<std::endl;
    thli[i] = (TH1D*)fli->Get(aname);
    thli[i]->SetLineColor(kRed);
    thli[i]->SetLineWidth(2);
    sprintf(aname,"hli_bgsub%06d",i);
    thlis[i]=(TH1D*)thli[i]->Clone(aname);
    //    hlibg->SetBins( thli[i]->GetNbinsX(),
    //		      thli[i]->GetXaxis()->GetXmin(),
    //		      thli[i]->GetXaxis()->GetXmax() );
    thlis[i]->Add( hlibg, -1.0 );
    thlis[i]->Scale( AreaNorm );
    thlis[i]->SetLineColor(kGreen);
    thlis[i]->SetLineWidth(2);
    thlis[i]->SetLineStyle(kDotted);
    
    if (i==13 || i==70) icaadder++;

    if ( i+54+icaadder<= 91){
      sprintf(aname,"150813/hTOFT130815_%04d_2",i+54+icaadder);
    } else {
      sprintf(aname,"150814/hTOFT140815_%04d_2", i+54-91+icaadder);
    }
    std::cout<<"Bunch "<<i<<" cascade histo = "<<aname<<std::endl;
    thca[i] = (TH1D*)fca->Get(aname);
    thca[i]->SetLineColor(kBlue);
    thca[i]->SetLineStyle(kDashed);
    thca[i]->SetLineWidth(2);
  }

  double licount[ nbunches ];
  double licounte[ nbunches ];

  double licounta[ nbunches ];
  double licountae[ nbunches ];

  double cacount[ nbunches ];
  double cacounte[ nbunches ];
  double ratio[ nbunches ];
  double ratioe[ nbunches ];
  double bunchnum[ nbunches ];
  double zeros[ nbunches ];

  TH1D* licounttot ;
  TH1D* cacounttot ;

  TFile* fout=new TFile("RateCompare.root","recreate");
  for (int i=0; i<nbunches; i++){
    bunchnum[i] = double(i);
    zeros[i] = 0.0;
    licount[i] = thli[i]->Integral();
    licounte[i] = TMath::Sqrt(licount[i]);
    licounta[i] = licount[i] * AreaNorm -libg;
    licountae[i] = TMath::Sqrt( fabs(licounta[i]) ) ;
    cacount[i] = thca[i]->Integral();
    cacounte[i] = TMath::Sqrt(cacount[i]);

    
    if ( i!=50 && i!=86 ){
      if (i==0) licounttot = (TH1D*)thlis[i]->Clone("hlicounttot");
      else licounttot->Add( thlis[i] );

      if (i==0) cacounttot = (TH1D*)thca[i]->Clone("hcacounttot");
      else cacounttot->Add( thca[i] );
    }


    ratio[i] = licounta[i] / cacount[i];
    ratioe[i] = ratio[i] * TMath::Sqrt( 1.0/licount[i] + 1.0/cacount[i] );

    
    std::cout<<"Bunch "<<i<<std::endl
	     <<" Li Count            ="<<licount[i]<<"+-"<<licounte[i]<<std::endl
	     <<" Ca Count            ="<<cacount[i]<<"+-"<<cacounte[i]<<std::endl
	     <<" Li area norm. Count ="<<licounta[i]<<"+-"<<licountae[i]<<std::endl
	     <<" Li norm / Ca ratio  ="<<ratio[i]<<"+-"<<ratioe[i]<<std::endl;
    
    sprintf(aname,"cbunch%04d",i);
    TCanvas* c = new TCanvas(aname,aname);
    c->Divide(1,2);
    c->cd(1);
    thli[i]->Draw("hist");
    thlis[i]->Draw("histsame");
    thca[i]->Draw("histsame");

    sprintf(aname,"hratio%04d",i);
    thratio[i] = (TH1D*)thlis[i]->Clone(aname);
    thratio[i]->SetName(aname);
    thratio[i]->SetTitle("Li6/Cascade; Time(s); Area normalized Li6/Cascade ratio per 0.1s");

    //    hlibg->SetBins( thli[i]->GetNbinsX(),
    //		      thli[i]->GetXaxis()->GetXmin(),
    //		      thli[i]->GetXaxis()->GetXmax() );
    hlibg->SetLineColor(kGreen+2);
    hlibg->Draw("histsame");
    c->cd(2);

    // now take ratio
    thca[i]->SetBins( thli[i]->GetNbinsX(),
   		      thli[i]->GetXaxis()->GetXmin(),
   		      thli[i]->GetXaxis()->GetXmax() );
    //thratio[i]->Scale( AreaNorm );
    thratio[i]->Divide( thca[i] );
    thratio[i]->Draw("e1");
    fout->cd();
    c->Write();
  }

  TGraphErrors* tgeli=new TGraphErrors( nbunches, bunchnum, licount, zeros, licounte );
  TGraphErrors* tgelia=new TGraphErrors( nbunches, bunchnum, licounta, zeros, licountae );
  TGraphErrors* tgeca=new TGraphErrors( nbunches, bunchnum, cacount, zeros, cacounte );
  TGraphErrors* tgerat=new TGraphErrors( nbunches, bunchnum, ratio, zeros, ratioe );
  tgeli->SetMarkerStyle(20);
  tgeli->SetMarkerColor(kRed);
  tgeli->SetLineColor(kRed);

  tgelia->SetMarkerStyle(24);
  tgelia->SetMarkerColor(kMagenta);
  tgelia->SetLineColor(kMagenta);

  tgeca->SetMarkerStyle(21);
  tgeca->SetMarkerColor(kBlue);
  tgeca->SetLineColor(kBlue);

  tgerat->SetMarkerStyle(27);
  tgerat->SetMarkerColor(kGreen+2);
  tgerat->SetLineColor(kGreen+2);
 
  TLegend * tl = new TLegend(0.65,0.65,0.95,0.95);
  tl->AddEntry( tgeli, "Li-6", "lp");
  tl->AddEntry( tgelia, "Li-6 area norm.", "lp");
  tl->AddEntry( tgeca, "Cascade", "lp");
  TCanvas* c=new TCanvas("ratecompare","rate comparison");
  c->Divide(1,2);
  c->cd(1);
  tgeli->Draw("AP");
  tgelia->Draw("P");
  tgeca->Draw("P");
  tl->Draw();
  c->cd(2);
  tgerat->Draw("AP");


  for  (int ibin=1; ibin <= licounttot->GetNbinsX(); ibin++){
    licounttot->SetBinError(ibin, TMath::Sqrt( licounttot->GetBinContent(ibin) ) );
    cacounttot->SetBinError(ibin, TMath::Sqrt( cacounttot->GetBinContent(ibin) ) );
  }
  licounttot->Scale( 1.0 / float(nbunches-2) );
  cacounttot->Scale( 1.0 / float(nbunches-2) );

  TH1D* hrattot = (TH1D*)licounttot->Clone("hrattot");

  cacounttot->SetBins(
		      licounttot->GetNbinsX(),
   		      licounttot->GetXaxis()->GetXmin(),
   		      licounttot->GetXaxis()->GetXmax() );

  hrattot->Divide( cacounttot );

  c=new TCanvas("hrattot","hrattot");
  c->Divide(1,2);
  c->cd(1);
  licounttot->Draw("e1");
  cacounttot->Draw("e1same");
  c->cd(2);
  hrattot->Draw("e1");
  c->Write();



  
  c->Write();
  fout->Write();
    




}
