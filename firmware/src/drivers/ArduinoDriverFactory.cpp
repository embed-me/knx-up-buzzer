#include "ArduinoDriverFactory.hpp"

#include "timer/arduino/ArduinoTimer.hpp"
#include "gpio/arduino/ArduinoGpioDriver.hpp"
#include "buzzer/arduino/ArduinoBuzzerDriver.hpp"
#include "logger/arduino/ArduinoLogger.hpp"
#include "knx/arduino/ThelsingKnxDriver.hpp"
#include "watchdog/arduino/ArduinoWatchdogDriver.hpp"

#include "logger/Logger.hpp"

using namespace drivers;
using namespace drivers::logger;

ArduinoDriverFactory::ArduinoDriverFactory()
{
}

std::shared_ptr<timer::ITimer> ArduinoDriverFactory::getTimer()
{
    logTrace("creating new Timer");
    auto timer = std::make_shared<timer::ArduinoTimer>();
    return timer;
}

std::shared_ptr<gpio::IGpioDriver> ArduinoDriverFactory::getGpioDriver()
{
    logTrace("creating new Gpio Driver");
    auto gpio = std::make_shared<gpio::ArduinoGpioDriver>();
    return gpio;
}

std::shared_ptr<logger::ILogger> ArduinoDriverFactory::getLoggerDriver()
{
    auto logger = std::make_shared<logger::ArduinoLogger>();
    return logger;
}

std::shared_ptr<audio::IBuzzerDriver> ArduinoDriverFactory::getBuzzerDriver(const drivers::gpio::GpioConfig &gpio)
{
    logTrace("creating new Buzzer Driver");
    auto buzzer = std::make_shared<audio::ArduinoBuzzerDriver>(gpio, shared_from_this());
    return buzzer;
}

std::shared_ptr<knx::IKnxDriver> ArduinoDriverFactory::getKnxDriver()
{
    logTrace("creating new Knx Driver");
    auto knx = std::make_shared<knx::ThelsingKnxDriver>(shared_from_this());
    return knx;
}

std::shared_ptr<watchdog::IWatchdogDriver> ArduinoDriverFactory::getWatchdogDriver()
{
    logTrace("creating new Watchdog Driver");
    auto watchdog = std::make_shared<watchdog::ArduinoWatchdogDriver>();
    return watchdog;
}