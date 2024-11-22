#ifndef _I_BUZZER_DRIVER_FACTORY_HPP_
#define _I_BUZZER_DRIVER_FACTORY_HPP_

#include "buzzer/IBuzzerDriver.hpp"

#include <memory>

namespace drivers {

class IBuzzerDriverFactory
{
public:
    ~IBuzzerDriverFactory() = default;

    virtual std::shared_ptr<audio::IBuzzerDriver> getBuzzerDriver(const drivers::gpio::GpioConfig &gpio) = 0;
};

}

#endif //_I_BUZZER_DRIVER_FACTORY_HPP_




