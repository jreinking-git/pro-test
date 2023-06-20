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

#include "protest/rtos/semaphore.h"
#include "protest/utils/debug.h"
#include "protest/core/context.h"
#include "protest/core/runner_raw.h"

using namespace protest::rtos;

// ---------------------------------------------------------------------------
struct Userdata
{
  Userdata() : mCount(0)
  {
  }

  // using the link in coro::Coroutine since core::RunnerRaw is already used
  // by the scheduler.
  // NOLINTNEXTLINE
  protest::List<protest::coro::Coroutine> mWaitingList;
  // NOLINTNEXTLINE
  uint16_t mCount;
};

// ---------------------------------------------------------------------------
Semaphore::Semaphore() : mUserdata(nullptr)
{
  // NOLINTNEXTLINE
  mUserdata = new Userdata();
  auto* userdata = reinterpret_cast<Userdata*>(mUserdata);
  userdata->mCount = 0;
}

Semaphore::~Semaphore()
{
  auto* userdata = reinterpret_cast<Userdata*>(mUserdata);
  // NOLINTNEXTLINE
  delete userdata;
  mUserdata = nullptr;
}

// ---------------------------------------------------------------------------
bool
Semaphore::acquire(time::Duration timeout)
{
  auto* userdata = reinterpret_cast<Userdata*>(mUserdata);
  bool gotTimeout = false;
  if (userdata->mCount > 0)
  {
    userdata->mCount--;
  }
  else
  {
    auto* context = core::Context::getCurrentContext();
    auto* runner = context->getCurrent();
    userdata->mWaitingList.prepend(*runner);
    gotTimeout = runner->waitInternal(timeout);
    if (gotTimeout)
    {
      userdata->mWaitingList.remove(*runner);
    }
    else
    {
      // woken up by other thread
      // already removed from list
    }
  }
  return !gotTimeout;
}

void
Semaphore::release()
{
  auto* userdata = reinterpret_cast<Userdata*>(mUserdata);
  if (userdata->mWaitingList.isAvailable())
  {
    // safe since only RunnerRaw are pushed into the list
    auto* runner = reinterpret_cast<core::RunnerRaw*>(
        &userdata->mWaitingList.removeFirst());
    runner->wakeup();
  }
  else
  {
    userdata->mCount++;
  }
}
