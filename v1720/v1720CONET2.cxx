/*****************************************************************************/
/**
\file v1720CONET2.cxx

## Contents

This file contains the class implementation for the v1720 module driver.
 *****************************************************************************/

#include "v1720CONET2.hxx"

#define UNUSED(x) ((void)(x)) //!< Suppress compiler warnings

using namespace std;

//! Configuration string for this board. (ODB: /Equipment/[eq_name]/Settings/[board_name]/)
const char * v1720CONET2::config_str_board[] = {\
    "Acq mode = INT : 2",\
    "Software Trig Mode = INT : 0",\
    "Trig Input Mode = INT :0",\
    "Channel Mask = DWORD : 255",\
    "Post Trigger = DWORD : 80",\
    "DPP Record Length = INT : 0",\
    "Save waveforms = INT : 0", \
    "DPP TrigHoldoff = INT : 0",\
    "DPP Threshold = INT[8] :",\
    "[0] 100",\
    "[1] 100",\
    "[2] 100",\
    "[3] 100",\
    "[4] 100",\
    "[5] 100",\
    "[6] 100",\
    "[7] 100",\
    "DPP Num Sample Baseline = INT[8] :",\
    "[0] 4",\
    "[1] 4",\
    "[2] 4",\
    "[3] 4",\
    "[4] 4",\
    "[5] 4",\
    "[6] 4",\
    "[7] 4",\
    "DPP Long Gate = INT[8] :",\
    "[0] 50",\
    "[1] 50",\
    "[2] 50",\
    "[3] 50",\
    "[4] 50",\
    "[5] 50",\
    "[6] 50",\
    "[7] 50",\
    "DPP Short Gate = INT[8] :",\
    "[0] 10",\
    "[1] 10",\
    "[2] 10",\
    "[3] 10",\
    "[4] 10",\
    "[5] 10",\
    "[6] 10",\
    "[7] 10",\
    "DPP Gate Offset = INT[8] :",\
    "[0] 8",\
    "[1] 8",\
    "[2] 8",\
    "[3] 8",\
    "[4] 8",\
    "[5] 8",\
    "[6] 8",\
    "[7] 8",\
    "DPP SelfTrig= INT[8] :",\
    "[0] 1",\
    "[1] 1",\
    "[2] 1",\
    "[3] 1",\
    "[4] 1",\
    "[5] 1",\
    "[6] 1",\
    "[7] 1",\
    "DPP Trig Valid Window = INT[8] :",\
    "[0] 0",\
    "[1] 0",\
    "[2] 0",\
    "[3] 0",\
    "[4] 0",\
    "[5] 0",\
    "[6] 0",\
    "[7] 0",\
    "DPP Charge Sensitivity = INT[8] :",\
    "[0] 0",\
    "[1] 0",\
    "[2] 0",\
    "[3] 0",\
    "[4] 0",\
    "[5] 0",\
    "[6] 0",\
    "[7] 0",\
    "DC Offset = INT[8] :",\
    "[0] 32768",\
    "[1] 32768",\
    "[2] 32768",\
    "[3] 32768",\
    "[4] 32768",\
    "[5] 32768",\
    "[6] 32768",\
    "[7] 32768",\
    "Pre-Trigger Size = INT[8] :",\
    "[0] 16",\
    "[1] 16",\
    "[2] 16",\
    "[3] 16",\
    "[4] 16",\
    "[5] 16",\
    "[6] 16",\
    "[7] 16",\
    "Baseline Threshold = INT : 100",	\
    NULL
};

const char v1720CONET2::history_settings[][NAME_LENGTH] = { "eStored", "busy" };

int BaselineSetValue;                  //added May 26, 2016//the target baseline that all other channels offset themselves to match
int Base[8]={0,0,0,0,0,0,0,0};         //added May 26, 2016//the baseline of the channel before an offset is applied (used in offset eqn)
int EventCount=0;                      //added May 26, 2016//counter to display how many times the FillEventBuffer function has been called
int HadEvent_m0[8]={0,0,0,0,0,0,0,0};  //added May 27, 2016//used to determine how many events channel i of module 1 has recieved
int HadEvent_m1[8]={0,0,0,0,0,0,0,0};  //added May 27, 2016//used to determine how many events channel i of module 2 has recieved
int verb=0;
void DCOFFSETCALC(int sum1[8], int sum0[8], int moduleID, int NumEvents1[8]);

/**
 * \brief   Constructor for the module object
 *
 * Set the basic hardware parameters
 *
 * \param   [in]  feindex   Frontend index number
 * \param   [in]  link      Optical link number
 * \param   [in]  board     Board number on the optical link
 * \param   [in]  moduleID  Unique ID assigned to module
 */
v1720CONET2::v1720CONET2(int feindex, int link, int board, int moduleID, HNDLE hDB)
: _feindex(feindex), _link(link), _board(board), _moduleID(moduleID), _odb_handle(hDB)
{
  _device_handle = -1;
  _settings_handle = 0;
  verbose = 1;
  _settings_loaded=false;
  _settings_touched=false;
  _running=false;
  mZLE=false;
  mDataType = 0;
 
}
/**
 * \brief   Destructor for the module object
 *
 * Nothing to do.
 */
v1720CONET2::~v1720CONET2()
{
}

/**
 * \brief   Get short string identifying the module's index, link and board number
 *
 * \return  name string
 */
string v1720CONET2::GetName()
{
  stringstream txt;
  txt << "F" << _feindex << _link << _board;
  return txt.str();
}
/**
 * \brief   Get connected status
 *
 * \return  true if board is connected
 */
bool v1720CONET2::IsConnected()
{
  return (_device_handle >= 0);
}
/**
 * \brief   Get run status
 *
 * \return  true if run is started
 */
bool v1720CONET2::IsRunning()
{
  return _running;
}
/**
 * \brief   Connect the board through the optical link
 *
 * \return  ConnectErrorCode (see v1720CONET2.hxx)
 */
v1720CONET2::ConnectErrorCode v1720CONET2::Connect()
{
  memset(&DPPConfig.Params, 0, sizeof(DigitizerParams_t));
  memset(&DPPConfig.DPPParams, 0, sizeof(CAEN_DGTZ_DPP_PSD_Params_t));
  
  // Direct optical connection - for DPP
  DPPConfig.Params.LinkType = CAEN_DGTZ_PCI_OpticalLink;  // Link Type
  DPPConfig.Params.VMEBaseAddress = 0;                    // For direct CONET connection, 
                                                          // VMEBaseAddress must be 0
  return Connect(2, 10);                                  //reasonable default values
}

/**
 * \brief   Connect the board through the optical link
 *
 * \return  ConnectErrorCode (see v1720CONET2.hxx)
 */
v1720CONET2::ConnectErrorCode v1720CONET2::Connect(int connAttemptsMax, 
						   int secondsBeforeTimeout)
{
  if (verbose) cout << GetName() << "::Connect()\n";
  ConnectErrorCode returnCode;

  if (IsConnected()) {
    cout << "Error: trying to connect already connected board" << endl;
    returnCode = ConnectErrorAlreadyConnected;
  }


  // open the digitizer  
  CAEN_DGTZ_ErrorCode zCAEN;
  zCAEN = CAEN_DGTZ_OpenDigitizer( DPPConfig.Params.LinkType, _link, _board, 
				   DPPConfig.Params.VMEBaseAddress, 
				   &_device_handle );
  if (zCAEN) {
    _device_handle = -1;
    printf("Connect failed for link:%d board:%d\n",_link,_board);
    returnCode = ConnectErrorCaenComm;    
  } else {
    printf("Link#:%d Board#:%d Module_Handle[%d]:%d\n",
	   _link, _board, _moduleID, this->GetDeviceHandle());
    returnCode = ConnectSuccess;
  }

  // read in information about the digitizer board
  CAEN_DGTZ_BoardInfo_t           BoardInfo;
  zCAEN = CAEN_DGTZ_GetInfo(_device_handle, &BoardInfo);
  if (zCAEN) {
    printf("<v1720CONET2::Connect> Can't read board info\n");
    return ConnectErrorTimeout;
  }

  // print type of board
  printf("\n<v1720CONET2::Connect> Connected to CAEN Digitizer Model %s, recognized as board %d\n", 
	 BoardInfo.ModelName, _board);
  printf("<v1720CONET2::Connect> ROC FPGA Release is %s\n", BoardInfo.ROC_FirmwareRel);
  printf("<v1720CONET2::Connect> AMC FPGA Release is %s\n", BoardInfo.AMC_FirmwareRel);
  
  // Check firmware revision (only DPP-PSD firmware can be used)                                 
  int MajorNumber;
  sscanf(BoardInfo.AMC_FirmwareRel, "%d", &MajorNumber);
  if (MajorNumber != 131 && MajorNumber != 132 ) {
    printf("<v1720CONET2::Connect> This digitizer doesn't have a DPP-PSD firmware\n");
    return ConnectErrorCaenComm;
  }

  return returnCode;
}

