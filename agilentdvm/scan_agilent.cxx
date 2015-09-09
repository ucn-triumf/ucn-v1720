/*
 * C version of Agilent 34970A data acquisition switch
 * python version by Blair Jamieson
 * Author: Lori Rebenitsch and Blair Jamieson
 */

// include files
#include "scan_agilent.hxx"
#include <errno.h>

// config settings available on MIDAS html
const char * scan_agilent::config_str_slowcntl[] = {\
  "data buffer size = : 192",\
NULL
};

// constructor
scan_agilent::scan_agilent() {

  INT status,size;
  HNDLE hDB, hSet;
  char set_str[80];

  fEnabled = FALSE;
  fLastCheck = 0;
  fFirstRead = TRUE;
  verbose = 1;

  // initialize frontend
  status =  init();
  cm_get_experiment_database(&hDB, NULL);

  // build frontend name
  char eqname[256];
  sprintf(eqname,"DVMRead");  
  fEqName = new char[ strlen(eqname)+1 ];
  strcpy( fEqName, eqname );
  
  // check if pressure gauge is active.
  // add gauge to odb
  size = sizeof(BOOL);
  sprintf(set_str,"/Equipment/%s/Common/Enabled", fEqName);
  if (status >= 0 ){
    fEnabled = TRUE;
    status = db_set_value(hDB, 0, set_str, &fEnabled, size, 1, TID_BOOL);
    cm_msg(MINFO,fEqName,"Pressure Gauge enabled successfully.");

  } else {
    fEnabled = FALSE;
    status = db_set_value(hDB, 0, set_str, &fEnabled, size, 1, TID_BOOL);
    std::cout<<"<scan_agilent::scan_agilent> Board for Pressure Gauge Disabled."
	     <<std::endl; 
  }

  sprintf(set_str, "/Equipment/%s/Settings", fEqName);
  status = db_find_key (hDB, 0, set_str, &hSet);
  if (status != DB_SUCCESS){
    //std::cout<<"Creating key "<<set_str<<std::endl;
    db_create_key(hDB,0,set_str,TID_KEY);
    status = db_find_key (hDB, 0, set_str, &hSet);
  } 
  if (status != DB_SUCCESS){
    cm_msg(MINFO,"FE","Key %s not found", set_str);
  } else {
    //std::cout<<"Found key "<<set_str<<std::endl;
  }
  
  status = db_create_record(hDB, hSet, "", PARAM_STR_DVM);   
  if (status != DB_SUCCESS){
    std::cout<<"Create record failed status = "<<status<<std::endl;
    //    return FE_ERR_ODB;
  } else {
    //std::cout<<"Create record success"<<std::endl;
  }

  
  

  return;
} // end constructor

// destructor
scan_agilent::~scan_agilent() {

  if (fEqName!=NULL) delete fEqName;

  // close port
  close(fFD);

  return;
} // end destructor

// init
INT scan_agilent::init() {

  /* hardware initialization */
  const char *device = "/dev/ttyS0";
  struct termios port_settings;

  // open serial port 
  fFD = open(device, O_RDWR | O_NOCTTY | O_NDELAY );
  if(fFD == -1) {
    printf( "failed to open port, fFD=%d errno=%d\n", fFD, errno );
    return fFD;
  }

  // configure port
  fcntl(fFD, F_SETFL, 0);
  tcgetattr( fFD, &port_settings );

  cfsetispeed(&port_settings, B115200);    // set baud rates
  cfsetospeed(&port_settings, B115200);
  
  port_settings.c_cflag &= ~PARENB;    // set no parity, stop bits, data bits
  port_settings.c_cflag &= ~CSTOPB;
  port_settings.c_cflag &= ~CSIZE;
  port_settings.c_cflag |= CS8;

  // Enable Raw Input
  port_settings.c_cflag     &=  ~CRTSCTS;       // no flow control
  port_settings.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore cntl lines
  port_settings.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

  // Disable Software Flow control
  port_settings.c_iflag &= ~(IXON | IXOFF | IXANY);

  // Chose raw (not processed) output
  port_settings.c_oflag &= ~OPOST;
  
  // Set port attributes
  if ( tcsetattr ( fFD, TCSANOW, &port_settings ) != 0) {
    std::cout << "Error " << errno << " from tcsetattr" << std::endl;
    return -1;
  }

  fcntl(fFD, F_SETFL, 0);
  printf( "tcsetattr, fFD=%d errno=%d\n", fFD, errno );

  char command[256];
  sprintf(command,"ROUT:SCAN (@101,102,103,120)\r\n");
  printf("%s",command);
  write(fFD, command, strlen( command ) );
  sleep(2);
  sprintf(command,"FORM:READ:TIME ON\r\n");
  printf("%s",command);
  write(fFD, command, strlen( command ) );
  sprintf(command,"FORM:READ:TIME:TYPE ABS\r\n");
  printf("%s",command);
  write(fFD, command, strlen( command ) );
  sprintf(command,"FORM:READ:CHAN ON\r\n");
  printf("%s",command);
  write(fFD, command, strlen( command ) );
  sprintf(command,"FORM:READ:UNIT ON\r\n");
  printf("%s",command);
  write(fFD, command, strlen( command ) );
  sprintf(command,"TRIG:SOURCE TIMER\r\n");
  printf("%s",command);
  write(fFD, command, strlen( command ) );
  //   # Set the time between readings in seconds:
  sprintf(command,"TRIG:TIMER 60\r\n");
  printf("%s",command);
  write(fFD, command, strlen( command ) );
  sprintf(command,"TRIG:COUNT INFINITY\r\n");
  printf("%s",command);
  write(fFD, command, strlen( command ) );

  sprintf(command,"INIT\r\n");
  printf("%s",command);
  write(fFD, command, strlen( command ) );

  sprintf(command,"FETCh?\r\n");
  printf("%s",command);
  write(fFD, command, strlen( command ) );


  return SUCCESS;

} // end init

