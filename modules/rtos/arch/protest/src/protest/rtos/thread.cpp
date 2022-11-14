// ---------------------------------------------------------------------------
/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2022 Janosch Reinking
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to 
 * deal in the Software without restriction, including without limitation the 
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "protest/rtos/thread.h"
#include "protest/utils/debug.h"
#include "protest/core/runner_raw.h"

using namespace protest::rtos;

// ---------------------------------------------------------------------------
/**
 * @class ThreadRunner
 */
class ThreadRunner : public protest::core::RunnerRaw
{
public:
  explicit ThreadRunner(const char* name, protest::rtos::Thread* thread);

  ThreadRunner(const ThreadRunner& other) = delete;

  ThreadRunner(ThreadRunner&& other) = delete;

  ThreadRunner&
  operator=(const ThreadRunner& other) = delete;

  ThreadRunner&
  operator=(ThreadRunner&& other) = delete;

  ~ThreadRunner() override = default;

// ---------------------------------------------------------------------------
  void
  process() override;

private:
  Thread* mThread;
};

// ---------------------------------------------------------------------------
ThreadRunner::ThreadRunner(const char* name, protest::rtos::Thread* thread) :
  protest::core::RunnerRaw(name),
  mThread(thread)
{
}

void
ThreadRunner::process()
{
  mThread->run();
  coroExit();
}

// ---------------------------------------------------------------------------
void
Thread::sleep(time::Duration duration)
{
  auto* context = core::Context::getCurrentContext();
  auto* runner = context->getCurrent();
  runner->waitInternal(duration);
}

// ---------------------------------------------------------------------------
Thread::Thread(const char* name) : mUserdata(nullptr)
{
  // NOLINTNEXTLINE
  mUserdata = new ThreadRunner(name, this);
}

Thread::Thread(uint8_t priority, const char* name) : mUserdata(nullptr)
{
  // NOLINTNEXTLINE
  mUserdata = new ThreadRunner(name, this);
}

Thread::Thread(uint8_t /* priority */,
               size_t /* stackSize */,
               const char* name) :
  mUserdata(nullptr)
{
  // NOLINTNEXTLINE
  mUserdata = new ThreadRunner(name, this);
}

Thread::~Thread()
{
  auto* userdata = reinterpret_cast<protest::core::RunnerRaw*>(mUserdata);
  // NOLINTNEXTLINE
  delete userdata;
  mUserdata = nullptr;
}

void
Thread::start()
{
  auto* userdata = reinterpret_cast<protest::core::RunnerRaw*>(mUserdata);
  auto* context = core::Context::getCurrentContext();
  context->addThread(userdata);
}
