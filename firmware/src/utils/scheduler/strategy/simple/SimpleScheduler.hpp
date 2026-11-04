#ifndef _SIMPLE_SCHEDULER_HPP_
#define _SIMPLE_SCHEDULER_HPP_

#include "../ISchedulerStrategy.hpp"

#include <array>
#include <functional>
#include <stdint.h>

#include "pico/critical_section.h"

namespace utils {

class SimpleScheduler : public ISchedulerStrategy
{
public:
    SimpleScheduler();
    ~SimpleScheduler() = default;

    virtual void process() override;
    virtual void schedule(std::function<void(void*)> work) override;

private:
    static const uint8_t MAX_QUEUE_SIZE = 100;
    static const uint8_t MAX_PER_CYCLE_LIMIT = 10;
    // Fixed-size ring buffer: schedule() runs in interrupt context (timer alarms)
    // and must never allocate, as the malloc lock is re-entrant on the same core.
    std::array<std::function<void(void*)>, MAX_QUEUE_SIZE> queue;
    uint8_t head = 0;   // next element to pop
    uint8_t count = 0;  // number of queued elements

    critical_section_t _cs;
};

}

#endif //_SIMPLE_SCHEDULER_HPP_