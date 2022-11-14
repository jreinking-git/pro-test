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

#include "protest/coro/coroutine.h"
#include "protest/coro/logical_clock.h"
#include "protest/utils/heap.h"
#include "protest/utils/queue.h"

#include <cstdio>
#include <cstdlib>

namespace protest
{

namespace coro
{

// ---------------------------------------------------------------------------
/**
 * @class Scheduler
 */
class Scheduler
{
public:
  static constexpr bool sleepOnWaiting = true;
  static constexpr size_t maxNumberOfThreads = 100u;

  explicit Scheduler();

  Scheduler(const Scheduler&) = delete;

  Scheduler(Scheduler&&) noexcept = delete;

  Scheduler&
  operator=(const Scheduler&) = delete;

  Scheduler&
  operator=(Scheduler&&) noexcept = delete;

  ~Scheduler() = default;

// ---------------------------------------------------------------------------
  void
  addThread(Coroutine* thread);

// ---------------------------------------------------------------------------
  void
  yield(Coroutine* thread);

  void
  sleep(Coroutine* thread, time::Duration duration);

  void
  wakeup(Coroutine* thread);

  void
  exit(Coroutine* thread);

  time::TimePoint
  now(Coroutine* thread);

  time::TimePoint
  now();

// ---------------------------------------------------------------------------
  void
  run();

  Coroutine*
  getCurrent();

// ---------------------------------------------------------------------------
protected:
  static constexpr size_t stackSize = 4096U;

  void
  executeNext(Coroutine* prev);

  Coroutine*
  getNext();

  Coroutine*
  popFromSleepQueue();

  void
  removeFromSleepQueue(Coroutine* thread);

  void
  pushToSleepQueue(Coroutine* thread);

  LogicalClock mClock;
  uint16_t mNumberOfSleepingCoroutines;
  Heap<Coroutine, maxNumberOfThreads> mSleepQueue;
  Queue<Coroutine> mRunQueue;
  Coroutine* mCurrent;

public:
  CoroutineBase::CoroContext mMain;
};

} // namespace coro

} // namespace protest
