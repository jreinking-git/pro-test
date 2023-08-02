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

#include "protest/core/expression.h"
#include "protest/core/job.h"
#include "protest/time/duration.h"
#include "protest/utils/list.h"
#include "protest/core/runner_raw.h"
#include "protest/utils/ref_counter.h"

namespace protest
{

namespace core
{

// ---------------------------------------------------------------------------
/**
 * @class Stopwatch
 */
class Stopwatch
{
private:
  class StopwatchInternal;

public:
// ---------------------------------------------------------------------------
  /**
   * @class Operator
   */
  class Operator : public Job::Listener, public Job
  {
  public:
    friend class List<Operator>;

    explicit Operator(StopwatchInternal& stopwatch, time::Duration duration);

    Operator(const Operator&);

    Operator(Operator&&) noexcept = delete;

    Operator&
    operator=(const Operator&) = delete;

    Operator&
    operator=(Operator&&) noexcept = delete;

    ~Operator();

    virtual void
    resetAndNotify() = 0;

// ---------------------------------------------------------------------------
    void
    added(Job* job) override;

    void
    removed(Job* job) override;

  protected:
    time::Duration mDuration;
    StopwatchInternal* mStopwatch;
    RunnerRaw* mRunner;
    Condition* mCondition;

  private:
    Operator* mNext;
  };

// ---------------------------------------------------------------------------
  /**
   * @class Equals
   */
  class Equals : public Operator, public CopyExprTag
  {
  public:
    using Type = bool;

    explicit Equals(StopwatchInternal& stopwatch, time::Duration duration);

    Equals(const Equals&) = default;

    Equals(Equals&&) noexcept = delete;

    Equals&
    operator=(const Equals&) = delete;

    Equals&
    operator=(Equals&&) noexcept = delete;

    ~Equals() = default;

    void
    conditionEnable(RunnerRaw* runner, Condition* condition);

    void
    conditionDisable();

    Type
    getValue();

    void
    notify(RunnerRaw* runner);

    void
    resetAndNotify();

  private:
    void
    reset();
  };

// ---------------------------------------------------------------------------
  /**
   * @class NotEquals
   */
  class NotEquals : public Operator, public CopyExprTag
  {
  public:
    using Type = bool;

    explicit NotEquals(StopwatchInternal& stopwatch, time::Duration duration);

    NotEquals(const NotEquals&) = default;

    NotEquals(NotEquals&&) noexcept = delete;

    NotEquals&
    operator=(const NotEquals&) = delete;

    NotEquals&
    operator=(NotEquals&&) noexcept = delete;

// ---------------------------------------------------------------------------
    ~NotEquals() = default;

    void
    conditionEnable(RunnerRaw* runner, Condition* condition);

    void
    conditionDisable();

    Type
    getValue();

    void
    notify(RunnerRaw* runner);

    void
    resetAndNotify();

  private:
    void
    reset();
  };

// ---------------------------------------------------------------------------
  /**
   * @class Greater
   */
  class Greater : public Operator, public CopyExprTag
  {
  public:
    using Type = bool;

    explicit Greater(StopwatchInternal& stopwatch, time::Duration duration);

    Greater(const Greater&) = default;

    Greater(Greater&&) noexcept = delete;

    Greater&
    operator=(const Greater&) = delete;

    Greater&
    operator=(Greater&&) noexcept = delete;

    ~Greater() = default;

    void
    conditionEnable(RunnerRaw* runner, Condition* condition);

    void
    conditionDisable();

    Type
    getValue();

    void
    notify(RunnerRaw* runner);

    void
    resetAndNotify();

  private:
    void
    reset();
  };

// ---------------------------------------------------------------------------
  /**
   * @class GreaterOrEquals
   */
  class GreaterOrEquals : public Operator, public CopyExprTag
  {
  public:
    using Type = bool;

    explicit GreaterOrEquals(StopwatchInternal& stopwatch,
                             time::Duration duration);

    GreaterOrEquals(const GreaterOrEquals&) = default;

    GreaterOrEquals(GreaterOrEquals&&) noexcept = delete;

    GreaterOrEquals&
    operator=(const GreaterOrEquals&) = delete;

    GreaterOrEquals&
    operator=(GreaterOrEquals&&) noexcept = delete;

