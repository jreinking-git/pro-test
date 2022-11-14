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

#include "protest/time/time_point.h"
#include "protest/time/duration.h"

namespace protest
{

template <typename T, size_t N>
class Heap;

template <typename T>
class List;

namespace core
{

class RunnerRaw;
class Condition;

// ---------------------------------------------------------------------------
/**
 * @class Job
 */
class Job
{
public:
  friend class Heap<Job, 100u>;// TODO magic number
// ---------------------------------------------------------------------------
  /**
   * @class Listener
   */
  class Listener
  {
  public:
    explicit Listener() = default;

    Listener(const Listener&) = default;

    Listener(Listener&&) noexcept = delete;

    Listener&
    operator=(const Listener&) = delete;

    Listener&
    operator=(Listener&&) noexcept = delete;

    ~Listener() = default;

// ---------------------------------------------------------------------------
    virtual void
    notify(RunnerRaw* runner) = 0;

    virtual void
    added(Job* job) {};

    virtual void
    removed(Job* job) {};

  private:
  };

// ---------------------------------------------------------------------------
  explicit Job();

  Job(const Job&) = default;

  Job(Job&&) noexcept = delete;

  Job&
  operator=(const Job&) = delete;

  Job&
  operator=(Job&&) noexcept = delete;

  ~Job() = default;

// ---------------------------------------------------------------------------
  void
  added();

  void
  removed();

  void
  setDue(time::TimePoint due, Job::Listener* listener);

  void
  updateDue(time::Duration duration);

  bool
  isDue(time::TimePoint now);

  time::Duration
  timeTilDue(time::TimePoint now);

  void
  execute(RunnerRaw* runner);

  void
  notExecuted();

  bool
  isArmed() const;

  bool
  operator<(Job& rhs);

  bool
  operator<=(Job& rhs);

private:
  bool mArmed;
  Listener* mCondition;
  time::TimePoint mDue;
  size_t mIndex;
};

} // namespace core

} // namespace protest
