#include <gtest/gtest.h>

#include "protest/rtos/thread.h"
#include "protest/rtos/queue.h"
#include "protest/time/time_point.h"
#include "protest/time/duration.h"

using namespace protest::rtos;

TEST(rtos_queue, should_wait_for_ever_on_pop)
{
  class MyCoroutine : public Thread
  {
  public:
    MyCoroutine(Queue<int, 10>& queue) :
      Thread(0, 0, ""),
      mQueue(queue),
      mReached(false)
    {
    }

    void
    run()
    {
      int value = 0;
      mQueue.pop(value);
      mReached = true;
    }

    bool mReached;

  private:
    Queue<int, 10>& mQueue;
  };

  static Queue<int, 10> queue;
  static MyCoroutine coro1(queue);
  coro1.start();
  sleep(1);// some kind of forever
  ASSERT_FALSE(coro1.mReached);
}

TEST(rtos_queue, should_wait_for_timeout_on_pop)
{
  class MyCoroutine : public Thread
  {
  public:
    MyCoroutine(Queue<int, 10>& queue) :
      Thread(0, 0, ""),
      mQueue(queue),
      mReached(false)
    {
    }

    void
    run()
    {
      int value = 0;
      bool timeout = mQueue.pop(value, protest::time::Millisecond(1000));
      mReached = timeout;
    }

    bool mReached;

  private:
    Queue<int, 10>& mQueue;
  };

  static Queue<int, 10> queue;
  static MyCoroutine coro1(queue);
  coro1.start();
  while (!coro1.mReached)
  {
    protest::rtos::Thread::sleep(protest::time::Millisecond(1));
  }
  ASSERT_TRUE(coro1.mReached);
}

TEST(rtos_queue, should_wait_for_ever_on_push)
{
  class MyCoroutine : public Thread
  {
  public:
    MyCoroutine(Queue<int, 10>& queue) :
      Thread(0, 0, ""),
      mQueue(queue),
      mReached(false)
    {
    }

    void
    run()
    {
      int value = 0;
      for (int i = 0; i < 11; i++)
      {
        mQueue.push(value);
      }
      mReached = true;
    }

    bool mReached;

  private:
    Queue<int, 10>& mQueue;
  };

  static Queue<int, 10> queue;
  static MyCoroutine coro1(queue);
  coro1.start();
  sleep(1);
  ASSERT_FALSE(coro1.mReached);
}

TEST(rtos_queue, should_wait_for_timeout_on_push)
{
  class MyCoroutine : public Thread
  {
  public:
    MyCoroutine(Queue<int, 10>& queue) :
      Thread(0, 0, ""),
      mQueue(queue),
      mReached(false)
    {
    }

    void
    run()
    {
      int value = 0;
      bool noTimeout = true;
      for (int i = 0; i < 11; i++)
      {
        noTimeout = !mQueue.push(value, protest::time::Millisecond(100u));
      }
      mReached = !noTimeout;
    }

    bool mReached;

  private:
    Queue<int, 10>& mQueue;
  };

  static Queue<int, 10> queue;
  static MyCoroutine coro1(queue);
  coro1.start();
  while (!coro1.mReached)
  {
    protest::rtos::Thread::sleep(protest::time::Millisecond(1));
  }
  ASSERT_TRUE(coro1.mReached);
}

TEST(rtos_queue, should_run_into_timeout)
{
  class MyCoroutine : public Thread
  {
  public:
    MyCoroutine(Queue<int, 10>& queue) :
      Thread(0, 0, ""),
      mQueue(queue),
      mReached(false)
    {
    }

    void
    run()
    {
      int id = 0;
      int value = 0;

      mQueue.acquire();
      if (mQueue.isAvailable())
      {
        id = 1;
        mQueue.pop(value);
      }
      else
      {
        id = 0;
        mQueue.push(value);
      }
      mQueue.release();

      if (id == 0)
      {
        sleep(protest::time::Millisecond(100u));
        mQueue.acquire();
        mQueue.push(value);
        mQueue.release();
        mReached = true;
      }
      else
      {
        mQueue.acquire();
        bool timeout = mQueue.pop(value, protest::time::Millisecond(10u));
        mQueue.release();
        mReached = timeout;
      }
    }

    bool mReached;

  private:
    Queue<int, 10>& mQueue;
  };

  static Queue<int, 10> queue;
  static MyCoroutine coro1(queue);
  static MyCoroutine coro2(queue);
  coro1.start();
  coro2.start();
  while (!coro1.mReached || !coro2.mReached)
  {
    protest::rtos::Thread::sleep(protest::time::Millisecond(1));
  }

  ASSERT_TRUE(coro1.mReached);
  ASSERT_TRUE(coro2.mReached);
}

TEST(rtos_queue, should_not_run_into_timeout)
{
  class MyCoroutine : public Thread
  {
  public:
    MyCoroutine(Queue<int, 10>& queue) :
      Thread(0, 0, ""),
      mQueue(queue),
      mReached(false)
    {
    }

    void
    run()
    {
      int id = 0;
      int value = 0;

      mQueue.acquire();
      if (mQueue.isAvailable())
      {
        id = 1;
        mQueue.pop(value);
      }
      else
      {
        id = 0;
        mQueue.push(value);
      }
      mQueue.release();

      if (id == 0)
      {
        sleep(protest::time::Millisecond(10u));
        mQueue.acquire();
        mQueue.push(value);
        mQueue.release();
      }
      else
      {
        mQueue.acquire();
        mQueue.pop(value, protest::time::Millisecond(100u));
        mQueue.release();
      }
      mReached = true;
    }

    bool mReached;

  private:
    Queue<int, 10>& mQueue;
  };

  static Queue<int, 10> queue;
  static MyCoroutine coro1(queue);
  static MyCoroutine coro2(queue);
  coro1.start();
  coro2.start();

  while (!coro1.mReached || !coro2.mReached)
  {
    protest::rtos::Thread::sleep(protest::time::Millisecond(1));
  }

  ASSERT_TRUE(coro1.mReached);
  ASSERT_TRUE(coro2.mReached);
}
