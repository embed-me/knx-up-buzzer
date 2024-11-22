#ifndef _I_BEHAVIOUR_FACTORY_HPP_
#define _I_BEHAVIOUR_FACTORY_HPP_

#include "src/application/behaviour/IBehaviour.hpp"
#include "src/drivers/knx/IKnxDriver.hpp"
#include "src/drivers/buzzer/IBuzzerDriver.hpp"
#include "src/drivers/ITimerDriverFactory.hpp"
#include "src/drivers/logger/ILogger.hpp"

#include "trigger/TriggerBehaviour.hpp"
#include "switch/SwitchBehaviour.hpp"
#include "venting_monitor/VentingMonitorBehaviour.hpp"

#include <memory>
#include <optional>

namespace application::behaviour {

class BehaviourFactory
{
public:
    BehaviourFactory(std::shared_ptr<drivers::knx::IKnxDriver> knxDriver, 
                     std::shared_ptr<drivers::ITimerDriverFactory> timerFactory,
                     std::shared_ptr<drivers::audio::IBuzzerDriver> buzzer);
    ~BehaviourFactory() = default;

    std::optional<std::shared_ptr<IBehaviour>> getBehaviour(struct drivers::knx::MelodyConfig melodyCfg);

private:
    std::shared_ptr<drivers::knx::IKnxDriver> knxDriver = nullptr; 
    std::shared_ptr<drivers::ITimerDriverFactory> timerFactory = nullptr;
    std::shared_ptr<drivers::audio::IBuzzerDriver> buzzer = nullptr;
};

}

#endif //_I_BEHAVIOUR_FACTORY_HPP_