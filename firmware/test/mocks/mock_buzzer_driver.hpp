#pragma once
#include "src/drivers/buzzer/IBuzzerDriver.hpp"

struct MockIBuzzerDriver : public drivers::audio::IBuzzerDriver
{
    drivers::knx::Melody lastMelody;
    bool playCalled = false;
    bool stopCalled = false;

    void play(const drivers::knx::Melody &melody) override {
        lastMelody = melody;
        playCalled = true;
    }
    void stop() override { stopCalled = true; }
    bool isPlaying() override { return playCalled && !stopCalled; }
    void registerBuzzerChangeObserver(drivers::audio::IBuzzerChangeObserver*) override {}
};
