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

#include "protest/utils/can_invoke.h"

#include <iostream>
#include <functional>
#include <tuple>

#include <cstdint>
#include <cstddef>
#include <cassert>

namespace protest
{

namespace mock
{

// ---------------------------------------------------------------------------
/**
 * @class ActionInterface
 */
template <typename F>
class ActionInterface
{
public:
  using ReturnType = typename F::ReturnType;
  using Args = typename F::Args;
  using Matchers = typename F::Matchers;

  explicit ActionInterface() = default;

  ActionInterface(const ActionInterface& other) = delete;

  ActionInterface(ActionInterface&& other) = delete;

  ActionInterface&
  operator=(const ActionInterface& other) = delete;

  ActionInterface&
  operator=(ActionInterface&& other) = delete;

  virtual ~ActionInterface() = default;

// ---------------------------------------------------------------------------
  virtual ReturnType
  perform(Args& args) = 0;

private:
};

// ---------------------------------------------------------------------------
/**
 * @class Action
 */
template <typename F>
class Action
{
public:
  using ReturnType = typename F::ReturnType;
  using Args = typename F::Args;
  using Matchers = typename F::Matchers;

  explicit Action();

  template <
      typename G,
      typename = std::enable_if_t<protest::CanInvokeTuple<G, Args>::value, G>>
  Action(G&& fun);

  explicit Action(ActionInterface<F>* interface);

  Action(const Action& other) = delete;

  Action(Action&& other);

  Action&
  operator=(const Action& other) = delete;

  Action&
  operator=(Action&& other);

  ~Action();

// ---------------------------------------------------------------------------
  ReturnType
  perform(Args& args);

  bool
  isDefaultAction();

  void
  cleanup();

private:
  // the move constructor releases the mInterface if not null. Therefore it
  // must be initialized with a nullptr since no constructor might be called
  // when emplacing a Action into vector. This would lead to a seg fault.
  ActionInterface<F>* mInterface = nullptr;
  std::function<typename F::FunctionType> mFun;
};

// ---------------------------------------------------------------------------
template <typename F>
Action<F>::Action() : mInterface(nullptr)
{
}

template <typename F>
template <typename G, typename>
Action<F>::Action(G&& fun) : mInterface(nullptr), mFun(fun)
{
}

template <typename F>
Action<F>::Action(ActionInterface<F>* interface) : mInterface(interface)
{
}

template <typename F>
Action<F>::Action(Action&& other)
{
  if (mInterface != nullptr)
  {
    delete mInterface;
  }
  mInterface = other.mInterface;
  other.mInterface = nullptr;
  mFun = other.mFun;
}

template <typename F>
Action<F>&
Action<F>::operator=(Action&& other)
{
  if (mInterface != nullptr)
  {
    delete mInterface;
  }
  mInterface = other.mInterface;
  other.mInterface = nullptr;
  mFun = other.mFun;
  return *this;
}

template <typename F>
Action<F>::~Action()
{
  if (mInterface != nullptr)
  {
    delete mInterface;
    mInterface = nullptr;
  }
}

// ---------------------------------------------------------------------------
template <typename F>
typename Action<F>::ReturnType
Action<F>::perform(Args& args)
{
  if (isDefaultAction())
  {
    return ReturnType();
  }
  else if (mInterface)
  {
    return mInterface->perform(args);
  }
  else
  {
    return std::apply(mFun, args);
  }
}

template <typename F>
void
Action<F>::cleanup()
{
  if (!isDefaultAction())
  {
    delete mInterface;
    mInterface = nullptr;
  }
  else
  {
  }
}

template <typename F>
bool
Action<F>::isDefaultAction()
{
  return mInterface == nullptr && mFun == nullptr;
}

// ---------------------------------------------------------------------------
/**
 * @class Return
 */
template <typename R>
class Return
{
public:
  explicit Return(R value);

  Return(const Return& other) = delete;

  Return(Return&& other) = delete;

  Return&
  operator=(const Return& other) = delete;

  Return&
  operator=(Return&& other) = delete;

  ~Return() = default;

// ---------------------------------------------------------------------------
  template <typename F>
  operator Action<F>();

private:
// ---------------------------------------------------------------------------
  /**
   * @class Impl
   */
  template <typename F>
  class Impl : public ActionInterface<F>
  {
  public:
    using ReturnType = typename F::ReturnType;
    using Args = typename F::Args;
    using Matchers = typename F::Matchers;

    explicit Impl(R value);

    Impl(const Impl& other) = default;

    Impl(Impl&& other) = default;

    Impl&
    operator=(const Impl& other) = default;

    Impl&
    operator=(Impl&& other) = default;

    ~Impl() = default;

// ---------------------------------------------------------------------------
    ReturnType
    perform(Args& args) override;

  private:
    R mValue;
  };

  R mValue;
};

// ---------------------------------------------------------------------------
template <typename R>
Return<R>::Return(R value) : mValue(value)
{
}

template <typename R>
template <typename F>
Return<R>::operator Action<F>()
{
  return Action<F>(new Return<R>::Impl<F>(mValue));
}

// ---------------------------------------------------------------------------
template <typename R>
template <typename F>
Return<R>::Impl<F>::Impl(R value) : mValue(value)
{
}

template <typename R>
template <typename F>
typename F::ReturnType
Return<R>::Impl<F>::perform(typename F::Args& args)
{
  return mValue;
}

} // namespace mock

} // namespace protest