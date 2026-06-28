#include <gtest/gtest.h>
#include "src/utils/scheduler/Scheduler.hpp"
#include "test/mocks/mock_scheduler_strategy.hpp"
#include "src/utils/scheduler/Scheduler.cpp"

TEST(SchedulerTest, ScheduleDelegatesToStrategy) {
    auto strategy = std::make_shared<MockISchedulerStrategy>();
    utils::Scheduler sched(strategy);

    auto task = [](void*) {};
    utils::Scheduler::schedule(task);
    EXPECT_TRUE(static_cast<bool>(strategy->lastTask));
}

TEST(SchedulerTest, ProcessDelegatesToStrategy) {
    auto strategy = std::make_shared<MockISchedulerStrategy>();
    utils::Scheduler sched(strategy);

    utils::Scheduler::process();
    EXPECT_TRUE(strategy->processCalled);
}

TEST(SchedulerTest, ScheduleWithNullStrategyNoCrash) {
    utils::Scheduler sched(nullptr);
    EXPECT_NO_THROW(utils::Scheduler::schedule([](void*) {}));
}

TEST(SchedulerTest, ProcessWithNullStrategyNoCrash) {
    utils::Scheduler sched(nullptr);
    EXPECT_NO_THROW(utils::Scheduler::process());
}
