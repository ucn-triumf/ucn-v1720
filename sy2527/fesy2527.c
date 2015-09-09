/********************************************************************\
Name: sy2527.c
Created by: Stefan Ritt
Contents: Example Slow Control Frontend program. Defines two
slow control equipments, one for a HV device and one
for a multimeter (usually a general purpose PC plug-in
card with A/D inputs/outputs. As a device driver,
the "null" driver is used which simulates a device
without accessing any hardware. The used class drivers
cd_hv and cd_multi act as a link between the ODB and
the equipment and contain some functionality like
ramping etc. To form a fully functional frontend,
the device driver "null" has to be replaces with
real device drivers.
$Id: frontend.c 2779 2005-10-19 13:14:36Z ritt $
\********************************************************************/
#include <stdio.h>
#include "midas.h"
#include "class/hv.h"
#include "device/dd_sy2527.h"
#include "bus/null.h"
/*-- Globals -------------------------------------------------------*/
/* The frontend name (client name) as seen by other MIDAS clients */
char *frontend_name = "FeSy2527-0";
/* The frontend file name, don't change it */
char *frontend_file_name = __FILE__;
/* frontend_loop is called periodically if this variable is TRUE */
BOOL frontend_call_loop = TRUE;
/* a frontend status page is displayed with this frequency in ms */
INT display_period = 000;
/* maximum event size produced by this frontend */
INT max_event_size = 10000;
/* maximum event size for fragmented events (EQ_FRAGMENTED) */
INT max_event_size_frag = 5 * 1024 * 1024;
/* buffer size to hold events */
INT event_buffer_size = 10 * 10000;
/*-- Equipment list ------------------------------------------------*/
/* device driver list */
DEVICE_DRIVER sy2527_driver[] = {
{"sy2527", dd_sy2527, 24, NULL, DF_HW_RAMP | DF_PRIO_DEVICE | DF_LABELS_FROM_DEVICE | DF_REPORT_TEMP | DF_REPORT_STATUS | DF_REPORT_CHSTATE | DF_REPORT_CRATEMAP},
{""}
};
EQUIPMENT equipment[] = {
{"HV-0", /* equipment name */
{30, 0, /* event ID, trigger mask */
"SYSTEM", /* event buffer */
EQ_SLOW, /* equipment type */
0, /* event source */
"MIDAS", /* format */
TRUE, /* enabled */
RO_ALWAYS | RO_TRANSITIONS, /* read when running and on transitions */
3000, /* read every 3 sec */
0, /* stop run after this event limit */
0, /* number of sub events */
1, /* log history every event */
"", "", ""} ,
cd_hv_read, /* readout routine */
cd_hv, /* class driver main routine */
sy2527_driver, /* device driver list */
NULL, /* init string */
},
{""}
};
/*-- Dummy routines ------------------------------------------------*/
INT poll_event(INT source[], INT count, BOOL test)
{
return 1;
};
INT interrupt_configure(INT cmd, INT source[], POINTER_T adr)
{
return 1;
};
/*-- Frontend Init -------------------------------------------------*/
INT frontend_init()
{
return CM_SUCCESS;
}
/*-- Frontend Exit -------------------------------------------------*/
INT frontend_exit()
{
return CM_SUCCESS;
}
/*-- Frontend Loop -------------------------------------------------*/
INT frontend_loop()
{
ss_sleep(100);
return CM_SUCCESS;
}
/*-- Begin of Run --------------------------------------------------*/
INT begin_of_run(INT run_number, char *error)
{
return CM_SUCCESS;
}
/*-- End of Run ----------------------------------------------------*/
INT end_of_run(INT run_number, char *error)
{
return CM_SUCCESS;
}
/*-- Pause Run -----------------------------------------------------*/
INT pause_run(INT run_number, char *error)
{
return CM_SUCCESS;
}
/*-- Resume Run ----------------------------------------------------*/
INT resume_run(INT run_number, char *error)
{
return CM_SUCCESS;
}
/*------------------------------------------------------------------*/
