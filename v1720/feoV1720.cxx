/*****************************************************************************/
/**
\file feoV1720.cxx

\mainpage

\section contents Contents

Standard Midas Frontend for Optical access to the CAEN v1720 using the A3818 CONET2 driver

\subsection organization File organization

- Compile time parameters setting
- MIDAS global variable defintion
- MIDAS function declaration
- Equipment variables
- functions

\subsection routines Callback routines for system transitions

These routines are called whenever a system transition like start/
stop of a run occurs. The routines are called on the following
occations:

- frontend_init:  When the frontend program is started. This routine
                should initialize the hardware.

- frontend_exit:  When the frontend program is shut down. Can be used
                to releas any locked resources like memory, commu-
                nications ports etc.

- begin_of_run:   When a new run is started. Clear scalers, open
                rungates, etc.

- end_of_run:     Called on a request to stop a run. Can send
                end-of-run event and close run gates.

- pause_run:      When a run is paused. Should disable trigger events.

- resume_run:     When a run is resumed. Should enable trigger events.

\subsection notes Notes about this frontend

This frontend has been designed so that it should compile and work
by default without actual actual access to v1720 hardware. We have turned
off portions of code which make use of the driver to the actual hardware.
Where data acquisition should be performed, we generate random data instead
(see v1720CONET2::ReadEvent()). See usage below to use real hardware.

The simulation code assumes the following setup:
- 1 frontend only
- Arbitrary number of v1720 modules
- Event builder not used

The code to use real hardware assumes this setup:
- 1 A3818 PCI-e board per PC to receive optical connections
- NBLINKSPERA3818 links per A3818 board
- NBLINKSPERFE optical links controlled by each frontend
- NB1720PERLINK v1720 modules per optical link (daisy chained)
- NBV1720TOTAL v1720 modules in total
- The event builder mechanism is used

\subsection usage Usage

\subsubsection simulation Simulation
Simply set the Nv1720 macro below to the number of boards you wish to simulate,
compile and run:
    make SIMULATION=1
    ./feoV1720.exe

\subsubsection real Real hardware
Adjust NBLINKSPERA3818, NBLINKSPERFE, NB1720PERLINK and NBV1720TOTAL below according
to your setup.  NBV1720TOTAL / (NBLINKSPERFE * NB1720PERLINK) frontends
must be started in total. When a frontend is started, it must be assigned an index
number:

    ./frontend -i 0

If no index number is supplied, it is assumed that only 1 frontend is used to control
all boards on all links on that computer.

For example, consider the following setup:

    NBLINKSPERA3818    4     // Number of optical links used per A3818
    NBLINKSPERFE       2     // Number of optical links controlled by each frontend
    NB1720PERLINK      2     // Number of daisy-chained v1720s per optical link
    NBV1720TOTAL       32    // Number of v1720 boards in total

We will need 32/(2*2) = 8 frontends (8 indexes; from 0 to 7).  Each frontend
controls 2*2 = 4 v1720 boards.  Compile and run:

    make SIMULATION=0
    ./feoV1720.exe



\section deap UCN-3600 notes

MIDAS_SERVER_HOST should be set to deap00:7071. Otherwise frontends will require the 
option -h deap00:7071

Each frontend will only access one link. To access all boards they 
should be run four times on each computer with each of the four indexes.

\subsection deapusage Usage

- on deap01: ./bin/feoV1720.exe -i [ 0, 1, 2, 3 ]
- on deap02: ./bin/feoV1720.exe -i [ 4, 5, 6, 7 ]
- on deap03: ./bin/feoV1720.exe -i [ 8, 9, 10, 11 ]
- on deap04: ./bin/feoV1720.exe -i [ 12, 13, 14, 15 ]

\subsection deapfiles Files

- feoV1720.cxx : Main front-end user code
- v1720CONET2.hxx / v1720CONET2.cxx : Driver class for the v1720 module
  using the CAEN CONET2 (optical) interface


$Id: feov1720.cxx 128 2011-05-12 06:26:34Z alex $
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sched.h>
#include <sys/resource.h>
#include <string>
#include <cerrno>
#include <vector>
using  std::vector;

#include "midas.h"

extern "C" {
#include "CAENComm.h"
#include "ov1720drv.h"
}

#include "v1720CONET2.hxx"

// __________________________________________________________________
// --- General feov1720 parameters

#ifndef NBLINKSPERA3818
#define NBLINKSPERA3818   2   //!< Number of optical links used per A3818
#define NBLINKSPERFE      2   //!< Number of optical links controlled by each frontend
#define NB1720PERLINK     1   //!< Number of daisy-chained v1720s per optical link
#define NBV1720TOTAL      2   //!< Number of v1720 boards in total
#endif

#define  EQ_EVID   1                //!< Event ID
#define  EQ_TRGMSK 0                //!< Trigger mask

#define  FE_NAME   "feov1720I"       //!< Frontend name

#define SLEEP_TIME_BETWEEN_CONNECTS 50 // in milliseconds

#define UNUSED(x) ((void)(x)) //!< Suppress compiler warnings

// __________________________________________________________________
// --- MIDAS global variables
extern HNDLE hDB;   //!< main ODB handle
extern BOOL debug;  //!< debug printouts

/* make frontend functions callable from the C framework */
#ifdef __cplusplus
extern "C" {
#endif

/*-- Globals -------------------------------------------------------*/

//! The frontend name (client name) as seen by other MIDAS clients
char *frontend_name = (char*)FE_NAME;
//! The frontend file name, don't change it
char *frontend_file_name = (char*)__FILE__;
//! frontend_loop is called periodically if this variable is TRUE
BOOL frontend_call_loop = FALSE;
//! a frontend status page is displayed with this frequency in ms
INT display_period = 000;
//! maximum event size produced by this frontend
//INT max_event_size = 100000000;
INT max_event_size = 4194304;
//! maximum event size for fragmented events (EQ_FRAGMENTED)
INT max_event_size_frag = 200 * 1024 * 1024;
//5 * 409600;//5 * 1024 * 1024;
//! buffer size to hold events
INT event_buffer_size = 10 * max_event_size;

bool runInProgress = false; //!< run is in progress
bool runOver = false; //!< run is over
bool runStopRequested = false; //!< stop run requested


// __________________________________________________________________
/*-- MIDAS Function declarations -----------------------------------------*/
INT frontend_init();
INT frontend_exit();
INT begin_of_run(INT run_number, char *error);
INT end_of_run(INT run_number, char *error);
INT pause_run(INT run_number, char *error);
INT resume_run(INT run_number, char *error);
INT frontend_loop();
extern void interrupt_routine(void);  //!< Interrupt Service Routine

INT read_trigger_event(char *pevent, INT off);
INT read_buffer_level(char *pevent, INT off);

// __________________________________________________________________
/*-- Equipment list ------------------------------------------------*/
#undef USE_INT
//! Main structure for midas equipment
EQUIPMENT equipment[] =
{
  {
    "Li6_Detector",            /* equipment name */
    {
      EQ_EVID, EQ_TRGMSK,     /* event ID, trigger mask */
      //      //Use this to make different frontends (indexes) write
      //      //to different buffers
      "SYSTEM",               /* event buffer */
//      "SYSTEM",               /* event buffer */

# ifdef USE_INT
      EQ_INTERRUPT,           /* equipment type */
# else
      EQ_POLLED | EQ_EB,      /* equipment type */
# endif //USE_INT

      LAM_SOURCE(0, 0x0),     /* event source crate 0, all stations */
      "MIDAS",                /* format */
      TRUE,                   /* enabled */
      RO_RUNNING,             /* read only when running */
      500,                    /* poll for 500ms */
      0,                      /* stop run after this event limit */
      0,                      /* number of sub events */
      0,                      /* don't log history */
      "", "", ""
    },
    read_trigger_event,       /* readout routine */
  },

  {
    "BUFLVL%02d",                /* equipment name */
    {
      2, 0,                   /* event ID, trigger mask */
      "SYSTEM",               /* event buffer */
      EQ_PERIODIC,   /* equipment type */
      0,                      /* event source */
      "MIDAS",                /* format */
      TRUE,                   /* enabled */
      RO_RUNNING | RO_TRANSITIONS |   /* read when running and on transitions */
      RO_ODB,                 /* and update ODB */
      1000,                  /* read every 1 sec */
      0,                      /* stop run after this event limit */
      0,                      /* number of sub events */
      1,                      /* log history */
      "", "", ""
    },
    read_buffer_level,       /* readout routine */
  },

  {""}
};

#ifdef __cplusplus
}
#endif

