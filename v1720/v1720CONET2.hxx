/*****************************************************************************/
/**
\file v1720CONET2.hxx

## Contents

This file contains the class definition for the v1720 module driver.
 *****************************************************************************/

#ifndef V1720_HXX_INCLUDE
#define V1720_HXX_INCLUDE

#define MaxNChannels 8
#define MAXNBITS 12

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <vector>



extern "C" {
#include "CAENComm.h"
#include "ov1720drv.h"
#include <CAENDigitizer.h>
}
//#include "TUCNAnaManager3.h"

#include "midas.h"

// structure of event to be saved to file
typedef struct {
  uint64_t TimeTag;
  int16_t Channel;
  int16_t ChargeShort;
  int16_t ChargeLong;
  int16_t Baseline;
  int16_t Pur;
  int16_t Length; //length of waveform
} DPP_Bank_Out_t;

// struct for defining digitizer parameters
typedef struct
{
  CAEN_DGTZ_ConnectionType LinkType;
  uint32_t VMEBaseAddress;
  uint32_t RecordLength;
  uint32_t SaveWaveforms;
  uint32_t ChannelMask;
  int EventAggr;
  CAEN_DGTZ_PulsePolarity_t PulsePolarity;
  CAEN_DGTZ_DPP_AcqMode_t AcqMode;
  CAEN_DGTZ_IOLevel_t IOlev;
} DigitizerParams_t;



/**
 * Driver class for the v1720 module using the CAEN CONET2 (optical) interface.
 * Contains all the methods necessary to:
 *
 * - Connect/disconnect the board through an optical connection
 * - Initialize the hardware (set the registers) for data acquisition
 * - Read and write to the ODB
 * - Poll the hardware and read the event buffer into a midas bank
 * - Handle ZLE data
 * - Send a software trigger to the board if desired
 */
class v1720CONET2
{
private:
  int _feindex,           //!< Frontend index number
  _link,                  //!< Optical link number
  _board,                 //!< Module/Board number
  _moduleID;              //!< Unique module ID

  int _device_handle;     //!< physical device handle
  HNDLE _odb_handle;      //!< main ODB handle
  HNDLE _settings_handle; //!< Handle for the device settings record
  bool _settings_loaded;  //!< ODB settings loaded
  bool _settings_touched; //!< ODB settings touched
  bool _running;          //!< Run in progress

  int EventCounter[8];    //!< save the number of events for each channel; 
  timeval v1720LastTime;  //!< timestamp for the last reset

  /* Buffers to store the data. The memory must be allocated using the appropriate
     CAENDigitizer API functions (see below), so they must not be initialized here
     NB: you must use the right type for different DPP analysis (in this case PSD) */
  char *buffer;            //!< Buffer for DGTZ event readout
  // readout buffer
  CAEN_DGTZ_DPP_PSD_Event_t *Events[MaxNChannels]; // DGTZ DPP Mode events buffer
  CAEN_DGTZ_DPP_PSD_Event_t *Event;
  CAEN_DGTZ_DPP_PSD_Waveforms_t *Waveform;  //!< DGTZ DPP Mode waveform buffer


  // Methods for internal use
  CAENComm_ErrorCode AcqCtl(uint32_t);
  CAENComm_ErrorCode SetupPreset(int);
  CAENComm_ErrorCode ChannelConfig(uint32_t);
  CAENComm_ErrorCode _ReadReg(DWORD, DWORD*);
  CAENComm_ErrorCode _WriteReg(DWORD, DWORD);

  void PrintSettings();
  //void SaveSettings();

  // Utilities, internal use
  /**
   * Data type for all channels:
   * - 0: Full data, 2 packing
   * - 1: Full data, 2.5 packing
   * - 2: ZLE data, 2 packing
   * - 3: ZLE data, 2.5 packing */
  int mDataType;
  BOOL mZLE; //!< true if ZLE (Zero-length encoding) is enabled on all channels
  std::string GetChannelConfig(DWORD aChannelConfig);
  BOOL IsZLEData();
  void FillQtBank(char * aDest, uint32_t * aZLEData);

public:
  int verbose = 1;  //!< Make the driver verbose
                //!< 0: off
                //!< 1: normal
                //!< 2: very verbose
void SaveSettings();

