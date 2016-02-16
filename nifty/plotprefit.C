
const int g_bpsdmin = 16;
const int g_bpsdmax = 40;

const int g_npdfs = 10;
const int g_cols[g_npdfs] = { kOrange, kYellow+1, kYellow+3, kSpring-7, kGreen+2, kRed, kPink+8, kViolet+7, kBlue, kCyan+3};

TH2D* g_hpdf[g_npdfs];
TH2D* g_hdt;


void plotslicesX(){

  // initialize slice histograms
  std::cout << "Initialize slice histograms" << std::endl;
  int nbinsy = g_bpsdmax - g_bpsdmin;
  TH1D** pp[ g_npdfs ];
  for (int j=0; j<g_npdfs; j++ ) pp[j] = new TH1D * [ nbinsy ];
  TH1D** psum = new TH1D * [ nbinsy ];
  TH1D** dd = new TH1D * [ nbinsy ];

  // create canvases for plotting
  std::cout <<"Create canvases for plotting" << std::endl;
  TCanvas* tcplotx = new TCanvas();
  tcplotx->Divide(4,nbinsy/4,0,0);

  TCanvas* canv=tcplotx;
  canv->SetRightMargin(0.);
  canv->SetLeftMargin(0.);
  canv->SetTopMargin(0.);
  canv->SetBottomMargin(0.);
  canv->SetTicks(1,1);
  canv->SetFillColor(0);
  canv->SetHighLightColor(0);

  canv->SetBorderMode(1);

  // assemble plots
  std::cout<<"Assembling sliced plots"<<std::endl;
  char aname[100];
  for (int i=0; i<nbinsy; i++){
    tcplotx->cd(i+1);
    tcplotx->GetPad(i+1)->SetLogy();

    TPad* pad = (TPad*)canv->GetPad(i+1);
    if (i%4==0) pad->SetLeftMargin(0.15);
    else pad->SetLeftMargin(0.01);
    if (i>=nbinsy-5) pad->SetBottomMargin(0.15);
    else pad->SetBottomMargin(0.01);
    pad->SetTopMargin(0.01);
    pad->SetRightMargin(0.01);
    pad->SetFillColor(0);
    pad->SetBorderMode(1); 

    for (int j=0; j<g_npdfs; j++){
      sprintf(aname,"pdf%02dsliceX%02d",j,i);
      pp[j][i] = (TH1D*) g_hpdf[j]->ProjectionX(aname,i+g_bpsdmin,i+g_bpsdmin);
      pp[j][i]->SetLineColor( g_cols[j] );
      pp[j][i]->SetLineWidth( 2 );
      if (j==0) {
	sprintf(aname,"pdfsumsliceX%02d",i);
	psum[i] = (TH1D*)pp[j][i]->Clone(aname);
	psum[i]->SetLineColor(kBlack);
      } else {
	psum[i]->Add( (TH1D*)(pp[j][i]) );
      }
    }

    // plot all slices on one pad
    // error bars = signal
    // black = sum of fits
    // red = signal
    // orange = background
    // green = retriggers
    // blue = pileup
    std::cout << "plot slice " << i << std::endl;
    sprintf(aname,"dtsliceX%02d",i);
    dd[i] = (TH1D*)g_hdt->ProjectionX(aname,i+g_bpsdmin,i+g_bpsdmin);
    dd[i]->SetMarkerStyle(20);
    dd[i]->SetLineWidth(2);
    dd[i]->Draw("e1");
    pp[0][i]->Draw("histsame");
    pp[1][i]->Draw("histsame");
    pp[2][i]->Draw("histsame");
    pp[3][i]->Draw("histsame");
    pp[4][i]->Draw("histsame");
    pp[5][i]->Draw("histsame");
    pp[6][i]->Draw("histsame");
    pp[7][i]->Draw("histsame");
    pp[8][i]->Draw("histsame");
    pp[8][i]->Draw("histsame");
    //pp[10][i]->Draw("histsame");
    psum[i]->Draw("histsame");



    double apsdval = g_hpdf[0]->GetYaxis()->GetBinCenter(i+g_bpsdmin);
    sprintf(aname,"PSD = %4.2f", apsdval);
    TLatex* tl=new TLatex(0.7,0.8,aname);
    tl->SetNDC();
    tl->Draw();
  }
  
  std::cout << "Done" << std::endl;
  return;
}