vector<v1720CONET2> ov1720; //!< objects for the v1720 modules controlled by this frontend
vector<v1720CONET2>::iterator itv1720;  //!< iterator


/********************************************************************/
/********************************************************************/
/********************************************************************/

/**
 * \brief   Sequencer callback info
 *
 * Function which gets called when record is updated
 *
 * \param   [in]  hDB main ODB handle
 * \param   [in]  hseq Handle for key where search starts in ODB, zero for root.
 * \param   [in]  info Record descriptor additional info
 */
void seq_callback(INT hDB, INT hseq, void *info){
  KEY key;

  for (itv1720 = ov1720.begin(); itv1720 != ov1720.end(); ++itv1720) {
    if (hseq == itv1720->GetSettingsHandle()){
      db_get_key(hDB, hseq, &key);
      itv1720->SetSettingsTouched(true);
      printf("Settings %s touched. Changes will take effect at start of next run.\n", key.name);
    }
  }
}


/**
 * \brief   Frontend initialization
 *
 * Runs once at application startup.  We initialize the hardware and optical
 * interfaces and set the equipment status in ODB.  We also lock the frontend
 *  to once physical cpu core.
 *
 * \return  Midas status code
 */
INT frontend_init(){

  int feIndex = get_frontend_index();

  set_equipment_status(equipment[0].name, "Initializing...", "#FFFF00");

  // --- Suppress watchdog for PICe for now
  cm_set_watchdog_params(FALSE, 0);

  int nExpected = 0; //Number of v1720 boards we expect to activate
  int nActive = 0;   //Number of v1720 boards activated at the end of frontend_init
  std::vector<std::pair<int,int> > errBoards;  //v1720 boards which we couldn't connect to

  // If no index was supplied on the command-line, assume 1 frontend
  // to control all the links and boards
  if(feIndex == -1){
    nExpected = NB1720PERLINK*NBLINKSPERA3818;

    printf("<<< No index supplied! Assuming only 1 frontend only and starting all boards on all links\n");
    printf("    nExpected=%d  nPerLink=%d  nPer3818=%d\n",nExpected,NB1720PERLINK, NBLINKSPERA3818);
    for (int iLink=0; iLink < NBLINKSPERA3818; iLink++)
    {
      for (int iBoard=0; iBoard < NB1720PERLINK; iBoard++)
      {
        printf("<<< Begin of Init \n link:%i, board:%i\n", iLink, iBoard);

        // Compose unique module ID
        int moduleID = iLink*NB1720PERLINK + iBoard;

        // Create module objects
        ov1720.emplace_back(v1720CONET2(feIndex, iLink, iBoard, moduleID, hDB));

	ov1720.back().SetBoardRecord(hDB,seq_callback);

 #if 1 //PAA
        // Open Optical interface
        printf("Opening optical interface Link %d, Board %d\n", iLink, iBoard);
        switch(ov1720.back().Connect()){
        case v1720CONET2::ConnectSuccess:
          nActive++;
        printf("InitializeForAcq Link %d, Board %d\n", iLink, iBoard);
	  ov1720.back().InitializeForAcq();
          break;
        case v1720CONET2::ConnectErrorCaenComm:
        case v1720CONET2::ConnectErrorTimeout:
          errBoards.push_back(std::pair<int,int>(iLink,iBoard));
          break;
        case v1720CONET2::ConnectErrorAlreadyConnected:
          //do nothing
          break;
        default:
          //Can't happen
          break;
        }

        if(!((iLink == (NBLINKSPERA3818-1)) && (iBoard == (NB1720PERLINK-1)))) {
          printf("Sleeping for %d milliseconds before next board\n", SLEEP_TIME_BETWEEN_CONNECTS);
          ss_sleep(SLEEP_TIME_BETWEEN_CONNECTS);
        }
#endif

      }
    }
  } else {  //index supplied

    nExpected = NB1720PERLINK*NBLINKSPERFE;

    if((NBV1720TOTAL % (NB1720PERLINK*NBLINKSPERFE)) != 0){
      printf("Incorrect setup: the number of boards controlled by each frontend"
          " is not a fraction of the total number of boards.");
    }

    int maxIndex = (NBV1720TOTAL/NB1720PERLINK)/NBLINKSPERFE - 1;
    if(feIndex < 0 || feIndex > maxIndex){
      printf("Front end index must be between 0 and %d\n", maxIndex);
      exit(1);
    }

    int firstLink = (feIndex % (NBLINKSPERA3818 / NBLINKSPERFE)) * NBLINKSPERFE;
    int lastLink = firstLink + NBLINKSPERFE - 1;
    for (int iLink=firstLink; iLink <= lastLink; iLink++)
    {
      for (int iBoard=0; iBoard < NB1720PERLINK; iBoard++)
      {

        printf("<<< Begin of Init \n feIndex:%i, link:%i, board:%i\n",
            feIndex, iLink, iBoard);

        // Compose unique module ID
        int moduleID = feIndex*NBLINKSPERFE*NB1720PERLINK + (iLink-firstLink)*NB1720PERLINK + iBoard;

        // Create module objects
        ov1720.push_back(v1720CONET2(feIndex, iLink, iBoard, moduleID, hDB));
        ov1720.back().verbose = 1;

        // Setup ODB record (create if necessary)
        ov1720.back().SetBoardRecord(hDB,seq_callback);
        // Set history ODB record (create if necessary)
        ov1720.back().SetHistoryRecord(hDB,seq_callback);

        // Open Optical interface
        printf("Opening optical interface Link %d, Board %d\n", iLink, iBoard);
        switch(ov1720.back().Connect()){
        case v1720CONET2::ConnectSuccess:
          nActive++;
          ov1720.back().InitializeForAcq();
	  //ov1720.back().SaveSettings();
          break;
        case v1720CONET2::ConnectErrorCaenComm:
        case v1720CONET2::ConnectErrorTimeout:
          errBoards.push_back(std::pair<int,int>(iLink,iBoard));
          break;
        case v1720CONET2::ConnectErrorAlreadyConnected:
          //do nothing
          break;
        default:
          //Can't happen
          break;
        }
      }
    }
  }
  //ov1720.back().SaveSettings();
  printf(">>> End of Init. %d active v1720. Expected %d\n\n", nActive, nExpected);
   
  //ov1720.back().InitializeForAcq();
  if(nActive == nExpected){
    set_equipment_status(equipment[0].name, "Initialized", "#00ff00");
  }
  else{

    return FE_ERR_HW;
  }


  // Lock to one core on the processor. Need to be moved to TFeCommon
  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(1, &mask);  // arbitrarily chose 1
  if( sched_setaffinity(0, sizeof(mask), &mask) < 0 ){
    printf("ERROR setting cpu affinity: %s\n", strerror(errno));
  }

  return SUCCESS;
}


