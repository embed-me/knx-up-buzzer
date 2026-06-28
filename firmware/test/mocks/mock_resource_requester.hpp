#pragma once
#include "src/application/IResourceRequester.hpp"
#include "src/application/IResourceGranter.hpp"

struct MockIResourceRequester : public application::behaviour::IResourceRequester
{
    application::behaviour::IResourceGranter *granter = nullptr;
    bool revokeCalled = false;

    void registerResourceGranter(application::behaviour::IResourceGranter *g) override {
        granter = g;
    }

    void revokeResourceAccess() override {
        revokeCalled = true;
    }
};
