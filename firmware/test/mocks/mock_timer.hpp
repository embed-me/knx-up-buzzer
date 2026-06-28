#pragma once
#include "src/drivers/timer/ITimer.hpp"
#include <functional>

struct MockITimer : public drivers::timer::ITimer
{
    uint32_t lastTimeoutUs = 0;
    drivers::timer::TimerMode lastMode = drivers::timer::TimerMode::SINGLE_SHOT;
    bool startCalled = false;
    bool stopCalled = false;
    std::function<void(void*)> handler;
    void *handlerArg = nullptr;

    void setupInterruptHandler(std::function<void(void*)> h, void *arg) override {
        handler = h;
        handlerArg = arg;
    }
    void start(uint32_t timeout_us, drivers::timer::TimerMode mode) override {
        lastTimeoutUs = timeout_us;
        lastMode = mode;
        startCalled = true;
    }
    void stop() override { stopCalled = true; startCalled = false; }

    void simulateTimerInterrupt() {
        if (handler) {
            handler(handlerArg);
        }
    }
};
