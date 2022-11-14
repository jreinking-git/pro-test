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

#include "protest/core/job.h"
#include "protest/coro/coroutine.h"
#include "protest/utils/heap.h"
#include "protest/log/logger.h"
#include "protest/core/condition.h"
#include "protest/core/context.h"

namespace protest
{

namespace core
{

class Condition;

// ---------------------------------------------------------------------------
/**
 * @class RunnerRaw
 */
class RunnerRaw : public coro::Coroutine, public Condition::Listener
{
public:
  friend class List<RunnerRaw>;

  explicit RunnerRaw(const char* name);

  explicit RunnerRaw(core::Context& context, const char* name);

  RunnerRaw(const RunnerRaw&) = delete;

  RunnerRaw(RunnerRaw&&) noexcept = delete;

  RunnerRaw&
  operator=(const RunnerRaw&) = delete;

  RunnerRaw&
  operator=(RunnerRaw&&) noexcept = delete;

  virtual ~RunnerRaw();

// ---------------------------------------------------------------------------
  void
  internalInitialize();

  virtual void
  initialize();

  virtual void
  process();

  void
  internalFinalize();

  virtual void
  finalize();

// ---------------------------------------------------------------------------
  void
  add(Job& job);

  void
  remove(Job& job);

// ---------------------------------------------------------------------------
  void
  coroRun() override;

// ---------------------------------------------------------------------------
  bool
  waitInternal(time::Duration timeout, Condition* condition);

  bool
  waitInternal(time::Duration timeout);

  void
  wakeup();

// ---------------------------------------------------------------------------
  void
  startSection(const char* name);

  void
  endSection();

// ---------------------------------------------------------------------------
  const char*
  getName();

  log::Logger&
  getLogger();

// ---------------------------------------------------------------------------
  core::Context&
  getContext();

private:
  static constexpr size_t seperatorLength = 79;

  void
  notify() override;

  Heap<Job, 100> mPriorityQueue;// TODO
  Condition* mCondition;
  const char* mName;
  bool mWakeUpEvent;
  log::Logger mLogger;
  RunnerRaw* mNext;

  uint32_t mTestSteps;
  const char* mCurrentTestStepName;
};

} // namespace core

} // namespace protest
