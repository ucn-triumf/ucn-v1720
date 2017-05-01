// show various 2D histograms for all 9 channels

void Show2D(char *infilename = "output00000045_0000.root"){

  gStyle->SetOptFit(1111);
  TCanvas *tc1 = new TCanvas();
  tc1->Divide(3,3);

  // open root file
  TFile* fin = new TFile(infilename,"read");
  TTree* tin = (TTree*) fin->Get("tUCN");

  // Plot charge long and charge short for each channel of detector

  // channel 1
  tc1->cd(1);
  int chan = 1;
  char anum[100];
  double xmin = 0.; 
  double xmax = 17000.;
  double ymin = 0;
  double ymax = 12000.;
  TH2D* qlqs1= new TH2D("qlqs1","qlqs1",1000,xmin,xmax,1000,ymin,ymax);
  sprintf(anum,"tChannel == %d",chan);
  tin->Draw("tChargeS:tChargeL>>qlqs1",anum,"colz");

  // channel 0
  tc1->cd(2);
  int chan = 0;
  TH2D* qlqs0= new TH2D("qlqs0","qlqs0",1000,xmin,xmax,1000,ymin,ymax);
  sprintf(anum,"tChannel == %d",chan);
  tin->Draw("tChargeS:tChargeL>>qlqs0",anum,"colz");

  // channel 7
  tc1->cd(3);
  int chan = 8;
  TH2D* qlqs7= new TH2D("qlqs7","qlqs7",1000,xmin,xmax,1000,ymin,ymax);
  sprintf(anum,"tChannel == %d",chan);
  tin->Draw("tChargeS:tChargeL>>qlqs7",anum,"colz");

  // channel 2
  tc1->cd(4);
  int chan = 2;
  TH2D* qlqs2= new TH2D("qlqs2","qlqs2",1000,xmin,xmax,1000,ymin,ymax);
  sprintf(anum,"tChannel == %d",chan);
  tin->Draw("tChargeS:tChargeL>>qlqs2",anum,"colz");

  // channel 8
  tc1->cd(5);
  int chan = 9;
  TH2D* qlqs8= new TH2D("qlqs8","qlqs8",1000,xmin,xmax,1000,ymin,ymax);
  sprintf(anum,"tChannel == %d",chan);
  tin->Draw("tChargeS:tChargeL>>qlqs8",anum,"colz");

  // channel 6
  tc1->cd(6);
  int chan = 6;
  TH2D* qlqs6= new TH2D("qlqs6","qlqs6",1000,xmin,xmax,1000,ymin,ymax);
  sprintf(anum,"tChannel == %d",chan);
  tin->Draw("tChargeS:tChargeL>>qlqs6",anum,"colz");

  // channel 3
  tc1->cd(7);
  int chan = 3;
  TH2D* qlqs3= new TH2D("qlqs3","qlqs3",1000,xmin,xmax,1000,ymin,ymax);
  sprintf(anum,"tChannel == %d",chan);
  tin->Draw("tChargeS:tChargeL>>qlqs3",anum,"colz");

  // channel 4
  tc1->cd(8);
  int chan = 4;
  TH2D* qlqs4= new TH2D("qlqs4","qlqs4",1000,xmin,xmax,1000,ymin,ymax);
  sprintf(anum,"tChannel == %d",chan);
  tin->Draw("tChargeS:tChargeL>>qlqs4",anum,"colz");

  // channel 5
  tc1->cd(9);
  int chan = 5;
  TH2D* qlqs5= new TH2D("qlqs5","qlqs5",1000,xmin,xmax,1000,ymin,ymax);
  sprintf(anum,"tChannel == %d",chan);
  tin->Draw("tChargeS:tChargeL>>qlqs5",anum,"colz");

  // plot charge long and psd for each channel
  TCanvas *tc2 = new TCanvas();
  tc2->Divide(3,3);

  // channel 1
  tc2->cd(1);
  chan = 1;
  xmin = 0.; 
  xmax = 17000.;
  ymin = -2;
  ymax = 1.25;
  TH2D* qlpsd1= new TH2D("qlpsd1","qlpsd1",1000,xmin,xmax,1000,ymin,ymax);
  sprintf(anum,"tChannel == %d",chan);
  tin->Draw("tPSD:tChargeL>>qlpsd1",anum,"colz");

  // channel 0
  tc2->cd(2);
  int chan = 0;
  TH2D* qlpsd0= new TH2D("qlpsd0","qlpsd0",1000,xmin,xmax,1000,ymin,ymax);
  sprintf(anum,"tChannel == %d",chan);
  tin->Draw("tPSD:tChargeL>>qlpsd0",anum,"colz");

  // channel 7
  tc2->cd(3);
  int chan = 8;
  TH2D* qlpsd7= new TH2D("qlpsd7","qlpsd7",1000,xmin,xmax,1000,ymin,ymax);
  sprintf(anum,"tChannel == %d",chan);
  tin->Draw("tPSD:tChargeL>>qlpsd7",anum,"colz");

  // channel 2
  tc2->cd(4);
  int chan = 2;
  TH2D* qlpsd2= new TH2D("qlpsd2","qlpsd2",1000,xmin,xmax,1000,ymin,ymax);
  sprintf(anum,"tChannel == %d",chan);
  tin->Draw("tPSD:tChargeL>>qlpsd2",anum,"colz");

  // channel 8
  tc2->cd(5);
  int chan = 9;
  TH2D* qlpsd8= new TH2D("qlpsd8","qlpsd8",1000,xmin,xmax,1000,ymin,ymax);
  sprintf(anum,"tChannel == %d",chan);
  tin->Draw("tPSD:tChargeL>>qlpsd8",anum,"colz");

  // channel 6
  tc2->cd(6);
  int chan = 6;
  TH2D* qlpsd6= new TH2D("qlpsd6","qlpsd6",1000,xmin,xmax,1000,ymin,ymax);
  sprintf(anum,"tChannel == %d",chan);
  tin->Draw("tPSD:tChargeL>>qlpsd6",anum,"colz");

  // channel 3
  tc2->cd(7);
  int chan = 3;
  TH2D* qlpsd3= new TH2D("qlpsd3","qlpsd3",1000,xmin,xmax,1000,ymin,ymax);
  sprintf(anum,"tChannel == %d",chan);
  tin->Draw("tPSD:tChargeL>>qlpsd3",anum,"colz");

  // channel 4
  tc2->cd(8);
  int chan = 4;
  TH2D* qlpsd4= new TH2D("qlpsd4","qlpsd4",1000,xmin,xmax,1000,ymin,ymax);
  sprintf(anum,"tChannel == %d",chan);
  tin->Draw("tPSD:tChargeL>>qlpsd4",anum,"colz");

  // channel 5
  tc2->cd(9);
  int chan = 5;
  TH2D* qlpsd5= new TH2D("qlpsd5","qlpsd5",1000,xmin,xmax,1000,ymin,ymax);
  sprintf(anum,"tChannel == %d",chan);
  tin->Draw("tPSD:tChargeL>>qlpsd5",anum,"colz");
}