/**
 * \brief   Frontend exit
 *
 * Runs at frontend shutdown.  Disconnect hardware and set equipment status in ODB
 *
 * \return  Midas status code
 */
INT frontend_exit(){

  set_equipment_status(equipment[0].name, "Exiting...", "#FFFF00");

  for (itv1720 = ov1720.begin(); itv1720 != ov1720.end(); ++itv1720) {
    if (itv1720->IsConnected()){
      itv1720->Disconnect();
    }
  }
  set_equipment_status(equipment[0].name, "Exited", "#00ff00");
  return SUCCESS;
}

/**
 * \brief   Begin of Run
 *
 * Called every run start transition.  Set equipment status in ODB,
 * start acquisition on the modules.
 *
 * \param   [in]  run_number Number of the run being started
 * \param   [out] error Can be used to write a message string to midas.log
 */
INT begin_of_run(INT run_number, char *error){

  set_equipment_status(equipment[0].name, "Starting run...", "#FFFF00");

  printf("<<< Begin of begin_of_run\n");

  // Reset and Start v1720s
  for (itv1720 = ov1720.begin(); itv1720 != ov1720.end(); ++itv1720) {
    if (! itv1720->IsConnected()) continue;   // Skip unconnected board
    // Start run then wait for trigger
    itv1720->StartRun();
  }

  runInProgress = true;

  set_equipment_status(equipment[0].name, "Started run", "#00ff00");
  printf(">>> End of begin_of_run\n\n");

  return SUCCESS;
}

