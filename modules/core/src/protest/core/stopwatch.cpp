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

#include "protest/core/stopwatch.h"
#include "protest/core/condition.h"

using namespace protest::time;
using namespace protest::core;

// ---------------------------------------------------------------------------
Stopwatch::Operator::Operator(StopwatchInternal& stopwatch,
                              time::Duration duration) :
  mDuration(duration),
  mStopwatch(&stopwatch),
  mRunner(nullptr),
  mCondition(nullptr),
  mNext(nullptr)
{
  mStopwatch->increment();
}

// NOLINTNEXTLINE
Stopwatch::Operator::Operator(const Operator& other) :
  mDuration(other.mDuration),
  mStopwatch(other.mStopwatch),
  mRunner(other.mRunner),
  mCondition(other.mCondition),
  mNext(other.mNext)
{
  mStopwatch->increment();
}

Stopwatch::Operator::~Operator()
{
  if (mStopwatch->decrement())
  {
    delete mStopwatch;
  }
}

// ---------------------------------------------------------------------------
void
Stopwatch::Operator::added(Job* /* job */)
{
  mStopwatch->increment();
}

void
Stopwatch::Operator::removed(Job* /* job */)
{
  if (mStopwatch->decrement())
  {
    delete mStopwatch;
  }
}

// ---------------------------------------------------------------------------
Stopwatch::Equals::Equals(StopwatchInternal& stopwatch,
                          time::Duration duration) :
  Operator(stopwatch, duration)
{
}

void
Stopwatch::Equals::conditionEnable(RunnerRaw* runner, Condition* condition)
{
  mRunner = runner;
  mCondition = condition;
  mStopwatch->add(*this, mCondition);
  reset();
}

void
Stopwatch::Equals::conditionDisable()
{
  mStopwatch->remove(*this, mCondition);
}

Stopwatch::Equals::Type
Stopwatch::Equals::getValue()
{
  return mStopwatch->lap() == mDuration;
}

void
Stopwatch::Equals::notify(RunnerRaw* /* runner */)
{
  mCondition->notifyListener();
}

void
Stopwatch::Equals::resetAndNotify()
{
  mCondition->notifyListener();
  reset();
}

void
Stopwatch::Equals::reset()
{
  if (mDuration == mStopwatch->lap())
  {
    mStopwatch->update(*this, mDuration + Duration::smallestNonZero());
  }
  else
  {
    mStopwatch->update(*this, mDuration);
  }
}

// ---------------------------------------------------------------------------
Stopwatch::NotEquals::NotEquals(StopwatchInternal& stopwatch,
                                time::Duration duration) :
  Operator(stopwatch, duration)
{
}

void
Stopwatch::NotEquals::conditionEnable(RunnerRaw* runner, Condition* condition)
{
  mRunner = runner;
  mCondition = condition;
  mStopwatch->add(*this, mCondition);
  reset();
}

void
Stopwatch::NotEquals::conditionDisable()
{
  mStopwatch->remove(*this, mCondition);
}

Stopwatch::NotEquals::Type
Stopwatch::NotEquals::getValue()
{
  return mStopwatch->lap() != mDuration;
}

void
Stopwatch::NotEquals::notify(RunnerRaw* /* runner */)
{
  mCondition->notifyListener();
}

void
Stopwatch::NotEquals::resetAndNotify()
{
  mCondition->notifyListener();
  reset();
}

void
Stopwatch::NotEquals::reset()
{
  if (mStopwatch->lap() == mDuration)
  {
    mStopwatch->update(*this, mDuration + Duration::smallestNonZero());
  }
  else
  {
    mStopwatch->update(*this, mDuration);
  }
}

// ---------------------------------------------------------------------------
Stopwatch::Greater::Greater(StopwatchInternal& stopwatch,
                            time::Duration duration) :
  Operator(stopwatch, duration)
{
}

void
Stopwatch::Greater::conditionEnable(RunnerRaw* runner, Condition* condition)
{
  mRunner = runner;
  mCondition = condition;
  mStopwatch->add(*this, mCondition);
  reset();
}

void
Stopwatch::Greater::conditionDisable()
{
  mStopwatch->remove(*this, mCondition);
}

Stopwatch::Greater::Type
Stopwatch::Greater::getValue()
{
  return mStopwatch->lap() > mDuration;
}

void
Stopwatch::Greater::notify(RunnerRaw* /* runner */)
{
  mCondition->notifyListener();
}

void
Stopwatch::Greater::resetAndNotify()
{
  mCondition->notifyListener();
  reset();
}

void
Stopwatch::Greater::reset()
{
  mStopwatch->update(*this, mDuration + Duration::smallestNonZero());
}

// ---------------------------------------------------------------------------
Stopwatch::GreaterOrEquals::GreaterOrEquals(StopwatchInternal& stopwatch,
                                            time::Duration duration) :
  Operator(stopwatch, duration)
{
}

