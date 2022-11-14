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

#pragma once

#include "protest/coro/scheduler.h"
#include "protest/log/logger.h"
#include "protest/meta/test_manager.h"
#include "protest/meta/call_context.h"
#include "protest/doc/doc_manager.h"

namespace protest
{

namespace core
{

class RunnerRaw;

/**
 * @class Context
 * 
 * Usally there will be one context per executable. Some top level protest
 * objects like runner or signals exists exactly once in a context.
 */
class Context : public coro::Scheduler
{
public:
// ---------------------------------------------------------------------------
  /**
   * @brief getCurrentContext
   * 
   * returns the current context.
   * 
   * @return
   *  the current context
   */
  static Context*
  getCurrentContext();

// ---------------------------------------------------------------------------
  explicit Context(protest::meta::CallContext& context =
                       protest::meta::CallContext::defaultContext());

  Context(const Context& other) = delete;

  Context(Context&& other) noexcept = delete;

  Context&
  operator=(const Context& other) = delete;

  Context&
  operator=(Context&& other) noexcept = delete;

  ~Context() = default;

// ---------------------------------------------------------------------------
  void
  addRunner(RunnerRaw* runner);

  /**
   * @brief initialize
   * 
   * must be called at the start of the program in the main function.
   */
  void
  initialize();

  int
  run();

// ---------------------------------------------------------------------------
  /**
   * @brief getCurrent
   * 
   * returns the current runner.
   * 
   * @return
   *  the current runner
   */
  RunnerRaw*
  getCurrent();

  /**
   * @brief getCurrentVirtual
   * 
   * returns the current virtual runner. This function distinguished between
   * the runner which is actually running ( @c getCurrent() ) and the runner
   * which is virtually running. For example: if runner A is executing a
   * callback of runner B, runner A is the actual runner executing a callback
   * for the virtual runner A. A.k.a A is executing with the name of B.
   * 
   * @return
   *  the current virtual runner
   */
  RunnerRaw*
  getCurrentVirtual();

  /**
   * @brief setCurrentVirtual
   */
  void
  setCurrentVirtual(RunnerRaw* runner);

// ---------------------------------------------------------------------------
  /**
   * @brief getTestManager
   * 
   * returns the test manager
   * 
   * @return
   *  the test manager
   */
  meta::TestManager&
  getTestManager();

  meta::CallContext&
  getCallContext();

  int
  getExitValue();

private:
  static Context* currentContext;
  meta::TestManager mTestManager;
  protest::List<RunnerRaw> mRunners;
  meta::CallContext& mCallContext;
  RunnerRaw* mCurrentVirtual;
  // TODO (jreinking) should not use doc manager directly. Use listener pattern
  // instead
  protest::doc::DocManager mDocManager;
};

} // namespace core

} // namespace protest