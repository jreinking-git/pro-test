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

#include "protest/core/condition.h"
#include "protest/meta.h"

namespace protest
{

namespace core
{

// ---------------------------------------------------------------------------
/**
 * @class Invariant
 */
class Invariant : public Condition::Listener
{
public:
  template <typename Expr>
  explicit Invariant(RunnerRaw* runner, Expr expr);

  template <typename Expr>
  explicit Invariant(RunnerRaw* runner, Expr expr, meta::Invariant* context);

  Invariant(Invariant&&) noexcept = default;

  Invariant&
  operator=(const Invariant&) = delete;

  Invariant&
  operator=(Invariant&&) noexcept = delete;

  ~Invariant();

// ---------------------------------------------------------------------------
  void
  start();

  void
  stop();

  bool
  holds() const;

private:
  Invariant(const Invariant&);

  void
  notify() override;

  Condition* mCondition;
  meta::Invariant* mContext;
  bool mHolds;
};

// ---------------------------------------------------------------------------
template <typename Expr>
Invariant::Invariant(RunnerRaw* runner, Expr expr) :
  mCondition(new ExprCondition<Expr>(runner, expr, this)),
  mContext(nullptr),
  mHolds(true)
{
  mContext->markAsCreated();
}

template <typename Expr>
Invariant::Invariant(RunnerRaw* runner, Expr expr, meta::Invariant* context) :
  mCondition(new ExprCondition<Expr>(runner, expr, this)),
  mContext(context),
  mHolds(true)
{
  mContext->markAsCreated();
}

} // namespace core

} // namespace protest
