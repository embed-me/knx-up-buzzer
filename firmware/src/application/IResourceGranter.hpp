#ifndef _I_RESOURCE_GRANTER_HPP_
#define _I_RESOURCE_GRANTER_HPP_

#include "src/drivers/knx/data/MelodyConfig.hpp"

namespace application::behaviour {


class IResourceRequester;


class IResourceGranter
{
public:
    ~IResourceGranter() = default;

    /*
     * It is up to the observer to determine if the request to the buzzer resource should be granted or not.
     * The behaviour class can decide based on the return value how to proceed.
     * 
     * Return: 
     *  true:  behaviour change was granted from observer
     *  false: behaviour change was denied from observer
     */
    virtual bool onBuzzerRequest(IResourceRequester *requester, struct drivers::knx::MelodyConfig melodyCfg) = 0;

    /*
     * It is up to the callers responsibility to notify the Granter about the release of the resource.
     * If not release the Granter must be able to release it any time using revoceResourceAccess().
     */
    virtual void onBuzzerRelease(IResourceRequester *requester) = 0;
};

}

#endif //_I_RESOURCE_GRANTER_HPP_