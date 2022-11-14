#include <gtest/gtest.h>

#include "protest/coro/scheduler.h"
#include "protest/time/time_point.h"

using namespace protest::coro;

TEST(coroutine, should_return_current_coroutine)
{
  class MyCoroutine : public Coroutine
  {
  public:
    MyCoroutine(Scheduler& scheduler) : Coroutine(scheduler), mReached(false)
    {
    }

    void
    coroRun() override
    {
      mCurrent = getScheduler().getCurrent();
      coroExit();
    }

    Coroutine* mCurrent;
    bool mReached;

  private:
  };

  Scheduler scheduler;
  MyCoroutine coro1(scheduler);
  scheduler.addThread(&coro1);
  scheduler.run();
  ASSERT_EQ(&coro1, coro1.mCurrent);
}

TEST(coroutine, should_signal_that_couroutine_is_waiting)
{
  class MyCoroutine : public Coroutine
  {
  public:
    MyCoroutine(Scheduler& scheduler, Coroutine* other) :
      Coroutine(scheduler),
      mOther(other),
      mReached(false)
    {
    }

    void
    coroRun() override
    {
      if (mOther == nullptr)
      {
        coroWait(protest::time::Millisecond(100u));
      }
      else
      {
        coroWait(protest::time::Millisecond(10));
        mReached = mOther->isWaiting();
      }
      coroExit();
    }

    Coroutine* mOther;
    bool mReached;

  private:
  };

  Scheduler scheduler;
  MyCoroutine coro1(scheduler, nullptr);
  MyCoroutine coro2(scheduler, &coro1);
  scheduler.addThread(&coro1);
  scheduler.addThread(&coro2);
  scheduler.run();
  ASSERT_TRUE(coro2.mReached);
}

TEST(coroutine, should_signal_that_couroutine_is_not_waiting)
{
  class MyCoroutine : public Coroutine
  {
  public:
    MyCoroutine(Scheduler& scheduler, Coroutine* other) :
      Coroutine(scheduler),
      mOther(other),
      mReached(true)
    {
    }

    void
    coroRun() override
    {
      if (mOther == nullptr)
      {
        coroWait(protest::time::Millisecond(10u));
      }
      else
      {
        coroWait(protest::time::Millisecond(100));
        mReached = mOther->isWaiting();
      }
      coroExit();
    }

    Coroutine* mOther;
    bool mReached;

  private:
  };

  Scheduler scheduler;
  MyCoroutine coro1(scheduler, nullptr);
  MyCoroutine coro2(scheduler, &coro1);
  scheduler.addThread(&coro1);
  scheduler.addThread(&coro2);
  scheduler.run();
  ASSERT_FALSE(coro2.mReached);
}

TEST(coroutine, should_return_now)
{
  class MyCoroutine : public Coroutine
  {
  public:
    MyCoroutine(Scheduler& scheduler) : Coroutine(scheduler)
    {
    }

    void
    coroRun() override
    {
      tp1 = now();
      coroWait(protest::time::Millisecond(100u));
      tp2 = now();
      coroExit();
    }

    protest::time::TimePoint tp1;
    protest::time::TimePoint tp2;

  private:
  };

  Scheduler scheduler;
  MyCoroutine coro1(scheduler);
  scheduler.addThread(&coro1);
  scheduler.run();
  ASSERT_EQ(coro1.tp1.milliseconds(), 0);
  ASSERT_EQ(coro1.tp2.milliseconds(), 100);
}
