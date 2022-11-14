#include <gtest/gtest.h>

#include "protest/coro/scheduler.h"
#include "protest/time/time_point.h"

using namespace protest::coro;

TEST(scheduler, should_run_without_coroutines)
{
  Scheduler scheduler;
  scheduler.run();
  auto time = scheduler.now();
  ASSERT_TRUE(time.milliseconds() == 0u);
}

TEST(scheduler, should_yield)
{
  class MyCoroutine : public Coroutine
  {
  public:
    MyCoroutine(Scheduler& scheduler) : Coroutine(scheduler), mReached(false)
    {
    }

    void
    coroRun()
    {
      coroYield();
      mReached = true;
      coroExit();
    }

    bool mReached;
  };

  Scheduler scheduler;
  MyCoroutine coro1(scheduler);
  scheduler.addThread(&coro1);
  scheduler.run();
  auto endOfTime = scheduler.now();
  ASSERT_TRUE(coro1.mReached);
  ASSERT_TRUE(endOfTime.milliseconds() == 0);
}

TEST(scheduler, should_yield2)
{
  class MyCoroutine : public Coroutine
  {
  public:
    MyCoroutine(Scheduler& scheduler) : Coroutine(scheduler), mReached(false)
    {
    }

    void
    coroRun()
    {
      coroYield();
      mReached = true;
      coroExit();
    }

    bool mReached;
  };

  Scheduler scheduler;
  MyCoroutine coro1(scheduler);
  MyCoroutine coro2(scheduler);
  scheduler.addThread(&coro1);
  scheduler.addThread(&coro2);
  scheduler.run();
  auto endOfTime = scheduler.now();
  ASSERT_TRUE(coro1.mReached);
  ASSERT_TRUE(coro2.mReached);
  ASSERT_TRUE(endOfTime.milliseconds() == 0);
}

TEST(scheduler, should_wait_100ms)
{
  class MyCoroutine : public Coroutine
  {
  public:
    MyCoroutine(Scheduler& scheduler) : Coroutine(scheduler), mReached(false)
    {
    }

    void
    coroRun()
    {
      coroWait(protest::time::Millisecond(100));
      mReached = true;
      coroExit();
    }

    bool mReached;
  };

  Scheduler scheduler;
  MyCoroutine coro2(scheduler);
  MyCoroutine coro1(scheduler);
  scheduler.addThread(&coro1);
  scheduler.addThread(&coro2);
  scheduler.run();
  auto endOfTime = scheduler.now();
  ASSERT_TRUE(coro1.mReached);
  ASSERT_TRUE(endOfTime.milliseconds() == 100);
}

TEST(scheduler, should_wait_0ms)
{
  class MyCoroutine : public Coroutine
  {
  public:
    MyCoroutine(Scheduler& scheduler) : Coroutine(scheduler), mReached(false)
    {
    }

    void
    coroRun()
    {
      coroWait(protest::time::Millisecond(0));
      mReached = true;
      coroExit();
    }

    bool mReached;
  };

  Scheduler scheduler;
  MyCoroutine coro2(scheduler);
  MyCoroutine coro1(scheduler);
  scheduler.addThread(&coro1);
  scheduler.addThread(&coro2);
  scheduler.run();
  auto endOfTime = scheduler.now();
  ASSERT_TRUE(coro1.mReached);
  ASSERT_TRUE(endOfTime.milliseconds() == 0);
}
