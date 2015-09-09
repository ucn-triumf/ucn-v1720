#include "TObject.h"


// class for HVEvent
class HVEvent : TObject {

 private:
  Float_t fVolt[HVCHANNELS];
  Float_t fCurr[HVCHANNELS];
  ULong_t fTimeH;
  ULong_t fEventH;

 public:
  // contructor/deconstructor
 HVEvent(int i=0) {
    init();
  };
 ~HVEvent(): TObject() {
    delete fVolt;
    delete fCurr;
  };

  void init(){
    fVolt = {0};
    fCurr = {0};
    fTimeH = 0;
    fEventH = 0;
    return;
  };

  // Setters
  int SetVolt(float * ptr) {
    std::copy(ptr,ptr+(sizeof(ptr)/sizeof(*ptr)),fVolt);
    return 1;
  };
  int SetCurr(float * ptr) {
    std::copy(ptr,ptr+(sizeof(ptr)/sizeof(*ptr)),fCurr);
    return 1;
  };
  int SetTime(ULong_t time) {
    fTimeH = time;
    return 1;
  };
  int SetEvent(ULong_t numEvent) {
    fEventH = numEvent;
    return 1;
  };

  ClassDef(HVEventNV,1);
};
