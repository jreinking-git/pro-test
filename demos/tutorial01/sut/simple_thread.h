#pragma once

#include <protest/rtos/thread.h>
#include <protest/rtos/semaphore.h>

class SimpleThread : public protest::rtos::Thread
{
public:
  SimpleThread();

  virtual void
  run() override;

  void
  waitForThread();

private:
  protest::rtos::Semaphore mSemaphore;
};
