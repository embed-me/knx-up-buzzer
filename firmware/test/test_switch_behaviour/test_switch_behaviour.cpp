#include <gtest/gtest.h>
#include "src/application/behaviour/switch/SwitchBehaviour.hpp"
#include "src/utils/scheduler/Scheduler.hpp"
#include "test/mocks/mock_knx_driver.hpp"
#include "test/mocks/mock_timer_factory.hpp"
#include "test/mocks/mock_timer.hpp"
#include "test/mocks/mock_buzzer_driver.hpp"
#include "test/mocks/mock_resource_granter.hpp"
#include "test/mocks/mock_scheduler_strategy.hpp"
#include "src/application/behaviour/switch/SwitchBehaviour.cpp"
#include "src/application/behaviour/BehaviourBase.cpp"
#include "src/utils/scheduler/Scheduler.cpp"
#include "src/drivers/logger/Logger.cpp"
#include "src/drivers/knx/data/modes/TriggerMode.cpp"
#include "src/drivers/knx/data/modes/SwitchMode.cpp"
#include "src/drivers/knx/data/modes/VentingMonitorMode.cpp"

using namespace application::behaviour;

class SwitchBehaviourTest : public ::testing::Test {
protected:
    void SetUp() override {
        knxDriver = std::make_shared<MockIKnxDriver>();
        timerFactory = std::make_shared<MockITimerDriverFactory>();
        buzzer = std::make_shared<MockIBuzzerDriver>();
        mockTimer = std::make_shared<MockITimer>();
        timerFactory->timers = {mockTimer};

        schedulerStrategy = std::make_shared<MockISchedulerStrategy>();
        sched = std::make_shared<utils::Scheduler>(schedulerStrategy);

        behaviour = std::make_shared<SwitchBehaviour>(knxDriver, timerFactory, buzzer);
    }

    void executeScheduledTask() {
        if (schedulerStrategy->lastTask) {
            schedulerStrategy->lastTask(nullptr);
        }
    }

    std::shared_ptr<MockIKnxDriver> knxDriver;
    std::shared_ptr<MockITimerDriverFactory> timerFactory;
    std::shared_ptr<MockIBuzzerDriver> buzzer;
    std::shared_ptr<MockITimer> mockTimer;
    std::shared_ptr<MockISchedulerStrategy> schedulerStrategy;
    std::shared_ptr<utils::Scheduler> sched;
    std::shared_ptr<SwitchBehaviour> behaviour;
};

TEST_F(SwitchBehaviourTest, IsSupportedSwitchMode) {
    drivers::knx::SwitchMode mode(1);
    EXPECT_TRUE(behaviour->isSupported(mode));
}

TEST_F(SwitchBehaviourTest, IsSupportedTriggerMode) {
    drivers::knx::TriggerMode mode(1);
    EXPECT_FALSE(behaviour->isSupported(mode));
}

TEST_F(SwitchBehaviourTest, IsSupportedVentingMonitorMode) {
    drivers::knx::VentingMonitorMode mode(1, 2, 10, 30, 15);
    EXPECT_FALSE(behaviour->isSupported(mode));
}

TEST_F(SwitchBehaviourTest, InitWithSwitchMode) {
    drivers::knx::MelodyConfig cfg;
    cfg.mode = std::make_shared<drivers::knx::SwitchMode>(0x42);
    cfg.melody.rtttl = "test";
    cfg.melody.pause_sec = 5;

    bool result = behaviour->init(cfg);
    EXPECT_FALSE(result);
    EXPECT_EQ(knxDriver->lastKoNumber, 0x42);
}

TEST_F(SwitchBehaviourTest, InitWithWrongMode) {
    drivers::knx::MelodyConfig cfg;
    cfg.mode = std::make_shared<drivers::knx::TriggerMode>(1);

    bool result = behaviour->init(cfg);
    EXPECT_TRUE(result);
}

TEST_F(SwitchBehaviourTest, KnxCallbackSwitchOnFirstTime) {
    auto granter = std::make_shared<MockIResourceGranter>();
    granter->grantResult = true;
    behaviour->registerResourceGranter(granter.get());

    drivers::knx::MelodyConfig cfg;
    cfg.mode = std::make_shared<drivers::knx::SwitchMode>(0x42);
    cfg.melody.rtttl = "test";
    cfg.melody.pause_sec = 5;
    behaviour->init(cfg);

    knxDriver->simulateKnxEvent(0x42, 1, DPT_Switch);

    EXPECT_TRUE(buzzer->playCalled);
    EXPECT_TRUE(mockTimer->startCalled);
    EXPECT_EQ(mockTimer->lastMode, drivers::timer::TimerMode::RECURRING);
    EXPECT_EQ(mockTimer->lastTimeoutUs, 5000000U);
}

