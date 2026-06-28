#include <gtest/gtest.h>
#include "src/application/behaviour/venting_monitor/VentingMonitorBehaviour.hpp"
#include "src/utils/scheduler/Scheduler.hpp"
#include "test/mocks/mock_knx_driver.hpp"
#include "test/mocks/mock_timer_factory.hpp"
#include "test/mocks/mock_timer.hpp"
#include "test/mocks/mock_buzzer_driver.hpp"
#include "test/mocks/mock_resource_granter.hpp"
#include "test/mocks/mock_scheduler_strategy.hpp"
#include "src/application/behaviour/venting_monitor/VentingMonitorBehaviour.cpp"
#include "src/application/behaviour/BehaviourBase.cpp"
#include "src/utils/scheduler/Scheduler.cpp"
#include "src/drivers/logger/Logger.cpp"
#include "src/drivers/knx/data/modes/SwitchMode.cpp"
#include "src/drivers/knx/data/modes/VentingMonitorMode.cpp"

using namespace application::behaviour;

class VentingMonitorTest : public ::testing::Test {
protected:
    void SetUp() override {
        knxDriver = std::make_shared<MockIKnxDriver>();
        timerFactory = std::make_shared<MockITimerDriverFactory>();
        buzzer = std::make_shared<MockIBuzzerDriver>();
        ventingTimer = std::make_shared<MockITimer>();
        melodyTimer = std::make_shared<MockITimer>();
        timerFactory->timers = {ventingTimer, melodyTimer};

        schedulerStrategy = std::make_shared<MockISchedulerStrategy>();
        sched = std::make_shared<utils::Scheduler>(schedulerStrategy);

        behaviour = std::make_shared<VentingMonitorBehaviour>(knxDriver, timerFactory, buzzer);
    }

    void executeScheduledTask() {
        if (schedulerStrategy->lastTask) {
            schedulerStrategy->lastTask(nullptr);
        }
    }

    std::shared_ptr<MockIKnxDriver> knxDriver;
    std::shared_ptr<MockITimerDriverFactory> timerFactory;
    std::shared_ptr<MockIBuzzerDriver> buzzer;
    std::shared_ptr<MockITimer> ventingTimer;
    std::shared_ptr<MockITimer> melodyTimer;
    std::shared_ptr<MockISchedulerStrategy> schedulerStrategy;
    std::shared_ptr<utils::Scheduler> sched;
    std::shared_ptr<VentingMonitorBehaviour> behaviour;
};

TEST_F(VentingMonitorTest, IsSupportedVentingMonitorMode) {
    drivers::knx::VentingMonitorMode mode(1, 2, 10, 30, 15);
    EXPECT_TRUE(behaviour->isSupported(mode));
}

TEST_F(VentingMonitorTest, IsSupportedSwitchMode) {
    drivers::knx::SwitchMode mode(1);
    EXPECT_FALSE(behaviour->isSupported(mode));
}

TEST_F(VentingMonitorTest, InitWithVentingMonitorMode) {
    drivers::knx::MelodyConfig cfg;
    cfg.mode = std::make_shared<drivers::knx::VentingMonitorMode>(0x42, 0x43, 10, 30, 15);
    cfg.melody.rtttl = "test";
    cfg.melody.pause_sec = 5;

    bool result = behaviour->init(cfg);
    EXPECT_FALSE(result);
    EXPECT_EQ(knxDriver->callbacks.size(), 2u);
}

TEST_F(VentingMonitorTest, InitWithWrongMode) {
    drivers::knx::MelodyConfig cfg;
    cfg.mode = std::make_shared<drivers::knx::SwitchMode>(1);

    bool result = behaviour->init(cfg);
    EXPECT_TRUE(result);
}

TEST_F(VentingMonitorTest, KnxCallbackVentingOnTempInRange) {
    auto granter = std::make_shared<MockIResourceGranter>();
    behaviour->registerResourceGranter(granter.get());

    drivers::knx::MelodyConfig cfg;
    cfg.mode = std::make_shared<drivers::knx::VentingMonitorMode>(0x42, 0x43, 10, 30, 15);
    cfg.melody.rtttl = "test";
    cfg.melody.pause_sec = 5;
    behaviour->init(cfg);

    knxDriver->simulateKnxEvent(0x42, 1, DPT_Switch);

    EXPECT_TRUE(ventingTimer->startCalled);
    EXPECT_FALSE(buzzer->playCalled);
}