void
Stopwatch::GreaterOrEquals::conditionEnable(RunnerRaw* runner,
                                            Condition* condition)
{
  mRunner = runner;
  mCondition = condition;
  mStopwatch->add(*this, mCondition);
  reset();
}

void
Stopwatch::GreaterOrEquals::conditionDisable()
{
  mStopwatch->remove(*this, mCondition);
}

Stopwatch::GreaterOrEquals::Type
Stopwatch::GreaterOrEquals::getValue()
{
  return mStopwatch->lap() >= mDuration;
}

void
Stopwatch::GreaterOrEquals::notify(RunnerRaw* /* runner */)
{
  mCondition->notifyListener();
}

void
Stopwatch::GreaterOrEquals::resetAndNotify()
{
  mCondition->notifyListener();
  reset();
}

void
Stopwatch::GreaterOrEquals::reset()
{
  if (mStopwatch->lap() == mDuration)
  {
    mStopwatch->update(*this, mDuration + Duration::smallestNonZero());
  }
  else
  {
    mStopwatch->update(*this, mDuration);
  }
}

// ---------------------------------------------------------------------------
Stopwatch::Less::Less(StopwatchInternal& stopwatch, time::Duration duration) :
  Operator(stopwatch, duration)
{
}

void
Stopwatch::Less::conditionEnable(RunnerRaw* runner, Condition* condition)
{
  mRunner = runner;
  mCondition = condition;
  mStopwatch->add(*this, mCondition);
  reset();
}

void
Stopwatch::Less::conditionDisable()
{
  mStopwatch->remove(*this, mCondition);
}

Stopwatch::Less::Type
Stopwatch::Less::getValue()
{
  return mStopwatch->lap() < mDuration;
}

void
Stopwatch::Less::notify(RunnerRaw* /* runner */)
{
  mCondition->notifyListener();
}

void
Stopwatch::Less::resetAndNotify()
{
  mCondition->notifyListener();
  reset();
}

void
Stopwatch::Less::reset()
{
  mStopwatch->update(*this, mDuration);
}

// ---------------------------------------------------------------------------
Stopwatch::LessOrEquals::LessOrEquals(StopwatchInternal& stopwatch,
                                      time::Duration duration) :
  Operator(stopwatch, duration)
{
}

void
Stopwatch::LessOrEquals::conditionEnable(RunnerRaw* runner,
                                         Condition* condition)
{
  mRunner = runner;
  mCondition = condition;
  mStopwatch->add(*this, mCondition);
  reset();
}

void
Stopwatch::LessOrEquals::conditionDisable()
{
  mStopwatch->remove(*this, mCondition);
}

Stopwatch::LessOrEquals::Type
Stopwatch::LessOrEquals::getValue()
{
  return mStopwatch->lap() <= mDuration;
}

void
Stopwatch::LessOrEquals::notify(RunnerRaw* /* runner */)
{
  mCondition->notifyListener();
}

void
Stopwatch::LessOrEquals::resetAndNotify()
{
  mCondition->notifyListener();
  reset();
}

void
Stopwatch::LessOrEquals::reset()
{
  mStopwatch->update(*this, mDuration + Duration::smallestNonZero());
}

// ---------------------------------------------------------------------------
Stopwatch::Stopwatch() : mInternal(nullptr)
{
}

Stopwatch&
Stopwatch::operator=(Stopwatch&& other) noexcept
{
  mInternal = other.mInternal;
  mInternal->increment();
  return *this;
}

// ---------------------------------------------------------------------------
void
Stopwatch::start()
{
  mInternal->start();
}

void
Stopwatch::stop()
{
  mInternal->stop();
}

void
Stopwatch::reset()
{
  mInternal->reset();
}

Duration
Stopwatch::lap()
{
  return mInternal->lap();
}

// NOLINTNEXTLINE
Stopwatch::Equals
Stopwatch::operator==(time::Duration duration)
{
  return mInternal->operator==(duration);
}

// NOLINTNEXTLINE
Stopwatch::NotEquals
Stopwatch::operator!=(time::Duration duration)
{
  return mInternal->operator!=(duration);
}

// NOLINTNEXTLINE
Stopwatch::Greater
Stopwatch::operator>(time::Duration duration)
{
  return mInternal->operator>(duration);
}

// NOLINTNEXTLINE
Stopwatch::GreaterOrEquals
Stopwatch::operator>=(time::Duration duration)
{
  return mInternal->operator>=(duration);
}

// NOLINTNEXTLINE
Stopwatch::Less
Stopwatch::operator<(time::Duration duration)
{
  return mInternal->operator<(duration);
}

// NOLINTNEXTLINE
Stopwatch::LessOrEquals
Stopwatch::operator<=(time::Duration duration)
{
  return mInternal->operator<=(duration);
}

