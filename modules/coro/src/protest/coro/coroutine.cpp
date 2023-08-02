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

#include "protest/coro/coroutine.h"
#include "protest/coro/scheduler.h"
#include "protest/utils/log.h"

#include <cstring>
#include <cassert>

using namespace protest::coro;

// ---------------------------------------------------------------------------
Coroutine::Coroutine(Scheduler& scheduler) :
  CoroutineBase(&scheduler.mMain),
  mScheduler(scheduler),
  mStack(nullptr),
  mSleepUntil(time::TimePoint::startOfEpoche()),
  mNext(nullptr),
  mIndex(0),
  mIsWaiting(false),
  mIsInSleepQueue(false)
{
}

// ---------------------------------------------------------------------------
void
Coroutine::contextSwitch(CoroutineBase::CoroContext* parent)
{
  CoroutineBase::swapContext(parent, &mContext);
}

// ---------------------------------------------------------------------------
bool
Coroutine::coroWait(time::Duration duration)
{
  if (duration == time::Duration::infinity())
  {
    mSleepUntil = time::TimePoint::endOfEpoche();
  }
  else
  {
    mSleepUntil = mScheduler.now(this) + duration;
    assert(mSleepUntil.nanoseconds() >= mScheduler.now(this).nanoseconds());
  }

  bool timeout = false;
  mIsWaiting = true;

  mScheduler.sleep(this, duration);
  if (mIsWaiting)
  {
    mIsWaiting = false;
    timeout = true;
  }
  else
  {
    // there was a call to wakeup()
    timeout = false;
  }
  return timeout;
}

void
Coroutine::coroYield()
{
  mScheduler.yield(this);
}

void
Coroutine::coroWakeup()
{
  PROTEST_ASSERT(mIsWaiting);
  mIsWaiting = false;
  mScheduler.wakeup(this);
}

void
Coroutine::coroExit()
{
  mScheduler.exit(this);
}

// ---------------------------------------------------------------------------
bool
Coroutine::isWaiting() const
{
  return mIsWaiting;
}

protest::time::TimePoint
Coroutine::now()
{
  return mScheduler.now(this);
}

// ---------------------------------------------------------------------------
Coroutine*
Coroutine::getCurrent()
{
  return mScheduler.getCurrent();
}

Scheduler&
Coroutine::getScheduler()
{
  return mScheduler;
}

// NOLINTNEXTLINE
bool
Coroutine::operator<(const Coroutine& other)
{
  return mSleepUntil < other.mSleepUntil;
}

// NOLINTNEXTLINE
bool
Coroutine::operator<=(const Coroutine& other)
{
  return mSleepUntil <= other.mSleepUntil;
}