TEST_F(VentingMonitorTest, KnxCallbackVentingOnTempOutsideLimits) {
    auto granter = std::make_shared<MockIResourceGranter>();
    granter->grantResult = true;
    behaviour->registerResourceGranter(granter.get());

    drivers::knx::MelodyConfig cfg;
    cfg.mode = std::make_shared<drivers::knx::VentingMonitorMode>(0x42, 0x43, 10, 30, 15);
    cfg.melody.rtttl = "test";
    cfg.melody.pause_sec = 5;
    behaviour->init(cfg);

    knxDriver->simulateKnxEvent(0x42, 1, DPT_Switch);

    EXPECT_TRUE(ventingTimer->startCalled);
}

TEST_F(VentingMonitorTest, KnxCallbackVentingOff) {
    auto granter = std::make_shared<MockIResourceGranter>();
    behaviour->registerResourceGranter(granter.get());

    drivers::knx::MelodyConfig cfg;
    cfg.mode = std::make_shared<drivers::knx::VentingMonitorMode>(0x42, 0x43, 10, 30, 15);
    cfg.melody.rtttl = "test";
    cfg.melody.pause_sec = 5;
    behaviour->init(cfg);

    knxDriver->simulateKnxEvent(0x42, 1, DPT_Switch);
    knxDriver->simulateKnxEvent(0x42, 0, DPT_Switch);

    EXPECT_TRUE(ventingTimer->stopCalled);
}

TEST_F(VentingMonitorTest, KnxCallbackTempOutsideLimits) {
    auto granter = std::make_shared<MockIResourceGranter>();
    granter->grantResult = true;
    behaviour->registerResourceGranter(granter.get());

    drivers::knx::MelodyConfig cfg;
    cfg.mode = std::make_shared<drivers::knx::VentingMonitorMode>(0x42, 0x43, 10, 30, 15);
    cfg.melody.rtttl = "test";
    cfg.melody.pause_sec = 5;
    behaviour->init(cfg);

    knxDriver->simulateKnxEvent(0x42, 1, DPT_Switch);

    knxDriver->simulateKnxEvent(0x43, 5, DPT_Value_Temp);

    // Temperature callback only sets isTemperatureInRange flag;
    // melody triggers via venting timer interrupt
    ventingTimer->simulateTimerInterrupt();
    executeScheduledTask();

    EXPECT_TRUE(buzzer->playCalled);
    EXPECT_TRUE(melodyTimer->startCalled);
}

TEST_F(VentingMonitorTest, VentingDurationExceededTempInside) {
    auto granter = std::make_shared<MockIResourceGranter>();
    granter->grantResult = true;
    behaviour->registerResourceGranter(granter.get());

    drivers::knx::MelodyConfig cfg;
    cfg.mode = std::make_shared<drivers::knx::VentingMonitorMode>(0x42, 0x43, 10, 30, 15);
    cfg.melody.rtttl = "test";
    cfg.melody.pause_sec = 5;
    behaviour->init(cfg);

    knxDriver->simulateKnxEvent(0x42, 1, DPT_Switch);

    buzzer->playCalled = false;
    ventingTimer->simulateTimerInterrupt();
    executeScheduledTask();

    EXPECT_FALSE(buzzer->playCalled);
}

TEST_F(VentingMonitorTest, VentingDurationExceededTempOutside) {
    auto granter = std::make_shared<MockIResourceGranter>();
    granter->grantResult = true;
    behaviour->registerResourceGranter(granter.get());

    drivers::knx::MelodyConfig cfg;
    cfg.mode = std::make_shared<drivers::knx::VentingMonitorMode>(0x42, 0x43, 10, 30, 15);
    cfg.melody.rtttl = "test";
    cfg.melody.pause_sec = 5;
    behaviour->init(cfg);

    knxDriver->simulateKnxEvent(0x42, 1, DPT_Switch);
    knxDriver->simulateKnxEvent(0x43, 5, DPT_Value_Temp);

    buzzer->playCalled = false;
    melodyTimer->startCalled = false;
    ventingTimer->simulateTimerInterrupt();
    executeScheduledTask();

    EXPECT_TRUE(buzzer->playCalled);
    EXPECT_TRUE(melodyTimer->startCalled);
}

TEST_F(VentingMonitorTest, MelodyStop) {
    behaviour->melodyStop();
    EXPECT_TRUE(melodyTimer->stopCalled);
}
