#include "ArduinoBuzzerDriver.hpp"

#include <Arduino.h>
#include <PlayRtttl.hpp>
#include "src/utils/scheduler/Scheduler.hpp"
#include "src/drivers/logger/Logger.hpp"

using namespace drivers::audio;
using namespace drivers::logger;

ArduinoBuzzerDriver::ArduinoBuzzerDriver(const drivers::gpio::GpioConfig &gpio,
                                         std::shared_ptr<drivers::ITimerDriverFactory> timerFactory) :
    gpio(gpio),
    timer(timerFactory->getTimer())
{
    timer->setupInterruptHandler(timerInterrupt, this);
}

void ArduinoBuzzerDriver::play(const drivers::knx::Melody &melody)
{
    char melodyName[32];
    getRtttlName(melody.rtttl, melodyName, sizeof(melodyName)-1);
    logInfo("Playing Melody %s",  melodyName);
    
    startPlayRtttl(gpio.pin, melody.rtttl, NULL);
    timer->start(100, drivers::timer::TimerMode::RECURRING);

    notifyObservers(BuzzerStatus::STATUS_ON);
}

void ArduinoBuzzerDriver::registerBuzzerChangeObserver(IBuzzerChangeObserver *observer)
{
    this->observer = observer;
}

bool ArduinoBuzzerDriver::isPlaying()
{
    return isPlayRtttlRunning();
}

void ArduinoBuzzerDriver::stop()
{
    logInfo("Stopping Melody");

    stopPlayRtttl();
    timer->stop();
    
    notifyObservers(BuzzerStatus::STATUS_OFF);
}

void ArduinoBuzzerDriver::notifyObservers(enum BuzzerStatus status)
{
    if (observer) {
        observer->onBuzzerChange(status);
    }
}

void ArduinoBuzzerDriver::timerInterrupt(void *arg)
{
    ArduinoBuzzerDriver *instance = static_cast<ArduinoBuzzerDriver*>(arg);
    if (instance == nullptr) {
        return;
    }

    bool isStillPlaying = updatePlayRtttl();
    if (!isStillPlaying) {
        // TODO: This is not ideal, the timer might fire multiple times until
        // the scheduler processes the timer->stop(), therefore resulting in multiple
        // scheduled stops.
        utils::Scheduler::schedule([instance](void*) {
            if (instance) {
                instance->stop();
            }
        });
    }
}
