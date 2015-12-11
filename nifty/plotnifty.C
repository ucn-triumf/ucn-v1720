
const int g_bpsdmin = 16;
const int g_bpsdmax = 40;

const int g_npdfs = 3;
const int g_cols[g_npdfs] = { kRed, kGreen+2, kBlue };

TH2D* g_hpdf[g_npdfs];
TH2D* g_hdt;


void plotslicesX(){

  int nbinsy = g_bpsdmax - g_bpsdmin+1;
  TH1D** pp[ g_npdfs ];
  for (int j=0; j<g_npdfs; j++ ) pp[j] = new TH1D * [ nbinsy ];
  TH1D** psum = new TH1D * [ nbinsy ];
  TH1D** dd = new TH1D * [ nbinsy ];

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
    sprintf(aname,"dtsliceX%02d",i);
    dd[i] = (TH1D*)g_hdt->ProjectionX(aname,i+g_bpsdmin,i+g_bpsdmin);
    psum[i]->Draw("hist");
    pp[0][i]->Draw("histsame");
    pp[1][i]->Draw("histsame");
    pp[2][i]->Draw("histsame");
    dd[i]->Draw("e1same");

    double apsdval = g_hpdf[0]->GetYaxis()->GetBinCenter(i+g_bpsdmin);
    sprintf(aname,"PSD = %4.2f", apsdval);
    TLatex* tl=new TLatex(0.7,0.8,aname);
    tl->SetNDC();
    tl->Draw();
  }
  
  return;
}



void plotnifty(){

  TFile * fin=new TFile("nifty.root","read");
  

  g_hpdf[0] = (TH2D*)fin->Get("hsig_psdql");     //hpdf[0]->Scale(450000.0);
  g_hpdf[1] = (TH2D*)fin->Get("hretrig_psdql");  //hpdf[1]->Scale(400000.0);
  g_hpdf[2] = (TH2D*)fin->Get("hbg_psdql");      //hpdf[2]->Scale(400000.0);
  
  TH1D* hpdfx[g_npdfs];
  TH1D* hpdfy[g_npdfs];
  TH1D* hxsum;
  TH1D* hysum;
  
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
  

  g_hdt = (TH2D*)fin->Get("hdata");
  TH1D* hdtx = (TH1D*)g_hdt->ProjectionX("hdtx");
  TH1D* hdty = (TH1D*)g_hdt->ProjectionY("hdty");
  
  TCanvas *xcan = new TCanvas();
  hdtx->Draw("e1");
  hxsum->Draw("histsame");
  hpdfx[0]->Draw("histsame");
  hpdfx[1]->Draw("histsame");
  hpdfx[2]->Draw("histsame");

  TCanvas *ycan = new TCanvas();
  hdty->Draw("e1");
  hysum->Draw("histsame");
  hpdfy[0]->Draw("histsame");
  hpdfy[1]->Draw("histsame");
  hpdfy[2]->Draw("histsame");


  plotslicesX( );


}
