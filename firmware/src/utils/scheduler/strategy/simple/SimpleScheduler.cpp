#include "SimpleScheduler.hpp"

using namespace utils;

SimpleScheduler::SimpleScheduler()
{
}

void SimpleScheduler::process()
{
    while (!queue.empty()) {
        std::function<void(void*)> work = queue.front();
        work(nullptr);
        queue.pop();
    }
}

void SimpleScheduler::schedule(std::function<void(void*)> work)
{
    queue.push(work);
}
