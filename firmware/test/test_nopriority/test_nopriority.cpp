#include <gtest/gtest.h>
#include "src/application/priority/no_priority/NoPriority.hpp"
#include "src/application/priority/no_priority/NoPriority.cpp"

using namespace application::priority;

TEST(NoPriorityTest, IsGrantedWithDefaultConfig) {
    NoPriority noPriority;
    drivers::knx::MelodyConfig cfg{};
    EXPECT_TRUE(noPriority.isGranted(cfg));
}

TEST(NoPriorityTest, IsGrantedWithArbitraryConfig) {
    NoPriority noPriority;
    drivers::knx::MelodyConfig cfg;
    cfg.priority = 5;
    EXPECT_TRUE(noPriority.isGranted(cfg));
}

TEST(NoPriorityTest, IsGrantedWithNullMode) {
    NoPriority noPriority;
    drivers::knx::MelodyConfig cfg;
    cfg.mode = nullptr;
    EXPECT_TRUE(noPriority.isGranted(cfg));
}
