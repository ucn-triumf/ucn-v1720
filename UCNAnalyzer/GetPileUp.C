void GetPileUp(){

  gStyle->SetOptFit(1111);

  TCanvas* tc1=new TCanvas();
  TH1D* erate=new TH1D("pileup","Rate of Pile-up;Frequency of Double Pulse (MHz);Percent Pile-up (%)",8,1,9);
  erate->Fill(1,100-100);
  erate->Fill(2,100-100);
  erate->Fill(3,(100-78.9)*2);
  erate->Fill(4,(100-78.4)*2);
  erate->Fill(5,(100-67.2)*2);
  erate->Fill(6,(100-50.0)*2);
  erate->Fill(7,(100-49.6)*2);
  erate->Fill(8,(100-50.2)*2);
  erate->Draw();


  // char * labels[8] = { "1MHz",
  // 		       "2MHz",
  // 		       "3MHz",
  // 		       "4MHz",
  // 		       "5MHz",
  // 		       "6MHz",
  // 		       "7MHz",
  // 		       "8MHz"
  // };

  // TLegend*tl=new TLegend(0.05,0.7,0.45,0.95);
  // tl->SetFillColor(kWhite);
  // TFile* fin1[8];
  // TTree* tin1[8];
  // TFile* fin1[0] = new TFile("output00580.root","read");
  // TTree* tin1[0] = (TTree*) fin1[0]->Get("tUCN");
  // TFile* fin1[1] = new TFile("output00579.root","read");
  // TTree* tin1[1] = (TTree*) fin1[1]->Get("tUCN");
  // TFile* fin1[2] = new TFile("output00578.root","read");
  // TTree* tin1[2] = (TTree*) fin1[2]->Get("tUCN");
  // TFile* fin1[3] = new TFile("output00577.root","read");
  // TTree* tin1[3] = (TTree*) fin1[3]->Get("tUCN");
  // TFile* fin1[4] = new TFile("output00576.root","read");
  // TTree* tin1[4] = (TTree*) fin1[4]->Get("tUCN");
  // TFile* fin1[5] = new TFile("output00575.root","read");
  // TTree* tin1[5] = (TTree*) fin1[5]->Get("tUCN");
  // TFile* fin1[6] = new TFile("output00574.root","read");
  // TTree* tin1[6] = (TTree*) fin1[6]->Get("tUCN");
  // TFile* fin1[7] = new TFile("output00573.root","read");
  // TTree* tin1[7] = (TTree*) fin1[7]->Get("tUCN");
  // TCanvas*tc2=new TCanvas();
  // tc2->Divide(2,2);


  // tc2->cd(1);
  // TH2D *hframe = new TH2D("hframe","tPSD",200,0.5,0.9,200,0,550000);
  // hframe->SetStats(0);
  // hframe->Draw();
  // tc2->cd(2);
  // TH2D *hframe1 = new TH2D("hframe1","tChargeL",200,0,18000,200,0,2000000);
  // hframe1->SetStats(0);
  // hframe1->Draw();
  // tc2->cd(3);
  // TH2D *hframe2 = new TH2D("hframe2","tChargeS",200,0,3600,200,0,500000);
  // hframe2->SetStats(0);
  // hframe2->Draw();
  // tc2->cd(4);
  // TH2D *hframe3 = new TH2D("hframe3","tBaseline",200,1850,2000,200,0,1000000);
  // hframe3->SetStats(0);
  // hframe3->Draw();
  // for (int i=0; i<8;i++) {
  //   tin1[i]->SetLineColor(1+i);
  //   tc2->cd(1);
  //   tin1[i]->Draw("tPSD","tPSD<1&&tPSD>0","same");
  //   tc2->cd(2);
  //   tin1[i]->Draw("tChargeL","tPSD<1&&tPSD>0","same");
  //   tc2->cd(3);
  //   tin1[i]->Draw("tChargeS","tPSD<1&&tPSD>0","same");
  //   tc2->cd(4);
  //   tin1[i]->Draw("tBaseline","tPSD<1&&tPSD>0","same");

  //   tl->AddEntry(tin1[i],labels[i],"l");
  // }
  // tc2->cd(1);
  // tl->Draw();
}
