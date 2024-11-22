
#include "TriggerBehaviour.hpp"

using namespace application::behaviour;
using namespace drivers::logger;

TriggerBehaviour::TriggerBehaviour(
    std::shared_ptr<drivers::knx::IKnxDriver> knxDriver,
    std::shared_ptr<drivers::audio::IBuzzerDriver> buzzer) :
    knxDriver(knxDriver),
    BehaviourBase(buzzer)
{
}

bool TriggerBehaviour::isSupported(const drivers::knx::IMode &mode) const
{
    return (mode.getType() == drivers::knx::ModeType::MODE_TRIGGER);
}

bool TriggerBehaviour::init(struct drivers::knx::MelodyConfig melodyCfg)
{
    if (!isSupported(*melodyCfg.mode)) {
        logError("Mode NOT supported");
        return true;
    }

    this->melodyCfg = melodyCfg;
    this->mode = std::static_pointer_cast<const drivers::knx::TriggerMode>(melodyCfg.mode);

    registerKnxEventCallback();
    
    return false;
}

void TriggerBehaviour::melodyStop()
{
}

void TriggerBehaviour::registerKnxEventCallback()
{
    knxDriver->register_cb(mode->ko_number, [this](GroupObject &go) {
        KNXValue trigger_value = go.value(DPT_Trigger);

        logTrace("KO %d value %d", mode->ko_number, (bool)trigger_value);

        if (!(bool)trigger_value) {
            logTrace("trigger value 0, no action");
            return;
        }

        bool isGranted = tryPlayMelody(this, this->melodyCfg);
        if (!isGranted) {
            logTrace("request to play melody denied");
        }
    });
}