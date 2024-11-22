#ifndef _NO_PRIORITY_HPP_
#define _NO_PRIORITY_HPP_

#include "../IPriority.hpp"

#include "src/drivers/knx/data/MelodyConfig.hpp"


namespace application::priority {

class NoPriority : public IPriority
{
public:
    NoPriority() = default;
    ~NoPriority() = default;

    virtual bool isGranted(struct drivers::knx::MelodyConfig melodyCfg) const override;
};

}

#endif //_NO_PRIORITY_HPP_