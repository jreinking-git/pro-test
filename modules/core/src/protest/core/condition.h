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
#include "protest/utils/list.h"
#include "protest/utils/log.h"

namespace protest
{

template <typename T>
class List;

namespace core
{

// ---------------------------------------------------------------------------
/**
 * @class Condition
 */
class Condition
{
public:
  friend class List<Condition>;

// ---------------------------------------------------------------------------
  /**
   * @class Listener
   */
  class Listener
  {
  public:
    explicit Listener() = default;

    Listener(const Listener&) = delete;

    Listener(Listener&&) noexcept = default;

    Listener&
    operator=(const Listener&) = delete;

    Listener&
    operator=(Listener&&) noexcept = delete;

    virtual ~Listener() = default;

// ---------------------------------------------------------------------------
    virtual void
    notify() = 0;

  private:
  };

// ---------------------------------------------------------------------------
  explicit Condition(RunnerRaw* runner, Listener* listener);

  Condition(const Condition&) = delete;

  Condition(Condition&&) noexcept = delete;

  Condition&
  operator=(const Condition&) = delete;

  Condition&
  operator=(Condition&&) noexcept = delete;

  virtual ~Condition() = default;

// ---------------------------------------------------------------------------
  void
  enable();

  void
  disable();

  virtual void
  conditionEnable() = 0;

  virtual void
  conditionDisable() = 0;

  virtual bool
  isFulfilled() = 0;

// ---------------------------------------------------------------------------
  void
  notifyListener();

  RunnerRaw*
  getOwner();

  bool
  isEnabled() const;

private:
  Condition* mNext;
  RunnerRaw* mRunner;
  Listener* mListener;
  bool mIsEnable;
};

// ---------------------------------------------------------------------------
/**
 * @class ExprCondition
 */
template <typename Expr>
class ExprCondition : public Condition
{
public:
  explicit ExprCondition(RunnerRaw* runner,
                         Expr expr,
                         Condition::Listener* listener);

  ExprCondition(const ExprCondition&) = delete;

  ExprCondition(ExprCondition&&) noexcept = delete;

  ExprCondition&
  operator=(const ExprCondition&) = delete;

  ExprCondition&
  operator=(ExprCondition&&) noexcept = delete;

  ~ExprCondition() = default;

// ---------------------------------------------------------------------------
  void
  conditionEnable() override;

  void
  conditionDisable() override;

  bool
  isFulfilled() override;

private:
  Expr mExpr;
  bool mResult;
};

// ---------------------------------------------------------------------------
template <typename Expr>
ExprCondition<Expr>::ExprCondition(RunnerRaw* runner,
                                   Expr expr,
                                   Condition::Listener* listener) :
  Condition(runner, listener),
  mExpr(expr),
  mResult(false)
{
}

// ---------------------------------------------------------------------------
template <typename Expr>
void
ExprCondition<Expr>::conditionEnable()
{
  mExpr.conditionEnable(getOwner(), this);
}

template <typename Expr>
void
ExprCondition<Expr>::conditionDisable()
{
  mExpr.conditionDisable();
}

template <typename Expr>
bool
ExprCondition<Expr>::isFulfilled()
{
  return mExpr.getValue();
}

} // namespace core

} // namespace protest
