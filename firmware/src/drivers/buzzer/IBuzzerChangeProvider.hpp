#ifndef _I_BUZZER_CHANGE_PROVIDER_HPP_
#define _I_BUZZER_CHANGE_PROVIDER_HPP_

#include "src/drivers/knx/data/MelodyConfig.hpp"
#include "IBuzzerChangeObserver.hpp"

namespace drivers::audio {

class IBuzzerChangeProvider
{
public:
    ~IBuzzerChangeProvider() = default;

    virtual void registerBuzzerChangeObserver(IBuzzerChangeObserver *observer) = 0;
};

}

#endif //_I_BUZZER_CHANGE_PROVIDER_HPP_