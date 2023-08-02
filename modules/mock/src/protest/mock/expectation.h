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

#include "protest/mock/action.h"
#include "protest/mock/cardinality.h"
#include "protest/mock/function_mocker.h"
#include "protest/log/universal_stream.h"
#include "protest/meta/call_context.h"
#include "protest/mock/sequence.h"
#include "protest/core/condition.h"

#include <memory>
#include <vector>
#include <set>

#include <cstdint>
#include <cstddef>

namespace protest
{

namespace mock
{

class ImplicitSequence;
class Sequence;

template <typename F>
class ExpectationHandle;

template <typename F>
class Expectation;

namespace internal
{

// ---------------------------------------------------------------------------
/**
 * @class ExpectationBase
 * 
 * Generic and untyped expectation which is the base class of all typed
 * expectation. It holds data which does not rely on any concrete function
 * type.
 */
class ExpectationBase
{
public:
  template <typename F>
  friend class FunctionMockerBase;

  friend class protest::mock::ImplicitSequence;
  friend class protest::mock::Sequence;

  friend class internal::FunctionMockerRaw;

  /**
   * @brief ExpectationBase
   * 
   * @param function
   *  this is an expectation for function
   * 
   * @param callContext
   *  call context of @c expectCall() which created this expectation
   */
  explicit ExpectationBase(std::shared_ptr<FunctionMockerRaw> function,
                           meta::ExpectCall& callContext);

  ExpectationBase(const ExpectationBase& other) = delete;

  ExpectationBase(ExpectationBase&& other) = delete;

  ExpectationBase&
  operator=(const ExpectationBase& other) = delete;

  ExpectationBase&
  operator=(ExpectationBase&& other) = delete;

  virtual ~ExpectationBase();

// ---------------------------------------------------------------------------
  void
  after(std::shared_ptr<ExpectationBase>& expectation);

  void
  addPrerequisites(std::shared_ptr<internal::ExpectationBase>& expectation);

  bool
  prerequisitesMet();

  void
  retireAllPrerequisites(bool first);

// ---------------------------------------------------------------------------
  /**
   * @brief explain
   * 
   * This function will be called on destruction of the mock object. The
   * function checks all expectations and reports missing function calls if
   * there are any.
   */
  void
  explain();

  /**
   * @brief getCallContext
   * 
   * The call context of the corresponding @c expectCall() call.
   * 
   * @return the call context
   */
  meta::ExpectCall&
  getCallContext();

  internal::Cardinality&
  getCardinality();

  bool
  isSaturated();

  bool
  isSatisfied();

  bool
  retiresOnSaturation() const;

  void
  incrementCallCounter();

// ---------------------------------------------------------------------------
  bool
  isWhenConditionFulfilled();

  bool
  hasWhenClause();

  const char*
  getWhenConditionAsString();

protected:
  core::Condition* mCondition;
  meta::CallContext* mConditionContext;
  bool mCardinalitySpecified;
  bool mRetiresOnSaturationCalled;
  bool mWillRepeatedlyCalled;
  size_t mCallCounter;
  internal::Cardinality mCardinality;
  meta::ExpectCall& mCallContext;
  std::shared_ptr<FunctionMockerRaw> mFunction;
  std::set<std::shared_ptr<ExpectationBase>> mPrerequisites;
};

} // namespace internal

// ---------------------------------------------------------------------------
/**
 * @class Expectation
 * 
 * Expectation for a function type. A expectation will be crated for any call
 * to @c expectThat . I.e.:
 * 
 * expectThat(mock.doSomething()).willOnce(Return(42));
 * 
 * will create a expectation, which will be added to the mock.
 * 
 * @tparam F
 *  this expectation belongs to the function reperesented by the type F
 */
template <typename F>
class Expectation : public internal::ExpectationBase
{
public:
  friend class internal::FunctionMockerBase<F>;

  friend class internal::FunctionMockerRaw;

  using Function = F;
  using ReturnType = typename F::ReturnType;
  using Args = typename F::Args;
  using Matchers = typename F::Matchers;

  /**
   * @brief Expectation
   * 
   * This class holds the expectation data and provides an interface for the
   * user to specify the expectation.
   * 
   * @param owner
   *  This expectation belongs to the given owner (mock)
   * 
   * @param matchers
   *  Matcher for the parameter
   * 
   * @param callContext 
   *  the call context of the corresponding @c expectCall() call
   */
  explicit Expectation(std::shared_ptr<internal::FunctionMockerBase<F>> owner,
                       Matchers&& matchers,
                       meta::ExpectCall& callContext);

  Expectation(const Expectation& other) = delete;

  Expectation(Expectation&& other) = delete;

  Expectation&
  operator=(const Expectation& other) = delete;

  Expectation&
  operator=(Expectation&& other) = delete;

  ~Expectation();

// ---------------------------------------------------------------------------
  void
  times(size_t n);

  void
  times(internal::Cardinality&& cardinality);

  void
  willOnce(Action<F>&& action);

