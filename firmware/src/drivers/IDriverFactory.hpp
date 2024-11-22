#ifndef _I_DRIVER_FACTORY_HPP_
#define _I_DRIVER_FACTORY_HPP_

#include "ITimerDriverFactory.hpp"
#include "IGpioDriverFactory.hpp"
#include "IBuzzerDriverFactory.hpp"
#include "IKnxDriverFactory.hpp"
#include "logger/ILogger.hpp"

namespace drivers {

class IDriverFactory : public ITimerDriverFactory,
                       public IGpioDriverFactory,
                       public IBuzzerDriverFactory,
                       public IKnxDriverFactory
{
public:
    ~IDriverFactory() = default;

    virtual std::shared_ptr<logger::ILogger> getLoggerDriver() = 0;
};

}

#endif //_I_DRIVER_FACTORY_HPP_




