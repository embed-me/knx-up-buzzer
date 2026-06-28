#include <gtest/gtest.h>
#include "src/drivers/knx/data/IMode.hpp"
#include "src/drivers/knx/data/modes/TriggerMode.cpp"
#include "src/drivers/knx/data/modes/SwitchMode.cpp"
#include "src/drivers/knx/data/modes/VentingMonitorMode.cpp"

using namespace drivers::knx;

TEST(TriggerModeTest, GetType) {
    TriggerMode mode(0x42);
    EXPECT_EQ(mode.getType(), ModeType::MODE_TRIGGER);
}

TEST(TriggerModeTest, StoresKoNumber) {
    TriggerMode mode(0x42);
    EXPECT_EQ(mode.ko_number, 0x42);
}

TEST(SwitchModeTest, GetType) {
    SwitchMode mode(0x17);
    EXPECT_EQ(mode.getType(), ModeType::MODE_SWITCH);
}

TEST(SwitchModeTest, StoresKoNumber) {
    SwitchMode mode(0x17);
    EXPECT_EQ(mode.ko_number, 0x17);
}

TEST(VentingMonitorModeTest, GetType) {
    VentingMonitorMode mode(1, 2, 10, 30, 15);
    EXPECT_EQ(mode.getType(), ModeType::MODE_VENTING_MONITOR);
}

TEST(VentingMonitorModeTest, StoresAllFields) {
    VentingMonitorMode mode(1, 2, 10, 30, 15);
    EXPECT_EQ(mode.ko_venting, 1);
    EXPECT_EQ(mode.ko_temp, 2);
    EXPECT_EQ(mode.lower_temp_limit_c, 10);
    EXPECT_EQ(mode.upper_temp_limit_c, 30);
    EXPECT_EQ(mode.venting_duration_limit_min, 15);
}

TEST(ModePolymorphismTest, TriggerModeViaIModeRef) {
    TriggerMode tm(1);
    IMode &ref = tm;
    EXPECT_EQ(ref.getType(), ModeType::MODE_TRIGGER);
}

TEST(ModePolymorphismTest, SwitchModeViaIModeRef) {
    SwitchMode sm(1);
    IMode &ref = sm;
    EXPECT_EQ(ref.getType(), ModeType::MODE_SWITCH);
}

TEST(ModePolymorphismTest, VentingMonitorModeViaIModeRef) {
    VentingMonitorMode vm(1, 2, 10, 30, 15);
    IMode &ref = vm;
    EXPECT_EQ(ref.getType(), ModeType::MODE_VENTING_MONITOR);
}
