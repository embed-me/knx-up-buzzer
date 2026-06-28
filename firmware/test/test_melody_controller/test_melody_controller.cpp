#include <gtest/gtest.h>
#include "src/application/MelodyController.hpp"
#include "test/mocks/mock_priority.hpp"
#include "test/mocks/mock_behaviour_factory.hpp"
#include "test/mocks/mock_behaviour.hpp"
#include "test/mocks/mock_resource_requester.hpp"
#include "src/application/MelodyController.cpp"
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

using namespace application;
using namespace application::behaviour;

class MelodyControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        priorityStrategy = std::make_shared<MockIPriority>();
        mockFactory = std::make_shared<MockBehaviourFactory>();
        controller = std::make_shared<MelodyController>(priorityStrategy, mockFactory);
    }

    std::shared_ptr<MockIPriority> priorityStrategy;
    std::shared_ptr<MockBehaviourFactory> mockFactory;
    std::shared_ptr<MelodyController> controller;
};

TEST_F(MelodyControllerTest, InitWithEmptyConfigList) {
    std::list<drivers::knx::MelodyConfig> emptyList;
    EXPECT_NO_THROW(controller->init(emptyList));
    EXPECT_EQ(mockFactory->getBehaviourCalledCount, 0);
}

TEST_F(MelodyControllerTest, InitWithOneConfigFactoryReturnsBehaviour) {
    auto mockBehaviour = std::make_shared<MockIBehaviour>();
    mockFactory->preconfiguredBehaviour = mockBehaviour;

    drivers::knx::MelodyConfig cfg;
    cfg.mode = std::make_shared<drivers::knx::TriggerMode>(1);
    std::list<drivers::knx::MelodyConfig> configs = {cfg};

    controller->init(configs);

    EXPECT_TRUE(mockBehaviour->granterRegistered != nullptr);
}

TEST_F(MelodyControllerTest, InitWithOneConfigFactoryReturnsNullopt) {
    mockFactory->preconfiguredBehaviour = nullptr;

    drivers::knx::MelodyConfig cfg;
    cfg.mode = std::make_shared<drivers::knx::TriggerMode>(1);
    std::list<drivers::knx::MelodyConfig> configs = {cfg};

    EXPECT_NO_THROW(controller->init(configs));
}

TEST_F(MelodyControllerTest, InitWithThreeConfigsTwoSupported) {
    auto b1 = std::make_shared<MockIBehaviour>();
    auto b2 = std::make_shared<MockIBehaviour>();

    mockFactory->preconfiguredBehaviour = b1;

    drivers::knx::MelodyConfig cfg1, cfg2, cfg3;
    cfg1.mode = std::make_shared<drivers::knx::TriggerMode>(1);
    cfg2.mode = std::make_shared<drivers::knx::TriggerMode>(2);
    cfg3.mode = std::make_shared<drivers::knx::TriggerMode>(3);

    controller->init({cfg1});

    mockFactory->preconfiguredBehaviour = b2;
    controller->init({cfg2});

    mockFactory->preconfiguredBehaviour = nullptr;
    controller->init({cfg3});

    EXPECT_TRUE(b1->granterRegistered != nullptr);
    EXPECT_TRUE(b2->granterRegistered != nullptr);
}

TEST_F(MelodyControllerTest, OnBuzzerRequestNoCurrentHolderGranted) {
    auto requester = std::make_shared<MockIResourceRequester>();
    drivers::knx::MelodyConfig cfg;
    priorityStrategy->grantResult = true;

    bool result = controller->onBuzzerRequest(requester.get(), cfg);
    EXPECT_TRUE(result);
}

TEST_F(MelodyControllerTest, OnBuzzerRequestNoCurrentHolderDenied) {
    auto requester = std::make_shared<MockIResourceRequester>();
    drivers::knx::MelodyConfig cfg;
    priorityStrategy->grantResult = false;

    bool result = controller->onBuzzerRequest(requester.get(), cfg);
    EXPECT_FALSE(result);
}

TEST_F(MelodyControllerTest, OnBuzzerRequestSameRequesterAsCurrentHolderReturnsTrue) {
    auto requester = std::make_shared<MockIResourceRequester>();
    drivers::knx::MelodyConfig cfg;
    priorityStrategy->grantResult = true;

    controller->onBuzzerRequest(requester.get(), cfg);
    bool result = controller->onBuzzerRequest(requester.get(), cfg);
    EXPECT_TRUE(result);
}

TEST_F(MelodyControllerTest, OnBuzzerRequestDifferentRequesterGrantedRevokesOldHolder) {
    auto requester1 = std::make_shared<MockIResourceRequester>();
    auto requester2 = std::make_shared<MockIResourceRequester>();
    drivers::knx::MelodyConfig cfg;
    priorityStrategy->grantResult = true;

    controller->onBuzzerRequest(requester1.get(), cfg);
    bool result = controller->onBuzzerRequest(requester2.get(), cfg);

    EXPECT_TRUE(result);
    EXPECT_TRUE(requester1->revokeCalled);
}

TEST_F(MelodyControllerTest, OnBuzzerReleaseClearsHolder) {
    auto requester = std::make_shared<MockIResourceRequester>();
    drivers::knx::MelodyConfig cfg;
    priorityStrategy->grantResult = true;

    controller->onBuzzerRequest(requester.get(), cfg);
    controller->onBuzzerRelease(requester.get());

    priorityStrategy->grantResult = false;
    bool result = controller->onBuzzerRequest(requester.get(), cfg);
    EXPECT_FALSE(result);
}

TEST_F(MelodyControllerTest, OnBuzzerReleaseWhenAlreadyNullNoCrash) {
    EXPECT_NO_THROW(controller->onBuzzerRelease(nullptr));
}
