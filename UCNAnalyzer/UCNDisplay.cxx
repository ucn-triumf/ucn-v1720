/// This is an example for how one could display data from 
/// UCN electronics.  The current example of this code will 
/// create a tabbed display with the following set of canvases:
/// 1) A canvas of V1720 waveforms, for all channels.
/// 2) A canvas of V1720 waveforms, organized by modules.
/// 3) A canvas of V1720 time histograms, from Q/T bank.
/// 4) A canvas of V1720 charge histograms, from Q/T bank.
/// 5) A canvas of V1720 charge histograms, for all channels together.
///
/// The creation and filling of histograms is organized by having a separate
/// TUCNAnaViewer class.  This class could be reused in other contextes;
/// for instance in a batch mode analyzer.
///
/// T. Lindner ; July 19, 2012
///
#include <stdio.h>
#include <iostream>

#include "TRootanaDisplay.hxx"

#include "TH1D.h"
#include "TH2D.h"

#include "TFancyHistogramCanvas.hxx"
#include "TV1720Histograms.h"

#include "TUCNAnaViewer.h"

class UCNDisplay: public TRootanaDisplay {
  

  /// UCN analysis manager; has instances of the histograms that 
  /// we want to plot.
  TUCNAnaViewer *anaViewer;

public:

  UCNDisplay() {
    SetDisplayName("UCN Display");    
  }
  
  void AddAllCanvases(){

    anaViewer = new TUCNAnaViewer();

    // Set up a canvas comparing the Q short and Q long values separated by modules
    TFancyHistogramCanvas* v1720_QSQL = 
      new TFancyHistogramCanvas(anaViewer->fV1720QSQLHistograms, "V1720 - Q Short vs Q Long",8,false);
    AddSingleCanvas(v1720_QSQL);

    // Set up a canvas comparing the PSD and Q long value separated by modules
    TFancyHistogramCanvas* v1720_PSDQL =
      new TFancyHistogramCanvas(anaViewer->fV1720PSDQLHistograms, "V1720 - PSD vs Q Long",9,false);
    AddSingleCanvas(v1720_PSDQL);
    
  };

  ~UCNDisplay() {};

  void UpdateHistograms(TDataContainer& dataContainer){

    // This is where all the work to update the histogram happens.
     anaViewer->ProcessMidasEvent(dataContainer);

  }


  void PlotCanvas(TDataContainer& dataContainer){
    
    if(GetDisplayWindow()->GetCurrentTabName().compare("V1720 - Q Short vs Q Long") == 0){       
      TCanvas* c1 = GetDisplayWindow()->GetCanvas("V1720 - Q Short vs Q Long");
      c1->Clear();
      anaViewer->fV1720QSQLHistograms->GetHistogram(1)->Draw();
      c1->Modified();
      c1->Update();
    }

  }


}; 


int main(int argc, char *argv[])
{
  std::cout<<"Size of ULong_t = "<<sizeof(ULong_t)
	   <<" size of uint64_t = "<<sizeof(uint64_t)
	   <<" if these differ -- maybe trouble for timestamp "
	   <<std::endl;


  UCNDisplay::CreateSingleton<UCNDisplay>();  
  return UCNDisplay::Get().ExecuteLoop(argc, argv);
}

