#pragma once
#include <stdint.h>
#include <string>
#include <functional>

template<typename P, typename B> class KnxFacade {
public:
    uint8_t paramByte(uint16_t) { return 0; }
    const char* paramData(uint16_t) { return ""; }
};

class RP2040ArduinoPlatform {};
class Bau07B0 {};

class GroupObject {
public:
    int value(int dpt) const { return val_; }
    int value(uint8_t dpt) const { return val_; }
    void setValue(int v) { val_ = v; }
private:
    int val_ = 0;
};

using KNXValue = int;

constexpr int DPT_Switch    = 1;
constexpr int DPT_Trigger   = 2;
constexpr int DPT_Value_Temp = 3;
