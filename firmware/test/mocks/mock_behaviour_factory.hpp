#pragma once
#include "src/application/behaviour/BehaviourFactory.hpp"

struct MockBehaviourFactory : public application::behaviour::BehaviourFactory
{
    MockBehaviourFactory()
        : BehaviourFactory(nullptr, nullptr, nullptr) {}

    std::optional<std::shared_ptr<application::behaviour::IBehaviour>> getBehaviour(
        struct drivers::knx::MelodyConfig melodyCfg) override
    {
        lastMelodyCfg = melodyCfg;
        getBehaviourCalledCount++;
        if (!preconfiguredBehaviour) {
            return std::nullopt;
        }
        return preconfiguredBehaviour;
    }

    drivers::knx::MelodyConfig lastMelodyCfg;
    std::shared_ptr<application::behaviour::IBehaviour> preconfiguredBehaviour = nullptr;
    int getBehaviourCalledCount = 0;
};
