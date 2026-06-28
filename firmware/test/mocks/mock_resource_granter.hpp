#pragma once
#include "src/application/IResourceGranter.hpp"
#include "src/application/IResourceRequester.hpp"

struct MockIResourceGranter : public application::behaviour::IResourceGranter
{
    application::behaviour::IResourceRequester *lastRequester = nullptr;
    drivers::knx::MelodyConfig lastMelodyCfg;
    bool grantResult = true;
    bool onBuzzerRequestCalled = false;
    bool onBuzzerReleaseCalled = false;

    bool onBuzzerRequest(application::behaviour::IResourceRequester *requester,
                         struct drivers::knx::MelodyConfig melodyCfg) override {
        lastRequester = requester;
        lastMelodyCfg = melodyCfg;
        onBuzzerRequestCalled = true;
        return grantResult;
    }

    void onBuzzerRelease(application::behaviour::IResourceRequester *requester) override {
        lastRequester = requester;
        onBuzzerReleaseCalled = true;
    }
};
