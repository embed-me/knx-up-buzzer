#include <gtest/gtest.h>
#include <cstdlib>
#include <new>
#include "src/utils/scheduler/strategy/simple/SimpleScheduler.hpp"
#include "src/utils/scheduler/strategy/simple/SimpleScheduler.cpp"
#include "src/drivers/logger/Logger.cpp"

// Count heap allocations so we can assert that schedule() never touches the heap.
// schedule() is called from alarm interrupt handlers (ArduinoTimer, 100us period),
// and the newlib malloc lock is a recursive mutex owned per core, so an ISR that
// allocates while the main loop is inside malloc/free re-enters the allocator
// and corrupts the heap.
static size_t g_allocCount = 0;
static bool g_countAllocs = false;

void *operator new(std::size_t n) {
    if (g_countAllocs) g_allocCount++;
    void *p = std::malloc(n);
    if (!p) throw std::bad_alloc();
    return p;
}
void operator delete(void *p) noexcept { std::free(p); }
void operator delete(void *p, std::size_t) noexcept { std::free(p); }

TEST(SimpleSchedulerTest, ScheduleDoesNotAllocateFromHeap) {
    utils::SimpleScheduler sched;
    int counter = 0;
    int *c = &counter;

    // many fill/drain cycles, like a long running device
    for (int cycle = 0; cycle < 50; cycle++) {
        g_allocCount = 0;
        g_countAllocs = true;
        for (int i = 0; i < 100; i++) {
            sched.schedule([c](void *) { (*c)++; });
        }
        g_countAllocs = false;
        EXPECT_EQ(0u, g_allocCount) << "schedule() allocated in cycle " << cycle;

        for (int i = 0; i < 10; i++) sched.process();
    }
    EXPECT_EQ(50 * 100, counter);
}

TEST(SimpleSchedulerTest, ExecutesInFifoOrderAndDropsWhenFull) {
    utils::SimpleScheduler sched;
    std::vector<int> order;

    for (int i = 0; i < 120; i++) {
        sched.schedule([&order, i](void *) { order.push_back(i); });
    }
    for (int i = 0; i < 20; i++) sched.process();

    ASSERT_EQ(100u, order.size());
    for (int i = 0; i < 100; i++) EXPECT_EQ(i, order[i]);
}

TEST(SimpleSchedulerTest, QueueIsReusableAfterWraparound) {
    utils::SimpleScheduler sched;
    int counter = 0;
    for (int round = 0; round < 1000; round++) {
        sched.schedule([&counter](void *) { counter++; });
        sched.process();
    }
    EXPECT_EQ(1000, counter);
}
