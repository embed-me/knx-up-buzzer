#ifndef _ARDUINO_BUZZER_DRIVER_HPP_
#define _ARDUINO_BUZZER_DRIVER_HPP_

#include "../IBuzzerDriver.hpp"
#include "../IBuzzerChangeObserver.hpp"

#include "src/drivers/ITimerDriverFactory.hpp"

#include <memory>

namespace drivers::audio {

class ArduinoBuzzerDriver : public IBuzzerDriver
{
public:
    ArduinoBuzzerDriver(const drivers::gpio::GpioConfig &gpio, 
                        std::shared_ptr<drivers::ITimerDriverFactory> timerFactory);
    ~ArduinoBuzzerDriver() = default;

    virtual void play(const drivers::knx::Melody &melody) override;
    virtual void registerBuzzerChangeObserver(IBuzzerChangeObserver *observer) override;
    virtual bool isPlaying();
    virtual void stop() override;

private:
    void notifyObservers(enum BuzzerStatus status);
    static void timerInterrupt(void *arg);

    std::shared_ptr<drivers::timer::ITimer> timer = nullptr;

    const drivers::gpio::GpioConfig &gpio;

    IBuzzerChangeObserver *observer = nullptr;
};

}

#endif //_ARDUINO_BUZZER_DRIVER_HPP_