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

#include "protest/utils/list.h"
#include "protest/log/universal_stream.h"
#include "protest/core/context.h"
#include "protest/core/runner_raw.h"
#include "protest/meta/call_context.h"

#include <memory>
#include <tuple>
#include <vector>

namespace protest
{

namespace mock
{

template <typename F>
class Expectation;

namespace internal
{

class MockBase;
class ExpectationBase;

// ---------------------------------------------------------------------------
/**
 * @class FunctionMockerRaw
 * 
 * Class which holds runtime information about a mocked function. Which
 * includes static information like the name of the parameters but also the
 * expectation added by the user.
 */
class FunctionMockerRaw
{
public:
  /** 
   * @brief isSaturated
   * 
   * calls @c expectation->isSatureted()
   * this is just to move it into the source file to avoid the usage of
   * incomplete type in template functions.
   */
  static bool
  isSaturated(ExpectationBase* expectation);

  /**
   * @brief FunctionMockerRaw
   * 
   * @param mock
   *  the mock which owns the function
   * 
   * @param name
   *  the functions name
   * 
   * @param paramTypes
   *  name of type of parameters of mocked function
   * 
   * @param params
   *  name of parameters of mocked function
   * 
   * @param numParams
   *  number of parameters of mocked function
   */
  explicit FunctionMockerRaw(MockBase& mock,
                             const char* name,
                             const char* const* paramTypes,
                             const char* const* params,
                             size_t numParams);

  FunctionMockerRaw(const FunctionMockerRaw&) = delete;

  FunctionMockerRaw(FunctionMockerRaw&&) noexcept = delete;

  FunctionMockerRaw&
  operator=(const FunctionMockerRaw&) = delete;

  FunctionMockerRaw&
  operator=(FunctionMockerRaw&&) noexcept = delete;

  ~FunctionMockerRaw() = default;

// ---------------------------------------------------------------------------
  /**
   * @brief getCallContext
   * 
   * get the call context of the @c createMock<F>() call.
   * 
   * @return the call context of the mock
   */
  meta::MockCreation&
  getCallContext();

  /**
   * @brief addExpectation
   * 
   * add a expectation to the mocked function.
   * 
   * @param expectation
   *  the expectation to add
   */
  void
  addExpectation(std::shared_ptr<internal::ExpectationBase>& expectation);

  /**
   * @brief checkMissingCalls
   * 
   * Iterate through all expectation and print missing function calls if needed.
   */
  void
  checkMissingCalls();

  void
  reportUnexpectedCall();

// ---------------------------------------------------------------------------
  /**
   * @brief getName
   * 
   * get the name of the mocked funtion. E.g.:
   * 
   * virtual void doSomething() = 0;
   * 
   * 
   * Then 'doSomething' is the name of the function
   * 
   * @return the name of the mocked function
   */
  const char*
  getName() const;

  /**
   * @brief getNumberOfParams
   * 
   * returns the number of params of the mocked function.
   * 
   * @return the number of params of the mocked function.
   */
  size_t
  getNumberOfParams() const;

  /**
   * @brief getParamType
   * 
   * get the nth param type of the mocked function as string.
   * 
   * @param index
   *  the index of the parameter to use
   * 
   * @return the name of the type of the parameter at index @c index 
   *  as string.
   */
  const char*
  getParamType(size_t index);

  /**
   * @brief getParam
   * 
   * get the name of the param as string
   * 
   * @param index
   * @return the name of the param at index @c index as string
   */
  const char*
  getParam(size_t index);

  /**
   * @brief enterPassiveMode
   * 
   * Must be called when a mock gets destroyed. This will delete all shared_ptr
   * to the expectations. This is necessary to avoid cyclic dependencies, which
   * might yield in memory leaks.
   */
  void
  enterPassiveMode();

protected:
  void
  printFunction(std::ostream& ostream);

  void
  printFunctionWithExpectation(std::ostream& ostream,
                               ExpectationBase* expectation) const;

  void
  printCallToMockFunction(ExpectationBase* expectation);

  void
  printUnexpectedFunctionCall(ExpectationBase* expectation);

  void
  printUnexpectedFunctionCall();

  void
  printUnmetPrerequisite(ExpectationBase* expectation);

  static void
  printWhenClause(ExpectationBase* expectation);

  std::vector<std::shared_ptr<internal::ExpectationBase>> mExpectations;

private:
  MockBase& mMock;
  const char* mName;
  const char* const* mParamTypes;
  const char* const* mParams;
  size_t mNumberOfParameter;
};

// ---------------------------------------------------------------------------
/**
 * @class FunctionMockerBase
 */
template <typename F>
class FunctionMockerBase : public FunctionMockerRaw
{
public:
  using ReturnType = typename F::ReturnType;
  using Args = typename F::Args;
  using Matchers = typename F::Matchers;