/**
 * \brief   Disconnect the board through the optical link
 *
 * \return  ConnectErrorCode (see v1720CONET2.hxx)
 */
bool v1720CONET2::Disconnect()
{
  if (verbose) cout << GetName() << "::Disconnect()\n";

  // check if the board is already disconnected from frontend
  if (!IsConnected()) {
    cout << "Error: trying to disconnect already disconnected board" << endl;
    return false;
  }

  // check if the board is still taking data
  if (IsRunning()) {
    cout << "Error: trying to disconnect running board" << endl;
    return false;
  }

  if (verbose) cout << "Closing device (i,l,b) = (" << _feindex << "," 
		    << _link << "," << _board << ")" << endl;

  // close digitizer board
  CAENComm_ErrorCode sCAEN = CAENComm_ErrorCode(CAEN_DGTZ_CloseDigitizer(_device_handle));
  if(sCAEN == CAENComm_Success)
    _device_handle = -1;
  else
    return false;

  // free memory
  CAEN_DGTZ_FreeReadoutBuffer(&buffer);
  CAEN_DGTZ_FreeDPPEvents(_device_handle, (void**)Events);
  //CAEN_DGTZ_FreeDPPWaveforms(_device_handle, Waveform);

  return true;
}
/**
 * \brief   Start data acquisition
 *
 * Write to Acquisition Control reg to put board in RUN mode. If ODB
 * settings have been changed, re-initialize the board with the new settings.
 * Set _running flag true.
 *
 * \return  true for success, false for fail
 */
bool v1720CONET2::StartRun()
{
  if (verbose) cout << GetName() << "::StartRun()\n";

  // check if board is already taking data
  if (IsRunning()) {
    cout << "Error: trying to start already started board" << endl;
    return false;
  }

  // check if board is connected to frontend
  if (!IsConnected()) {
    cout << "Error: trying to start disconnected board" << endl;
    return false;
  }

  // check if the ODB settings have been changed since the previous run
  // if so, reinitialize board with new settings
  if (_settings_touched) {
    cm_msg(MINFO, "feoV1720", "Note: settings on board %s touched. Re-initializing board.",
	   GetName().c_str());
    cout << "reinitializing" << endl;
    InitializeForAcq();
  }

  // save config settings to file


  // begin data taking
  CAEN_DGTZ_ErrorCode e = CAEN_DGTZ_SWStartAcquisition(_device_handle);
  if ( e== CAEN_DGTZ_Success) {
    printf("<v1720CONET2::StartRun> Acquisition Started for Link %d Board %d\n", _link, _board);
    _running=true;
  } else {
    printf("<v1720CONET2::StartRun> Acquisition Start FAILED for Link %d Board %d  Error %d\n", _link,  _board, e);
    return false;
  }
}
/**
 * \brief   Stop data acquisition
 *
 * Write to Acquisition Control reg to put board in STOP mode.
 * Set _running flag false.
 *
 * \return  true for success, false for fail
 */
bool v1720CONET2::StopRun()
{
  if (verbose) cout << GetName() << "::StopRun()\n";

  // check if board is already stopped
  if (!IsRunning()) {
    cout << "Error: trying to stop already stopped board" << endl;
    return false;
  }

  // check if board is connected to frontend
  if (!IsConnected()) {
    cout << "Error: trying to stop disconnected board" << endl;
    return false;
  }

  // stop data taking
  CAEN_DGTZ_ErrorCode e = CAEN_DGTZ_SWStopAcquisition(_device_handle);
  if(e == CAEN_DGTZ_Success) {
    printf("<v1720CONET2::StopRun> Acquisition Successfully Stopped for Link %d, Board %d\n", _link, _board);
    _running = false;
  }
  else
    printf("<v1720CONET2::StopRun> Acquisition Stopped with Errors for Link %d, Board %d\n", _link, _board);
  return true;
}

/**
 * \brief   Setup board registers using preset (see ov1720.c:ov1720_Setup())
 *
 * Setup board registers using a preset defined in the midas file ov1720.c
 * - Mode 0x0: "Setup Skip\n"
 * - Mode 0x1: "Trigger from FP, 8ch, 1Ks, postTrigger 800\n"
 * - Mode 0x2: "Trigger from LEMO\n"
 *
 * \param   [in]  mode Configuration mode number
 * \return  CAENComm Error Code (see CAENComm.h)
 */
CAENComm_ErrorCode v1720CONET2::SetupPreset(int mode)
{
  return ov1720_Setup(_device_handle, mode);
}
/**
 * \brief   Control data acquisition
 *
 * Write to Acquisition Control reg
 *
 * \param   [in]  operation acquisition mode (see v1720.h)
 * \return  CAENComm Error Code (see CAENComm.h)
 */
CAENComm_ErrorCode v1720CONET2::AcqCtl(uint32_t operation)
{
  // read in acquisition control status into register
  uint32_t reg;
  CAEN_DGTZ_ErrorCode zCAEN;
  zCAEN = CAEN_DGTZ_ReadRegister(_device_handle, V1720_ACQUISITION_CONTROL, &reg);

  // send command to board
  switch (operation) {
  case V1720_RUN_START:               // begin data taking
    zCAEN = CAEN_DGTZ_WriteRegister(_device_handle, V1720_ACQUISITION_CONTROL, (reg | 0x4));
    break;
  case V1720_RUN_STOP:                // stop data taking
    zCAEN = CAEN_DGTZ_WriteRegister(_device_handle, V1720_ACQUISITION_CONTROL, (reg & ~( 0x4)));
    break;
  case V1720_REGISTER_RUN_MODE:       // set run mode
    zCAEN = CAEN_DGTZ_WriteRegister(_device_handle, V1720_ACQUISITION_CONTROL, 0x100);
    break;
  case V1720_SIN_RUN_MODE:            // set software trigger in
    zCAEN = CAEN_DGTZ_WriteRegister(_device_handle, V1720_ACQUISITION_CONTROL, 0x101);
    break;
  case V1720_SIN_GATE_RUN_MODE:       // set software trigger gate
    zCAEN = CAEN_DGTZ_WriteRegister(_device_handle, V1720_ACQUISITION_CONTROL, 0x102);
    break;
  case V1720_MULTI_BOARD_SYNC_MODE:   // set sync 
    zCAEN = CAEN_DGTZ_WriteRegister(_device_handle, V1720_ACQUISITION_CONTROL, 0x103);
    break;
  case V1720_COUNT_ACCEPTED_TRIGGER:  // omit pileup
    zCAEN = CAEN_DGTZ_WriteRegister(_device_handle, V1720_ACQUISITION_CONTROL, (reg & ~( 0x8)));
    break;
  case V1720_COUNT_ALL_TRIGGER:       // count all events including pileup
    zCAEN = CAEN_DGTZ_WriteRegister(_device_handle, V1720_ACQUISITION_CONTROL, (reg | 0x8));
    break;
  default:
    printf("operation %d not defined\n", operation);
    break;
  }
  return CAENComm_ErrorCode(zCAEN);
}
/**
 * \brief   Control data acquisition
 *
 * Write to Acquisition Control reg
 *
 * \param   [in]  operation acquisition mode (see v1720.h)
 * \return  CAENComm Error Code (see CAENComm.h)
 */
