#include <memory>
#include <list>

#include "drivers/ArduinoDriverFactory.hpp"

#include "drivers/logger/Logger.hpp"
#include "drivers/logger/ILogger.hpp"

#include "drivers/gpio/IGpioDriver.hpp"

#include "drivers/knx/IKnxDriver.hpp"
#include "drivers/knx/data/KnxConfig.hpp"

#include "drivers/buzzer/IBuzzerDriver.hpp"

#include "drivers/timer/ITimer.hpp"

#include "drivers/watchdog/IWatchdogDriver.hpp"

#include "utils/scheduler/Scheduler.hpp"
#include "utils/scheduler/strategy/ISchedulerStrategy.hpp"
#include "utils/scheduler/strategy/simple/SimpleScheduler.hpp"

#include "src/application/behaviour/IBehaviour.hpp"
#include "src/application/behaviour/BehaviourFactory.hpp"

#include "src/application/priority/IPriority.hpp"
#include "src/application/priority/no_priority/NoPriority.hpp"

#include "application/MelodyController.hpp"

/* GPIOs */
struct drivers::gpio::GpioConfig knxProgButton = {
    .pin = 7,
    .sensitivity = drivers::gpio::GPIO_SENSITIVITY::INTERRUPT_CHANGE,
    .direction = drivers::gpio::GPIO_DIRECTION::DIRECTION_INPUT,
    .pull = drivers::gpio::GPIO_PULL::PULL_HIGH
};

struct drivers::gpio::GpioConfig knxProgLed = {
    .pin = 6,
    .sensitivity = drivers::gpio::GPIO_SENSITIVITY::ACTIVE_HIGH,
    .direction = drivers::gpio::GPIO_DIRECTION::DIRECTION_OUTPUT,
    .pull = drivers::gpio::GPIO_PULL::PULL_NONE
};

struct drivers::gpio::GpioConfig buzzerPin = {
    .pin = 13,
    .sensitivity = drivers::gpio::GPIO_SENSITIVITY::ACTIVE_HIGH,
    .direction = drivers::gpio::GPIO_DIRECTION::DIRECTION_OUTPUT,
    .pull = drivers::gpio::GPIO_PULL::PULL_NONE
};

/* Drivers */
std::shared_ptr<drivers::IDriverFactory> driverFactory = std::make_shared<drivers::ArduinoDriverFactory>();
#ifdef KNX_UP_BUZZER_DISABLE_LOGGING
    std::shared_ptr<drivers::logger::ILogger> logger = nullptr;
#else
    std::shared_ptr<drivers::logger::ILogger> logger = driverFactory->getLoggerDriver();
#endif
std::shared_ptr<drivers::gpio::IGpioDriver> gpio = driverFactory->getGpioDriver();
std::shared_ptr<drivers::audio::IBuzzerDriver> buzzer = driverFactory->getBuzzerDriver(buzzerPin);
std::shared_ptr<drivers::knx::IKnxDriver> knx = driverFactory->getKnxDriver();
std::shared_ptr<drivers::watchdog::IWatchdogDriver> watchdog = driverFactory->getWatchdogDriver();

/* Utilities */
std::shared_ptr<utils::ISchedulerStrategy> schedulerStrategy = std::make_shared<utils::SimpleScheduler>();
std::shared_ptr<utils::Scheduler> scheduler = std::make_shared<utils::Scheduler>(schedulerStrategy);

/* Applications */
std::shared_ptr<application::priority::IPriority> priorityStrategy = std::make_shared<application::priority::NoPriority>();
std::shared_ptr<application::behaviour::BehaviourFactory> behaviourFactory = std::make_shared<application::behaviour::BehaviourFactory>(knx, driverFactory, buzzer);

std::shared_ptr<application::MelodyController> controller = std::make_shared<application::MelodyController>(priorityStrategy, behaviourFactory);

#define WDT_TIMEOUT_MS 8000
#define PROG_BTN_PRESS_MIN_MILLIS 50
#define PROG_BTN_PRESS_MAX_MILLIS 500

void buttonPressHandler(void *arg) 
{
    static uint32_t lastEvent = 0;
    static uint32_t lastPressed = 0;

    uint32_t diff = millis() - lastEvent;

    if (diff >= PROG_BTN_PRESS_MIN_MILLIS && diff <= PROG_BTN_PRESS_MAX_MILLIS)
    {
        if (millis() - lastPressed > 200)
        {
            knx->toggleProgMode();
            lastPressed = millis();
        }
    }

    lastEvent = millis();
}

void buttonInterrupt()
{
    utils::Scheduler::schedule(buttonPressHandler);
}

/* Core 0 */
void setup() {
    //drivers::logger::Logger::setLogger(logger);
    //logger->init(drivers::logger::LOGLEVEL::LOGLEVEL_TRACE, true);

    watchdog->enable(WDT_TIMEOUT_MS);

    gpio->setConfig(knxProgLed);
    gpio->setConfig(knxProgButton);
    gpio->setConfig(buzzerPin);
    gpio->setupInterruptHandler(knxProgButton, buttonInterrupt);

    bool stackConfigured = knx->init();
    knx->setProgmodeChangeCallback([](bool val) {
        gpio->writeValue(knxProgLed, val);
    });

    std::string version = knx->getApplicationVersion();
    logInfo("Application Version: " + version);

    drivers::knx::KnxConfig cfg = knx->getKnxConfig();
    std::list<drivers::knx::MelodyConfig> melodyCfgs = cfg.getMelodyConfigs();
  
    if (stackConfigured) {
        controller->init(melodyCfgs);
    }
}

void loop() {
    watchdog->feed();
    scheduler->process();
}


/* Core 1 */
void setup1() {
}

void loop1() {
}