  void
  willRepeatedly(Action<F>&& action);

  void
  retireOnSaturation();

  template <typename Expr>
  void
  when(Expr&& expr, protest::meta::CallContext& callContext);

// ---------------------------------------------------------------------------
private:
  Action<F>&
  getCurrentAction();

  Action<F>&
  getRepeatAction();

  bool
  isMatch(Args& args);

  template <size_t I>
  bool
  isMatchInternal(Args& args);

  Matchers mMatchers;
  std::vector<Action<F>> mActions;
  Action<F> mReapetedAction;
};

// ---------------------------------------------------------------------------
template <typename F>
Expectation<F>::Expectation(
    std::shared_ptr<internal::FunctionMockerBase<F>> owner,
    Matchers&& matchers,
    meta::ExpectCall& callContext) :
  protest::mock::internal::ExpectationBase(owner, callContext),
  mMatchers(std::move(matchers))
{
}

template <typename F>
Expectation<F>::~Expectation()
{
}

// ---------------------------------------------------------------------------
template <typename F>
void
Expectation<F>::times(size_t n)
{
  mCardinalitySpecified = true;
  mCardinality = exactly(n);
}

template <typename F>
void
Expectation<F>::times(internal::Cardinality&& cardinality)
{
  mCardinalitySpecified = true;
  mCardinality = std::move(cardinality);
}

template <typename F>
void
Expectation<F>::willOnce(Action<F>&& action)
{
  assert(!mWillRepeatedlyCalled);// willOnce cannot appear after willRepeatedly
  assert(!mRetiresOnSaturationCalled);// retire on saturation must be last
  mActions.emplace_back(std::move(action));
  if (!mCardinalitySpecified)
  {
    mCardinality = exactly(mActions.size());
  }
}

template <typename F>
void
Expectation<F>::willRepeatedly(Action<F>&& action)
{
  assert(!mWillRepeatedlyCalled);// should not be called twice
  assert(!mRetiresOnSaturationCalled);// retire on saturation must be last
  mWillRepeatedlyCalled = true;
  mReapetedAction = std::move(action);
  if (!mCardinalitySpecified)
  {
    mCardinality = atLeast(mActions.size());
  }
}

template <typename F>
void
Expectation<F>::retireOnSaturation()
{
  assert(!mRetiresOnSaturationCalled);
  mRetiresOnSaturationCalled = true;
}

template <typename F>
template <typename Expr>
void
Expectation<F>::when(Expr&& expr, protest::meta::CallContext& callContext)
{
  // should only be called once
  assert(mCondition == nullptr);
  auto* context = core::Context::getCurrentContext();
  auto* runner = context->getCurrentVirtual();
  mCondition = new core::ExprCondition<Expr>(runner, expr, nullptr);
  mConditionContext = &callContext;
}

// ---------------------------------------------------------------------------
template <typename F>
Action<F>&
Expectation<F>::getCurrentAction()
{
  retireAllPrerequisites(true);
  incrementCallCounter();

  if (mCallCounter <= mActions.size())
  {
    return mActions.at(mCallCounter - 1);
  }
  else
  {
    return getRepeatAction();
  }
}

template <typename F>
Action<F>&
Expectation<F>::getRepeatAction()
{
  return mReapetedAction;
}

template <typename F>
bool
Expectation<F>::isMatch(Args& args)
{
  return isMatchInternal<0>(args);
}

// ---------------------------------------------------------------------------
template <typename F>
template <size_t I>
bool
Expectation<F>::isMatchInternal(typename F::Args& args)
{
  if constexpr (std::tuple_size<typename F::Args>::value == I)
  {
    return true;
  }
  else
  {
    return std::get<I>(mMatchers).check(std::get<I>(args)) &&
           isMatchInternal<I + 1>(args);
  }
}

// ---------------------------------------------------------------------------
/**
 * @class ExpectationHandle
 */
template <typename F>
class ExpectationHandle
{
public:
  using Function = F;

  explicit ExpectationHandle(std::shared_ptr<Expectation<F>> expectation);

  ExpectationHandle(const ExpectationHandle& other) = default;

  ExpectationHandle(ExpectationHandle&& other) = default;

  ExpectationHandle&
  operator=(const ExpectationHandle& other) = default;

  ExpectationHandle&
  operator=(ExpectationHandle&& other) = default;

  ~ExpectationHandle() = default;

  operator std::shared_ptr<internal::ExpectationBase>();

// ---------------------------------------------------------------------------
  /**
   * @brief times
   * 
   * Short for @c times(exactly(n))
   * 
   * @param n
   *  the number of expected calls
   * 
   * @return this object
   */
  ExpectationHandle
  times(size_t n);

  /**
   * @brief times
   * 
   * Specify the cardinality. There can only be one per expectation.
   * 
   * @param cardinality
   *  the cardinality to check
   * 
   * @return this object
   */
  ExpectationHandle
  times(internal::Cardinality&& cardinality);