CAENComm_ErrorCode v1720CONET2::ChannelConfig(uint32_t operation)
{
  return ov1720_ChannelConfig(_device_handle, operation);
}

/**
 * \brief   Read 32-bit register
 *
 * \param   [in]  address  address of the register to read
 * \param   [out] val      value read from register
 * \return  CAENComm Error Code (see CAENComm.h)
 */
CAENComm_ErrorCode v1720CONET2::_ReadReg(DWORD address, DWORD *val)
{
  if (verbose >= 2) cout << GetName() << "::ReadReg(" << hex << address << ")" << endl;
  CAEN_DGTZ_ErrorCode zCAEN = CAEN_DGTZ_ReadRegister(_device_handle, address, val);
  if (zCAEN != CAEN_DGTZ_Success) 
    cout << "Error Reading register: " << zCAEN << endl;
  return CAENComm_ErrorCode(zCAEN);
}

/**
 * \brief   Write to 32-bit register
 *
 * \param   [in]  address  address of the register to write to
 * \param   [in]  val      value to write to the register
 * \return  CAENComm Error Code (see CAENComm.h)
 */
CAENComm_ErrorCode v1720CONET2::_WriteReg(DWORD address, DWORD val)
{
  if (verbose >= 2) cout << GetName() << "::WriteReg(" << hex << address << "," << val << ")" << endl;
  CAEN_DGTZ_ErrorCode zCAEN = CAEN_DGTZ_WriteRegister(_device_handle, address, val);
  if (zCAEN != CAEN_DGTZ_Success) 
    cout << "Error Writing register: " << zCAEN << endl;
  return CAENComm_ErrorCode(zCAEN);
}

bool v1720CONET2::ReadReg(DWORD address, DWORD *val)
{
  return (_ReadReg(address, val) == CAENComm_Success);
}

bool v1720CONET2::WriteReg(DWORD address, DWORD val)
{
  return (_WriteReg(address, val) == CAENComm_Success);
}

/**
 * \brief   Poll Event Stored register
 *
 * Check Event Stored register for any event stored
 *
 * \param   [out]  val     Number of events stored
 * \return  CAENComm Error Code (see CAENComm.h)
 */
bool v1720CONET2::Poll(DWORD *val)
{
  CAENComm_ErrorCode sCAEN = CAENComm_ErrorCode(CAEN_DGTZ_ReadRegister(_device_handle, V1720_EVENT_STORED, val));
  return (sCAEN == CAENComm_Success);
}

//! Maximum size of data to read using BLT (32-bit) cycle
#define MAX_BLT_READ_SIZE_BYTES 10000
/**
 * \brief   Read event buffer and create Midas bank
 *
 * Read the event buffer for this module using BLT (32-bit) cycles.
 * This function reads nothing if EVENT_SIZE register was zero.
 *
 * \param   [out]  data         Where to write content of event buffer
 * \param   [out]  dwords_read  Number of DWORDs read from the buffer
 * \return  CAENComm Error Code (see CAENComm.h)
 */
