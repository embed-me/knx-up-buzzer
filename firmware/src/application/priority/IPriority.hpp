#ifndef _I_PRIORITY_HPP_
#define _I_PRIORITY_HPP_

#include "src/drivers/knx/data/MelodyConfig.hpp"

namespace application::priority {

class IPriority
{
public:
    ~IPriority() = default;

    virtual bool isGranted(struct drivers::knx::MelodyConfig melodyCfg) const = 0;
};

}

#endif //_I_PRIORITY_HPP_