  explicit FunctionMockerBase(MockBase& mock,
                              const char* name,
                              const char* const* paramTypes,
                              const char* const* params,
                              size_t numParams);

  FunctionMockerBase(const FunctionMockerBase& other) = delete;

  FunctionMockerBase(FunctionMockerBase&& other) = delete;

  FunctionMockerBase&
  operator=(const FunctionMockerBase& other) = delete;

  FunctionMockerBase&
  operator=(FunctionMockerBase&& other) = delete;

  ~FunctionMockerBase() = default;

// ---------------------------------------------------------------------------
  void
  addExpectation(std::shared_ptr<Expectation<F>>& expectation);

  Expectation<F>*
  findMatchingExpectation(Args& args);

  ReturnType
  evaluatedCall(Args& args);

private:
  ReturnType
  handleCallToMockFunction(Expectation<F>* expectation, Args& args);

  void
  printUnmetPrerequisites(Expectation<F>* expectation);

  ReturnType
  handleUnexpectedCall(Args& args);

  ReturnType
  handleUnexpectedCall(ExpectationBase* expectation);

  void
  printArgs(log::UniversalStream& stream, Args& args);

  // must be a template other wise the compiler tries to generate a
  // specialization for F::ReturnValue == void which means to form
  // a reference to a void type.
  template <typename R>
  void
  printReturnValue(log::UniversalStream& stream, R& returnValue);

