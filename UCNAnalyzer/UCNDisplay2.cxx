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
/// TUCNAnaViewer2 class.  This class could be reused in other contextes;
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

#include "TUCNAnaViewer2.h"

class UCNDisplay2: public TRootanaDisplay {
  

  /// UCN analysis manager; has instances of the histograms that 
  /// we want to plot.
  TUCNAnaViewer2 *anaViewer;

public:

  UCNDisplay2() {
    SetDisplayName("UCN Display");    
  };
  

  // void BeginRun(int  	transition,
  // 		int  	run,
  // 		int  	time) {
  //   std::cout << "Begin of run " << run << " at time " << time << std::endl;
  // };

  void AddAllCanvases(){

    anaViewer = new TUCNAnaViewer2();
    AddSingleCanvas("V1720 Time");
  };

  ~UCNDisplay2() {};

  void ResetHistograms(){
      anaViewer->fV1720TimeHistogram->Reset();
  }

  void UpdateHistograms(TDataContainer& dataContainer){

    /// This is where all the work to update the histogram happens.
    anaViewer->ProcessMidasEvent(dataContainer);

  }


  void PlotCanvas(TDataContainer& dataContainer){
    
    if(GetDisplayWindow()->GetCurrentTabName().compare("V1720 Time") == 0){       
      TCanvas* c1 = GetDisplayWindow()->GetCanvas("V1720 Time");
      c1->Clear();
      anaViewer->fV1720TimeHistogram->Draw("e1");
      c1->Modified();
      c1->Update();
    }

  }


}; 

int main(int argc, char *argv[])
{
  UCNDisplay2::CreateSingleton<UCNDisplay2>();  
  return UCNDisplay2::Get().ExecuteLoop(argc, argv);
}

