#pragma once
#include "src/application/behaviour/IBehaviour.hpp"
#include "src/application/IResourceGranter.hpp"

struct MockIBehaviour : public application::behaviour::IBehaviour
{
    bool initCalled = false;
    drivers::knx::MelodyConfig lastMelodyCfg;
    application::behaviour::IResourceGranter *granterRegistered = nullptr;
    bool isSupportedReturn = true;

    bool isSupported(const drivers::knx::IMode &mode) const override {
        return isSupportedReturn;
    }

    bool init(struct drivers::knx::MelodyConfig melodyCfg) override {
        initCalled = true;
        lastMelodyCfg = melodyCfg;
        return false;
    }

    void registerResourceGranter(application::behaviour::IResourceGranter *granter) override {
        granterRegistered = granter;
    }

    void revokeResourceAccess() override {}
};