bool v1720CONET2::FillEventBank(char * pevent)
{
  //printf("* ");
  // check if board is connected
  if (! this->IsConnected()) {
    cout << "Error: trying to ReadEvent disconnected board" << endl;
    return false;
  }
  
  // Double checking that board is ready to read
  //DWORD vmeStat;
  //this->ReadReg(V1720_VME_STATUS, &vmeStat);
  //if(!(vmeStat & 0x1))
  // return true;

  // grab data stream
  DWORD *pdata = (DWORD *)pevent;
  int ch,ret=0;
  int curev=0;
  char bankName[5];
  DWORD NTotal =0;
  uint32_t NumEvents[MaxNChannels];
  
   int sum0[8]={0,0,0,0,0,0,0,0};//added May 26, 2016
   int sum1[8]={0,0,0,0,0,0,0,0};//added May 30, 2016 //may not need
 

  // read in data
  ret = CAEN_DGTZ_ReadData(_device_handle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer, &BufferSize);
  //ret = CAEN_DGTZ_ReadData(_device_handle, CAEN_DGTZ_POLLING_2eSST, buffer, &BufferSize);
  if (ret) {
    printf("<v1720CONET::FillEventBank> Readout Error %d\n",ret);
    exit(0);
    //return false;    
  }
  
  if ( BufferSize == 0 )
    return true;  
  
  // read in DPP data
  ret = CAEN_DGTZ_GetDPPEvents(_device_handle, buffer, BufferSize, (void**)Events, NumEvents);
  if (ret) {
    //printf("<v1720CONET2::FillEventBank> Data Error: %d\n", ret);
    return false;
  }
  
  // find the total number of events for this read
  for(ch=0; ch<MaxNChannels; ch++) {
    if (!(DPPConfig.Params.ChannelMask & (1<<ch)))
      continue;
    NTotal+=NumEvents[ch];
  }
 

  // >>> create data bank
  sprintf(bankName, "W2%02d", this->_moduleID);
  bk_create(pevent, bankName, TID_DWORD, (void**)&pdata);
  *pdata++ = NTotal;
  
  for(ch=0; ch<MaxNChannels; ch++) {
    if (!(DPPConfig.Params.ChannelMask & (1<<ch)))
      continue;
    
    // save data to event format
    for(ev=0; ev<NumEvents[ch]; ev++) {
      
      /* Channel */
      out.Channel = ch;
      /* Time Tag */
      out.TimeTag = Events[ch][ev].TimeTag+Events[ch][ev].Extras;
      /* Energy */
      out.ChargeShort= Events[ch][ev].ChargeShort;
      out.ChargeLong = Events[ch][ev].ChargeLong;
      out.Baseline =  Events[ch][ev].Baseline;

      if (this->_moduleID == 0) sum0[ch]=sum0[ch]+out.Baseline;///////added May 26, 2016
      if (this->_moduleID == 1) sum1[ch]=sum1[ch]+out.Baseline;
	
      //std::cout<<"Channel: "<<ch<<"Event: "<<ev<<"Baseline: "<<Events[ch][ev].Baseline<<endl;
      out.Pur = Events[ch][ev].Pur;
	
      Event = &Events[ch][ev];
      
      
      if (DPPConfig.Params.SaveWaveforms==0){
	out.Length = 0;
	// save output data to the pointer before saving to the bank
	memcpy( pdata, (void*)&out, sizeof( out ) ); 
	pdata += sizeof(out)/sizeof(DWORD);

      } else {
	// grab the waveform used for DPP calculations
	if(CAEN_DGTZ_DecodeDPPWaveforms(_device_handle, (void**)Event,Waveform)){
	  printf("Event Error: cannot find DPPWaveforms\n");
	  return -1;
	}
	out.Length = Waveform->Ns;
	
	// save output data to the pointer before saving to the bank
	memcpy( pdata, (void*)&out, sizeof( out ) ); 
	pdata += sizeof(out)/sizeof(DWORD);

   
	// save waveform to the pointer before saving to the bank
	// copy from the first element of the waveform in memory 
	// to the full length of the array
	memcpy( pdata, (void*)&Waveform->Trace1[0], out.Length * sizeof( uint16_t ) ); 
	pdata += out.Length*sizeof( uint16_t )/sizeof(DWORD);

      }
      // clear waveform for next event
      curev++;	   
    }
  }
  int NumEvents1[8]={0,0,0,0,0,0,0,0};
  for (ch=0; ch<8; ch++)
    {
      NumEvents1[ch]=(int)(NumEvents[ch]);
    }
    
  //DCOFFSETCALC(sum1, sum0, this->_moduleID, NumEvents1);
  /* if (this->_moduleID==0)////////////////////////////added May 27, 2016
    {
      int returnval;
      if (verb ==1)
	{
	  std::cout<<"ModuleID: "<<this->_moduleID<<endl;
	  std::cout<<"EventCount: "<<EventCount<<endl;
	}
   for (ch=0;ch<MaxNChannels; ch++)////////added May 26, 2016
    {
      // std::cout<<"NumEvents: "<<NumEvents[ch]<<endl;
      if (NumEvents[ch]!=0) HadEvent_m0[ch]++;
      if (verb ==1) if (NumEvents[ch] != 0) std::cout<<"Base: "<<sum0[ch]/NumEvents[ch]<<endl;
      if(NumEvents[ch]!=0 && HadEvent_m0[ch]==1)
	{
	  Base[ch]=(int)(sum0[ch]/NumEvents[ch]);//gets the average basline of the channel
	                                        //and sets that as Base[ch]
	  if (verb ==1) std::cout<<"Channel: "<<ch<<" Base: "<<Base[ch]<<endl;
	}
    else if (NumEvents[ch]!=0 && abs(BaselineSetValue - ((int)(sum0[ch]/NumEvents[ch]))) >= 20)
	{
	  std::cout<<"Channel "<<ch<<" of board "<<this->_moduleID<<" was weird"<<endl;
	  Base[ch]=(int)((sum0[ch]/NumEvents[ch]));//gets the average basline of the channel
	                                         //and sets that as Base[ch]
	  if (verb ==1) std::cout<<"Channel: "<<ch<<" Base: "<<Base[ch]<<endl;
	   config.DCoffset[ch] =(int)(13.37*(Base[ch]-BaselineSetValue)+32767.5);
	   returnval |= CAEN_DGTZ_SetChannelDCOffset(_device_handle, ch, config.DCoffset[ch]);
	    
	}
      if (ch==0 && HadEvent_m0[0]==1)//sets the target baseline for all offsets to adjust to
	{
	      BaselineSetValue = Base[ch];
	      if (verb==1)
		{
		  std::cout<<"BaselineSetValue: "<<BaselineSetValue<<endl;
		  std::cout<<"Module ID: "<<this->_moduleID<<endl;
		}
	}
    }
    int z=0;
    for (z=0; z<MaxNChannels; z++)//////////added May 26, 2016
    {
      if (Base[z]!=0 && HadEvent_m0[z]==2)
	{ 
	  config.DCoffset[z] =(int)(13.37*(Base[z]-BaselineSetValue)+32767.5);//derived equation for calculating DC Offset
	  returnval |= CAEN_DGTZ_SetChannelDCOffset(_device_handle, z, config.DCoffset[z]);//this is where the DC Offset is actually changed
	}
      if (verb ==1) std::cout<<"Channel: "<<z<<" DC Offset: "<<config.DCoffset[z]<<endl;
    }
    }

  if (this->_moduleID==1)//added May 27, 2016 //have separate if statements for each module allows the offsets for one module to change
                                              //while on the other module they stay the same
    {
 int returnval;
 if (verb ==1)
   {
     std::cout<<"ModuleID: "<<this->_moduleID<<endl;
     std::cout<<"EventCount: "<<EventCount<<endl;
   }
   for (ch=0;ch<MaxNChannels; ch++)////////added May 26, 2016
    {
      // std::cout<<"NumEvents: "<<NumEvents[ch]<<endl;
      if (NumEvents[ch]!=0) HadEvent_m1[ch]++;
      if (verb ==1)
	{
	  if (NumEvents[ch]!=0) std::cout<<"BaselineSetValue: "<<BaselineSetValue<<endl;
	  if (NumEvents[ch] != 0) std::cout<<"Base: "<<sum1[ch]/NumEvents[ch]<<endl;
	}
      if((NumEvents[ch]!=0 && HadEvent_m1[ch]==1))
	{
	  Base[ch]=(int)((sum1[ch]/NumEvents[ch]));//gets the average basline of the channel
	                                         //and sets that as Base[ch]
	  if (verb ==1)  std::cout<<"Channel: "<<ch<<" Base: "<<Base[ch]<<endl;
	}
      else if (NumEvents[ch]!=0 && abs(BaselineSetValue - ((int)(sum1[ch]/NumEvents[ch]))) >= 20)
	{
	  Base[ch]=(int)((sum1[ch]/NumEvents[ch]));//gets the average basline of the channel
	                                         //and sets that as Base[ch]
	  if (verb ==1) std::cout<<"Channel: "<<ch<<" Base: "<<Base[ch]<<endl;
	   config.DCoffset[ch] =(int)(13.37*(Base[ch]-BaselineSetValue)+32767.5);
	   returnval |= CAEN_DGTZ_SetChannelDCOffset(_device_handle, ch, config.DCoffset[ch]);
	    
	}
    }

    int z=0;
    for (z=0; z<MaxNChannels; z++)//////////added May 26, 2016
    {
      if ((Base[z]!=0 && HadEvent_m1[z]==1))
	{ 
	  config.DCoffset[z] =(int)(13.37*(Base[z]-BaselineSetValue)+32767.5);
	  returnval |= CAEN_DGTZ_SetChannelDCOffset(_device_handle, z, config.DCoffset[z]);
	}
      if (verb ==1) std::cout<<"Channel: "<<z<<" DC Offset: "<<config.DCoffset[z]<<endl;
    }
    }*/
  bk_close(pevent,pdata);
  /*  EventCount++;/////////added May 26, 2016
  if(verb ==1)
    {
      if (this->_moduleID==0)
	{
	  for (ch=0; ch<8; ch++)
	    {
	      std::cout<<"had event "<<ch<<": "<<HadEvent_m0[ch]<<endl;
	    }
	}
      if (this->_moduleID==1)
	{
	  for (ch=0; ch<8; ch++)
	    {
	      std::cout<<"had event "<<ch<<": "<<HadEvent_m1[ch]<<endl;
	    }
	}
	}*/
  return true;
}

/**
 * \brief   Send a software trigger to the board
 *
 * Send a software trigger to the board.  May require
 * software triggers to be enabled in register 0x810C.
 *
 * \return  CAENComm Error Code (see CAENComm.h)
 */
bool v1720CONET2::SendTrigger()
{
  if (verbose) cout << GetName() << "::SendTrigger()" << endl;
  if (!IsConnected()) {
    cout << "Error: trying to SendTrigger disconnected board" << endl;
    return false;
  }

  if (verbose) cout << "Sending Trigger (l,b) = (" << _link << "," << _board << ")" << endl;

  return (WriteReg(V1720_SW_TRIGGER, 0x1) == CAENComm_Success);
}
/**
 * \brief   Set the ODB record for this board
 *
 * Create a record for the board with settings from the configuration
 * string (v1720CONET2::config_str_board) if it doesn't exist or merge with
 * existing record. Create hotlink with callback function for when the
 * record is updated.  Get the handle to the record.
 *
 * Ex: For a frontend with index number 2 and board number 0, this
 * record will be created/merged:
 *
 * /Equipment/FEV1720I2/Settings/Module0
 *
 * \param   [in]  h        main ODB handle
 * \param   [in]  cb_func  Callback function to call when record is updated
 * \return  ODB Error Code (see midas.h)
 */
int v1720CONET2::SetBoardRecord(HNDLE h, void(*cb_func)(INT,INT,void*))
{
  char set_str[500];

  // if _feindex == -1, no index supplied; assume 1 frontend for all boards
  // else set up the number of modules with the _feindex supplies
  if(_feindex == -1)
    sprintf(set_str, "/Equipment/FEV1720I/Settings/Module%d", _moduleID);
  else
    sprintf(set_str, "/Equipment/FEV1720I%02d/Settings/Module%d", _feindex, _moduleID);

  if (verbose) cout << GetName() << "::SetBoardRecord(" << h << "," << set_str << ",...)" << endl;
  int status,size;

  


  //create record if doesn't exist and find key
  status = db_find_key(h, 0, set_str, &_settings_handle);
  if ( status != DB_SUCCESS ){
    status = db_create_record(h, 0, set_str, strcomb(config_str_board));
    if (verbose) cout<<"    "<<set_str<<endl;
    status = db_find_key(h, 0, set_str, &_settings_handle);
  }
  if (status != DB_SUCCESS) cm_msg(MINFO,"FE","Key %s not found", set_str);

  //hotlink
  size = sizeof(V1720_CONFIG_SETTINGS);
  printf("size: %d\n", size);
  status = db_open_record(h, _settings_handle, &config, size, MODE_READ, cb_func, NULL);

  //get actual record
  status = db_get_record(h, _settings_handle, &config, &size, 0);
  //printf("get config %d\n",status);
  if (status == DB_SUCCESS) 
    _settings_loaded = true;
  _settings_touched = true;

  return status; //== DB_SUCCESS for success
}
/**
 * \brief   Set the ODB record for history variable names
 *
 * \param   [in]  h        main ODB handle
 * \param   [in]  cb_func  Callback function to call when record is updated
 * \return  ODB Error Code (see midas.h)
 */