    ~GreaterOrEquals() = default;

    void
    conditionEnable(RunnerRaw* runner, Condition* condition);

    void
    conditionDisable();

    Type
    getValue();

    void
    notify(RunnerRaw* runner);

    void
    resetAndNotify();

  private:
    void
    reset();
  };

// ---------------------------------------------------------------------------
  /**
   * @class Less
   */
  class Less : public Operator, public CopyExprTag
  {
  public:
    using Type = bool;

    explicit Less(StopwatchInternal& stopwatch, time::Duration duration);

    Less(const Less&) = default;

    Less(Less&&) noexcept = delete;

    Less&
    operator=(const Less&) = delete;

    Less&
    operator=(Less&&) noexcept = delete;

    ~Less() = default;

    void
    conditionEnable(RunnerRaw* runner, Condition* condition);

    void
    conditionDisable();

    Type
    getValue();

    void
    notify(RunnerRaw* runner);

    void
    resetAndNotify();

  private:
    void
    reset();
  };

// ---------------------------------------------------------------------------
  /**
   * @class LessOrEquals
   */
  class LessOrEquals : public Operator, public CopyExprTag
  {
  public:
    using Type = bool;

    explicit LessOrEquals(StopwatchInternal& stopwatch,
                          time::Duration duration);

    LessOrEquals(const LessOrEquals&) = default;

    LessOrEquals(LessOrEquals&&) noexcept = delete;

    LessOrEquals&
    operator=(const LessOrEquals&) = delete;

    LessOrEquals&
    operator=(LessOrEquals&&) noexcept = delete;

    ~LessOrEquals() = default;

    void
    conditionEnable(RunnerRaw* runner, Condition* condition);

    void
    conditionDisable();

    Type
    getValue();

    void
    notify(RunnerRaw* runner);

    void
    resetAndNotify();

  private:
    void
    reset();
  };

// ---------------------------------------------------------------------------
  explicit Stopwatch();

  explicit Stopwatch(RunnerRaw* runner);

  Stopwatch(const Stopwatch&) = delete;

  Stopwatch(Stopwatch&&) noexcept = delete;

  Stopwatch&
  operator=(const Stopwatch&) = delete;

  Stopwatch&
  operator=(Stopwatch&& other) noexcept;

  ~Stopwatch();

// ---------------------------------------------------------------------------
  void
  start();

  void
  stop();

  void
  reset();

  time::Duration
  lap();

// ---------------------------------------------------------------------------
  Equals
  operator==(time::Duration duration);

  NotEquals
  operator!=(time::Duration duration);

  Greater
  operator>(time::Duration duration);

  GreaterOrEquals
  operator>=(time::Duration duration);

  Less
  operator<(time::Duration duration);

  LessOrEquals
  operator<=(time::Duration duration);

private:
// ---------------------------------------------------------------------------
  /**
   * @class StopwatchInternal
   */
  class StopwatchInternal : public RefCounter
  {
  public:
    friend class GreaterEquals;

    explicit StopwatchInternal(RunnerRaw* runner);

    StopwatchInternal(const StopwatchInternal&) = delete;

    StopwatchInternal(StopwatchInternal&&) noexcept = delete;

    StopwatchInternal&
    operator=(const StopwatchInternal&) = delete;

    StopwatchInternal&
    operator=(StopwatchInternal&&) noexcept = delete;

    ~StopwatchInternal() = default;

    void
    start();

    void
    stop();

    void
    reset();

    void
    update(Operator& opr, time::Duration duration);

    time::Duration
    lap();

    Equals
    operator==(time::Duration duration);

    NotEquals
    operator!=(time::Duration duration);

    Greater
    operator>(time::Duration duration);

    GreaterOrEquals
    operator>=(time::Duration duration);

    Less
    operator<(time::Duration duration);

    LessOrEquals
    operator<=(time::Duration duration);

    void
    add(Operator& opr, Condition* condition);

    void
    remove(Operator& opr, Condition* condition);

  private:
    List<Condition> mConditions;
    List<Operator> mOperators;
    RunnerRaw* mRunner;
    time::TimePoint mStartOrStopedAt;
    time::Duration mTimePassed;
    bool mRunning;
  };

  StopwatchInternal* mInternal;
};

} // namespace core

} // namespace protest