/**
 * \brief   End of Run
 *
 * Called every stop run transition. Set equipment status in ODB,
 * stop acquisition on the modules.
 *
 * \param   [in]  run_number Number of the run being ended
 * \param   [out] error Can be used to write a message string to midas.log
 */
INT end_of_run(INT run_number, char *error){

  DWORD eStored;
  bool result;

  set_equipment_status(equipment[0].name, "Ending run...", "#FFFF00");

  cm_msg(MINFO,"EOR", "Beginning of end_of_run");
  printf("<<< Beginning of end_of_run \n");

  // Stop run
  for (itv1720 = ov1720.begin(); itv1720 != ov1720.end(); ++itv1720) {
    if (itv1720->IsConnected()) {  // Skip unconnected board
      result = itv1720->StopRun();

      if(!result)
        cm_msg(MERROR, "feov1720:EOR",
            "Could not stop the run for module %d\n", itv1720->GetModuleID());
    }
  }

  runOver = false;
  runStopRequested = false;
  runInProgress = false;
  result = ov1720[0].Poll(&eStored);
  if(eStored != 0x0) {
    cm_msg(MERROR, "feov1720:EOR", "Events left in the v1720: %d",eStored);
  }

  printf(">>> End Of end_of_run\n\n");
  set_equipment_status(equipment[0].name, "Ended run", "#00ff00");

  return SUCCESS;

}

