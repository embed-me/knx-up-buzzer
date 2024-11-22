#include "MelodyController.hpp"

using namespace application;
using namespace application::behaviour;

MelodyController::MelodyController(
    std::shared_ptr<application::priority::IPriority> priorityStrategy,
    std::shared_ptr<application::behaviour::BehaviourFactory> behaviourFactory) :
    priorityStrategy(priorityStrategy),
    behaviourFactory(behaviourFactory)
{
}

bool MelodyController::onBuzzerRequest(IResourceRequester *requester, struct drivers::knx::MelodyConfig melodyCfg)
{
    if (requester == currentResourceHolder) {
        return true;
    }

    logTrace("buzzer request");

    bool isGranted = priorityStrategy->isGranted(melodyCfg);
    if (isGranted) {
        if (currentResourceHolder) {
            currentResourceHolder->revokeResourceAccess();
        }
        currentResourceHolder = requester;
    }

    return isGranted;
}

void MelodyController::onBuzzerRelease(IResourceRequester *requester)
{
    currentResourceHolder = nullptr;
}

void MelodyController::init(std::list<drivers::knx::MelodyConfig> melodyCfgs)
{
    saveSupportedBehaviours(melodyCfgs);
    registerObservers();
}

void MelodyController::saveSupportedBehaviours(std::list<drivers::knx::MelodyConfig> melodyCfgs)
{
    for (auto melodyCfg : melodyCfgs) {
        if (auto behaviour = this->behaviourFactory->getBehaviour(melodyCfg)) {
            behaviours.push_back(*behaviour);
        }
    }
}

void MelodyController::registerObservers()
{
    for (auto behaviour : behaviours) {
        behaviour->registerResourceGranter(this);
    }
}