  v1720CONET2(int feindex, int link, int board, int moduleID, HNDLE hDB);
  ~v1720CONET2();

  enum ConnectErrorCode
  {
    ConnectSuccess,
    ConnectErrorCaenComm,
    ConnectErrorTimeout,
    ConnectErrorAlreadyConnected
  };

  // Methods
  ConnectErrorCode Connect();
  ConnectErrorCode Connect(int, int);
  std::string connectStatusMsg;
  bool Disconnect();
  bool StartRun();
  bool StopRun();
  bool ReadReg(DWORD, DWORD*);
  bool WriteReg(DWORD, DWORD);
  bool FillEventBank(char *);
  bool FillStatBank(char *, suseconds_t);
  bool FillBufferBank(char *);
  bool SendTrigger();
  bool Poll(DWORD*);
  int SetBoardRecord(HNDLE h, void(*cb_func)(INT,INT,void*));
  int SetHistoryRecord(HNDLE h, void(*cb_func)(INT,INT,void*));
  int InitializeForAcq();
  bool IsConnected();
  bool IsRunning();
  bool FillBufferLevelBank(char *);

  // Getters
  std::string GetName();
  int GetDeviceHandle() {
    return _device_handle;                //! returns physical device handle
  }
  HNDLE GetODBHandle() {                  //! returns main ODB handle
    return _odb_handle;
  }
  HNDLE GetSettingsHandle() {             //! returns settings record handle
    return _settings_handle;
  }
  bool GetSettingsTouched() {             //! returns true if odb settings  touched
    return _settings_touched;
  }
  void SetSettingsTouched(bool t) {       //! set _settings_touched
    _settings_touched = t;
  }
  int GetModuleID() { return _moduleID; } //!< returns unique module ID
  int GetLink() { return _link; }         //!< returns optical link number
  int GetBoard() { return _board; }       //!< returns board number
  int GetFEIndex() { return _feindex; }   //!< returns frontend index


  //! Settings structure for this v1720 module
  struct V1720_CONFIG_SETTINGS {
    INT       acq_mode;                //!< 0x8100@[ 1.. 0]
    INT       sw_trig_mode;  // software trigger mode
    INT       trig_input_mode; // trigger mode (per channel, or external, or both)
    DWORD     channel_mask;            //!< 0x8120@[ 7.. 0]
    DWORD     post_trigger;            //!< 0x8114@[31.. 0]

    // DPP settings
    INT recordLen;                     //separate from SetDPPParameters
    INT savewaveforms;                 //separate from SetDPPParameters
    INT DPPTrigHoldoff;            
    INT DPPthresh[8];
    INT DPPnsbl[8];
    INT DPPLongGate[8];
    INT DPPShortGate[8];
    INT DPPPregateWidth[8];
    INT DPPSelfTrig[8];
    INT DPPtvw[8];
    INT DPPChargeSen[8];
    INT DCoffset[8];                //separate from SetDPPParameters     
    INT PreTriggerSize[8];
    INT BLthr;
  } config; //!< instance of config structure

  struct DPP_CONFIG_SETTINGS {
    /* The following variables will store the digitizer configuration parameters */
    CAEN_DGTZ_DPP_PSD_Params_t DPPParams;
    DigitizerParams_t Params;
  } DPPConfig;  //!< instance of DPP config structure

  static const char *config_str_board[]; //!< Configuration string for this board
  static const char history_settings[][NAME_LENGTH];


  // method for finding the number of 1's in a binary number
  int popcnt(DWORD i) {
    int c = 0;
    DWORD z = i;
    for(; z; c++) {
      z &= z - 1;
    }
    return c;
  }

  uint32_t ev;
  DPP_Bank_Out_t out;
  uint32_t BufferSize; 

};

#endif // V1720_HXX_INCLUDE
