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

#include "protest/core/job.h"
#include "protest/core/condition.h"

using namespace protest;
using namespace protest::core;

// ---------------------------------------------------------------------------
Job::Job() :
  mArmed(false),
  mCondition(nullptr),
  mDue(time::TimePoint::endOfEpoche()),
  mIndex(0U)
{
}

void
Job::execute(RunnerRaw* runner)
{
  PROTEST_ASSERT(mArmed);
  mArmed = false;
  mCondition->notify(runner);
}

void
Job::notExecuted()
{
  PROTEST_ASSERT(mArmed);
  mArmed = false;
}

bool
Job::isArmed() const
{
  return mArmed;
}

void
Job::added()
{
  mCondition->added(this);
}

void
Job::removed()
{
  mCondition->removed(this);
}

void
Job::setDue(time::TimePoint due, Job::Listener* listener)
{
  PROTEST_ASSERT(!mArmed);
  mCondition = listener;
  mArmed = true;
  mDue = due;
}

void
Job::updateDue(time::Duration duration)
{
  PROTEST_ASSERT(!mArmed);
  mDue = mDue + duration;
  mArmed = true;
}

bool
Job::isDue(time::TimePoint now)
{
  return mDue <= now;
}

time::Duration
Job::timeTilDue(time::TimePoint now)
{
  return mDue - now;
}

// NOLINTNEXTLINE
bool
Job::operator<(Job& rhs)
{
  PROTEST_ASSERT(this->isArmed());
  PROTEST_ASSERT(rhs.isArmed());
  return this->mDue < rhs.mDue;
}

// NOLINTNEXTLINE
bool
Job::operator<=(Job& rhs)
{
  PROTEST_ASSERT(this->isArmed());
  PROTEST_ASSERT(rhs.isArmed());
  return this->mDue <= rhs.mDue;
}
