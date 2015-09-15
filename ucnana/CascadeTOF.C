#include "CascadeTOF.h"

CascadeTOF::CascadeTOF( char * infilename ){//, int nx, int ny, int ntbins, float binwid  ){  
  //first parse the table of content file
  char fname[100];
  sprintf(fname,"%s.txt",infilename);
  ifstream ftoc;
  ftoc.open(fname);
  std::string line;
  std::string datafilename;
  int nx;
  int ny;
  int dwelltime;
  float binwid;
  int ntbins;
  while (ftoc){
    getline( ftoc, line );
    //std::cout<<line<<std::endl;
    std::size_t loc;
    loc = line.find("File Name : ");
    if ( loc != std::string::npos ){
      datafilename = line.substr( loc+12 );
      std::replace( datafilename.begin(), datafilename.end(), '\r', '\0');
      std::cout<<"Found File Name= "<<datafilename<<std::endl;
      std::stringstream convert( line.substr( loc+13, 6 ) );
      convert >> fBegYr;
      fBegDay = fBegYr/10000;
      fBegMon = (fBegYr-fBegDay*10000)/100;
      fBegYr -= fBegDay*10000 + fBegMon*100;
      std::cout<<"Extracted YY="<<fBegYr<<" MM="<<fBegMon<<" DD="<<fBegDay<<" from filename"<<std::endl;
      continue;
    }
    
    loc = line.find("No of X Stripes or Pixel : ");
    if ( loc != std::string::npos ){
      std::stringstream convert( line.substr( loc+27, 2 ) );
      convert >> nx;
      std::cout<<"Found nx= "<<nx<<std::endl;
      continue;
    }

    loc = line.find("No of Y Stripes or Pixel : ");
    if ( loc != std::string::npos ){
      std::stringstream convert( line.substr( loc+27, 2 ) );
      convert >> ny;
      std::cout<<"Found ny= "<<nx<<std::endl;
      continue;
    }

    loc = line.find("Dwell Time : ");
    if ( loc != std::string::npos ){
      std::stringstream convert( line.substr( loc+13 ) );
      convert >> dwelltime;
      binwid = float(dwelltime)/10000000.0;
      std::cout<<"Found dwelltime="<<dwelltime<<" binwid="<<binwid<<std::endl;
      continue;
    }

    loc = line.find("No of Bins : ");
    if ( loc != std::string::npos ){
      std::stringstream convert( line.substr( loc+13 ) );
      convert >> ntbins;
      std::cout<<"Found ntbins="<<ntbins<<std::endl;
      continue;
    }

    loc = line.find("Start Time   : ");
    if ( loc != std::string::npos ){
      std::stringstream convhh( line.substr( loc+15,2 ) );
      std::stringstream convmm( line.substr( loc+18,2 ) );
      std::stringstream convss( line.substr( loc+21,2 ) );
      convhh >> fBegHr;
      convmm >> fBegMin;
      convss >> fBegSec;
      std::cout<<"Found start time= "
	       <<fBegHr<<":"
	       <<fBegMin<<":"
	       <<fBegSec<<std::endl;
      // build the unix timestamp
      std::tm timeinfo;
      timeinfo.tm_year = fBegYr+2000-1900;
      timeinfo.tm_mon  = fBegMon-1;
      timeinfo.tm_mday = fBegDay;
      timeinfo.tm_hour = fBegHr-6; // may need to adjust for timezone -- this number works for winnipeg?
      timeinfo.tm_min  = fBegMin;
      timeinfo.tm_sec  = fBegSec;
      fStartTime = std::mktime( &timeinfo );
      std::cout<<"Start Time= "<<fStartTime<<std::endl;
      continue;
    }

    loc = line.find("Stop Time    : ");
    if ( loc != std::string::npos ){
      //Stop Time   : 04:48:21
      //std::replace( line.begin(), line.end(), ':', ' ');
      std::stringstream convhh( line.substr( loc+15,2 ) );
      std::stringstream convmm( line.substr( loc+18,2 ) );
      std::stringstream convss( line.substr( loc+21,2 ) );
      convhh >> fEndHr;
      convmm >> fEndMin;
      convss >> fEndSec;
      std::cout<<"Found stop time= "
	       <<fEndHr<<":"
	       <<fEndMin<<":"
	       <<fEndSec<<std::endl;
      continue; 
    }
    
    loc = line.find("Absolute Time : ");
    if ( loc != std::string::npos ){
      std::replace( datafilename.begin(), datafilename.end(), '\r', '\0');      
      std::stringstream convert( line.substr( loc+16 ) );
      convert >> fRunLength;
      fEndTime = fStartTime + fRunLength / 10000000 ;
      std::cout<<"End Time = "<<fEndTime<<std::endl;
      break;
    }
  }
  
  ftoc.close();
  

  char histname[100];
  sprintf( histname, "hTOF%s",infilename );
  fTOF=new TH1D(histname," ; Time (s) ; Count/bin",ntbins,0.0, float(ntbins)*binwid );

  //return;

  ifstream fin;
  int bin;
  int count;
  int junk;
 
  std::cout<<"Open file : "<< datafilename.c_str() <<std::flush;
  std::cout<<"."<< std::endl;
  fin.open( datafilename.c_str() );
  if ( fin.is_open() == false ){
    std::cout<<"Couldnt open file!"<<std::endl;
    exit(0);
  }
  std::cout<<"nx="<<nx<<" ny="<<ny<<" nbins="<<ntbins<<std::endl; 
  for (int i = 0 ; i < ntbins; i++ ){
    fin >> bin >> count;
    if (i%1000==0) std::cout<<bin<<" "<<count<<std::endl;
    for (int j=0; j< nx*ny; j++ ) fin >> junk;
    fTOF->SetBinContent(bin,float(count));
    fTOF->SetBinError(bin,sqrt(float(count)));
  }
  
}
