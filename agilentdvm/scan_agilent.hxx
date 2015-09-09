/**
 * @file
 * @author  Lori Rebenitsch <l.rebenitsch-RA@uwinnipeg.ca
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * The class holds information about a pressure gauge.  
 * 
 */



#ifndef SCAN_AGILENT_h_
#define SCAN_AGILENT_h_

#include <iostream>
#include <fstream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "midas.h"
#include <unistd.h> 
#include <fcntl.h> 
#include <errno.h> 
#include <termios.h>

#define PARAM_STR_DVM "\
Names = STRING[4] : \n\
[32] T1degC\n\
[32] Patm\n\
[32] HVVDC\n\
[32] T2degC\n\ "

class scan_agilent {

 public:

  // constructor
  scan_agilent();
  
  // destructor
  ~scan_agilent();
  
  // Initializes the gauge in the odb
  // return SUCCESS if it works
  INT init();

  // grabs the slow control settings from the odb
  // return SUCCESS if it works
  // wnp
  //INT slowctlRecord(INT fd);

  // Return the name of this frontend
  char * GetFrontendName(){return fEqName;};

  // Saves event to file
  // return number of bytes read
  INT saveEvent(char *pevent, INT off);

  // Flag that says if the temperature board was found
  // TRUE if it is found, FALSE if it is not found
  BOOL fEnabled;

  // struct for slow control settings
  struct SLOWCNTL_CONFIG_SETTINGS{
    INT buffSize;
  } config;
  static const char *config_str_slowcntl[]; //! Configuration string for slow control

 private:

  char * fEqName;
  time_t tnow;                   // time of current check
  time_t fLastCheck;             // time of last offset check
  INT    fFD;                    // file descriptor
  BOOL   fFirstRead;
  float  fT1;
  float  fT2;
  float  fP;
  float  fHV;
  INT    fTime;
  char   buf[1024];
  char   *parse;
  float  *pdata; 

  int verbose;   

};

#endif //SCAN_AGILENT_h_
