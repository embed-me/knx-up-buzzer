#ifndef _MELODY_CONTROLLER_HPP_
#define _MELODY_CONTROLLER_HPP_

#include "behaviour/IBehaviour.hpp"
#include "IResourceGranter.hpp"
#include "IResourceRequester.hpp"

#include "behaviour/BehaviourFactory.hpp"
#include "priority/IPriority.hpp"

#include "src/drivers/knx/data/MelodyConfig.hpp"
#include "src/drivers/logger/ILogger.hpp"


#include <memory>
#include <list>

namespace application {

class MelodyController : public behaviour::IResourceGranter
{
public:
    MelodyController(std::shared_ptr<application::priority::IPriority> priorityStrategy,
                     std::shared_ptr<application::behaviour::BehaviourFactory> behaviourFactory);
    ~MelodyController() = default;

    virtual bool onBuzzerRequest(application::behaviour::IResourceRequester *requester, struct drivers::knx::MelodyConfig melodyCfg) override;
    virtual void onBuzzerRelease(application::behaviour::IResourceRequester *requester) override;

    void init(std::list<drivers::knx::MelodyConfig> melodyCfgs);

private:
    void saveSupportedBehaviours(std::list<drivers::knx::MelodyConfig> melodyCfgs);
    void registerObservers();

    static const uint8_t LOWEST_PRIORITY = UINT8_MAX;

    std::shared_ptr<priority::IPriority> priorityStrategy = nullptr;
    std::shared_ptr<behaviour::BehaviourFactory> behaviourFactory = nullptr;
    std::list<drivers::knx::MelodyConfig> melodyCfgs;

    std::list<std::shared_ptr<behaviour::IBehaviour>> behaviours;
    behaviour::IResourceRequester *currentResourceHolder = nullptr;
};

}

#endif //_MELODY_CONTROLLER_HPP_