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

#include "protest/core/runner_raw.h"
#include "protest/core/condition.h"
#include "protest/utils/log.h"

using namespace protest::core;

// ---------------------------------------------------------------------------
RunnerRaw::RunnerRaw(const char* name) :
  coro::Coroutine(*Context::getCurrentContext()),
  mCondition(nullptr),
  mName(name),
  mWakeUpEvent(false),
  mNext(nullptr),
  mTestSteps(1),
  mCurrentTestStepName(nullptr)
{
  Context::getCurrentContext()->addRunner(this);
}

RunnerRaw::RunnerRaw(core::Context& context, const char* name) :
  coro::Coroutine(context),
  mCondition(nullptr),
  mName(name),
  mWakeUpEvent(false),
  mNext(nullptr),
  mTestSteps(1),
  mCurrentTestStepName(nullptr)
{
  context.Scheduler::addThread(this);
  context.addRunner(this);
}

RunnerRaw::~RunnerRaw()
{
  assert(mPriorityQueue.isEmpty());
}

// ---------------------------------------------------------------------------
void
RunnerRaw::internalInitialize()
{
  auto stream = mLogger.startLog("INFO", "main");
  stream.operator std::ostream&() << std::string(seperatorLength, '-') << "\n"
                                  << "Start runner '" << getName() << "'\n"
                                  << std::string(seperatorLength, '-') << "\n";
  initialize();
}

void
RunnerRaw::initialize()
{
}

void
RunnerRaw::process()
{
}

void
RunnerRaw::internalFinalize()
{
  finalize();
  auto stream = mLogger.startLog("INFO", "main");
  stream.operator std::ostream&() << std::string(seperatorLength, '-') << "\n";
  stream.operator std::ostream&() << "Exit of '" << getName() << "'\n";
  stream.operator std::ostream&() << std::string(seperatorLength, '-') << "\n";
}

void
RunnerRaw::finalize()
{
}

// ---------------------------------------------------------------------------
void
RunnerRaw::add(Job& job)
{
  mPriorityQueue.push(&job);
  job.added();
  if (isWaiting())
  {
    coro::Coroutine::coroWakeup();
  }
  else
  {
  }
}

void
RunnerRaw::remove(Job& job)
{
  PROTEST_ASSERT(job.isArmed());
  mPriorityQueue.remove(&job);

  // might be that a job is about to be removed and is due at the same time
  // -> in this case execute it on removing
  if (job.isDue(now()))
  {
    job.execute(this);
  }
  else
  {
    job.notExecuted();
  }

  job.removed();

  if (isWaiting())
  {
    coro::Coroutine::coroWakeup();
  }
  else
  {
  }
}

// ---------------------------------------------------------------------------
void
RunnerRaw::coroRun()
{
  process();
  // this makes sure that all timers etc. are executed before destroying the
  // coroutine
  waitInternal(time::Duration::infinity());
  coroExit();
}

// ---------------------------------------------------------------------------
bool
RunnerRaw::waitInternal(time::Duration timeout, Condition* condition)
{
  PROTEST_ASSERT(mCondition == nullptr);
  mCondition = condition;
  bool gotTimeout = waitInternal(timeout);
  mCondition = nullptr;
  return gotTimeout;
}

bool
RunnerRaw::waitInternal(time::Duration timeout)
{
  // does not sleep yet -> there cannot be a wakeup call.
  // therefore it is safe to set it back to false here so that it can enter
  // the loop.
  // might be true from a previouse call to wakeInternal
  mWakeUpEvent = false;

  while (timeout > time::Duration::zero() &&
         (mCondition == nullptr || !mCondition->isFulfilled()) && !mWakeUpEvent)
  {
    auto startAt = now();
    auto timeToSleep = timeout;

    if (mPriorityQueue.isAvailable())
    {
      Job* job = mPriorityQueue.peek();
      PROTEST_ASSERT(job->timeTilDue(now()) >= time::Duration::zero());

      if (timeout >= job->timeTilDue(now()))
      {
        timeToSleep = job->timeTilDue(now());
      }
      else
      {
        // timeout is fine
      }
    }
    else
    {
      // timeout is fine
    }

    assert(mWakeUpEvent == false);
    coro::Coroutine::coroWait(timeToSleep);

    // if sleeping for ever, do not set a new timeout
    if (timeout != time::Duration::infinity())
    {
      timeout = timeout - (now() - startAt);
      PROTEST_ASSERT(timeout >= time::Duration::zero());
    }

    while (mPriorityQueue.isAvailable() &&
           mPriorityQueue.peek()->timeTilDue(now()) == time::Duration::zero())
    {
      Job* job = mPriorityQueue.pop();
      job->execute(this);
      job->removed();
    }
  }

  bool gotTimeout =
      !((mCondition != nullptr && mCondition->isFulfilled()) || mWakeUpEvent);
  return gotTimeout;
}

void
RunnerRaw::wakeup()
{
  if (isWaiting())
  {
    mWakeUpEvent = true;
    coroWakeup();
  }
  else
  {
  }
}

// ---------------------------------------------------------------------------
void
RunnerRaw::startSection(const char* name)
{
  std::string number = "(" + std::to_string(mTestSteps) + ")";
  if (name != nullptr)
  {
    number = std::string(name) + " " + number;
  }
  std::stringstream sstream;
  const std::string beginTestStepAsString = "Section";
  sstream << beginTestStepAsString;
  sstream << std::setfill(' ')
          << std::setw(static_cast<int>(seperatorLength -
                                        beginTestStepAsString.size()))
          << std::right << number;
  mCurrentTestStepName = name;
  auto stream = mLogger.startLog("INFO", getName());
  stream.operator std::ostream&() << std::string(seperatorLength, '=') << "\n"
                                  << sstream.str() << "\n"
                                  << std::string(seperatorLength, '-') << "\n";
}

void
RunnerRaw::endSection()
{
  std::string number = "(" + std::to_string(mTestSteps) + ")";
  if (mCurrentTestStepName != nullptr)
  {
    number = std::string(mCurrentTestStepName) + " " + number;
  }
  std::stringstream sstream;
  std::string endTeststepAsString = "End section";
  sstream << endTeststepAsString;
  sstream << std::setfill(' ')
          << std::setw(
                 static_cast<int>(seperatorLength - endTeststepAsString.size()))
          << std::right << number;

  auto stream = mLogger.startLog("INFO", getName());
  stream.operator std::ostream&() << std::string(seperatorLength, '-') << "\n"
                                  << sstream.str() << "\n"
                                  << std::string(seperatorLength, '=') << "\n";
  mTestSteps++;
}

// ---------------------------------------------------------------------------
const char*
RunnerRaw::getName()
{
  return mName;
}

protest::log::Logger&
RunnerRaw::getLogger()
{
  return mLogger;
}

protest::core::Context&
RunnerRaw::getContext()
{
  // NOLINTNEXTLINE
  return static_cast<protest::core::Context&>(getScheduler());
}

// ---------------------------------------------------------------------------
void
RunnerRaw::notify()
{
  if (isWaiting())
  {
    coroWakeup();
  }
  else
  {
  }
}
