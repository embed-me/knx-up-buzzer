#ifndef _I_BUZZER_CHANGE_OBSERVER_HPP_
#define _I_BUZZER_CHANGE_OBSERVER_HPP_

#include "src/drivers/knx/data/MelodyConfig.hpp"

namespace drivers::audio {

enum BuzzerStatus {
    STATUS_OFF = 0,
    STATUS_ON = 1
};

class IBuzzerChangeObserver
{
public:
    ~IBuzzerChangeObserver() = default;

    /*
     * Called whenever the buzzer state changes.
     */
    virtual void onBuzzerChange(enum BuzzerStatus status) = 0;
};

}

#endif //_I_BUZZER_CHANGE_OBSERVER_HPP_