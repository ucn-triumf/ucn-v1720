#ifndef PressData_hxx_seen
#define PressData_hxx_seen

#include <vector>

#include "TGenericData.hxx"

///
class PressData: public TGenericData {

public:

  /// Constructor
  PressData(int bklen, int bktype, const char* name, void *pdata);

  /// Get the number of 32-bit words in bank.
  uint32_t GetEventSize() const {return (fGlobalHeader0 & 0xffffff);};

  /// Get the channel mask; ie, the set of channels for which we 
  /// have data for this event.
  uint32_t GetChannelMask() const {return (fGlobalHeader1 & 0xff);};

  /// Get event counter
  uint32_t GetEventCounter() const {return ((fGlobalHeader2) & 0xffffff);};

  /// Get trigger tag
  uint32_t GetTriggerTag() const {return ((fGlobalHeader3) & 0xffffffff);};



private:
 
  /// Helper method to handle uncompressed data.
  void HandlUncompressedData();

  /// The overall global headers
  uint32_t fGlobalHeader0;
  uint32_t fGlobalHeader1;
  uint32_t fGlobalHeader2;
  uint32_t fGlobalHeader3;

  float fT1;
  float fT2;
  float fP;

};

#endif
