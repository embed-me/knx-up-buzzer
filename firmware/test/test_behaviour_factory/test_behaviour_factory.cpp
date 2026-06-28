#include <gtest/gtest.h>
#include "src/application/behaviour/BehaviourFactory.hpp"
#include "test/mocks/mock_knx_driver.hpp"
#include "test/mocks/mock_timer_factory.hpp"
#include "test/mocks/mock_timer.hpp"
#include "test/mocks/mock_buzzer_driver.hpp"
#include "src/application/behaviour/BehaviourFactory.cpp"
#include "src/application/behaviour/BehaviourBase.cpp"
#include "src/application/behaviour/trigger/TriggerBehaviour.cpp"
#include "src/application/behaviour/switch/SwitchBehaviour.cpp"
#include "src/application/behaviour/venting_monitor/VentingMonitorBehaviour.cpp"
#include "src/drivers/knx/data/modes/TriggerMode.cpp"
#include "src/drivers/knx/data/modes/SwitchMode.cpp"
#include "src/drivers/knx/data/modes/VentingMonitorMode.cpp"
#include "src/utils/scheduler/Scheduler.cpp"
#include "src/drivers/logger/Logger.cpp"

using namespace application::behaviour;

class BehaviourFactoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        knxDriver = std::make_shared<MockIKnxDriver>();
        timerFactory = std::make_shared<MockITimerDriverFactory>();
        buzzer = std::make_shared<MockIBuzzerDriver>();

        auto timer1 = std::make_shared<MockITimer>();
        auto timer2 = std::make_shared<MockITimer>();
        timerFactory->timers = {timer1, timer2};

        factory = std::make_shared<BehaviourFactory>(knxDriver, timerFactory, buzzer);
    }

    std::shared_ptr<MockIKnxDriver> knxDriver;
    std::shared_ptr<MockITimerDriverFactory> timerFactory;
    std::shared_ptr<MockIBuzzerDriver> buzzer;
    std::shared_ptr<BehaviourFactory> factory;
};

TEST_F(BehaviourFactoryTest, GetBehaviourWithTriggerMode) {
    drivers::knx::MelodyConfig cfg;
    cfg.mode = std::make_shared<drivers::knx::TriggerMode>(1);
    auto result = factory->getBehaviour(cfg);
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE((*result)->isSupported(*cfg.mode));
}

TEST_F(BehaviourFactoryTest, GetBehaviourWithSwitchMode) {
    drivers::knx::MelodyConfig cfg;
    cfg.mode = std::make_shared<drivers::knx::SwitchMode>(1);
    auto result = factory->getBehaviour(cfg);
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE((*result)->isSupported(*cfg.mode));
}

TEST_F(BehaviourFactoryTest, GetBehaviourWithVentingMonitorMode) {
    drivers::knx::MelodyConfig cfg;
    cfg.mode = std::make_shared<drivers::knx::VentingMonitorMode>(1, 2, 10, 30, 15);
    auto result = factory->getBehaviour(cfg);
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE((*result)->isSupported(*cfg.mode));
}

class FakeUnknownMode : public drivers::knx::IMode {
public:
    drivers::knx::ModeType getType() const override {
        return static_cast<drivers::knx::ModeType>(99);
    }
};

TEST_F(BehaviourFactoryTest, GetBehaviourWithUnknownModeReturnsNullopt) {
    drivers::knx::MelodyConfig cfg;
    cfg.mode = std::make_shared<FakeUnknownMode>();
    auto result = factory->getBehaviour(cfg);
    EXPECT_FALSE(result.has_value());
}
