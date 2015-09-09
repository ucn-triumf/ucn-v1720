#include "PressData.h"

#include <iomanip>
#include <iostream>


void PressData::HandlUncompressedData(){

  // Skip the header.
  uint32_t iPtr=4;
  uint32_t chMask    = GetData32()[1] & 0xFF;

  // Assume that we have readout the same number of samples for each channel.
  // The number of 32 bit double-samples per channel is then
  // N32samples = (bank size - 4)/ nActiveChannels 

  int Nsamples = (GetEventSize() - 4);
  std::cout<<GetData32()[4] << " ";
  std::cout<<GetData16()[9] << std::endl;
      // channel.AddADCSample(samp1);
      // channel.AddADCSample(samp2); 
      
 
}
PressData::PressData(int bklen, int bktype, const char* name, void *pdata):
    TGenericData(bklen, bktype, name, pdata)
{
  
  fGlobalHeader0 = GetData32()[0];
  fGlobalHeader1 = GetData32()[1];
  fGlobalHeader2 = GetData32()[2];
  fGlobalHeader3 = GetData32()[3];

  HandlUncompressedData();
}

