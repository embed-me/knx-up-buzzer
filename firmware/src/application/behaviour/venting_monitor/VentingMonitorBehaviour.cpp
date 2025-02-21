
#include "VentingMonitorBehaviour.hpp"

using namespace application::behaviour;

VentingMonitorBehaviour::VentingMonitorBehaviour(
    std::shared_ptr<drivers::knx::IKnxDriver> knxDriver, 
    std::shared_ptr<drivers::ITimerDriverFactory> timerFactory,
    std::shared_ptr<drivers::audio::IBuzzerDriver> buzzer) :
    knxDriver(knxDriver),
    timerFactory(timerFactory),
    BehaviourBase(buzzer),
    ventingTimer(timerFactory->getTimer()),
    melodyTimer(timerFactory->getTimer())
{
    ventingTimer->setupInterruptHandler(ventingTimerInterrupt, this);
    melodyTimer->setupInterruptHandler(melodyTimerInterrupt, this);
}

bool VentingMonitorBehaviour::isSupported(const drivers::knx::IMode &mode) const
{
    return (mode.getType() == drivers::knx::ModeType::MODE_VENTING_MONITOR);
}

bool VentingMonitorBehaviour::init(struct drivers::knx::MelodyConfig melodyCfg)
{
    if (!isSupported(*melodyCfg.mode)) {
        logError("Mode NOT supported");
        return true;
    }

    this->melodyCfg = melodyCfg;
    this->mode = std::static_pointer_cast<const drivers::knx::VentingMonitorMode>(melodyCfg.mode);

    registerKnxEventCallback();
    
    return false;
}

void VentingMonitorBehaviour::melodyStop()
{
    melodyTimer->stop();
}

uint32_t VentingMonitorBehaviour::secondsToUseconds(uint32_t seconds)
{
    return (seconds * USEC_PER_SEC);
}

uint32_t VentingMonitorBehaviour::minutesToUseconds(uint32_t minutes)
{
    return secondsToUseconds(minutes * SEC_PER_MIN);
}

void VentingMonitorBehaviour::melodyTimerInterrupt(void *arg)
{
    auto instance = static_cast<VentingMonitorBehaviour*>(arg);

    utils::Scheduler::schedule([instance](void*) {
        bool isPlaying = false;

        if (!instance) {
            return;
        }

        if (instance->isVentingOn) {
            isPlaying = instance->tryPlayMelody(instance, instance->melodyCfg);
        }
        
        if (!isPlaying) {
            instance->melodyStop();
        }
    });
}

void VentingMonitorBehaviour::ventingTimerInterrupt(void *arg)
{
    auto instance = static_cast<VentingMonitorBehaviour*>(arg);

    utils::Scheduler::schedule([instance](void*) {
        if (!instance) {
            return;
        }

        if (!instance->isTemperatureInRange) {
            logTrace("Ventilation time exceeded, but temperature acceptable");
            instance->melodyStop();
            return;
        }

        bool isPlaying = instance->tryPlayMelody(instance, instance->melodyCfg);
        if (isPlaying) {
            instance->melodyTimer->start(instance->secondsToUseconds(instance->melodyCfg.melody.pause_sec), 
                                         drivers::timer::TimerMode::RECURRING);
        } else {
            instance->melodyStop();
        }
    });
}

void VentingMonitorBehaviour::registerKnxEventCallback()
{
    knxDriver->register_cb(mode->ko_venting, [this](GroupObject &go) {
        KNXValue ventingOn = go.value(DPT_Switch);

        logTrace("KO %d value %d", mode->ko_venting, (bool)ventingOn);

        if ((bool)ventingOn == isVentingOn) {
            logTrace("State unchanged, no action");
            return;
        }

        if (ventingOn) {
            ventingTimer->start(minutesToUseconds(mode->venting_duration_limit_min), drivers::timer::TimerMode::RECURRING);
        } else {
            ventingTimer->stop();
        }
            
        isVentingOn = ventingOn;
    });


    knxDriver->register_cb(mode->ko_temp, [this](GroupObject &go) {
        KNXValue temp = go.value(DPT_Value_Temp);

        logTrace("KO %d value %.2f", mode->ko_temp, (float)temp);

        if (((float)temp < mode->lower_temp_limit_c) ||
            ((float)temp > mode->upper_temp_limit_c)) {
            logTrace("temperature within arming range");
            isTemperatureInRange = true;
        } else {
            isTemperatureInRange = false;
        }
    });
}
