#include "TLine.h"
#include "TMath.h"
#include "TASImage.h"
#include "TEllipse.h"
#include "TH2D.h"
#include <iostream>

double mmperpixel = 0.0406223;
double mmperpixele = mmperpixel/20.0;
double mm2perpixel2 = mmperpixel*mmperpixel;
double mm2perpixel2e = 2.0 * mmperpixel * mmperpixele;

// Function to return 1 if point is above line, -1 if it is below, or 0 if it is on line
int PtAboveBelowLine( TLine * tl,  double xx, double yy){
  int retval=0;
  double x1 = tl->GetX1();
  double x2 = tl->GetX2();
  double y1 = tl->GetY1();
  double y2 = tl->GetY2();
  // never called with infinite slope!
  // handle zero slope first
  if ( y1 == y2 ){
    if ( yy > y1 ) retval = 1;
    else if ( yy < y1 ) retval = -1;
    else retval = 0;
  } else {
    double slope = (y2-y1) / (x2-x1);
    double yline = slope * ( xx - x1 ) + y1;
    if ( yy > yline )  retval =1;
    else if ( yy < yline ) retval = -1;
    else retval = 0;
  }

  return retval;
}

// Function to return 1 if point is right of line, -1 if it is left of line, or 0 if it is on line
int PtLeftRightLine( TLine * tl , double xx, double yy) {
  int retval=0;
  double x1 = tl->GetX1();
  double x2 = tl->GetX2();
  double y1 = tl->GetY1();
  double y2 = tl->GetY2();
  // never called with zero slope!
  // handle infinite slope first
  if ( x1 == x2 ){
    if ( xx > x1 ) retval = 1;
    else if ( xx < x1 ) retval = -1;
    else retval = 0;
  } else {
    double slope = (x2-x1) / (y2-y1);
    double xline = slope * ( yy - y1 ) + x1;
    if ( xx > xline )  retval =1;
    else if ( xx < xline ) retval = -1;
    else retval = 0;
  }

  return retval;
}



double PrintLineLength( TLine* tl, char * linename, char * unitname ){

  double x1 = tl->GetX1();
  double x2 = tl->GetX2();
  double y1 = tl->GetY1();
  double y2 = tl->GetY2();

  double len = TMath::Sqrt(  (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) );

  std::cout<<"Line "<<linename<<" has length "<<len<<" "<<unitname<<std::endl;

  return len;

}


TEllipse * GetCircleFromLine( TLine * tl ){

  double x1 = tl->GetX1();
  double x2 = tl->GetX2();
  double y1 = tl->GetY1();
  double y2 = tl->GetY2();
  
  double len = PrintLineLength( tl, "circle diameter", "pixels" );
  std::cout<<"Diameter of circle is "<<len * mmperpixel<<" mm "<<std::endl;
  double rcircle = len / 2.0;

  // find circle center
  double xc=0.0;
  double yc=0.0;
  if ( x2 == x1 ){
    // vertical line
    xc = x1;
    yc = y1+rcircle;
  } else if ( y2 == y1 ){
    xc = x1 + rcircle;
    yc = y1;
  } else{
    double slope = (y2-y1)/(x2-x1);
    double theta = TMath::ATan( slope );
    xc = x1 + rcircle * TMath::Cos( theta );
    yc = y1 + rcircle * TMath::Sin( theta );
  }

  TEllipse * elopen = new TEllipse( xc, yc, rcircle, rcircle );
  elopen->SetLineWidth(2);
  elopen->SetFillStyle(0);
  return elopen;

}


