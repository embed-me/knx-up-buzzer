#ifndef _VENTING_MONITOR_BEHAVIOUR_HPP_
#define _VENTING_MONITOR_BEHAVIOUR_HPP_

#include "../BehaviourBase.hpp"

#include "src/drivers/timer/ITimer.hpp"
#include "src/drivers/knx/IKnxDriver.hpp"
#include "src/drivers/buzzer/IBuzzerDriver.hpp"
#include "src/drivers/ITimerDriverFactory.hpp"
#include "src/drivers/timer/ITimer.hpp"
#include "src/drivers/logger/ILogger.hpp"
#include "src/utils/scheduler/Scheduler.hpp"

#include "src/drivers/knx/data/modes/SwitchMode.hpp"

#include <memory>

namespace application::behaviour {

class VentingMonitorBehaviour : public BehaviourBase
{
public:
    VentingMonitorBehaviour(std::shared_ptr<drivers::knx::IKnxDriver> knxDriver, 
                            std::shared_ptr<drivers::ITimerDriverFactory> timerFactory,
                            std::shared_ptr<drivers::audio::IBuzzerDriver> buzzer);
    ~VentingMonitorBehaviour() = default;

    virtual bool isSupported(const drivers::knx::IMode &mode) const override;
    virtual bool init(struct drivers::knx::MelodyConfig melodyCfg) override;

    virtual void melodyStop() override;

private:
    uint32_t secondsToUseconds(uint32_t seconds);
    uint32_t minutesToUseconds(uint32_t minutes);
    static void melodyTimerInterrupt(void *arg);
    static void ventingTimerInterrupt(void *arg);
    void registerKnxEventCallback();

    static const uint32_t USEC_PER_SEC = 1000000;
    static const uint32_t SEC_PER_MIN = 60;

    bool isVentingOn = false;
    bool isTemperatureInRange = false;

    IResourceGranter *granter = nullptr;

    std::shared_ptr<drivers::knx::IKnxDriver> knxDriver = nullptr;
    std::shared_ptr<drivers::ITimerDriverFactory> timerFactory = nullptr;
    std::shared_ptr<drivers::timer::ITimer> ventingTimer = nullptr;
    std::shared_ptr<drivers::timer::ITimer> melodyTimer = nullptr;

    drivers::knx::MelodyConfig melodyCfg;
    std::shared_ptr<const drivers::knx::VentingMonitorMode> mode = nullptr;
};

}

#endif //_VENTING_MONITOR_BEHAVIOUR_HPP_