void plotprefit(){

  // open fitted file
  TFile * fmc=new TFile("PDFs_00050.root","read");
  
  // get fit data
  std::cout << "Get fit data"<< std::endl;
  g_hpdf[0]  = (TH2D*)fmc->Get("hbg_psdql");     
  g_hpdf[1]  = (TH2D*)fmc->Get("hgbg_psdql"); 
  g_hpdf[2]  = (TH2D*)fmc->Get("hgbgpile_psdql");
  g_hpdf[4]  = (TH2D*)fmc->Get("hlate_psdql");     
  g_hpdf[3]  = (TH2D*)fmc->Get("hretrig_psdql"); 
  g_hpdf[5]  = (TH2D*)fmc->Get("hsig_psdql");    
  g_hpdf[6]  = (TH2D*)fmc->Get("hsiggbg_psdql");      
  g_hpdf[7]  = (TH2D*)fmc->Get("hsiggbgpile_psdql");
  g_hpdf[8]  = (TH2D*)fmc->Get("hsigpile_psdql");
  g_hpdf[9]  = (TH2D*)fmc->Get("hsiggbgpile_psdql");
  //g_hpdf[10] = (TH2D*)fmc->Get("hpile_psdql");

  // first normalize pdfs
  std::cout << "Scale pdfs" << std::endl;
  double sf;
  for (int i=0; i<g_npdfs; i++){
    sf = g_hpdf[i]->Integral("width");
    if (sf !=0) g_hpdf[i]->Scale( 1.0/ sf );
  }
  std::cout <<"ready to scale"<<std::endl;
  g_hpdf[0]->Scale(8.5e6);  // kOrange
  g_hpdf[1]->Scale(7.0e5);  // kYellow+1
  g_hpdf[2]->Scale(1.0e6);  // kYellow+3 -> brown
  g_hpdf[3]->Scale(4.0e4);  // kSpring-7
  g_hpdf[4]->Scale(1.0e6);  // kGreen+2
  g_hpdf[5]->Scale(7.73e7); // kRed 
  g_hpdf[6]->Scale(3.5e5);  // kPink+8
  g_hpdf[7]->Scale(1.7e5);  // kViolet+7
  g_hpdf[8]->Scale(5.0e4);  // kBlue
  g_hpdf[9]->Scale(1.7e0);  // kCyan+3
  //g_hpdf[10]->Scale(5.7e2); // kTeal-6

  // now apply scale factor to match data

  TH1D* hpdfx[g_npdfs];
  TH1D* hpdfy[g_npdfs];
  TH1D* hxsum;
  TH1D* hysum;
  
  // initialize histogram structures
  std::cout << "Initializing structures"<< std::endl;
  for (int i=0; i<g_npdfs ; i++){
    hpdfx[i] = (TH1D*)g_hpdf[i]->ProjectionX();
    hpdfx[i]->SetLineColor( g_cols[i] );
    hpdfx[i]->SetLineWidth(2);
    hpdfy[i] = (TH1D*)g_hpdf[i]->ProjectionY();
    hpdfy[i]->SetLineColor( g_cols[i] );
    hpdfy[i]->SetLineWidth(2);
    if (i==0) {
      hxsum = (TH1D*)hpdfx[i]->Clone("hxsum");
      hxsum->SetLineColor(kBlack);
    }
    else hxsum->Add( hpdfx[i] );
    if (i==0) {
      hysum = (TH1D*)hpdfy[i]->Clone("hysum");
      hysum->SetLineColor(kBlack);
    }
    else hysum->Add( hpdfy[i] );
  }
  
  // get data
  std::cout << "Get data" << std::endl;
  TFile * fin= new TFile("dataforfit_output00000063_0000.root","read");
  g_hdt = (TH2D*)fin->Get("hpsdqlrsum");
  TH1D* hdtx = (TH1D*)g_hdt->ProjectionX("hdtx");
  TH1D* hdty = (TH1D*)g_hdt->ProjectionY("hdty");
  
  // draw the projection along the x axis (QL)
  std::cout<<"Draw the projection along the x axis (QL)" << std::endl;
  TCanvas *xcan = new TCanvas();
  hdtx->Draw("e1");
  hxsum->Draw("histsame");
  hpdfx[0]->Draw("histsame");
  hpdfx[1]->Draw("histsame");
  hpdfx[2]->Draw("histsame");
  hpdfx[3]->Draw("histsame");
  hpdfx[4]->Draw("histsame");
  hpdfx[5]->Draw("histsame");
  hpdfx[6]->Draw("histsame");
  hpdfx[7]->Draw("histsame");
  hpdfx[8]->Draw("histsame");
  hpdfx[9]->Draw("histsame");
  //hpdfx[10]->Draw("histsame");

  // draw the projection along the y axis (PSD)
  std::cout<<"Draw the projection along the y axis (PSD)" << std::endl;
  TCanvas *ycan = new TCanvas();
  hdty->Draw("e1");
  hysum->Draw("histsame");
  hpdfy[0]->Draw("histsame");
  hpdfy[1]->Draw("histsame");
  hpdfy[2]->Draw("histsame");
  hpdfy[3]->Draw("histsame");
  hpdfy[4]->Draw("histsame");
  hpdfy[5]->Draw("histsame");
  hpdfy[6]->Draw("histsame");
  hpdfy[7]->Draw("histsame");
  hpdfy[8]->Draw("histsame");
  hpdfy[9]->Draw("histsame");
  //hpdfy[10]->Draw("histsame");

  // plot slices of the projection along the x axis (QL)
  plotslicesX( );


}
