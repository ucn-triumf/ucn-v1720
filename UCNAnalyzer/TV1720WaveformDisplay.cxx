/// A simple batch script that creates a bunch of UCN histograms using 
/// TUCNAnaManager class.  
///
/// T. Lindner ; July 19, 2012

#include <stdio.h>
#include <iostream>

#include "TRootanaEventLoop.hxx"
#include "TUCNAnaManager.h"

class UCNAnalyzer: public TRootanaEventLoop {

  /// UCN analysis manager; has instances of the histograms that 
  /// we want.
  TUCNAnaManager *anaManager;
  int numEvents;

public:

  UCNAnalyzer() {
    anaManager = 0;
    numEvents = 0;
  };

  virtual ~UCNAnalyzer() {};

  void BeginRun(int transition,int run,int time){
    if(anaManager) delete anaManager;
    anaManager = new TUCNAnaManager(0,0);
  }


  bool ProcessMidasEvent(TDataContainer& dataContainer){

    numEvents += anaManager->ProcessMidasEvent(dataContainer);
    return true;
  }


}; 


int main(int argc, char *argv[])
{

  UCNAnalyzer::CreateSingleton<UCNAnalyzer>();
  return UCNAnalyzer::Get().ExecuteLoop(argc, argv);

}