  /**
   * @brief willOnce
   * 
   * Specify the action. This function can be called multiple time on a
   * expectation object. The actions will be executed in the order there
   * where added to the expectation
   * 
   * @param action 
   *  the action to add
   * 
   * @return this object
   */
  ExpectationHandle
  willOnce(Action<F>&& action);

  /**
   * @brief willRepeatedly
   * 
   * This function adds an fallback action if all Actions registered with
   * @c willOnce are saturated.
   * 
   * @param action
   *  the default action to add
   * 
   * @return this object
   */
  ExpectationHandle
  willRepeatedly(Action<F>&& action);

  /**
   * @brief retireOnSaturation
   * 
   * When the cardinality of this expectation saturates and this expectation
   * is set to 'retire on saturation' this expectation will retire (I.e.: it
   * will be ignored for any further calls to the mock function)
   * 
   * @return this object 
   */
  ExpectationHandle
  retireOnSaturation();

  /**
   * @brief after
   * 
   * Add a expectation to the set of prerequisites.
   * 
   * @param expectation
   *  the expectation to add to the prerequisites
   * 
   * @return this object
   */
  ExpectationHandle
  after(std::shared_ptr<internal::ExpectationBase> expectation);

  /**
   * @brief inSequence
   * 
   * Add this expectation to the sequence. 
   * 
   * @brief s1
   *  the sequence to add the expectation to
   */
  ExpectationHandle
  inSequence(Sequence& s1);

  ExpectationHandle
  inSequence(Sequence& s1, Sequence& s2);

  ExpectationHandle
  inSequence(Sequence& s1, Sequence& s2, Sequence& s3);

  /**
   * @brief when
   * 
   * Add a conditon to the expectation. E.g.:
   * 
   * expectCall(mock.doSomething())
   *  .willRepeatedly(Return(1))
   *  .when(stopwatch < 3000_ms);
   * 
   * expectCall(mock.doSomething())
   *  .willRepeatedly(Return(2))
   *  .when(stopwatch >= 3000_ms);
   */
  template <typename Expr>
  ExpectationHandle
  when(Expr&& expr,
       protest::meta::CallContext& callContext =
           protest::meta::CallContext::defaultContext());

  std::shared_ptr<Expectation<F>>
  getExpectation();

private:
  std::shared_ptr<Expectation<F>> mExpectation;
};

// ---------------------------------------------------------------------------
template <typename F>
ExpectationHandle<F>::ExpectationHandle(
    std::shared_ptr<Expectation<F>> expectation) :
  mExpectation(expectation)
{
}

template <typename F>
ExpectationHandle<F>::operator std::shared_ptr<internal::ExpectationBase>()
{
  return mExpectation;
}

// ---------------------------------------------------------------------------
template <typename F>
ExpectationHandle<F>
ExpectationHandle<F>::times(size_t n)
{
  mExpectation->times(n);
  return *this;
}

template <typename F>
ExpectationHandle<F>
ExpectationHandle<F>::times(internal::Cardinality&& cardinality)
{
  mExpectation->times(std::move(cardinality));
  return *this;
}

template <typename F>
ExpectationHandle<F>
ExpectationHandle<F>::willOnce(Action<F>&& action)
{
  mExpectation->willOnce(std::move(action));
  return *this;
}

template <typename F>
ExpectationHandle<F>
ExpectationHandle<F>::willRepeatedly(Action<F>&& action)
{
  mExpectation->willRepeatedly(std::move(action));
  return *this;
}

template <typename F>
ExpectationHandle<F>
ExpectationHandle<F>::retireOnSaturation()
{
  mExpectation->retireOnSaturation();
  return *this;
}

template <typename F>
ExpectationHandle<F>
ExpectationHandle<F>::after(
    std::shared_ptr<internal::ExpectationBase> expectation)
{
  mExpectation->after(expectation);
  return *this;
}

template <typename F>
ExpectationHandle<F>
ExpectationHandle<F>::inSequence(Sequence& s1)
{
  auto exp = std::dynamic_pointer_cast<internal::ExpectationBase>(mExpectation);
  s1.addExpectation(exp);
  return *this;
}

template <typename F>
ExpectationHandle<F>
ExpectationHandle<F>::inSequence(Sequence& s1, Sequence& s2)
{
  inSequence(s1).inSequence(s2);
  return *this;
}

template <typename F>
ExpectationHandle<F>
ExpectationHandle<F>::inSequence(Sequence& s1, Sequence& s2, Sequence& s3)
{
  inSequence(s1).inSequence(s2, s3);
  return *this;
}

template <typename F>
template <typename Expr>
ExpectationHandle<F>
ExpectationHandle<F>::when(Expr&& expr, protest::meta::CallContext& callContext)
{
  mExpectation->when(std::forward<Expr>(expr), callContext);
  return *this;
}

template <typename F>
std::shared_ptr<Expectation<F>>
ExpectationHandle<F>::getExpectation()
{
  return mExpectation;
}

} // namespace mock

} // namespace protest