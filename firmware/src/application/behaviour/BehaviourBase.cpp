#include "BehaviourBase.hpp"

using namespace application::behaviour;

BehaviourBase::BehaviourBase(std::shared_ptr<drivers::audio::IBuzzerDriver> buzzer) : 
    buzzer(buzzer)
{
}

void BehaviourBase::registerResourceGranter(IResourceGranter *granter)
{
    this->granter = granter;
}

void BehaviourBase::revokeResourceAccess()
{
    buzzer->stop();
    granter->onBuzzerRelease(this);
}

bool BehaviourBase::tryPlayMelody(IResourceRequester *requester, struct drivers::knx::MelodyConfig melodyCfg)
{
    bool isGranted = false;
    
    if (granter) {
        isGranted = granter->onBuzzerRequest(this, melodyCfg);
        if (isGranted) {
            buzzer->play(melodyCfg.melody);
        }
    }

    return isGranted;
}
