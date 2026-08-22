#pragma once

#include <nw/types.h>

namespace nw {
namespace demo {
namespace internal {

class PadButton{
  public:
    static const int BUTTON_COUNT_MAX = 32;
    static const int REPEAT_START = 25;
    static const int REPEAT_SPAN  = 6;

    PadButton();
    void Update(u32 buttonStatus);

    void ClearTriggerFlag();

    void Reset();

    bool IsButtonPress(u32 buttonBit) const { return (mButtonPress & buttonBit) != 0; }
    bool IsButtonDown(u32 buttonBit) const { return (mButtonDown & buttonBit) != 0; }
    bool IsButtonUp(u32 buttonBit) const { return ( mButtonUp & buttonBit) != 0; }
    bool IsButtonRepeat(u32 buttonBit) const { return ( mButtonRepeat & buttonBit) != 0; }
    bool IsButtonRepeatFast(u32 buttonBit) const { return ( mButtonRepeatFast & buttonBit) != 0; }

  private:
    u32 mButtonPress;
    u32 mButtonDown;
    u32 mButtonUp;
    u32 mButtonRepeat;
    u32 mButtonRepeatFast;
    u8 mRepeatCounter[BUTTON_COUNT_MAX];
    u8 mRepeatCounterFast[BUTTON_COUNT_MAX];
    bool mResetFlag;
    u8 padding[3];
};

}
}
}