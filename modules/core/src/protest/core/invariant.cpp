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

#include "protest/core/invariant.h"
#include "protest/core/runner_raw.h"
#include "protest/log/operator.h"

#include <cassert>

using namespace protest::core;

Invariant::~Invariant()
{
  if (mCondition->isEnabled())
  {
    mCondition->disable();
  }
  else
  {
  }

  delete mCondition;
  mCondition = nullptr;
}

void
Invariant::start()
{
  mCondition->enable();
  // after starting the inv it might already not hold before any
  // other action will occure. E.g. :
  //
  // auto inv = createInvariant(stopwatch != 0_ms);
  // inv.start();
  // wait(0_ms);
  // inv.holds() -> true;
  //
  // the job of (stopwatch != 0_ms) will be executed when the condition
  // changed it's value. This is the case when stopwatch is greater then
  // 0. This is a initial check which prevents the fault explained here.
  notify();
}

void
Invariant::stop()
{
  mCondition->disable();
  // there might be one job (1) that stops the invariant
  // and an other (2) wich would cause the inv to fail.
  // when (1) will be executed before (2) the inv will hold
  // otherwise fail. This call to notify will make sure that
  // this case will not occur since the inv will be checked
  // any way.
  notify();
}

// ---------------------------------------------------------------------------
Invariant::Invariant(const Invariant& /* invariant */) :
  mCondition(nullptr),
  mHolds(false),
  mContext(nullptr)
{
  // must exists but will never be called because of RVO
  // could be '= default' as well
  PROTEST_ASSERT(false);
}

bool
Invariant::holds() const
{
  return mHolds;
}

void
Invariant::notify()
{
  assert(mCondition);
  if (mHolds && !mCondition->isFulfilled())
  {
    assert(mContext);
    mContext->markAsNotHold();
    auto* runner = mCondition->getOwner();
    auto stream =
        runner->getLogger().startLog("INV ",
                                     runner->getName(),
                                     mContext->getUnit().getFileName(),
                                     mContext->getLine(),
                                     runner->now());
    stream.operator std::ostream&() << "Invariant does not hold:\n"
                                    << "'" << mContext->getArg(0) << "'\n";
    mHolds = false;
  }
  else
  {
  }
}
