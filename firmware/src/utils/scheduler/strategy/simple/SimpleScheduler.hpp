#ifndef _SIMPLE_SCHEDULER_HPP_
#define _SIMPLE_SCHEDULER_HPP_

#include "../ISchedulerStrategy.hpp"

#include <queue>
#include <functional>

namespace utils {

class SimpleScheduler : public ISchedulerStrategy
{
public:
    SimpleScheduler();
    ~SimpleScheduler() = default;

    virtual void process() override;
    virtual void schedule(std::function<void(void*)>) override;

private:
    std::queue<std::function<void(void*)>> queue;
};

}

#endif //_SIMPLE_SCHEDULER_HPP_