// ---------------------------------------------------------------------------
Stopwatch::StopwatchInternal::StopwatchInternal(RunnerRaw* runner) :
  mRunner(runner),
  mStartOrStopedAt(TimePoint::endOfEpoche()),
  mTimePassed(Duration::zero()),
  mRunning(false)
{
  // make sure that there is not initial state
  // start and stop to set variable like it run at least onces
  // -> easiert to test since there are only two states (started, stopped)
  start();
  stop();
  start();
}

void
Stopwatch::StopwatchInternal::start()
{
  PROTEST_ASSERT(!mRunning);
  TimePoint now = mRunner->now();
  if (mStartOrStopedAt == protest::time::TimePoint::endOfEpoche())
  {
    // started for the first time
    mStartOrStopedAt = now;
  }
  else
  {
  }
  Duration diff = now - mStartOrStopedAt;
  mStartOrStopedAt = now;
  auto iter = mOperators.begin();
  while (iter != mOperators.end())
  {
    iter->updateDue(diff);
    if (!iter->isDue(mRunner->now()))
    {
      mRunner->add(*iter);
    }
    else
    {
      iter->notExecuted();
    }
    ++iter;
  }
  mRunning = true;
}

void
Stopwatch::StopwatchInternal::stop()
{
  PROTEST_ASSERT(mRunning);
  TimePoint now = mRunner->now();

  mTimePassed = mTimePassed + (now - mStartOrStopedAt);
  auto iter = mOperators.begin();
  while (iter != mOperators.end())
  {
    if (iter->isArmed())
    {
      mRunner->remove(*iter);
    }
    else
    {
    }
    ++iter;
  }
  mStartOrStopedAt = now;
  mRunning = false;
}

void
Stopwatch::StopwatchInternal::reset()
{
  bool wasRunning = false;
  if (mRunning)
  {
    // remove all jobs since it cannot be in the heap
    // during updating the due
    stop();
    wasRunning = true;
  }
  else
  {
    wasRunning = false;
  }

  mTimePassed = time::Duration::zero();
  auto iter = mOperators.begin();
  while (iter != mOperators.end())
  {
    iter->resetAndNotify();
    ++iter;
  }

  if (wasRunning)
  {
    start();
  }
  else
  {
  }
}

void
Stopwatch::StopwatchInternal::update(Operator& opr, time::Duration duration)
{
  PROTEST_ASSERT(!opr.isArmed());
  Duration diff = duration - lap();
  opr.setDue(mRunner->now() + diff, &opr);
  if (mRunning && !opr.isDue(mRunner->now()))
  {
    mRunner->add(opr);
  }
  else
  {
    opr.notExecuted();
  }
}

Duration
Stopwatch::StopwatchInternal::lap()
{
  time::Duration duration = Duration::zero();
  if (mRunning)
  {
    TimePoint now = mRunner->now();
    duration = mTimePassed + (now - mStartOrStopedAt);
  }
  else
  {
    duration = mTimePassed;
  }
  return duration;
}

// ---------------------------------------------------------------------------
// NOLINTNEXTLINE
Stopwatch::Equals
Stopwatch::StopwatchInternal::operator==(time::Duration duration)
{
  return Equals(*this, duration);
}

// NOLINTNEXTLINE
Stopwatch::NotEquals
Stopwatch::StopwatchInternal::operator!=(time::Duration duration)
{
  return NotEquals(*this, duration);
}

// NOLINTNEXTLINE
Stopwatch::Greater
Stopwatch::StopwatchInternal::operator>(time::Duration duration)
{
  return Greater(*this, duration);
}

// NOLINTNEXTLINE
Stopwatch::GreaterOrEquals
Stopwatch::StopwatchInternal::operator>=(time::Duration duration)
{
  return GreaterOrEquals(*this, duration);
}

// NOLINTNEXTLINE
Stopwatch::Less
Stopwatch::StopwatchInternal::operator<(time::Duration duration)
{
  return Less(*this, duration);
}

// NOLINTNEXTLINE
Stopwatch::LessOrEquals
Stopwatch::StopwatchInternal::operator<=(time::Duration duration)
{
  return LessOrEquals(*this, duration);
}

void
Stopwatch::StopwatchInternal::add(Operator& opr, Condition* condition)
{
  mConditions.prepend(*condition);
  mOperators.prepend(opr);
}

void
Stopwatch::StopwatchInternal::remove(Operator& opr, Condition* condition)
{
  mConditions.remove(*condition);
  mOperators.remove(opr);
  if (opr.isArmed())
  {
    mRunner->remove(opr);
  }
  else
  {
    // already execute -> not in scheduler
  }
}

// ---------------------------------------------------------------------------
Stopwatch::Stopwatch(RunnerRaw* runner) :
  mInternal(new StopwatchInternal(runner))
{
  mInternal->increment();
}

Stopwatch::~Stopwatch()
{
  if (mInternal->decrement())
  {
    delete mInternal;
    mInternal = nullptr;
  }
}
