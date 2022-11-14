#include "simple_thread.h"

#include "protest/rtos/log.h"

SimpleThread::SimpleThread() : protest::rtos::Thread("thd")
{
}

void
SimpleThread::run()
{
  PT_INFO("Start and ... ");
  protest::rtos::Thread::sleep(protest::time::Millisecond(1000));
  PT_INFO(" ... stop ");
  mSemaphore.release();
}

void
SimpleThread::waitForThread()
{
  mSemaphore.acquire();
}