TEST_F(SwitchBehaviourTest, KnxCallbackSwitchOnBuzzerDenied) {
    auto granter = std::make_shared<MockIResourceGranter>();
    granter->grantResult = false;
    behaviour->registerResourceGranter(granter.get());

    drivers::knx::MelodyConfig cfg;
    cfg.mode = std::make_shared<drivers::knx::SwitchMode>(0x42);
    cfg.melody.rtttl = "test";
    cfg.melody.pause_sec = 5;
    behaviour->init(cfg);

    knxDriver->simulateKnxEvent(0x42, 1, DPT_Switch);

    EXPECT_FALSE(mockTimer->startCalled);
}

TEST_F(SwitchBehaviourTest, KnxCallbackSwitchOnAlreadyOn) {
    auto granter = std::make_shared<MockIResourceGranter>();
    granter->grantResult = true;
    behaviour->registerResourceGranter(granter.get());

    drivers::knx::MelodyConfig cfg;
    cfg.mode = std::make_shared<drivers::knx::SwitchMode>(0x42);
    cfg.melody.rtttl = "test";
    cfg.melody.pause_sec = 5;
    behaviour->init(cfg);

    knxDriver->simulateKnxEvent(0x42, 1, DPT_Switch);
    buzzer->playCalled = false;
    mockTimer->startCalled = false;

    knxDriver->simulateKnxEvent(0x42, 1, DPT_Switch);

    EXPECT_FALSE(buzzer->playCalled);
    EXPECT_FALSE(mockTimer->startCalled);
}

TEST_F(SwitchBehaviourTest, KnxCallbackSwitchOff) {
    auto granter = std::make_shared<MockIResourceGranter>();
    granter->grantResult = true;
    behaviour->registerResourceGranter(granter.get());

    drivers::knx::MelodyConfig cfg;
    cfg.mode = std::make_shared<drivers::knx::SwitchMode>(0x42);
    cfg.melody.rtttl = "test";
    cfg.melody.pause_sec = 5;
    behaviour->init(cfg);

    knxDriver->simulateKnxEvent(0x42, 1, DPT_Switch);
    knxDriver->simulateKnxEvent(0x42, 0, DPT_Switch);

    EXPECT_TRUE(mockTimer->stopCalled);
}

TEST_F(SwitchBehaviourTest, MelodyStop) {
    behaviour->melodyStop();
    EXPECT_TRUE(mockTimer->stopCalled);
}

TEST_F(SwitchBehaviourTest, TimerInterruptBuzzerPlays) {
    auto granter = std::make_shared<MockIResourceGranter>();
    granter->grantResult = true;
    behaviour->registerResourceGranter(granter.get());

    drivers::knx::MelodyConfig cfg;
    cfg.mode = std::make_shared<drivers::knx::SwitchMode>(0x42);
    cfg.melody.rtttl = "test";
    cfg.melody.pause_sec = 5;
    behaviour->init(cfg);

    knxDriver->simulateKnxEvent(0x42, 1, DPT_Switch);
    buzzer->playCalled = false;

    mockTimer->simulateTimerInterrupt();
    executeScheduledTask();

    EXPECT_TRUE(buzzer->playCalled);
}

TEST_F(SwitchBehaviourTest, TimerInterruptBuzzerDeniedStopsMelody) {
    auto granter = std::make_shared<MockIResourceGranter>();
    behaviour->registerResourceGranter(granter.get());

    drivers::knx::MelodyConfig cfg;
    cfg.mode = std::make_shared<drivers::knx::SwitchMode>(0x42);
    cfg.melody.rtttl = "test";
    cfg.melody.pause_sec = 5;
    behaviour->init(cfg);

    granter->grantResult = true;
    knxDriver->simulateKnxEvent(0x42, 1, DPT_Switch);

    granter->grantResult = false;
    mockTimer->stopCalled = false;
    mockTimer->simulateTimerInterrupt();
    executeScheduledTask();

    EXPECT_TRUE(mockTimer->stopCalled);
}
