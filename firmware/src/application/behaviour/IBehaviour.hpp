#ifndef _I_BEHAVIOUR_HPP_
#define _I_BEHAVIOUR_HPP_

#include "src/drivers/knx/data/IMode.hpp"
#include "src/drivers/knx/data/MelodyConfig.hpp"
#include "../IResourceRequester.hpp"

namespace application::behaviour {

class IBehaviour : public IResourceRequester
{
public:
    ~IBehaviour() = default;

    virtual bool isSupported(const drivers::knx::IMode &mode) const = 0;
    virtual bool init(struct drivers::knx::MelodyConfig melodyCfg) = 0;
};

}

#endif //_I_BEHAVIOUR_HPP_