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

#pragma once

#include "protest/core/job.h"
#include "protest/core/runner_raw.h"
#include "protest/time/duration.h"
#include "protest/utils/can_invoke.h"
#include "protest/utils/ref_counter.h"

namespace protest
{

namespace core
{

// ---------------------------------------------------------------------------
/**
 * @class Timer
 */
template <typename Callback>
class Timer
{
public:
  explicit Timer(RunnerRaw* runner,
                 Callback callback,
                 meta::CallContext& context);

  explicit Timer(RunnerRaw* runner,
                 Callback callback,
                 time::Duration timeout,
                 meta::CallContext& context);

  Timer(Timer&&) noexcept = delete;

  Timer&
  operator=(const Timer&) = delete;

  Timer&
  operator=(Timer&&) noexcept = delete;

  ~Timer();

// ---------------------------------------------------------------------------
  void
  start(time::Duration duration);

  void
  start();

  void
  stop();

  void
  reset();

private:
  /**
   * @class TimerInternal
   */
  class TimerInternal : public Job::Listener, private RefCounter
  {
  public:
    friend class Timer;

    TimerInternal(const TimerInternal&) = delete;

    TimerInternal(TimerInternal&&) noexcept = delete;

    TimerInternal&
    operator=(const TimerInternal&) = delete;

    TimerInternal&
    operator=(TimerInternal&&) noexcept = delete;

    ~TimerInternal() = default;

// ---------------------------------------------------------------------------
    void
    start(time::Duration duration);

    void
    start();

    void
    stop();

    void
    reset();

// ---------------------------------------------------------------------------
    void
    notify(RunnerRaw* runner) override;

    void
    added(Job* job) override;

    void
    removed(Job* job) override;

  private:
    explicit TimerInternal(RunnerRaw* runner, Callback callback);

    explicit TimerInternal(RunnerRaw* runner,
                           Callback callback,
                           meta::CallContext& callContext);

    RunnerRaw* mRunner;
    Callback mCallback;
    time::Duration mRemaining;
    time::Duration mStartValue;
    time::TimePoint mStartedAt;
    bool mArmed;
    Job mJob;
    meta::CallContext* mCallContext;
  };

  explicit Timer(TimerInternal* internal);

  Timer(const Timer&);

  TimerInternal* mInternal;
};

// ---------------------------------------------------------------------------
template <typename Callback>
Timer<Callback>::~Timer()
{
  // PROTEST_ASSERT(mInternal);
  if (mInternal->decrement())
  {
    delete mInternal;
    mInternal = nullptr;
  }
  else
  {
  }
}

// ---------------------------------------------------------------------------
template <typename Callback>
void
Timer<Callback>::start(time::Duration duration)
{
  mInternal->start(duration);
}

template <typename Callback>
void
Timer<Callback>::start()
{
  mInternal->start();
}

template <typename Callback>
void
Timer<Callback>::stop()
{
  mInternal->stop();
}

template <typename Callback>
void
Timer<Callback>::reset()
{
  mInternal->reset();
}

// ---------------------------------------------------------------------------
template <typename Callback>
void
Timer<Callback>::TimerInternal::start(time::Duration duration)
{
  PROTEST_ASSERT(!mArmed);
  mStartValue = duration;
  mRemaining = duration;
  mStartedAt = mRunner->now();
  mJob.setDue(mStartedAt + mRemaining, this);
  mRunner->add(mJob);
  mArmed = true;
}

template <typename Callback>
void
Timer<Callback>::TimerInternal::start()
{
  PROTEST_ASSERT(!mArmed);
  mStartedAt = mRunner->now();
  mJob.setDue(mStartedAt + mRemaining, this);
  mRunner->add(mJob);
  mArmed = true;
}

template <typename Callback>
void
Timer<Callback>::TimerInternal::stop()
{
  mRemaining = mRunner->now() - mStartedAt;
  PROTEST_ASSERT(mArmed);
  mRunner->remove(mJob);
  mArmed = false;
}

template <typename Callback>
void
Timer<Callback>::TimerInternal::reset()
{
  if (mArmed)
  {
    stop();
  }
  else
  {
  }
  start(mStartValue);
}

template <typename Callback>
void
Timer<Callback>::TimerInternal::notify(RunnerRaw* /* runner */)
{
  PROTEST_ASSERT(mArmed);
  Timer timer(this);

  mArmed = false;

  const char* name = mCallContext->getObjectName();
  if (strlen(name) == 0)
  {
    name = "<anno>";
  }

  auto stream =
      mRunner->getLogger().startLog("EXPR",
                                    mRunner->getName(),
                                    mCallContext->getUnit().getFileName(),
                                    mCallContext->getLine(),
                                    mRunner->now());
  stream.operator std::ostream&() << "Timer '" << name << "' expired after:\n"
                                  << mStartValue.milliseconds() << " ms\n";

  if constexpr (CanInvoke<decltype(mCallback), Timer&>::value)
  {
    mCallback(timer);
  }
  else
  {
    mCallback();
  }
}

template <typename Callback>
void
Timer<Callback>::TimerInternal::added(Job* job)
{
  increment();
}

template <typename Callback>
void
Timer<Callback>::TimerInternal::removed(Job* job)
{
  if (decrement())
  {
    delete this;
  }
  else
  {
  }
}

// ---------------------------------------------------------------------------
template <typename Callback>
Timer<Callback>::TimerInternal::TimerInternal(RunnerRaw* runner,
                                              Callback callback,
                                              meta::CallContext& callContext) :
  mRunner(runner),
  mCallback(callback),
  mRemaining(time::Duration::infinity()),
  mStartValue(time::Duration::infinity()),
  mStartedAt(time::TimePoint::startOfEpoche()),
  mArmed(false),
  mCallContext(&callContext)
{
}

// ---------------------------------------------------------------------------
template <typename Callback>
Timer<Callback>::Timer(TimerInternal* internal) : mInternal(internal)
{
  mInternal->increment();
}

template <typename Callback>
Timer<Callback>::Timer(const Timer&)
{
  // RVO
  PROTEST_ASSERT(false);
}

template <typename Callback>
Timer<Callback>::Timer(RunnerRaw* runner,
                       Callback callback,
                       meta::CallContext& callContext) :
  mInternal(new TimerInternal(runner, callback, callContext))
{
  mInternal->increment();
}

template <typename Callback>
Timer<Callback>::Timer(RunnerRaw* runner,
                       Callback callback,
                       time::Duration timeout,
                       meta::CallContext& callContext) :
  mInternal(new TimerInternal(runner, callback, callContext))
{
  mInternal->increment();
  mInternal->start(timeout);
}

} // namespace core

} // namespace protest
