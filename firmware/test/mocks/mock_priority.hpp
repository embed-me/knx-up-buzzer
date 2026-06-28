#pragma once
#include "src/application/priority/IPriority.hpp"

struct MockIPriority : public application::priority::IPriority
{
    drivers::knx::MelodyConfig lastMelodyCfg;
    bool grantResult = true;

    bool isGranted(struct drivers::knx::MelodyConfig melodyCfg) const override {
        const_cast<MockIPriority*>(this)->lastMelodyCfg = melodyCfg;
        return grantResult;
    }
};