int v1720CONET2::SetHistoryRecord(HNDLE h, void(*cb_func)(INT,INT,void*))
{
  char settings_path[200] = "/Equipment/BUFLVL/Settings/";

  if(_feindex == -1)
    sprintf(settings_path, "/Equipment/BUFLVL/Settings/");
  else
    sprintf(settings_path, "/Equipment/BUFLVL%02d/Settings/", _feindex);

  if (verbose) cout << GetName() << "::SetHistoryRecord(" << h << "," << settings_path << ",...)" << endl;
  int status;

  HNDLE settings_key;
  status = db_find_key(h, 0, settings_path, &settings_key);

  if(status == DB_NO_KEY){
    db_create_key(h, 0, settings_path, TID_KEY);
    db_find_key(h, 0, settings_path, &settings_key);
  }

  char tmp[11];
  sprintf(tmp, "Names BL%02d", this->_moduleID);

  char names_path[100];
  strcpy(names_path, settings_path);
  strcat(names_path, tmp);

  db_create_key(h, 0, names_path, TID_STRING);
  HNDLE path_key;
  status = db_find_key(h, 0, names_path, &path_key);

  db_set_data(h, path_key, history_settings, sizeof(history_settings),
      sizeof(history_settings)/NAME_LENGTH, TID_STRING);

  if (status != DB_SUCCESS) cm_msg(MINFO,"SetHistoryRecord","Key %s not found", names_path);
  return status;
}
/**
 * \brief   Initialize the hardware for data acquisition
 *
 * ### Initial setup:
 * - Set FP I/O Ctrl (0x811C) to default settings (output trigger).
 * - Do software reset + clear.
 * - Set up busy daisy chaining
 * - Put acquisition on stop.
 *
 * ### Checks
 * - AMC firmware version (and check that each channel has the same AMC firmware)
 * - ROC firmware version
 * - board type
 *
 * ### Set registers
 * Use a preset if setup != 0 in the config string, or set them manually otherwise.
 *
 * \return  0 on success, -1 on error
 */
