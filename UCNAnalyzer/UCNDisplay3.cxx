//Andrew Sikora
//June 9, 2016

#include <stdio.h>
#include <algorithm>
#include <cmath>

#include "TH1D.h"
#include "TH2D.h"

#include "TFancyHistogramCanvas.hxx"
#include "TV1720Histograms.h"
#include "TV1720WaveformDisplay.h"

#include "TUCNAnaViewer3.h"

#include "TRootanaDisplay.hxx"
#include "PulseShapeStruct.h"
#include "TDataContainer.hxx"
#include "TV1720Histograms.h"

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <sys/stat.h>

//using namespace std;

int board, ch, ev, preev, eventSelect;
int firstPlot = 0;
int m=0;

fstream eventNumber;

class UCNDisplay3: public TRootanaDisplay {

  TUCNAnaViewer3 *anaViewer;

public:

  UCNDisplay3()
  {
    SetDisplayName("UCN Display3");
  };
  
  void AddAllCanvases()
  {
    anaViewer = new TUCNAnaViewer3();
    
    TFancyHistogramCanvas* v1720_waveform = 
      new TFancyHistogramCanvas(anaViewer->fV1720Waveform, "V1720 Waveform",9,false);
    AddSingleCanvas(v1720_waveform);

     TFancyHistogramCanvas * v1720_CLQEvNum = new TFancyHistogramCanvas(anaViewer->fV1720CLQEvNum, "V1720 CLQvsEvNum", 9, false);
     AddSingleCanvas(v1720_CLQEvNum);

    TFancyHistogramCanvas * v1720_QLQL = new TFancyHistogramCanvas(anaViewer->fV1720QLQL, "V1720 QLvsQL", 9, false);
    AddSingleCanvas(v1720_QLQL);

    TFancyHistogramCanvas * v1720_QSQS = new TFancyHistogramCanvas(anaViewer->fV1720QSQS, "V1720 QSvsQS", 9, false);
    AddSingleCanvas(v1720_QSQS);

    TFancyHistogramCanvas * v1720_CSQEvNum = new TFancyHistogramCanvas(anaViewer->fV1720CSQEvNum, "V1720 CSQvsEvNum", 9, false);
    AddSingleCanvas(v1720_CSQEvNum);
  };
  
  ~UCNDisplay3() {};
  
  
  void UpdateHistograms(TDataContainer& dataContainer)
  {
    std::cout<<"entering update histograms"<<std::endl;
    TMidasEvent sample = dataContainer.GetMidasEvent();
    TMidas_BANK32 * bank = NULL;  
    char * pdata = sample.GetData();
    DPP_Bank_Out_t *b;   

    std::cout<<"Before Process midas Event"<<std::endl;
    anaViewer->ProcessMidasEvent(dataContainer);
    std::cout<<"after process midas event"<<std::endl;
  }
  
  void PlotCanvas(TDataContainer& dataContainer)
  {
    std::cout<<"Start of PlotCanvas"<<std::endl;
    if(GetDisplayWindow()->GetCurrentTabName().compare("V1720 Waveform")==0)
      {
	
	TCanvas* cl= GetDisplayWindow()->GetCanvas("V1720 Waveform");
	
	cl->Clear();
	
	anaViewer->fV1720Waveform->GetHistogram(1)->Draw("hist");
	
	cl->Modified();
	
	cl->Update();
      }
  }
bool fileExists(const std::string& filename)
{
    struct stat buf;
    if (stat(filename.c_str(), &buf) != -1)
    {
        return true;
    }
    return false;
}

  
};

int main(int argc, char *argv[])
{
  UCNDisplay3::CreateSingleton<UCNDisplay3>();
  return UCNDisplay3::Get().ExecuteLoop(argc, argv);
}




