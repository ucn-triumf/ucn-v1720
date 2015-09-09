/********************************************************************\

  Name:           scanfrontend.cxx
  Created by:     L.A.Rebenitsch

  Contents:       Frontend for manometer

  Last modified:  4/30/2014

\********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "midas.h"

#include "scan_agilent.hxx"


/* make frontend functions callable from the C framework */
#ifdef __cplusplus
extern "C" {
#endif

  
/*-- Globals -------------------------------------------------------*/
/* The frontend name (client name) as seen by other MIDAS clients   */
  char *frontend_name = (char*)"DVMRead";
/* The frontend file name, don't change it */
  char *frontend_file_name = (char*)__FILE__;

/* frontend_loop is called periodically if this variable is TRUE    */
BOOL frontend_call_loop = TRUE;

/* a frontend status page is displayed with this frequency in ms */
INT display_period = 0000;

/* maximum event size produced by this frontend */
INT max_event_size = 10000;

/* maximum event size for fragmented events (EQ_FRAGMENTED) */
INT max_event_size_frag = 5 * 1024 * 1024;

/* buffer size to hold events */
INT event_buffer_size = 100 * 10000;
  
/*-- Function declarations -----------------------------------------*/
INT frontend_init();
INT frontend_exit();
INT begin_of_run(INT run_number, char *error);
INT end_of_run(INT run_number, char *error);
INT pause_run(INT run_number, char *error);
INT resume_run(INT run_number, char *error);
INT frontend_loop();

INT read_pressure(char *pevent, INT off);

/*-- Equipment list ------------------------------------------------*/

  EQUIPMENT equipment[] = { 
 
    {"DVMRead",                 /* equipment name */
     {4, 0,  /* event ID, trigger mask */
      "SYSTEM",               /* event buffer */
      EQ_PERIODIC,            /* equipment type */
      0,                      /* event source */
      "MIDAS",                /* format */
      TRUE,                   /* enabled */
      RO_ALWAYS|RO_ODB,       /* when to read this event */
      60000,                   /* poll time in milliseconds */
      0,                      /* stop run after this event limit */
      0,                      /* number of sub events */
      60,                     /* whether to log history */
      "", "", "",}
     ,
     read_pressure,          /* readout routine */
     NULL,
     NULL,
     NULL,                    /* bank list */
    }
    ,
    {""}
  };

#ifdef __cplusplus
}
#endif
/********************************************************************\
              Callback routines for system transitions

  These routines are called whenever a system transition like start/
  stop of a run occurs. The routines are called on the following
  occations:

  frontend_init:  When the frontend program is started. This routine
                  should initialize the hardware.

  frontend_exit:  When the frontend program is shut down. Can be used
                  to releas any locked resources like memory, commu-
                  nications ports etc.

  begin_of_run:   When a new run is started. Clear scalers, open
                  rungates, etc.

  end_of_run:     Called on a request to stop a run. Can send
                  end-of-run event and close run gates.

  pause_run:      When a run is paused. Should disable trigger events.

  resume_run:     When a run is resumed. Should enable trigger events.
\********************************************************************/

/*-- Global variables ----------------------------------------------*/

static int gHaveRun        = 0;
static int gRunNumber      = 0; // from ODB

scan_agilent *gPressureGauge;

/*-- Frontend Init -------------------------------------------------*/
INT frontend_init()
{
  BOOL enabled=FALSE;
    
  gPressureGauge = new scan_agilent( );
  if (gPressureGauge->fEnabled)
    enabled = TRUE;
  
  if (enabled){
    cm_msg(MINFO,"scan_agilent.cxx","Enabled pressure gauge, now collecting pressure data.");
    return SUCCESS;
  } else {
    cm_msg(MERROR,"scan_agilent.cxx","No pressure gauge found, exiting frontend.");
    return FE_ERR_HW;
  }

  cm_enable_watchdog(0);

  return SUCCESS;
}

/*-- Frontend Exit -------------------------------------------------*/
INT frontend_exit()
{
  if ( gPressureGauge != NULL )
    delete gPressureGauge;

  gHaveRun = 0;

  return SUCCESS;
}

/*-- Begin of Run --------------------------------------------------*/
INT begin_of_run(INT run_number, char *error)
{
  gRunNumber = run_number;
  gHaveRun = 1;

  printf("begin run: %d\n", run_number);

  return SUCCESS;
}

/*-- End of Run ----------------------------------------------------*/
INT end_of_run(INT run_number, char *error)
{
  gRunNumber = run_number;

  gHaveRun = 0;
  printf("end run %d\n",run_number);

  return SUCCESS;
}

/*-- Pause Run -----------------------------------------------------*/
INT pause_run(INT run_number, char *error)
{
  gHaveRun = 0;
  gRunNumber = run_number;

  return SUCCESS;
}

/*-- Resume Run ----------------------------------------------------*/
INT resume_run(INT run_number, char *error)
{
  gHaveRun = 1;
  gRunNumber = run_number;

  return SUCCESS;
}

/*-- Frontend Loop -------------------------------------------------*/
INT frontend_loop()
{
   /* if frontend_call_loop is true, this routine gets called when
      the frontend is idle or once between every event */
  //assert(!"frontend_loop() is not implemented");
  //read_pressure();

  return SUCCESS;
}

/*------------------------------------------------------------------*/
/********************************************************************\

  Readout routines for different events

\********************************************************************/
/*-- Trigger event routines ----------------------------------------*/
extern "C" INT poll_event(INT source, INT count, BOOL test)
/* Polling routine for events. Returns TRUE if event
   is available. If test equals TRUE, don't return. The test
   flag is used to time the polling */
{
   int i;
   DWORD lam;

   for (i = 0; i < count; i++) {
     lam = 1;
      if (lam & LAM_SOURCE_STATION(source))
         if (!test)
            return lam;
   }
   return 0;
}

/*-- Interrupt configuration ---------------------------------------*/
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

/*-- equipment-specific variables ----------------------------------*/

extern HNDLE hDB;
int isInitialized = 0;

INT read_pressure(char* pevent, INT offs) {

  // Is equipment set to collect data?
  if ( gPressureGauge->fEnabled == TRUE ){
    // Is frontend set to save data to file?
    //if (gHaveRun == 1) {
    return gPressureGauge->saveEvent(pevent,offs);
    //return SUCCESS;
      //}
  } else 
    return FE_ERR_DISABLED;
  
  // no read
  //return 0;

} // end read_pressure


//end frontend