int v1720CONET2::InitializeForAcq()
{
  CAEN_DGTZ_ErrorCode ret;
  
  uint32_t reg, reg2;
  ret = CAEN_DGTZ_ReadRegister(_device_handle, 0x8000, &reg);
  ret = CAEN_DGTZ_WriteRegister(_device_handle, 0x8000, (reg | (1 << 17)));
  ret = CAEN_DGTZ_ReadRegister(_device_handle, 0x8080, &reg2);
  ret = CAEN_DGTZ_WriteRegister(_device_handle, 0x8080, (reg2 | (1<<7)));


  if (verbose) cout << GetName() << "::InitializeForAcq()" << endl;

  // check if the ODB settings are loaded
  if (!_settings_loaded) {
    cout << "Error: cannot call InitializeForAcq() without settings loaded properly" << endl;
    return -1;    }

  // check if the board is connected to the frontend
  if (!IsConnected())     {
    cout << "Error: trying to call InitializeForAcq() to unconnected board" << endl;
    return -1;    }

  // check if the board is already taking data
  if (IsRunning())    {
    cout << "Error: trying to call InitializeForAcq() to already running board" << endl;
    return -1;    }


  cm_msg(MINFO,"feoV1720","### In InitializeForAcq, _settings_loaded: %d, _settings_touched: %d",
	 _settings_loaded, _settings_touched);

  // don't do anything if settings haven't been changed
  if (_settings_loaded && !_settings_touched) {
    return 0;
  }

  /****************************\
  *      DPP parameters        *
  \****************************/
  DPPConfig.Params.IOlev = CAEN_DGTZ_IOLevel_NIM;
  
  // Direct optical connection
  DPPConfig.Params.LinkType = CAEN_DGTZ_PCI_OpticalLink;   // Link Type
  DPPConfig.Params.VMEBaseAddress = 0;                     // For direct CONET connection, VMEBaseAddress must be 0
  DPPConfig.Params.AcqMode = (CAEN_DGTZ_DPP_AcqMode_t)config.acq_mode;              // 0== CAEN_DGTZ_DPP_ACQ_MODE_Oscilloscope
                                                           // 1==CAEN_DGTZ_DPP_ACQ_MODE_List or 
                                                           // 2== CAEN_DGTZ_DPP_ACQ_MODE_Mixed; 
                                                           // Warning: List mode doesn't save baseline info
  DPPConfig.Params.RecordLength = config.recordLen;        // Num of samples of the waveforms (only for 
                                                           // Oscilloscope mode)
  DPPConfig.Params.SaveWaveforms = config.savewaveforms;
  DPPConfig.Params.ChannelMask = config.channel_mask;      // Channel enable mask
  DPPConfig.Params.EventAggr = 255;//0;// 255;// 512;//1024;//0;//1024;                          // number of events in one aggregate (0=automatic)
  DPPConfig.Params.PulsePolarity = CAEN_DGTZ_PulsePolarityNegative; // Pulse Polarity (this parameter can be 
                                                                    // individual)
  for(int ch=0; ch<8; ch++) {
    DPPConfig.DPPParams.thr[ch] = config.DPPthresh[ch];    // Trigger Threshold
    /* The following parameter is used to specifiy the number of samples for the baseline averaging:
       0 -> absolute Bl
       1 -> 4samp
       2 -> 8samp
       3 -> 16samp
       4 -> 32samp
       5 -> 64samp
       6 -> 128samp */
    DPPConfig.DPPParams.nsbl[ch]  = config.DPPnsbl[ch];   
    DPPConfig.DPPParams.lgate[ch] = config.DPPLongGate[ch];    // Long Gate Width (N*4ns)
    DPPConfig.DPPParams.sgate[ch] = config.DPPShortGate[ch];   // Short Gate Width (N*4ns)
    DPPConfig.DPPParams.pgate[ch] = config.DPPPregateWidth[ch];// Pre Gate Width (N*4ns) also known as gate offset
    /* Self Trigger Mode:
       0 -> Disabled
       1 -> Enabled
    */
    DPPConfig.DPPParams.selft[ch] = config.DPPSelfTrig[ch];
    // Trigger configuration:
    // CAEN_DGTZ_DPP_TriggerConfig_Peak       -> trigger on peak. NOTE: Only for FW <= 13X.5
    // CAEN_DGTZ_DPP_TriggerConfig_Threshold  -> trigger on threshold */
    DPPConfig.DPPParams.trgc[ch] = CAEN_DGTZ_DPP_TriggerConfig_Threshold;
    /* Trigger Validation Acquisition Window */
    DPPConfig.DPPParams.tvaw[ch] = config.DPPtvw[ch];
    /* Charge sensitivity: 0->40fc/LSB; 1->160fc/LSB; 2->640fc/LSB; 3->2,5pc/LSB */
    DPPConfig.DPPParams.csens[ch] = config.DPPChargeSen[ch];

    //DPPConfig.DPPParams.dcoffset[ch] = config.DCoffset[ch];
    
  }
  /* Pile-Up rejection Mode
     CAEN_DGTZ_DPP_PSD_PUR_DetectOnly -> Only Detect Pile-Up
     CAEN_DGTZ_DPP_PSD_PUR_Enabled -> Reject Pile-Up */
  // purity not yet usable
  DPPConfig.DPPParams.blthr = config.BLthr;
  DPPConfig.DPPParams.purh = CAEN_DGTZ_DPP_PSD_PUR_DetectOnly;
  DPPConfig.DPPParams.purgap = 100;    // Purity Gap  
  DPPConfig.DPPParams.bltmo = 100;     // Baseline Timeout
  DPPConfig.DPPParams.trgho = config.DPPTrigHoldoff;      // Trigger HoldOff 
  // end setting DPP Parameters
 
  // reset digitizer
  ret = CAEN_DGTZ_Reset(_device_handle);
  ret = CAEN_DGTZ_SetDPPAcquisitionMode(_device_handle, DPPConfig.Params.AcqMode, 
					CAEN_DGTZ_DPP_SAVE_PARAM_EnergyAndTime);
  if (ret != CAEN_DGTZ_Success) {
    printf("<v1720CONET2::InitializeForAcq> Error in CAEN_DGTZ_SetDPPAcquisitionMode %d\n",ret);
    printf("%d\n",_device_handle);
    return (int)ret;
  }

  // Set the digitizer acquisition mode (CAEN_DGTZ_SW_CONTROLLED or CAEN_DGTZ_S_IN_CONTROLLED)
  ret = CAEN_DGTZ_SetAcquisitionMode(_device_handle, CAEN_DGTZ_SW_CONTROLLED);
  if(ret) {
    printf("Error setting Acquisition Mode\n");
    return (int)ret;
  }

  // Set the I/O level (CAEN_DGTZ_IOLevel_NIM or CAEN_DGTZ_IOLevel_TTL)
  ret = CAEN_DGTZ_SetIOLevel(_device_handle, DPPConfig.Params.IOlev);
  if(ret) {
    printf("Error setting IOLevel %d\n",ret);
    return (int)ret;
  }

  // Set the digitizer's behaviour when an external trigger arrives:
  // 0==CAEN_DGTZ_TRGMODE_DISABLED: do nothing
  // 2==CAEN_DGTZ_TRGMODE_EXTOUT_ONLY: generate the Trigger Output signal
  // 1==CAEN_DGTZ_TRGMODE_ACQ_ONLY = generate acquisition trigger X
  // 3==CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT = generate both Trigger Output and acquisition trigger
  // see CAENDigitizer user manual, chapter "Trigger configuration" for details 
  ret = CAEN_DGTZ_SetExtTriggerInputMode(_device_handle, (CAEN_DGTZ_TriggerMode_t)config.trig_input_mode);
  if (ret != CAEN_DGTZ_Success) {
    printf("<v1720CONET2::InitializeForAcq> Error in CAEN_DGTZ_SetExtTriggerInputMode %d\n",ret);
    return (int)ret;
  }
  
  ret = CAEN_DGTZ_SetSWTriggerMode(_device_handle, (CAEN_DGTZ_TriggerMode_t)config.sw_trig_mode );
  if (ret != CAEN_DGTZ_Success) {
    printf("<v1720CONET2::InitializeForAcq> Error in CAEN_DGTZ_SetSWTriggerMode %d\n",ret);
    return (int)ret;
  }

  // Set how many events to accumulate in the board memory before being available for readout
  // DPPConfig.Params.EventAggr==0 - let the board decide
  if ( config.sw_trig_mode != 0 || 1 ){
    ret = CAEN_DGTZ_SetDPPEventAggregation(_device_handle, DPPConfig.Params.EventAggr, 0);
    //   re = CAEN_DGTZ_SetDPPEventAggregation(_device_handle, DPPConfig.Params.EventAggr, 512);
    if (ret != CAEN_DGTZ_Success) {
      printf("<v1720CONET2::InitializeForAcq> Error in CAEN_DGTZ_SetDPPEventAggregation %d\n",ret);
      return (int)ret;
    }
  }

  int chmask=1;
  for (int ich=0; ich<8; ich++){
    ret = CAEN_DGTZ_SetChannelSelfTrigger(_device_handle, (CAEN_DGTZ_TriggerMode_t)config.DPPSelfTrig[ich], chmask);
    if (ret != CAEN_DGTZ_Success) {
      printf("<v1720CONET2::InitializeForAcq> Error in CAEN_DGTZ_SetChannelSelfTrigger ch=%d mask=%d selftrig=%d retval=\n",ich, chmask, config.DPPSelfTrig[ich], ret);
      return (int)ret;
    }
    chmask <<= 1;
  }

  // Set the enabled channels
  ret = CAEN_DGTZ_SetChannelEnableMask(_device_handle, DPPConfig.Params.ChannelMask);  
  if (ret != CAEN_DGTZ_Success) {
    printf("<v1720CONET2::InitializeForAcq> Error in CAEN_DGTZ_SetChannelEnableMask %d\n",ret);
    return (int)ret;
  }

  // Set post trigger fraction (as a percentage)
  ret = CAEN_DGTZ_SetPostTriggerSize(_device_handle, config.post_trigger);
  if (ret != CAEN_DGTZ_Success) {
    printf("<v1720CONET2::InitializeForAcq> Error in CAEN_DGTZ_SetPostTriggerSize %d\n",ret);
    return (int)ret;
  }
    
  // Set the mode used to syncronize the acquisition between different boards.
  // In this example the sync is disabled 
  // CAEN_DGTZ_RUN_SYNC_Disabled - only one board; no need to syncronize
  ret = CAEN_DGTZ_SetRunSynchronizationMode(_device_handle, CAEN_DGTZ_RUN_SYNC_Disabled);
    
  // Set the DPP specific parameters for the channels in the given channelMask
  // Note that this sets all of the settings in DPPConfig.DPPParams!
  ret = CAEN_DGTZ_SetDPPParameters(_device_handle, config.channel_mask, &DPPConfig.DPPParams);
  if (ret != CAEN_DGTZ_Success) {
    printf("<v1720CONET2::InitializeForAcq> Error in CAEN_DGTZ_SetDPPParameters %d\n",ret);
    return (int)ret;
  }


  // Set DPP parameters for individual channels
  int retval=0;
  for(int i=0; i<8; i++) {

    if (DPPConfig.Params.ChannelMask & (1<<i)) {

      // Set the number of samples for each waveform (you can set different RL for different channels)             
      retval |= CAEN_DGTZ_SetRecordLength(_device_handle, DPPConfig.Params.RecordLength, i);
      
      // Set a DC offset to the input signal to adapt it to digitizer's dynamic range                              
      retval |= CAEN_DGTZ_SetChannelDCOffset(_device_handle, i, config.DCoffset[i]);
      
      // Set the Pre-Trigger size (in samples)                                                                    
      retval |= CAEN_DGTZ_SetDPPPreTriggerSize(_device_handle, i, config.PreTriggerSize[i]); 
      if(retval)
	printf("<v1720CONET2::InitializeForAcq> Error setting DPPPreTriggerSize %d\n",retval);
      
      // Set the polarity for the given channel (CAEN_DGTZ_PulsePolarityPositive or 
      // CAEN_DGTZ_PulsePolarityNegative)
      retval |= CAEN_DGTZ_SetChannelPulsePolarity(_device_handle, i, DPPConfig.Params.PulsePolarity);
      if(retval)
	printf("<v1720CONET2::InitializeForAcq> Error setting ChannelPulsePolarity %d\n",retval);
    }
  } // end for
  
  
  // int adefaultdcoffset = 0x8000; 

  // retval |= CAEN_DGTZ_SetChannelDCOffset(_device_handle, 0, adefaultdcoffset + 16*37  );
  // retval |= CAEN_DGTZ_SetChannelDCOffset(_device_handle, 1, adefaultdcoffset + 16*55  );
  // retval |= CAEN_DGTZ_SetChannelDCOffset(_device_handle, 2, adefaultdcoffset +16*(-22)  );
  // retval |= CAEN_DGTZ_SetChannelDCOffset(_device_handle, 3, adefaultdcoffset + 16*6  );
  // retval |= CAEN_DGTZ_SetChannelDCOffset(_device_handle, 4, adefaultdcoffset + 16*(-1)  );
  // retval |= CAEN_DGTZ_SetChannelDCOffset(_device_handle, 5, adefaultdcoffset + 16*15  );
  // retval |= CAEN_DGTZ_SetChannelDCOffset(_device_handle, 6, adefaultdcoffset + 16*23  );
  // retval |= CAEN_DGTZ_SetChannelDCOffset(_device_handle, 7, adefaultdcoffset + 16*(-1)  );



  // Set the virtual probes
  // DPP-PSD for x720 boards can save:
  // 2 analog waveforms:
  //     Analog Trace 1: it is always the input signal;
  //     Analog Trace 2: it can be specified with the VIRTUALPROBE parameter
  // 4 digital waveforms:
  //     Digital Trace 1:   it is always the trigger
  //     Digital Trace 2:   it is always the long gate
  //     Digital Trace 3/4: they can be specified with the DIGITALPROBE 1 and 2 parameters
  // CAEN_DGTZ_DPP_VIRTUALPROBE_SINGLE    -> Save only the Input Signal waveform
  // CAEN_DGTZ_DPP_VIRTUALPROBE_DUAL      -> Save also the waveform specified in VIRTUALPROBE
  // Virtual Probes types for Trace 2:
  //     CAEN_DGTZ_DPP_PSD_VIRTUALPROBE_Baseline         -> Save the Baseline waveform (mean on nsbl parameter)
  //   ### Virtual Probes only for FW <= 13X.5 ###
  //     CAEN_DGTZ_DPP_PSD_VIRTUALPROBE_Threshold        -> Save the (Baseline - Threshold) waveform. NOTE: x720 only
  // Digital Probes types for Digital Trace 3(x720)/2(x751):
  //     CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_R6_ExtTrg       NOTE: x720 only
  //     CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_R6_OverThr
  //     CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_R6_TrigOut
  //     CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_R6_CoincWin
  //     CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_R6_PileUp
  //     CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_R6_Coincidence
  // Digital Probes types for Digital Trace 4(x720)/3(x751):
  //     CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_R6_GateShort
  //     CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_R6_OverThr
  //     CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_R6_TrgVal
  //     CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_R6_TrgHO
  //     CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_R6_PileUp
  //     CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_R6_Coincidence
  ret = CAEN_DGTZ_SetDPP_PSD_VirtualProbe(_device_handle, CAEN_DGTZ_DPP_VIRTUALPROBE_SINGLE, 
   					  CAEN_DGTZ_DPP_PSD_VIRTUALPROBE_Baseline, 
   					  CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_R6_ExtTrg,
   					  CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_R6_OverThr);
  if (ret != CAEN_DGTZ_Success) {
    printf("<v1720CONET2::InitializeForAcq> Error in CAEN_DGTZ_SetDPP_PSD_VirtualProbe %d\n",ret);
    return (int)retval;
  }




  /* WARNING: The mallocs MUST be done after the digitizer programming,
     because the following functions needs to know the digitizer configuration
     to allocate the right memory amount */
  /* Allocate memory for the readout buffer */
  uint32_t AllocatedSize = 0;
  buffer = NULL;
  retval = CAEN_DGTZ_MallocReadoutBuffer(_device_handle, &buffer, &AllocatedSize);
  printf("<v1720CONET2::InitializeForAcq> readout buffer malloced size=%d\n",AllocatedSize);
  if (retval != CAEN_DGTZ_Success) {
    printf("<v1720CONET2::InitializeForAcq> Error in CAEN_DGTZ_MallocReadoutBuffer %d\n",retval);
    return (int)retval;
  }

  /* Allocate memory for the events */
  retval |= CAEN_DGTZ_MallocDPPEvents(_device_handle, (void**)Events, &AllocatedSize); 
  printf("<v1720CONET2::InitializeForAcq> dpp events malloced size=%d\n",AllocatedSize);
  if (retval != CAEN_DGTZ_Success) {
    printf("<v1720CONET2::InitializeForAcq> Error in CAEN_DGTZ_MallocDPPEvents %d\n",retval);
    return (int)retval;
  }

  /* Allocate memory for the waveforms */
  retval |= CAEN_DGTZ_MallocDPPWaveforms(_device_handle, (void**)&Waveform, &AllocatedSize); 
  printf("<v1720CONET2::InitializeForAcq> waveforms malloced size=%d\n",AllocatedSize);

  if (retval != CAEN_DGTZ_Success) {
    printf("<v1720CONET2::InitializeForAcq> Error in CAEN_DGTZ_MallocDPPWaveforms %d\n",retval);
    return (int)retval;
  }
  
  
  // WriteReg(0x8004, 1);
  WriteReg(0x800C, 9);
  


  PrintSettings();
  _settings_touched = false;

  return 0;
}
/**
 * \brief   Get data type and ZLE configuration
 *
 * Takes the channel configuration (0x8000) as parameter and checks
 * against the fields for data type (pack 2 or pack 2.5) and for ZLE
 * (Zero-length encoding).  Puts the results in fields mDataType and
 * mZLE.
 *
 * \param   [in]  aChannelConfig  Channel configuration (32-bit)
 */
