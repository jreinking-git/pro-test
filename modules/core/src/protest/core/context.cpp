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

#include "protest/core/context.h"
#include "protest/core/runner_raw.h"

using namespace protest::core;
using namespace protest::coro;
using namespace protest::meta;

// ---------------------------------------------------------------------------
// NOLINTNEXTLINE
Context* Context::currentContext = nullptr;

// ---------------------------------------------------------------------------
Context::Context(protest::meta::CallContext& context) :
  mCallContext(context),
  mCurrentVirtual(nullptr),
  mDocManager(*this)
{
}

// ---------------------------------------------------------------------------
Context*
Context::getCurrentContext()
{
  return currentContext;
}

protest::log::Logger&
Context::getCurrentLogger()
{
  auto* context = Context::getCurrentContext();
  auto* runner = context->getCurrentVirtual();
  return runner->getLogger();
}

// ---------------------------------------------------------------------------
RunnerRaw*
Context::getCurrent()
{
  auto* current =
      (dynamic_cast<protest::core::RunnerRaw*>(coro::Scheduler::getCurrent()));
  PROTEST_ASSERT(current);
  return current;
}

// ---------------------------------------------------------------------------
void
Context::addRunner(RunnerRaw* runner)
{
  // Scheduler::addThread(runner);
  mRunners.prepend(*runner);
}

void
Context::initialize()
{
  currentContext = this;
  mTestManager.initialize();
}

int
Context::run()
{
  mDocManager.printPreamble();

  {
    auto iter = mRunners.begin();
    while (iter != mRunners.end())
    {
      mCurrent = &*iter;
      iter->internalInitialize();
      ++iter;
    }
  }

  Scheduler::run();

  {
    auto iter = mRunners.begin();
    while (iter != mRunners.end())
    {
      mCurrent = &*iter;
      iter->internalFinalize();
      ++iter;
    }
  }

  mCurrent = nullptr;
  mDocManager.printPostamble();

  return getExitValue();
}

// ---------------------------------------------------------------------------
RunnerRaw*
Context::getCurrentVirtual()
{
  auto* runner = getCurrent();
  if (mCurrentVirtual != nullptr)
  {
    runner = mCurrentVirtual;
  }
  return runner;
}

void
Context::setCurrentVirtual(RunnerRaw* runner)
{
  mCurrentVirtual = runner;
}

// ---------------------------------------------------------------------------
TestManager&
Context::getTestManager()
{
  return mTestManager;
}

protest::meta::CallContext&
Context::getCallContext()
{
  return mCallContext;
}

// ---------------------------------------------------------------------------
int
Context::getExitValue()
{
  return (mTestManager.getNumberOfFailedAssertions() > 0 ||
          mTestManager.getNumberOfFailedInvariants() > 0)
             ? 1
             : 0;
}
