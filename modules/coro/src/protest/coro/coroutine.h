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

#include "protest/utils/queue.h"
#include "protest/utils/list.h"
#include "protest/utils/heap.h"
#include "protest/time/duration.h"
#include "protest/time/time_point.h"
#include "protest/coro/coroutine_base.h"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

namespace protest
{

namespace coro
{

class Scheduler;
class Semaphore;

// ---------------------------------------------------------------------------
/**
 * @class Coroutine
 */
class Coroutine : public CoroutineBase
{
public:
  friend class Scheduler;
  friend class Queue<Coroutine>;
  friend class Heap<Coroutine, 100>;// TODO
  friend class List<Coroutine>;
  friend class Condition;

  explicit Coroutine(Scheduler& scheduler);

  Coroutine(const Coroutine&) = delete;

  Coroutine(Coroutine&&) noexcept = delete;

  Coroutine&
  operator=(const Coroutine&) = delete;

  Coroutine&
  operator=(Coroutine&&) noexcept = delete;

  virtual ~Coroutine() = default;

// ---------------------------------------------------------------------------
  void
  contextSwitch(CoroutineBase::CoroContext* parent);

  bool
  coroWait(time::Duration duration);

  void
  coroYield();

  void
  coroWakeup();

  void
  coroExit();

// ---------------------------------------------------------------------------
  bool
  isWaiting() const;

  time::TimePoint
  now();

  Coroutine*
  getCurrent();

  Scheduler&
  getScheduler();

// ---------------------------------------------------------------------------
  bool
  operator<(const Coroutine& other);

  bool
  operator<=(const Coroutine& other);

// ---------------------------------------------------------------------------
  // TODO (jreinking) make private
protected:
  Scheduler& mScheduler;
  // TODO move to port
  char* mStack;
  time::TimePoint mSleepUntil;
  Coroutine* mNext;
  size_t mIndex;
  bool mIsWaiting;
  bool mIsInSleepQueue;
};

} // namespace coro

} // namespace protest
