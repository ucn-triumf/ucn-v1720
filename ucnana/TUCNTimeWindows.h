#ifndef _TUCNTimeWindows_h_
#define _TUCNTimeWindows_h_

#include <TROOT.h>
#include <TTree.h>
#include <TH1D.h>
#include <TDirectory.h>
#include <TF1.h>

#include "TVConstants.h"
#include "TTimeOffsets.h"

const double PBEAMTIMESEP = 301.0; //< time between start of proton pulses (seconds)
const double PBNSTOSEC = 1.0e-9; //< conversion ns to seconds

const bool PBUSEPBSIGNAL = true; //< Set to true to use proton beam signal 
                                 //< otherwise use rate analysis to decide

const int PBCHAN = 10;            //< Channel with proton beam signal
const double PBQLMIN = 15000.0;   //< Proton beam signal minimum QL 
const double PBQLMAX = 20000.0;   //< Proton beam signal maximum QL

/// TCUNTimeWindows:  Class to search for proton beam times
/// by looking at the rate of events as a function of time.
/// These times are all in clock cycles relative to the 
/// smallest clock cycle count in the first file analyzed.
///
/// Author: Blair Jamieson (Aug. 2015)
class TUCNTimeWindows {

 private:
  int runnum;
  ULong64_t tstart; //< Earliest time found in first file checked
  ULong64_t base;   //< Sum of last time in files that CalcTimes was called for
  ULong64_t time1;  //< Earliest time found in last file checked (base corrected)
  ULong64_t time2;  //< Latest time found in last file checked (base corrected)
  std::vector< double > vBeamTimes; //< Times of UCN "beam bunches" in sec since start of run
  std::vector< double > vStartTimes; //< Times of UCN "beam bunches" in sec since start of run
  std::vector< double > vEndTimes; //< Times of UCN "beam bunches" in sec since start of run

  std::vector< bool > isGVclosed; //< Guess at whether gate valve is closed

  int nfileschecked; //< Keep count of files checked
  std::vector< TH1D* > twhists; //< Keep vector of pointers to rate histograms
                        //< that were used to do time windowing. Time relative to start of run.

  std::vector< TH1D* > twhistsrt; //< Keep vector of pointers to rate histograms
                        //< with unix time stamp as x axis

  TH1D* twhistch[ NDPPBOARDS * PSD_MAXNCHAN]; //< Time window hist for each chan

  TH1D* hdtwin; //< Time difference between proton beam pulse times
  TDirectory * twdir; //< Keep a pointer to the directory in which to put
                      //< the rate histograms used for the time windowing

  std::vector< int > vRunStartTimes; //< vector of unix timestamps of run start times.
  
 public:

  /// Construtor takes tUCN data tree as input
  /// along with tRunTran tree with times of run start/stop
  /// and does the search for the UCN pulse times
  TUCNTimeWindows( int arunnum, TTree * atUCN, TTree* atRunTran );

  // ~TUCNTimeWindows(){return;}

  /// Method to add another file to the time windows
  /// Assumption will be that this file is directly
  /// after the previous one, and that any subsequent
  /// calls to CalcTimes are with new TTrees at
  /// later sequential times.
  void CalcTimes( int arunnum, TTree * atUCN, TTree* atRunTran );

  void Print();

  bool IsGVclosed( int ibunch ){
    if ( ibunch >= 0 && ibunch < isGVclosed.size() ) 
      return isGVclosed.at(ibunch);
    else 
      return true;
  }

  int NBunches(){ return vBeamTimes.size(); };

  ULong64_t GetTBeg(){ return time1; }; ///< Get first time in file (ns)
  ULong64_t GetTEnd(){ return time2; }; ///< Get last time in file (ns)

  double GetTBegs(){ return time1*PBNSTOSEC; }; ///< Get first time in file (seconds)
  double GetTEnds(){ return time2*PBNSTOSEC; }; ///< Get last time in file (seconds)
  std::vector<double> & GetPBeamTimes(){ return vBeamTimes; }; ///< Get times of UCN pulses
  std::vector<double> & GetStartTimes(){ return vStartTimes; }; ///< Get times of UCN pulses
  std::vector<double> & GetEndTimes(){ return vEndTimes; }; ///< Get times of UCN pulses

  int GetRunStartTime(int ibunch){
    if ( ibunch >= 0 && ibunch < vRunStartTimes.size() ) return vRunStartTimes.at(ibunch);
    else return 0;
  }
  double GetPBeamTime(int ibunch){ 
    if ( ibunch >= 0 && ibunch < vBeamTimes.size() ) return vBeamTimes.at(ibunch);
    else return 0;
  }
   
  double GetStartTime(int ibunch){ 
    if ( ibunch >= 0 && ibunch < vStartTimes.size() ) return vStartTimes.at( ibunch );
    else return 0;
  }

  double GetEndTime(int ibunch){ 
    if ( ibunch >= 0 && ibunch< vEndTimes.size() ) return vEndTimes.at( ibunch );
    else return 0;
  }

  /// GetBunchNum( atime ): given "atime" in seconds, what proton
  /// bunch number does this correspond to?  If "atime" does not
  /// fall within one of the time bunches, return -1.
  int GetBunchNum( double aTime){
    int curbunch=-1;
    for (int ibunch=0; ibunch<vStartTimes.size(); ibunch++){
      if ( aTime > vStartTimes.at(ibunch) &&
	   aTime < vEndTimes.at(ibunch) ) {
	curbunch = ibunch;
	break;
      }
    }
    return curbunch;
  }

};


#endif
