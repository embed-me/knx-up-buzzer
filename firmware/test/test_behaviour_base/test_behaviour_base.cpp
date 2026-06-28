#include <gtest/gtest.h>
#include "src/application/behaviour/BehaviourBase.hpp"
#include "test/mocks/mock_resource_granter.hpp"
#include "test/mocks/mock_resource_requester.hpp"
#include "test/mocks/mock_buzzer_driver.hpp"
#include "src/application/behaviour/BehaviourBase.cpp"

using namespace application::behaviour;

class TestableBehaviourBase : public BehaviourBase {
public:
    using BehaviourBase::BehaviourBase;
    using BehaviourBase::tryPlayMelody;

    void melodyStop() override {}

    bool isSupported(const drivers::knx::IMode &mode) const override {
        (void)mode;
        return false;
    }

    bool init(struct drivers::knx::MelodyConfig melodyCfg) override {
        (void)melodyCfg;
        return false;
    }
};

class BehaviourBaseTest : public ::testing::Test {
protected:
    void SetUp() override {
        buzzer = std::make_shared<MockIBuzzerDriver>();
        behaviour = std::make_shared<TestableBehaviourBase>(buzzer);
        granter = std::make_shared<MockIResourceGranter>();
    }

    std::shared_ptr<MockIBuzzerDriver> buzzer;
    std::shared_ptr<TestableBehaviourBase> behaviour;
    std::shared_ptr<MockIResourceGranter> granter;
};

TEST_F(BehaviourBaseTest, RegisterResourceGranter) {
    behaviour->registerResourceGranter(granter.get());

    drivers::knx::MelodyConfig cfg;
    auto requester = std::make_shared<MockIResourceRequester>();

    granter->grantResult = true;
    bool result = behaviour->tryPlayMelody(requester.get(), cfg);

    EXPECT_TRUE(result);
    EXPECT_TRUE(granter->onBuzzerRequestCalled);
    EXPECT_TRUE(buzzer->playCalled);
}

TEST_F(BehaviourBaseTest, TryPlayMelodyGranterGrants) {
    behaviour->registerResourceGranter(granter.get());
    granter->grantResult = true;

    drivers::knx::MelodyConfig cfg;
    cfg.melody.rtttl = "test";
    auto requester = std::make_shared<MockIResourceRequester>();

    bool result = behaviour->tryPlayMelody(requester.get(), cfg);

    EXPECT_TRUE(result);
    EXPECT_TRUE(granter->onBuzzerRequestCalled);
    EXPECT_EQ(granter->lastRequester, behaviour.get());
    EXPECT_TRUE(buzzer->playCalled);
}

TEST_F(BehaviourBaseTest, TryPlayMelodyGranterDenies) {
    behaviour->registerResourceGranter(granter.get());
    granter->grantResult = false;

    drivers::knx::MelodyConfig cfg;
    auto requester = std::make_shared<MockIResourceRequester>();

    bool result = behaviour->tryPlayMelody(requester.get(), cfg);

    EXPECT_FALSE(result);
    EXPECT_TRUE(granter->onBuzzerRequestCalled);
    EXPECT_FALSE(buzzer->playCalled);
}

TEST_F(BehaviourBaseTest, TryPlayMelodyNoGranterRegistered) {
    drivers::knx::MelodyConfig cfg;
    auto requester = std::make_shared<MockIResourceRequester>();

    bool result = behaviour->tryPlayMelody(requester.get(), cfg);

    EXPECT_FALSE(result);
    EXPECT_FALSE(buzzer->playCalled);
}

TEST_F(BehaviourBaseTest, RevokeResourceAccess) {
    behaviour->registerResourceGranter(granter.get());
    behaviour->revokeResourceAccess();

    EXPECT_TRUE(buzzer->stopCalled);
    EXPECT_TRUE(granter->onBuzzerReleaseCalled);
    EXPECT_EQ(granter->lastRequester, behaviour.get());
}
