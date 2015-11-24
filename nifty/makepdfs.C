void makepdfs(){

  TFile * finbg = new TFile("ucnrateN0.0G100000.0T10.0Run00.root","read");
  TFile * finsig = new TFile("ucnrateN100000.0G0.0T10.0Run00.root","read");
  TFile * fintoy = new TFile("ucnrateN50000.0G50000.0T10.0Run00.root","read");
  

  TTree*tbg=(TTree*)finbg->Get("TPSD");
  TTree*tsig=(TTree*)finsig->Get("TPSD");
  TTree*ttoy=(TTree*)fintoy->Get("TPSD");
  

  TFile * fout = new TFile("PDFs.root","recreate");
  TH2D* hbg_psdql = new TH2D("hbg_psdql","BG ; QL; PSD",150,0.,15000.0,40,-1., 1.);
  TH2D* hsig_psdql = new TH2D("hsig_psdql","Signal ; QL; PSD",150,0.,15000.0,40,-1., 1.);
  TH2D* hretrig_psdql = new TH2D("hretrig_psdql","Retrig ; QL; PSD",150,0.,15000.0,40,-1., 1.);

  TH2D* htoy_psdql = new TH2D("htoy_psdql","Toy ; QL; PSD",150,0.,15000.0,40,-1., 1.);


  TH2D* hretrig1_psdql = new TH2D("hretrig1_psdql","Retrig ; QL; PSD",150,0.,15000.0,40,-1., 1.);
  TH2D* hretrig2_psdql = new TH2D("hretrig2_psdql","Retrig ; QL; PSD",150,0.,15000.0,40,-1., 1.);
  TH2D* hretrig3_psdql = new TH2D("hretrig3_psdql","Retrig ; QL; PSD",150,0.,15000.0,40,-1., 1.);
  TH2D* hretrig4_psdql = new TH2D("hretrig4_psdql","Retrig ; QL; PSD",150,0.,15000.0,40,-1., 1.);

  TH2D* hsig1_psdql = new TH2D("hsig1_psdql","Signal ; QL; PSD",150,0.,15000.0,40,-1., 1.);
  TH2D* hsig2_psdql = new TH2D("hsig2_psdql","Signal ; QL; PSD",150,0.,15000.0,40,-1., 1.);
  TH2D* hsig3_psdql = new TH2D("hsig3_psdql","Signal ; QL; PSD",150,0.,15000.0,40,-1., 1.);
  TH2D* hsig4_psdql = new TH2D("hsig4_psdql","Signal ; QL; PSD",150,0.,15000.0,40,-1., 1.);

  
  TH1D* hdeltat = new TH1D("hdeltat","dt; #Delta t (ns)",10000,0.,50000.);

  TH2D* hqsvsdt = new TH2D("hqsvsdt","PSD>0.85 ; #Delta t(ns); QS", 
			   1000,0.,50000.,
			   70, 0.,7000.);//->Fill( t-tlast , qs );
  TH2D* hqsvsdtall = new TH2D("hqsvsdtall","All events ; #Delta t(ns); QS", 
			   1000,0.,50000.,
			   70, 0.,7000.);//->Fill( t-tlast , qs );

  float t;
  float qs;
  float ql;
  float bl;
  tbg->SetBranchAddress("T",&t);
  tbg->SetBranchAddress("QS",&qs);
  tbg->SetBranchAddress("QL",&ql);
  tbg->SetBranchAddress("BL",&bl);

  // Fill background histogram
  Long64_t nentries = tbg->GetEntries();
  for (Long64_t ientry=0; ientry<nentries; ientry++){
    tbg->GetEntry(ientry);
    double psd = -0.999;
    if ( ql!=0 ) psd = ( ql - qs ) / ql;
    hbg_psdql->Fill( ql, psd );
  }



  // Fill the signal histogram and retrigger histogram
  tsig->SetBranchAddress("T",&t);
  tsig->SetBranchAddress("QS",&qs);
  tsig->SetBranchAddress("QL",&ql);
  tsig->SetBranchAddress("BL",&bl);
  Long64_t nentries = tsig->GetEntries();
  float tlast=0.0;
  for (Long64_t ientry=0; ientry<nentries; ientry++){
    tsig->GetEntry(ientry);
    double psd = -0.999;
    if ( ql!=0 ) psd = ( ql - qs ) / ql;
    if (ientry!=0) {
      if ( t-tlast < 6000. && ql < 2000.0 ){
	// call this a retrigger!
	hretrig_psdql->Fill( ql, psd );
      } else {
	// call this signal!
	hsig_psdql->Fill( ql, psd );
      }

      hqsvsdtall->Fill( t-tlast , qs );

      if ( psd > 0.85 ){
	hdeltat->Fill( t-tlast );
	hqsvsdt->Fill( t-tlast , qs );
      }


      if ( t-tlast < 2000. && ql < 2000.0 ){
	hretrig1_psdql->Fill( ql, psd );
      } else {
	hsig1_psdql->Fill( ql, psd );
      }
      if ( t-tlast < 3000. && ql < 2000.0 ){
	hretrig2_psdql->Fill( ql, psd );
      } else {
	hsig2_psdql->Fill( ql, psd );
      }
      if ( t-tlast < 4000. && ql < 2000.0 ){
	hretrig3_psdql->Fill( ql, psd );
      } else {
	hsig3_psdql->Fill( ql, psd );
      }
      if ( t-tlast < 6000. && ql < 2000.0 ){
	hretrig4_psdql->Fill( ql, psd );
      } else {
	hsig4_psdql->Fill( ql, psd );
      }


    }
    tlast = t;
    
  }


  // Fill the toy signal+background histogram 
  ttoy->SetBranchAddress("T",&t);
  ttoy->SetBranchAddress("QS",&qs);
  ttoy->SetBranchAddress("QL",&ql);
  ttoy->SetBranchAddress("BL",&bl);
  Long64_t nentries = ttoy->GetEntries();
  for (Long64_t ientry=0; ientry<nentries; ientry++){
    ttoy->GetEntry(ientry);
    double psd = -0.999;
    if ( ql!=0 ) psd = ( ql - qs ) / ql;
    htoy_psdql->Fill( ql, psd );    
  }

  TCanvas* tc=new TCanvas();
  tc->Divide(2,2);
  tc->cd(1);
  hsig_psdql->Draw("colz");
  tc->cd(2);
  hbg_psdql->Draw("colz");
  tc->cd(3);
  hretrig_psdql->Draw("colz");
  tc->cd(4);
  htoy_psdql->Draw("colz");
  


  TCanvas* tc2=new TCanvas();
  tc2->Divide(2,2);
  tc2->cd(1);
  hretrig1_psdql->Draw("colz");
  hretrig1_psdql->SetMaximum(50000.0);
  tc2->cd(2);
  hretrig2_psdql->Draw("colz");
  hretrig2_psdql->SetMaximum(50000.0);
  tc2->cd(3);
  hretrig3_psdql->Draw("colz");
  hretrig3_psdql->SetMaximum(50000.0);
  tc2->cd(4);
  hretrig4_psdql->Draw("colz");
  hretrig4_psdql->SetMaximum(50000.0);


  TCanvas* tc3=new TCanvas();
  tc3->Divide(2,2);
  tc3->cd(1);
  hsig1_psdql->Draw("colz");
  hsig1_psdql->SetMaximum(50000.0);
  tc3->cd(2);
  hsig2_psdql->Draw("colz");
  hsig2_psdql->SetMaximum(50000.0);
  tc3->cd(3);
  hsig3_psdql->Draw("colz");
  hsig3_psdql->SetMaximum(50000.0);
  tc3->cd(4);
  hsig4_psdql->Draw("colz");
  hsig4_psdql->SetMaximum(50000.0);


  TCanvas * tc4=new TCanvas();
  tc4->Divide(1,2);
  tc4->cd(1);
  hqsvsdt->Draw("colz");
  tc4->cd(2);
  hqsvsdtall->Draw("colz");

  fout->Write();
  


  

}