std::string v1720CONET2::GetChannelConfig(DWORD aChannelConfig){
	
  // Set Device, data type and packing for QT calculation later
  int dataType = ((aChannelConfig >> 11) & 0x1);
  if(((aChannelConfig >> 16) & 0xF) == 0) {
    if(dataType == 1) {
      // 2.5 pack, full data
      mDataType = 1;
      mZLE=0;
      return std::string("Raw Data 2.5 Packing");
    } else {
      // 2 pack, full data
      mZLE=0;
      mDataType = 0;
      return std::string("Raw Data");
    }
  } else if(((aChannelConfig >> 16) & 0xF) == 2) {
    if(dataType == 1) {
      // 2.5 pack, ZLE data
      mDataType = 3;
      mZLE=1;
      return std::string("ZLE Data 2.5 Packing");
    } else {
      // 2 pack, ZLE data
      mDataType = 2;
      mZLE=1;
      return std::string("ZLE Data");
    } 
  } else
    return std::string("V1720 Data format Unrecognised");
}

/**
 * \brief   Get ZLE setting
 *
 * Get the current ZLE setting from the channel configuration.
 *
 * \return  true if data is ZLE
 */
BOOL v1720CONET2::IsZLEData(){
  return mZLE;
}


void v1720CONET2::PrintSettings(){

    // // DPP settings
    // INT recordLen;                     //separate from SetDPPParameters
    // INT savewaveforms;                 //separate from SetDPPParameters
    // INT DPPTrigHoldoff;            
    // INT DPPthresh[8];
    // INT DPPnsbl[8];
    // INT DPPLongGate[8];
    // INT DPPShortGate[8];
    // INT DPPPregateWidth[8];
    // INT DPPSelfTrig[8];
    // INT DPPtvw[8];
    // INT DPPChargeSen[8];
    // INT DCoffset[8];                //separate from SetDPPParameters     
    // INT BLthr;

  CAEN_DGTZ_ErrorCode retval;

  printf("v1720CONET2::PrintSettings vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\n");
  CAEN_DGTZ_AcqMode_t amode;
  retval = CAEN_DGTZ_GetAcquisitionMode(_device_handle,&amode);
  if (retval != CAEN_DGTZ_Success) 
    printf("<v1720CONET2::Print> Error in GetAcquisitionMode %d\n",retval);
  printf("acq_mode           Set = %08d Read = %08d\n",config.acq_mode,amode);     
  uint32_t achanmask;
  retval = CAEN_DGTZ_GetChannelEnableMask(_device_handle, &achanmask);  
  if (retval != CAEN_DGTZ_Success) 
    printf("<v1720CONET2::Print> Error in GetChannelEnableMask %d\n",retval);
  printf("channel_mask       Set = %08d Read = %08d\n",config.channel_mask, achanmask);
  CAEN_DGTZ_TriggerMode_t aswtrigmode;
  retval = CAEN_DGTZ_GetSWTriggerMode(_device_handle, &aswtrigmode );
  if (retval != CAEN_DGTZ_Success) 
    printf("<v1720CONET2::Print> Error in GetSWTriggerMode %d\n",retval);
  printf("sw_trig_mode       Set = %08d Read = %08d\n",config.sw_trig_mode, aswtrigmode);
  CAEN_DGTZ_TriggerMode_t atriginputmode;
  retval = CAEN_DGTZ_GetExtTriggerInputMode(_device_handle, &atriginputmode);
  if (retval != CAEN_DGTZ_Success) 
    printf("<v1720CONET2::Print> Error in GetExtTriggerInputMode %d\n",retval);
  printf("trig_input_mode    Set = %08d Read = %08d\n",config.trig_input_mode, aswtrigmode);
  uint32_t aposttrig;           
  retval = CAEN_DGTZ_GetPostTriggerSize(_device_handle, &aposttrig);
  if (retval != CAEN_DGTZ_Success) 
    printf("<v1720CONET2::Print> Error in GetPostTriggerSize %d\n",retval);

  printf("post_trigger       Set = %08d Read = %08d\n",config.post_trigger, aposttrig);


  // No way to read back these settings:  Just print what I think we set
  //  ret = CAEN_DGTZ_SetDPPParameters(_device_handle, config.channel_mask, &DPPConfig.DPPParams);

  printf("DPPTrigHoldoff     Set = %08d\n",config.DPPTrigHoldoff);
  printf("Baseline threshold Set = %08d\n",config.BLthr);
  for (int i=0; i<8; i++){
    printf("Channel %d Settings:\n",i);
    printf("  threshold          = %08d\n", config.DPPthresh[i]);
    printf("  nsbl               = %08d\n", config.DPPnsbl[i]);
    printf("  long gate          = %08d\n", config.DPPLongGate[i]);
    printf("  short gate         = %08d\n", config.DPPShortGate[i]);
    printf("  pre gate           = %08d\n", config.DPPPregateWidth[i]);
    printf("  self trig          = %08d\n", config.DPPSelfTrig[i]);
    printf("  trig valid acq win = %08d\n", config.DPPtvw[i]);
    printf("  charge sens        = %08d\n", config.DPPChargeSen[i]);
    uint32_t adcoffset;
    retval = CAEN_DGTZ_GetChannelDCOffset(_device_handle, i, &adcoffset);
    if (retval != CAEN_DGTZ_Success) {
      printf("<v1720CONET2::Print> Error in GetChannelDCOffset %d\n",retval);
    }
    printf("  dcoffset       Set = %08d Read = %08d\n",config.DCoffset[i], adcoffset);
    uint32_t pretrigsize;
    retval = CAEN_DGTZ_GetDPPPreTriggerSize(_device_handle, i, &pretrigsize); 
    printf("  pre trig size  Set = %08d Read = %08d\n",config.PreTriggerSize[i], pretrigsize);

  }

  printf("v1720CONET2::PrintSettings ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");

  return;

}