// save event
INT scan_agilent::saveEvent(char *pevent, INT off) {

  int i;
  // float *pdata = (float*;
  // char *parse;
  // char buf [1024];

  // check if gauge is enabled for reading
  if (!fEnabled){
    std::cout<<"Attempt to read disabled pressure gauge thwarted."<<std::endl;
    return 0;
  } 
  
  // update real time clock
  tnow = time (NULL);
  std::cout<<" lastcheck="<<fLastCheck
	   <<" now="<<tnow<<" dt/3600="<<float(tnow-fLastCheck)/3600.0<<std::endl;
  fLastCheck = tnow;

  // reading data from serial port
  i = read( fFD, &buf, 1000 ); // returns number of bytes read

  if(verbose) std::cout << "fFD " << fFD << " " << i << " bytes read" <<std::endl;
   
  // make sure data to be saved is complete
  if (i==192){

    buf[i+1] = NULL;

    // init bank structure
    bk_init(pevent);
    
    // create bank 
    char cbkname[5];
    sprintf(cbkname,"DVMR");
    bk_create(pevent, cbkname, TID_FLOAT, &pdata);

    // parse data
    std::cout << "fFD " << fFD << " " << i << " " << buf << std::endl;
    
    // begin parsing data
    parse = strtok(buf," ,CVD");

    // first temp reading
    fT1 = atof(parse);
    
    // parse past intermediary data 
    parse = strtok(NULL," ,CVD");  // year
    parse = strtok(NULL," ,CVD");  // month
    parse = strtok(NULL," ,CVD");  // day
    parse = strtok(NULL," ,CVD");  // hour
    parse = strtok(NULL," ,CVD");  // min
    parse = strtok(NULL," ,CVD");  // sec
    parse = strtok(NULL," ,CVD");  // channel
    
    // pressure reading
    parse = strtok(NULL," ,CVD");
    fP = atof(parse);
    
    // parse past intermediary data
    parse = strtok(NULL," ,CVD");  // year
    parse = strtok(NULL," ,CVD");  // month
    parse = strtok(NULL," ,CVD");  // day
    parse = strtok(NULL," ,CVD");  // hour
    parse = strtok(NULL," ,CVD");  // min
    parse = strtok(NULL," ,CVD");  // sec
    parse = strtok(NULL," ,CVD");  // channel
    
    // high voltage reading
    parse = strtok(NULL," ,CVD");
    fHV = atof(parse);

    // parse past intermediary data
    parse = strtok(NULL," ,CVD");  // year
    parse = strtok(NULL," ,CVD");  // month
    parse = strtok(NULL," ,CVD");  // day
    parse = strtok(NULL," ,CVD");  // hour
    parse = strtok(NULL," ,CVD");  // min
    parse = strtok(NULL," ,CVD");  // sec
    parse = strtok(NULL," ,CVD");  // channel
    
    // second temp reading
    parse = strtok(NULL," ,CVD");
    fT2 = atof(parse);
    
    if (verbose) std::cout << "T1 " << fT1 << " fP " << fP << " fT2 " << fT2 << " fHV " << fHV << std::endl;

    *pdata = float(fT1);
    pdata++;
    *pdata = float(fP);
    pdata++;
    *pdata = float(fT2);
    pdata++;
    *pdata = float(fHV);
    pdata++;

    // close bank
    bk_close(pevent, pdata);
    return bk_size(pevent);
  }
  else
    return 0;

} // end saveEvent

