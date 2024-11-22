#ifndef _I_BUZZER_DRIVER_HPP_
#define _I_BUZZER_DRIVER_HPP_

#include "src/drivers/buzzer/IBuzzerChangeProvider.hpp"

#include "src/drivers/knx/data/Melody.hpp"
#include "src/drivers/gpio/GpioConfig.hpp"

namespace drivers::audio {

class IBuzzerDriver : public IBuzzerChangeProvider
{
public:
    ~IBuzzerDriver() = default;

    virtual void play(const drivers::knx::Melody &melody) = 0;
    virtual void registerBuzzerChangeObserver(IBuzzerChangeObserver *observer) = 0;
    virtual bool isPlaying() = 0;
    virtual void stop() = 0;
private:
};

}

#endif //_I_BUZZER_DRIVER_HPP_