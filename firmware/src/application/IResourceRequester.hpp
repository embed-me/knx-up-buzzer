#ifndef _I_RESOURCE_REQUESTER_HPP_
#define _I_RESOURCE_REQUESTER_HPP_

#include "src/drivers/knx/data/MelodyConfig.hpp"

namespace application::behaviour {

class IResourceGranter;

class IResourceRequester
{
public:
    ~IResourceRequester() = default;

    virtual void registerResourceGranter(IResourceGranter *granter) = 0;
    virtual void revokeResourceAccess() = 0;
};

}

#endif //_I_RESOURCE_REQUESTER_HPP_