void CountFace(){

  TASImage * tas = new TASImage("P1010002.JPG");
  tas->Gray();

  UInt_t *array = tas->GetArgbArray();
  UInt_t nxpixels = tas->GetWidth();
  UInt_t nypixels = tas->GetHeight();

  TH2D * himg = new TH2D("himg","himg",
			 nxpixels,0.,float(nxpixels),
			 nypixels,0.,float(nypixels));

  std::cout<<" width = "<<nxpixels<<" height = "<< nypixels << std::endl;

  for ( UInt_t row = 0; row < nxpixels; ++row ) {
    for ( UInt_t col = 0; col < nypixels; ++col ) {
      UInt_t index = col*nxpixels + row;
      //      float grey = float( array[index]&0xff )/256.0;
      float grey = float( array[index] );
      himg->SetBinContent( row+1, nypixels-col, grey );
    }
  }

  //gStyle->SetPalette(53);
  himg->Draw("colz");

  //Lines:
  // top left glass
  TLine* l0_x1 = new TLine(774.2,1566.1,1264.4,1559.7);    l0_x1->SetLineWidth(2); l0_x1->Draw();
  TLine* l0_y1 = new TLine(1264.4,1559.7,1266.96,2059.1);  l0_y1->SetLineWidth(2); l0_y1->Draw();

  // top middle glass
  TLine* l1_x1 = new TLine(1267.85,1573.9,1980.1,1560.0);   l1_x1->SetLineWidth(2); l1_x1->Draw();
  TLine* l1_y0 = new TLine(1267.85,1573.9,1276.99,2063.62); l1_y0->SetLineWidth(2); l1_y0->Draw();
  TLine* l1_y1 = new TLine(1980.1,1562.0,1998.5,2052.3);    l1_y1->SetLineWidth(2); l1_y1->Draw();

  // top right glass
  TLine* l2_x1 = new TLine(1987.2,1571.3,2474.7,1569.3);  l2_x1->SetLineWidth(2); l2_x1->Draw();
  TLine* l2_y0 = new TLine(1987.2,1571.3,2005.4,2048.4);  l2_y0->SetLineWidth(2); l2_y0->Draw();

  // middle left glass
  TLine* l3_x0 = new TLine(767.5,1556.8,1262.0,1554.2);  l3_x0->SetLineWidth(2); l3_x0->Draw();
  TLine* l3_x1 = new TLine(778.7,841.7,1258.9,836.1);    l3_x1->SetLineWidth(2); l3_x1->Draw();
  TLine* l3_y1 = new TLine(1258.9,836.1,1262.03,1554.2); l3_y1->SetLineWidth(2); l3_y1->Draw();

  // middle middle glass
  TLine* l4_x0 = new TLine(1281.6,1564.2,1993.9,1547.0);  l4_x0->SetLineWidth(2); l4_x0->Draw();
  TLine* l4_x1 = new TLine(1265.8,850.6,1981.9,834.1);    l4_x1->SetLineWidth(2); l4_x1->Draw();
  TLine* l4_y0 = new TLine(1265.8,850.6,1281.6,1564.2); l4_y0->SetLineWidth(2); l4_y0->Draw();
  TLine* l4_y1 = new TLine(1981.9,834.1,1993.9,1547.0); l4_y1->SetLineWidth(2); l4_y1->Draw();

  double lens[4];
  lens[0] = PrintLineLength( l4_x0, "l4_x0", "xpixels" );
  lens[1] = PrintLineLength( l4_x1, "l4_x1", "xpixels" );
  lens[2] = PrintLineLength( l4_y0, "l4_y0", "ypixels" );
  lens[3] = PrintLineLength( l4_y1, "l4_y1", "ypixels" );

  double avglen=0.0;
  double stderrlen=0.0;
  double sumlen  = 0.0;
  double sumlen2 = 0.0;
  for (int i=0; i<4; i++){
    sumlen += lens[i];
    sumlen2 += lens[i]*lens[i];
  }
  avglen = sumlen / 4.0;
  stderrlen =  TMath::Sqrt(  (sumlen2 / 3.0 - 4.0/3.0 * avglen * avglen) / 4.0 );
  std::cout<<"Average length = "<<avglen<<" +- "<<stderrlen<<std::endl;
  std::cout<<"mm per pixel="<< 29.0 / avglen<<" +- "<< 29.0/avglen * stderrlen/avglen<<std::endl;
  mmperpixel =  29.0 / avglen;
  mmperpixele = mmperpixel * stderrlen/avglen;

  mm2perpixel2 = mmperpixel*mmperpixel;
  mm2perpixel2e = 2.0 * mmperpixel * mmperpixele;

  // middle right glass
  TLine* l5_x0 = new TLine(1996.4,1563.3,2479.8,1555.6);  l5_x0->SetLineWidth(2); l5_x0->Draw();
  TLine* l5_x1 = new TLine(1991.1,843.7,2472.5,839.8);    l5_x1->SetLineWidth(2); l5_x1->Draw();
  TLine* l5_y0 = new TLine(1991.1,843.7,1996.4,1563.3); l5_y0->SetLineWidth(2); l5_y0->Draw();


  // bottom left glass
  TLine* l6_x0 = new TLine(785.4,824.4,1257.9,823.46);     l6_x0->SetLineWidth(2); l6_x0->Draw();
  TLine* l6_y1 = new TLine(1260.1,358.0,1257.9,823.46);    l6_y1->SetLineWidth(2); l6_y1->Draw();

  // bottom middle glass
  TLine* l7_x0 = new TLine(1276.9,838.4,1992.1,824.6);   l7_x0->SetLineWidth(2); l7_x0->Draw();
  TLine* l7_y0 = new TLine(1270.7,356.8,1276.9,838.4);   l7_y0->SetLineWidth(2); l7_y0->Draw();
  TLine* l7_y1 = new TLine(1986.8,363.5,1992.1,824.1);   l7_y1->SetLineWidth(2); l7_y1->Draw();

  // bottom right glass
  TLine* l8_x0 = new TLine(1998.6,831.1,2466.2,827.3);    l8_x0->SetLineWidth(2); l8_x0->Draw();
  TLine* l8_y0 = new TLine(1995.5,367.1,1998.6,831.1);   l8_y0->SetLineWidth(2); l8_y0->Draw();

  // line accross diagonal of opening.
  //TLine* lopen = new TLine(980.88,554.6,2293.55,1844.28); lopen->SetLineWidth(2); lopen->Draw();
  //TEllipse * elopen = GetCircleFromLine( lopen );
  //elopen->Draw();
  
  TEllipse * elopen = new TEllipse( 1626.54, 1210.35, 922.9, 922.9 );
  elopen->SetFillStyle(0);
  elopen->SetLineWidth(2); elopen->Draw();
  std::cout<<"Diameter of opening is "<<2.0 * 922.9<<" pixels or "<< 2.0*922.9*mmperpixel <<" mm "<<std::endl;

  // now calculate area of each glass that is seen, and area of circle
  double areas[9];
  for (int i=0; i<9; i++) areas[i] = 0.0;
  double totarea=0.0;

  double xc = elopen->GetX1();
  double yc = elopen->GetY1();
  double rc2 = elopen->GetR1(); rc2 *= rc2;
  for ( UInt_t row = 0; row < nxpixels; ++row ) {
    //std::cout<<"Calc area for row"<<row<<" of "<<nxpixels<<std::endl;
    for ( UInt_t col = 0; col < nypixels; ++col ) {
      double xval = himg->GetXaxis()->GetBinCenter( row+1 );
      double yval = himg->GetYaxis()->GetBinCenter( nypixels-col );
      double rpt2 = (xval-xc)*(xval-xc) + (yval-yc)*(yval-yc);
      if (rpt2 < rc2 ){
	// pixel is inside circle
	totarea += 1.0;
	// top left:  check if pixel is above(+1) l0_x1 and left(-1) of l0_y1
	if (  PtAboveBelowLine( l0_x1, xval, yval) == 1 &&
	      PtLeftRightLine( l0_y1, xval, yval) == -1 ) areas[0] += 1.0;
	
	// top middle glass: above l1_x1(+1), right of l1_y0(+1), left of l1_y1 (-1)
	if ( PtAboveBelowLine( l1_x1, xval, yval ) == 1 &&
	     PtLeftRightLine( l1_y0, xval, yval ) == 1 &&
	     PtLeftRightLine( l1_y1, xval, yval ) == -1 ) areas[1] += 1.0;
	
	// top right glass: above l2_x1 (+1), and right of l2_y0 (+1)
	if ( PtAboveBelowLine( l2_x1, xval, yval ) == 1 &&
	     PtLeftRightLine( l2_y0, xval, yval ) == 1 ) areas[2] += 1.0;
	
	// middle left glass: below l3_x0 (-1), above l3_x1 (+1), and left of l3_y1 (-1)
	if ( PtAboveBelowLine( l3_x0, xval, yval ) == -1 &&
	     PtAboveBelowLine( l3_x1, xval, yval ) == 1 &&
	     PtLeftRightLine( l3_y1, xval, yval ) == -1 ) areas[3] += 1.0;
	
	// middle middle: 
	if ( PtAboveBelowLine( l4_x0, xval, yval ) == -1 &&
	     PtAboveBelowLine( l4_x1, xval, yval ) == 1 &&
	     PtLeftRightLine( l4_y0, xval, yval ) == 1 &&
	     PtLeftRightLine( l4_y1, xval, yval ) == -1 ) areas[4] += 1.0;
	
	// middle right: 
	if ( PtAboveBelowLine( l5_x0, xval, yval ) == -1 &&
	     PtAboveBelowLine( l5_x1, xval, yval ) == 1 &&
	     PtLeftRightLine( l5_y0, xval, yval ) == 1 ) areas[5] += 1.0;
	
	// bottom left
	if ( PtAboveBelowLine( l6_x0, xval, yval ) == -1 &&
	     PtLeftRightLine( l6_y1, xval, yval ) == -1 ) areas[6] += 1.0;
	
	// bottom middle: 
	if ( PtAboveBelowLine( l7_x0, xval, yval ) == -1 &&
	     PtLeftRightLine( l7_y0, xval, yval ) == 1 &&
	     PtLeftRightLine( l7_y1, xval, yval ) == -1 ) areas[7] += 1.0;
	
	// bottom right:
	if ( PtAboveBelowLine( l8_x0, xval, yval ) == -1 &&
	     PtLeftRightLine( l8_y0, xval, yval ) == 1  ) areas[8] += 1.0;
      }
    }
  }
  
  // print areas:
  std::cout<<"Total area is "<<totarea<<" pixels"<<std::endl;
  for (int i=0; i<9; i++){
    std::cout<<"Glass"<<i<<" area is "<<areas[i]<<" pixels"<<std::endl;
    std::cout<<"Glass"<<i<<" area is "
	     <<areas[i] * mm2perpixel2<<" +- "
	     <<areas[i] * 2.0 * mmperpixel * mmperpixele
	     <<" mm^2 "<<std::endl;
    
  }

  
  //s = side of channel = 29mm
  //r = radius of opening = 40 mm
  //u = sqrt(r^2-x^2)
  //
  //Here's what I got for the channels:
  //
  //Area of middle channel = A_m = s^2 = 841 mm^2
  //
  //Area of each edge channel = A_e = (1/2)(s * u + r^2 * arctan ( s/ u)) = 567 mm^2
  //
  //Area of each corner channel = A_c = ((Area of opening) - A_m - 4*A_e)/4 = 479 mm^2
  //
  // check areas:
  double ss = 29.0 ;//mm
  double rr = 75.0 / 2.0 ; //mm
  double theta = TMath::ASin( ss/2.0/rr );
  double Am = ss*ss;
  double Ae = 0.5*ss*rr*TMath::Cos( theta ) - ss*ss/2.0 + rr*rr*theta;
  double Ac = ( TMath::Pi()*rr*rr - Am - 4.0*Ae ) / 4.0;
  // TF1 * altAe = new TG1("altAe","altAe", "TMath::Sqrt(37.5*37.5 - (x-29./2)*(x-29./2)");

  std::cout<<" Area of middle channel "<<Am<<" mm^2"<<std::endl;
  std::cout<<" Area of each edge channel "<< Ae <<" mm^2"<<std::endl;
  std::cout<<" Area of each corner channel "<<Ac<<" mm^2"<<std::endl;
  
}


