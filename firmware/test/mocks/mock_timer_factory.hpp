#pragma once
#include "src/drivers/ITimerDriverFactory.hpp"
#include <memory>
#include <vector>

struct MockITimerDriverFactory : public drivers::ITimerDriverFactory
{
    std::vector<std::shared_ptr<drivers::timer::ITimer>> timers;
    size_t getTimerCalledCount = 0;

    std::shared_ptr<drivers::timer::ITimer> getTimer() override {
        getTimerCalledCount++;
        if (!timers.empty()) {
            size_t idx = getTimerCalledCount - 1;
            if (idx >= timers.size()) idx = timers.size() - 1;
            return timers[idx];
        }
        return nullptr;
    }
};