void v1720CONET2::SaveSettings(){

  string output = "";
  char *ptr;

  CAEN_DGTZ_ErrorCode retval;

  output += "v1720CONET2::SaveSettings vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\n";
  CAEN_DGTZ_AcqMode_t amode;
  retval = CAEN_DGTZ_GetAcquisitionMode(_device_handle,&amode);
  if (retval != CAEN_DGTZ_Success) {
    sprintf(ptr,"<v1720CONET2::Print> Error in GetAcquisitionMode %d\n",retval);
    output += std::string(ptr);
  }
  sprintf(ptr,"acq_mode           Set = %08d Read = %08d\n",config.acq_mode,amode );  
  output += std::string(ptr);
 
  uint32_t achanmask;
  retval = CAEN_DGTZ_GetChannelEnableMask(_device_handle, &achanmask);  
  if (retval != CAEN_DGTZ_Success) {
    sprintf(ptr,"<v1720CONET2::Print> Error in GetChannelEnableMask %d\n",retval);
    output += std::string(ptr);
  }
  sprintf(ptr,"channel_mask       Set = %08d Read = %08d\n",config.channel_mask, achanmask); 
  output += std::string(ptr);
 
  CAEN_DGTZ_TriggerMode_t aswtrigmode;
  retval = CAEN_DGTZ_GetSWTriggerMode(_device_handle, &aswtrigmode );
  if (retval != CAEN_DGTZ_Success) { 
    sprintf(ptr,"<v1720CONET2::Print> Error in GetSWTriggerMode %d\n",retval);
    output += std::string(ptr); 
  }
  sprintf(ptr,"sw_trig_mode       Set = %08d Read = %08d\n",config.sw_trig_mode, aswtrigmode);
  output += std::string(ptr);

  output += std::string(ptr);
  CAEN_DGTZ_TriggerMode_t atriginputmode;
  retval = CAEN_DGTZ_GetExtTriggerInputMode(_device_handle, &atriginputmode);
  if (retval != CAEN_DGTZ_Success) { 
    sprintf(ptr,"<v1720CONET2::Print> Error in GetExtTriggerInputMode %d\n",retval);
    output += std::string(ptr);
  }  
  sprintf(ptr,"trig_input_mode    Set = %08d Read = %08d\n",config.trig_input_mode, aswtrigmode);
  output += std::string(ptr);

  output += std::string(ptr);
  uint32_t aposttrig;           
  retval = CAEN_DGTZ_GetPostTriggerSize(_device_handle, &aposttrig);
  if (retval != CAEN_DGTZ_Success) {
    sprintf(ptr,"<v1720CONET2::Print> Error in GetPostTriggerSize %d\n",retval);
    output += std::string(ptr);
  }
  sprintf(ptr,"post_trigger       Set = %08d Read = %08d\n",config.post_trigger, aposttrig);
  output += std::string(ptr);

  // No way to read back these settings:  Just print what I think we set
  //  ret = CAEN_DGTZ_SetDPPParameters(_device_handle, config.channel_mask, &DPPConfig.DPPParams);

  sprintf(ptr,"DPPTrigHoldoff     Set = %08d\n",config.DPPTrigHoldoff);
  output += std::string(ptr);
  sprintf(ptr,"Baseline threshold Set = %08d\n",config.BLthr);
  output += std::string(ptr);
  for (int i=0; i<8; i++){
    sprintf(ptr,"Channel %d Settings:\n",i);
    output += std::string(ptr);
    sprintf(ptr,"  threshold          = %08d\n", config.DPPthresh[i]);
    output += std::string(ptr);
    sprintf(ptr,"  nsbl               = %08d\n", config.DPPnsbl[i]);
    output += std::string(ptr);
    sprintf(ptr,"  long gate          = %08d\n", config.DPPLongGate[i]);
    output += std::string(ptr);
    sprintf(ptr,"  short gate         = %08d\n", config.DPPShortGate[i]);
    output += std::string(ptr);
    sprintf(ptr,"  pre gate           = %08d\n", config.DPPPregateWidth[i]);
    output += std::string(ptr);
    sprintf(ptr,"  self trig          = %08d\n", config.DPPSelfTrig[i]);
    output += std::string(ptr);
    sprintf(ptr,"  trig valid acq win = %08d\n", config.DPPtvw[i]);
    output += std::string(ptr);
    sprintf(ptr,"  charge sens        = %08d\n", config.DPPChargeSen[i]);
    output += std::string(ptr);

    uint32_t adcoffset;
    retval = CAEN_DGTZ_GetChannelDCOffset(_device_handle, i, &adcoffset);
    if (retval != CAEN_DGTZ_Success) {
      sprintf(ptr,"<v1720CONET2::Print> Error in GetChannelDCOffset %d\n",retval);
      output += std::string(ptr);
    }
    sprintf(ptr,"  dcoffset       Set = %08d Read = %08d\n",config.DCoffset[i], adcoffset);
    output += std::string(ptr);

    uint32_t pretrigsize;
    retval = CAEN_DGTZ_GetDPPPreTriggerSize(_device_handle, i, &pretrigsize); 
    sprintf(ptr,"  pre trig size  Set = %08d Read = %08d\n",config.PreTriggerSize[i], pretrigsize);
    output += std::string(ptr);
  }

  sprintf(ptr,"v1720CONET2::PrintSettings ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
  output += std::string(ptr);

  printf("%s",output.c_str());
  return;

}
