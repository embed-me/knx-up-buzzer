#include <gtest/gtest.h>
#include "src/application/behaviour/trigger/TriggerBehaviour.hpp"
#include "test/mocks/mock_knx_driver.hpp"
#include "test/mocks/mock_buzzer_driver.hpp"
#include "test/mocks/mock_resource_granter.hpp"
#include "src/application/behaviour/trigger/TriggerBehaviour.cpp"
#include "src/application/behaviour/BehaviourBase.cpp"
#include "src/drivers/logger/Logger.cpp"
#include "src/drivers/knx/data/modes/TriggerMode.cpp"
#include "src/drivers/knx/data/modes/SwitchMode.cpp"

using namespace application::behaviour;

class TriggerBehaviourTest : public ::testing::Test {
protected:
    void SetUp() override {
        knxDriver = std::make_shared<MockIKnxDriver>();
        buzzer = std::make_shared<MockIBuzzerDriver>();
        behaviour = std::make_shared<TriggerBehaviour>(knxDriver, buzzer);
    }

    std::shared_ptr<MockIKnxDriver> knxDriver;
    std::shared_ptr<MockIBuzzerDriver> buzzer;
    std::shared_ptr<TriggerBehaviour> behaviour;
};

TEST_F(TriggerBehaviourTest, IsSupportedTriggerMode) {
    drivers::knx::TriggerMode mode(1);
    EXPECT_TRUE(behaviour->isSupported(mode));
}

TEST_F(TriggerBehaviourTest, IsSupportedSwitchMode) {
    drivers::knx::SwitchMode mode(1);
    EXPECT_FALSE(behaviour->isSupported(mode));
}

TEST_F(TriggerBehaviourTest, InitWithTriggerMode) {
    drivers::knx::MelodyConfig cfg;
    cfg.mode = std::make_shared<drivers::knx::TriggerMode>(0x42);
    cfg.melody.rtttl = "test";

    bool result = behaviour->init(cfg);
    EXPECT_FALSE(result);
    EXPECT_EQ(knxDriver->lastKoNumber, 0x42);
}

TEST_F(TriggerBehaviourTest, InitWithWrongMode) {
    drivers::knx::MelodyConfig cfg;
    cfg.mode = std::make_shared<drivers::knx::SwitchMode>(1);

    bool result = behaviour->init(cfg);
    EXPECT_TRUE(result);
}

TEST_F(TriggerBehaviourTest, KnxCallbackTriggerValueOne) {
    auto granter = std::make_shared<MockIResourceGranter>();
    granter->grantResult = true;
    behaviour->registerResourceGranter(granter.get());

    drivers::knx::MelodyConfig cfg;
    cfg.mode = std::make_shared<drivers::knx::TriggerMode>(0x42);
    cfg.melody.rtttl = "test";
    behaviour->init(cfg);

    knxDriver->simulateKnxEvent(0x42, 1, DPT_Trigger);

    EXPECT_TRUE(buzzer->playCalled);
}

TEST_F(TriggerBehaviourTest, KnxCallbackTriggerValueZero) {
    auto granter = std::make_shared<MockIResourceGranter>();
    granter->grantResult = true;
    behaviour->registerResourceGranter(granter.get());

    drivers::knx::MelodyConfig cfg;
    cfg.mode = std::make_shared<drivers::knx::TriggerMode>(0x42);
    cfg.melody.rtttl = "test";
    behaviour->init(cfg);

    knxDriver->simulateKnxEvent(0x42, 0, DPT_Trigger);

    EXPECT_FALSE(buzzer->playCalled);
}

TEST_F(TriggerBehaviourTest, MelodyStopNoOp) {
    EXPECT_NO_THROW(behaviour->melodyStop());
}