/**
 * \brief   Pause Run
 *
 * Called every pause run transition.
 *
 * \param   [in]  run_number Number of the run being ended
 * \param   [out] error Can be used to write a message string to midas.log
 *
 * \return  Midas status code
 */
INT pause_run(INT run_number, char *error)
{
  runInProgress = false;
  return SUCCESS;
}

/**
 * \brief   Resume Run
 *
 * Called every resume run transition.
 *
 * \param   [in]  run_number Number of the run being ended
 * \param   [out] error Can be used to write a message string to midas.log
 *
 * \return  Midas status code
 */
INT resume_run(INT run_number, char *error)
{
  runInProgress = true;
  return SUCCESS;
}


DWORD sn=0;
/**
 * \brief   Frontend loop
 *
 * If frontend_call_loop is true, this routine gets called when
 * the frontend is idle or once between every event.
 *
 * \return  Midas status code
 */
INT frontend_loop()
{
  char str[128];
  static DWORD evlimit;

  if (runStopRequested && !runOver) {
    db_set_value(hDB,0,"/logger/channels/0/Settings/Event limit", &evlimit, sizeof(evlimit), 1, TID_DWORD);
    if (cm_transition(TR_STOP, 0, str, sizeof(str), BM_NO_WAIT, FALSE) != CM_SUCCESS) {
      cm_msg(MERROR, "feov1720", "cannot stop run: %s", str);
    }
    runInProgress = false;
    runOver = true;
    cm_msg(MERROR, "feov1720","feov1720 Stop requested");
  }
  return SUCCESS;
}

