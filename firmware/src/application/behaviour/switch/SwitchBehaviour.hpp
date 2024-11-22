#ifndef _SWITCH_BEHAVIOUR_HPP_
#define _SWITCH_BEHAVIOUR_HPP_

#include "../BehaviourBase.hpp"

#include "src/drivers/knx/IKnxDriver.hpp"
#include "src/drivers/buzzer/IBuzzerDriver.hpp"
#include "src/drivers/ITimerDriverFactory.hpp"
#include "src/drivers/timer/ITimer.hpp"
#include "src/utils/scheduler/Scheduler.hpp"

#include "src/drivers/knx/data/modes/SwitchMode.hpp"

#include <memory>

namespace application::behaviour {

class SwitchBehaviour : public BehaviourBase
{
public:
    SwitchBehaviour(std::shared_ptr<drivers::knx::IKnxDriver> knxDriver, 
                    std::shared_ptr<drivers::ITimerDriverFactory> timerFactory,
                    std::shared_ptr<drivers::audio::IBuzzerDriver> buzzer);
    ~SwitchBehaviour() = default;

    virtual bool isSupported(const drivers::knx::IMode &mode) const override;
    virtual bool init(struct drivers::knx::MelodyConfig melodyCfg) override;

    virtual void melodyStop() override;

private:
    uint32_t secondsToUseconds(uint32_t seconds);
    static void timerInterrupt(void *arg);
    void registerKnxEventCallback();

    static const uint32_t USEC_PER_SEC = 1000000;

    bool isSwitchedOn = false;

    IResourceGranter *granter = nullptr;

    std::shared_ptr<drivers::knx::IKnxDriver> knxDriver = nullptr;
    std::shared_ptr<drivers::ITimerDriverFactory> timerFactory = nullptr;
    std::shared_ptr<drivers::timer::ITimer> timer = nullptr;

    drivers::knx::MelodyConfig melodyCfg;
    std::shared_ptr<const drivers::knx::SwitchMode> mode = nullptr;
};

}

#endif //_SWITCH_BEHAVIOUR_HPP_