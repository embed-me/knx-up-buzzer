#ifndef _BEHAVIOUR_BASE_HPP_
#define _BEHAVIOUR_BASE_HPP_

#include "IBehaviour.hpp"
#include "../IResourceGranter.hpp"

#include "src/drivers/knx/IKnxDriver.hpp"
#include "src/drivers/buzzer/IBuzzerDriver.hpp"
#include "src/drivers/knx/data/modes/TriggerMode.hpp"
#include "src/drivers/logger/ILogger.hpp"

#include <memory>

namespace application::behaviour {

class BehaviourBase : public IBehaviour
{
public:
    BehaviourBase(std::shared_ptr<drivers::audio::IBuzzerDriver> buzzer);
    ~BehaviourBase() = default;

    virtual void registerResourceGranter(IResourceGranter *granter) final override;
    virtual void revokeResourceAccess() final override;

    virtual void melodyStop() = 0;
    
protected:    
    bool tryPlayMelody(IResourceRequester *requester, struct drivers::knx::MelodyConfig melodyCfg);

private:
    IResourceGranter *granter = nullptr;

    std::shared_ptr<drivers::audio::IBuzzerDriver> buzzer = nullptr;
};

}

#endif //_BEHAVIOUR_BASE_HPP_