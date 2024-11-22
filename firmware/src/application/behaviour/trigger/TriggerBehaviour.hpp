#ifndef _TRIGGER_BEHAVIOUR_HPP_
#define _TRIGGER_BEHAVIOUR_HPP_

#include "../BehaviourBase.hpp"

#include "src/drivers/knx/IKnxDriver.hpp"
#include "src/drivers/buzzer/IBuzzerDriver.hpp"
#include "src/drivers/knx/data/modes/TriggerMode.hpp"
#include "src/drivers/logger/Logger.hpp"

#include <memory>

namespace application::behaviour {

class TriggerBehaviour : public BehaviourBase
{
public:
    TriggerBehaviour(std::shared_ptr<drivers::knx::IKnxDriver> knxDriver,
                     std::shared_ptr<drivers::audio::IBuzzerDriver> buzzer);
    ~TriggerBehaviour() = default;

    virtual bool isSupported(const drivers::knx::IMode &mode) const override;
    virtual bool init(struct drivers::knx::MelodyConfig melodyCfg) override;

    virtual void melodyStop() override;

private:
    void registerKnxEventCallback();

    std::shared_ptr<drivers::knx::IKnxDriver> knxDriver = nullptr;

    drivers::knx::MelodyConfig melodyCfg;
    std::shared_ptr<const drivers::knx::TriggerMode> mode = nullptr;
};

}

#endif //_TRIGGER_BEHAVIOUR_HPP_