/*------------------------------------------------------------------*/
/********************************************************************\
  Readout routines for different events
\********************************************************************/
int Nloop;  //!< Number of loops executed in event polling
int Ncount; //!< Loop count for event polling timeout
DWORD acqStat; //!< ACQUISITION STATUS reg, must be global because read by poll_event, accessed by read_trigger_event
// ___________________________________________________________________
/*-- Trigger event routines ----------------------------------------*/
/**
 * \brief   Polling routine for events.
 *
 * \param   [in]  source Event source (LAM/IRQ)
 * \param   [in]  count Loop count for event polling timeout
 * \param   [in]  test flag used to time the polling
 * \return  1 if event is available, 0 if done polling (no event).
 * If test equals TRUE, don't return.
 */
extern "C" INT poll_event(INT source, INT count, BOOL test) {

 
    DWORD vmeStat;
    DWORD acqStat;
    register int i;


    //printf("poll_event source=%d count=%d test=%d\n",source, count, test);
    for (i = 0; i < count; i++) {
      
      //read the vme status register and check if event ready
      bool evtReady = false;
      //bool evtReady = true;
      int j = 0;
      for (itv1720 = ov1720.begin(); itv1720 != ov1720.end(); ++itv1720){
	j++;
	itv1720->ReadReg(V1720_VME_STATUS, &vmeStat);
	
	//if (vmeStat !=0x8) printf(" vmeStat=%d\n",vmeStat);
        if((!test) && (vmeStat != 0x8)) 
          fflush(stdout);
	
        //if( !(vmeStat & 0x1) )
	//  evtReady = false;
	if( vmeStat & 0x1 ){
	  evtReady = true;
	}

      }	
      
      //If event not ready or we're in test phase, keep looping     
      if (evtReady==true && !test)
        return 1;
    }
    return 0;

}

/**
 * \brief   Interrupt configuration (not implemented)
 *
 * Routine for interrupt configuration if equipment is set in EQ_INTERRUPT
 * mode.  Not implemented right now, returns SUCCESS.
 *
 * \param   [in]  cmd Command for interrupt events (see midas.h)
 * \param   [in]  source Equipment index number
 * \param   [in]  adr Interrupt routine (see mfe.c)
 *
 * \return  Midas status code
 */
extern "C" INT interrupt_configure(INT cmd, INT source, POINTER_T adr)
{
  switch (cmd) {
  case CMD_INTERRUPT_ENABLE:
    break;
  case CMD_INTERRUPT_DISABLE:
    break;
  case CMD_INTERRUPT_ATTACH:
    break;
  case CMD_INTERRUPT_DETACH:
    break;
  }
  return SUCCESS;
}

/**
 * \brief   Event readout
 *
 * Event readout routine.  This is called by the polling or interrupt routines.
 * (see mfe.c).  For each module, read the event buffer into a midas data bank.
 * If ZLE data exists, create another bank for it.  Finally, create a statistical
 * bank for data throughput analysis.
 *
 * \param   [in]  pevent Pointer to event buffer
 * \param   [in]  off Caller info (unused here), see mfe.c
 *
 * \return  Size of the event
 */
INT read_trigger_event(char *pevent, INT off) {

  if (!runInProgress) return 0;

  sn = SERIAL_NUMBER(pevent);

  bk_init32(pevent);
  for (itv1720 = ov1720.begin(); itv1720 != ov1720.end(); ++itv1720) {
 
    if (! itv1720->IsConnected()) continue;   // Skip unconnected board
  
#if 0
    // >>> Get time before read (for data throughput analysis. To be removed)
    timeval tv;
    gettimeofday(&tv,0);
    suseconds_t usStart = tv.tv_usec;
#endif

    // Try forcing a flushing of data buffers, so we get data from all channels for each event...
    // itv1720->WriteReg(0x8040, 1);
    //usleep(100);

    // >>> Fill Event bank
    itv1720->FillEventBank(pevent);

  }

  //primitive progress bar
  if (sn % 100 == 0) printf(".");

  return bk_size(pevent);
}

// do nothing here
INT read_buffer_level(char *pevent, INT off) {

  bk_init32(pevent);
  for (itv1720 = ov1720.begin(); itv1720 != ov1720.end(); ++itv1720){
    itv1720->FillBufferLevelBank(pevent);
  }
  
  return bk_size(pevent);

  return 0;
}

