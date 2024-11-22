#include "BehaviourFactory.hpp"

using namespace application::behaviour;

BehaviourFactory::BehaviourFactory(
    std::shared_ptr<drivers::knx::IKnxDriver> knxDriver, 
    std::shared_ptr<drivers::ITimerDriverFactory> timerFactory,
    std::shared_ptr<drivers::audio::IBuzzerDriver> buzzer) :
    knxDriver(knxDriver),
    timerFactory(timerFactory),
    buzzer(buzzer)
{
}

std::optional<std::shared_ptr<IBehaviour>> BehaviourFactory::getBehaviour(struct drivers::knx::MelodyConfig melodyCfg)
{
    std::shared_ptr<TriggerBehaviour> triggerBehaviour = std::make_shared<TriggerBehaviour>(knxDriver, buzzer);
    if (triggerBehaviour->isSupported(*melodyCfg.mode)) {
        triggerBehaviour->init(melodyCfg);
        return triggerBehaviour;
    }
    
    std::shared_ptr<SwitchBehaviour> switchBehaviour = std::make_shared<SwitchBehaviour>(knxDriver, timerFactory, buzzer);
    if (switchBehaviour->isSupported(*melodyCfg.mode)) {
        switchBehaviour->init(melodyCfg);
        return switchBehaviour;
    } 

    std::shared_ptr<VentingMonitorBehaviour> ventingMonitorBehaviour = std::make_shared<VentingMonitorBehaviour>(knxDriver, timerFactory, buzzer);
    if (ventingMonitorBehaviour->isSupported(*melodyCfg.mode)) {
        ventingMonitorBehaviour->init(melodyCfg);
        return ventingMonitorBehaviour;
    } 

    return std::nullopt;  //no suitable behaviour found
}