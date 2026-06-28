#pragma once
#include "src/utils/scheduler/strategy/ISchedulerStrategy.hpp"
#include <functional>

struct MockISchedulerStrategy : public utils::ISchedulerStrategy
{
    std::function<void(void*)> lastTask;
    bool processCalled = false;

    void process() override { processCalled = true; }

    void schedule(std::function<void(void*)> task) override {
        lastTask = task;
    }
};
