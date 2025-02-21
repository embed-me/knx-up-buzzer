
#include "SwitchBehaviour.hpp"
#include "src/drivers/logger/Logger.hpp"

using namespace application::behaviour;
using namespace drivers::logger;

SwitchBehaviour::SwitchBehaviour(
    std::shared_ptr<drivers::knx::IKnxDriver> knxDriver, 
    std::shared_ptr<drivers::ITimerDriverFactory> timerFactory,
    std::shared_ptr<drivers::audio::IBuzzerDriver> buzzer) :
    knxDriver(knxDriver),
    timerFactory(timerFactory),
    BehaviourBase(buzzer),
    timer(timerFactory->getTimer())
{
    timer->setupInterruptHandler(timerInterrupt, this);
}

bool SwitchBehaviour::isSupported(const drivers::knx::IMode &mode) const
{
    return (mode.getType() == drivers::knx::ModeType::MODE_SWITCH);
}

bool SwitchBehaviour::init(struct drivers::knx::MelodyConfig melodyCfg)
{
    if (!isSupported(*melodyCfg.mode)) {
        logError("Mode NOT supported");
        return true;
    }

    this->melodyCfg = melodyCfg;
    this->mode = std::static_pointer_cast<const drivers::knx::SwitchMode>(melodyCfg.mode);

    registerKnxEventCallback();
    
    return false;
}

void SwitchBehaviour::melodyStop()
{
    timer->stop();
    isSwitchedOn = false;
}

uint32_t SwitchBehaviour::secondsToUseconds(uint32_t seconds)
{
    return (seconds * USEC_PER_SEC);
}

void SwitchBehaviour::timerInterrupt(void *arg)
{
    auto instance = static_cast<SwitchBehaviour*>(arg);

    utils::Scheduler::schedule([instance](void*) {
        if (instance && instance->isSwitchedOn) {
            bool isPlaying = instance->tryPlayMelody(instance, instance->melodyCfg);
            if (!isPlaying) {
                instance->melodyStop();
            }
        }
    });
}

void SwitchBehaviour::registerKnxEventCallback()
{
    knxDriver->register_cb(mode->ko_number, [this](GroupObject &go) {
        KNXValue switchOn = go.value(DPT_Switch);

        logTrace("KO %d value %d", mode->ko_number, (bool)switchOn);

        if ((bool)switchOn == isSwitchedOn) {
            logTrace("State unchanged, no action");
            return;
        }

        if (switchOn) {
            bool isPlaying = tryPlayMelody(this, this->melodyCfg);
            if (isPlaying) {
                isSwitchedOn = true;
                timer->start(secondsToUseconds(melodyCfg.melody.pause_sec), drivers::timer::TimerMode::RECURRING);
            }
        } else {
            melodyStop();
        }
    });
}