  template <size_t N>
  void
  printArgsInternal(log::UniversalStream& stream, Args& args);
};

// ---------------------------------------------------------------------------
template <typename F>
FunctionMockerBase<F>::FunctionMockerBase(MockBase& mock,
                                          const char* name,
                                          const char* const* paramTypes,
                                          const char* const* params,
                                          size_t numParams) :
  FunctionMockerRaw(mock, name, paramTypes, params, numParams)
{
}

template <typename F>
void
FunctionMockerBase<F>::addExpectation(
    std::shared_ptr<Expectation<F>>& expectation)
{
  auto exp = std::dynamic_pointer_cast<ExpectationBase>(expectation);
  FunctionMockerRaw::addExpectation(exp);
}

template <typename F>
Expectation<F>*
FunctionMockerBase<F>::findMatchingExpectation(typename F::Args& args)
{
  Expectation<F>* returnValue = nullptr;
  Expectation<F>* lastBest = nullptr;
  auto iter = mExpectations.rbegin();
  while (iter != mExpectations.rend() && !returnValue)
  {
    auto expectation = static_cast<Expectation<F>*>(iter->get());
    if (expectation->isMatch(args))
    {
      // the args match the expectation
      if ((!isSaturated(expectation) || !expectation->retiresOnSaturation()) &&
          expectation->prerequisitesMet() &&
          expectation->isWhenConditionFulfilled())
      {
        // two possibilities:
        // 1. its not saturated -> its a perfect match
        // 2. its saturated but does not retire on saturation
        //    -> return the expectation for better diagnostics
        returnValue = expectation;
      }
      else
      {
        /// if the prerequisites is not met, this expectation might be the
        /// canditate to return. Instead of setting @c returnValue
        /// @c lastBest will be set. The loop will continue to check if there
        /// is a better canditate to return. E.g.:
        ///
        /// auto first = expectCall(mock.doSomething(Eq(42)))
        ///   .willOnce(Return(2)); // #1
        /// expectCall(mock.doSomething(Eq(42))).
        ///   .willOnce(Return(2)).after(first); // #2
        ///
        /// object.doSomething(42);
        ///
        /// In this case #1 will be choosen since #2 has unmet prerequisites
        /// Without prerequisites #2 will be choosen since #2 overrides #1

        // when the when-clause is not fulfilled, it will also be returned. But
        // not executed. It is just use for better diagnostic.
        if (!expectation->prerequisitesMet() ||
            !expectation->isWhenConditionFulfilled())
        {
          lastBest = expectation;
        }
        // 1. is retired -> check more expectation
      }
    }
    ++iter;
  }
  if (lastBest != nullptr && returnValue == nullptr)
  {
    returnValue = lastBest;
  }
  return returnValue;
}

template <typename F>
typename F::ReturnType
FunctionMockerBase<F>::evaluatedCall(Args& args)
{
  auto* expectation = findMatchingExpectation(args);
  if (expectation)
  {
    if (!isSaturated(expectation))
    {
      if (!expectation->isWhenConditionFulfilled())
      {
        expectation->getCallContext().incrementNumberOfUnexpectedCalls();
        return handleUnexpectedCall(expectation);
      }
      else
      {
        return handleCallToMockFunction(expectation, args);
      }
    }
    else
    {
      expectation->getCallContext().incrementNumberOfUnexpectedCalls();
      // does not retire -> unexpected function call
      return handleUnexpectedCall(expectation);
    }
  }
  else
  {
    reportUnexpectedCall();
    return handleUnexpectedCall(args);
  }
}

// ---------------------------------------------------------------------------
template <typename F>
typename FunctionMockerBase<F>::ReturnType
FunctionMockerBase<F>::handleCallToMockFunction(Expectation<F>* expectation,
                                                Args& args)
{
  auto& logger = core::Context::getCurrentLogger();
  auto& ustream = logger.getStream();

  printCallToMockFunction(expectation);
  ustream.incrementIndent();
  printArgs(ustream, args);
  auto& action = expectation->getCurrentAction();

  if (!expectation->prerequisitesMet())
  {
    expectation->getCallContext().incrementNumberOfUnmetPrerequisites();
  }

  if constexpr (std::is_same_v<typename F::ReturnType, void>)
  {
    ustream.decrementIndent();
    printUnmetPrerequisites(expectation);
    return action.perform(args);
  }
  else
  {
    auto ret = action.perform(args);
    printReturnValue(ustream, ret);
    ustream.decrementIndent();
    printUnmetPrerequisites(expectation);
    return ret;
  }
}

template <typename F>
void
FunctionMockerBase<F>::printUnmetPrerequisites(Expectation<F>* expectation)
{
  bool prerequisitesMet = true;
  auto iter = expectation->mPrerequisites.begin();
  while (iter != expectation->mPrerequisites.end())
  {
    auto* prerequiste = static_cast<Expectation<F>*>(iter->get());
    if (!prerequiste->isSatisfied())
    {
      printUnmetPrerequisite(&*prerequiste);
      prerequisitesMet = false;
    }
    else
    {
    }
    ++iter;
  }
}

template <typename F>
typename FunctionMockerBase<F>::ReturnType
FunctionMockerBase<F>::handleUnexpectedCall(Args& args)
{
  auto& logger = core::Context::getCurrentLogger();
  auto& ustream = logger.getStream();

  printUnexpectedFunctionCall();

  ustream.incrementIndent();
  printArgs(ustream, args);
  if constexpr (!std::is_same_v<ReturnType, void>)
  {
    auto ret = ReturnType();
    printReturnValue(ustream, ret);
    ustream.decrementIndent();
    return ret;
  }
  else
  {
    ustream.decrementIndent();
  }
}

template <typename F>
typename FunctionMockerBase<F>::ReturnType
FunctionMockerBase<F>::handleUnexpectedCall(ExpectationBase* expectation)
{
  auto& logger = core::Context::getCurrentLogger();
  auto& ustream = logger.getStream();

  printUnexpectedFunctionCall(expectation);

  ustream.flush();
  ustream.incrementIndent();
  if constexpr (!std::is_same_v<ReturnType, void>)
  {
    if (!isSaturated(expectation))
    {
      printWhenClause(expectation);
    }
    auto ret = ReturnType();
    printReturnValue(ustream, ret);
    ustream.decrementIndent();
    return ret;
  }
  else
  {
    if (!isSaturated(expectation))
    {
      printWhenClause(expectation);
    }
    ustream.decrementIndent();
  }
}

// ---------------------------------------------------------------------------
template <typename F>
void
FunctionMockerBase<F>::printArgs(log::UniversalStream& stream, Args& args)
{
  printArgsInternal<0>(stream, args);
}

template <typename F>
template <size_t N>
void
FunctionMockerBase<F>::printArgsInternal(log::UniversalStream& stream,
                                         Args& args)
{
  size_t max = 0;
  for (int i = 0; i < getNumberOfParams(); i++)
  {
    if (max < strlen(getParam(i)))
    {
      max = strlen(getParam(i));
    }
  }

  if constexpr (N < std::tuple_size<Args>::value)
  {
    std::stringstream ss;
    // 'return' has 6 charcters. Align with 'return'
    stream.printIndent();
    ss << std::right << std::setw(std::max<int>((int) max, (int) 6))
       << getParam(N);
    stream.getPlain() << ss.str() << ": ";
    stream << std::get<N>(args);
    stream.getPlain() << "\n";
    stream.flush();
  }
  else
  {
    // stop recursion
  }
}

template <typename F>
template <typename R>
void
FunctionMockerBase<F>::printReturnValue(log::UniversalStream& stream,
                                        R& returnValue)
{
  stream.printIndent();
  stream.getPlain() << "return: ";
  stream << returnValue;
  stream.getPlain() << "\n";
  stream.flush();
}

} // namespace internal

} // namespace mock

} // namespace protest