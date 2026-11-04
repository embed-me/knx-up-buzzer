#include "SimpleScheduler.hpp"
#include "src/drivers/logger/Logger.hpp"

using namespace utils;
using namespace drivers::logger;

SimpleScheduler::SimpleScheduler()
{
    critical_section_init(&_cs);
}

void SimpleScheduler::process()
{
    uint8_t processed = 0;
    
    while (processed < MAX_PER_CYCLE_LIMIT) {
        std::function<void(void*)> work;

        critical_section_enter_blocking(&_cs);
        if (count == 0) {
            critical_section_exit(&_cs);
            break;
        }
        work = std::move(queue[head]);
        queue[head] = nullptr;
        head = (head + 1) % MAX_QUEUE_SIZE;
        count--;
        critical_section_exit(&_cs);

        if (work) {
            work(nullptr);
        }
        processed++;
    }
}

void SimpleScheduler::schedule(std::function<void(void*)> work)
{
    if (!work) {
        return;
    }

    critical_section_enter_blocking(&_cs);
    bool isSpaceInQueue = (count < MAX_QUEUE_SIZE);
    if (isSpaceInQueue) {
        queue[(head + count) % MAX_QUEUE_SIZE] = work;
        count++;
    } else {
        logError("Scheduler queue full (%d items), discarding task", MAX_QUEUE_SIZE);
    }
    critical_section_exit(&_cs);
}
