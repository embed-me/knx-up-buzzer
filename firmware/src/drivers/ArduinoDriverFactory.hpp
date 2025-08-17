#ifndef _ARDUINO_DRIVER_FACTORY_HPP_
#define _ARDUINO_DRIVER_FACTORY_HPP_

#include "IDriverFactory.hpp"

#include "timer/ITimer.hpp"
#include "gpio/IGpioDriver.hpp"
#include "buzzer/IBuzzerDriver.hpp"
#include "watchdog/IWatchdogDriver.hpp"

#include "logger/ILogger.hpp"
#include "gpio/GpioConfig.hpp"

#include <memory>

namespace drivers {

class ArduinoDriverFactory : public IDriverFactory,
                             public std::enable_shared_from_this<ArduinoDriverFactory>
{
public:
    ArduinoDriverFactory();
    ~ArduinoDriverFactory() = default;

    virtual std::shared_ptr<timer::ITimer> getTimer() override;
    virtual std::shared_ptr<gpio::IGpioDriver> getGpioDriver() override;
    virtual std::shared_ptr<logger::ILogger> getLoggerDriver() override;
    virtual std::shared_ptr<audio::IBuzzerDriver> getBuzzerDriver(const drivers::gpio::GpioConfig &gpio) override;
    virtual std::shared_ptr<knx::IKnxDriver> getKnxDriver() override;
    virtual std::shared_ptr<watchdog::IWatchdogDriver> getWatchdogDriver() override;
private:
};

}

#endif //_ARDUINO_DRIVER_FACTORY_HPP_




