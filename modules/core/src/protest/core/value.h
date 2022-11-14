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

#include "protest/core/expression.h"
#include "protest/core/condition.h"

namespace protest
{

// type deduction not allowed for type aliases -> direct into protest namespace
// namespace core
// {

// ---------------------------------------------------------------------------
/**
 * @class Value
 * 
 * A wrapper class to wrap primitiv types so that there can be used to form
 * protest conditions:
 * 
 * auto value = Value(32);
 * auto inv = createInvariant(value < 59);
 * 
 */
template <typename T>
class Value : public core::ConvertableToCopyExprTag
{
private:
  class Listener;
  class ValueInternal;

public:
// ---------------------------------------------------------------------------
  /**
   * @class ValueExpr
   */
  class ValueExpr : public Listener, public core::CopyExprTag
  {
  public:
    using Type = T;

    explicit ValueExpr(ValueInternal* port);

    ValueExpr(const ValueExpr&);

    ValueExpr(ValueExpr&&) noexcept = delete;

    ValueExpr&
    operator=(const ValueExpr&) = delete;

    ValueExpr&
    operator=(ValueExpr&&) noexcept = delete;

    ~ValueExpr();

// ---------------------------------------------------------------------------
    void
    conditionEnable(core::RunnerRaw* runner, core::Condition* condition);

    void
    conditionDisable();

    void
    notify() override;

    Type
    getValue();

  private:
    ValueInternal* mInternal;
    core::Condition* mCondition;
  };

  using Type = ValueExpr;

  /**
   * @class Converter
   */
  struct Converter
  {
  public:
    // toCopyExpr must be public. But it's better to hide it from the user
    // in the auto completion -> use a extra class with static method.
    static Type
    toCopyExpr(Value& port);
  };

// ---------------------------------------------------------------------------
  explicit Value(T value);

  Value(const Value&);

  Value(Value&&) noexcept = delete;

  Value&
  operator=(const Value&) = delete;

  Value&
  operator=(Value&&) noexcept = delete;

  ~Value();

// ---------------------------------------------------------------------------
  template <typename U>
  Value&
  operator=(U&& other);

  template <typename U>
  Value&
  operator+=(U&& other);

  template <typename U>
  Value&
  operator-=(U&& other);

  template <typename U>
  Value&
  operator*=(U&& other);

  template <typename U>
  Value&
  operator/=(U&& other);

  operator T();

private:
// ---------------------------------------------------------------------------
  /**
   * @class Listener
   */
  class Listener
  {
  public:
    friend class List<Listener>;

    explicit Listener();

    Listener(const Listener&) = default;

    Listener&
    operator=(const Listener&) = delete;

    Listener(Listener&&) noexcept = delete;

    Listener&
    operator=(Listener&&) noexcept = delete;

    virtual ~Listener() = default;

    virtual void
    notify() = 0;

  private:
    Listener* mNext;
    ValueInternal* mInternal;
  };

// ---------------------------------------------------------------------------
  /**
   * @class ValueInternal
   */
  class ValueInternal : protest::RefCounter
  {
  public:
    friend class Value;

    explicit ValueInternal(T value);

    ValueInternal(const ValueInternal&) = delete;

    ValueInternal(ValueInternal&&) noexcept = delete;

    ValueInternal&
    operator=(const ValueInternal&) = delete;

    ValueInternal&
    operator=(ValueInternal&&) noexcept = delete;

    ~ValueInternal() = default;

// ---------------------------------------------------------------------------
    void
    add(Listener* listener);

    void
    remove(Listener* listener);

// ---------------------------------------------------------------------------
  private:
    void
    notify();

    T mValue;
    protest::List<Listener> mListeners;
  };

  ValueInternal* mInternal;
};

// ---------------------------------------------------------------------------
template <typename T>
Value<T>::ValueExpr::ValueExpr(ValueInternal* port) :
  mInternal(port),
  mCondition(nullptr)
{
  assert(mInternal);
  assert(port);
  mInternal->increment();
}

template <typename T>
Value<T>::ValueExpr::ValueExpr(const ValueExpr& other) :
  mInternal(other.mInternal),
  mCondition(other.mCondition)
{
  assert(mInternal);
  mInternal->increment();
}

template <typename T>
Value<T>::ValueExpr::~ValueExpr()
{
  if (mInternal->decrement())
  {
    delete mInternal;
    mInternal = nullptr;
  }
  else
  {
  }
}

// ---------------------------------------------------------------------------
template <typename T>
void
Value<T>::ValueExpr::conditionEnable(core::RunnerRaw* runner,
                                     core::Condition* condition)
{
  assert(mInternal);
  mCondition = condition;
  mInternal->add(this);
}

template <typename T>
void
Value<T>::ValueExpr::conditionDisable()
{
  assert(mInternal);
  mInternal->remove(this);
}

template <typename T>
void
Value<T>::ValueExpr::notify()
{
  assert(mCondition);
  mCondition->notifyListener();
}

template <typename T>
typename Value<T>::ValueExpr::Type
Value<T>::ValueExpr::getValue()
{
  assert(mInternal);
  return mInternal->mValue;
}

// ---------------------------------------------------------------------------
template <typename T>
typename Value<T>::ValueExpr
Value<T>::Converter::toCopyExpr(Value<T>& value)
{
  return Value<T>::ValueExpr(value.mInternal);
}

// ---------------------------------------------------------------------------
template <typename T>
Value<T>::Value(T value) : mInternal(new ValueInternal(value))
{
  mInternal->increment();
}

template <typename T>
Value<T>::Value(const Value<T>& other) : mInternal(other.mInternal)
{
  mInternal->increment();
}

template <typename T>
Value<T>::~Value()
{
  if (mInternal->decrement())
  {
    delete mInternal;
    mInternal = nullptr;
  }
}

// ---------------------------------------------------------------------------
template <typename T>
template <typename U>
Value<T>&
Value<T>::operator=(U&& other)
{
  mInternal->mValue = other;
  mInternal->notify();
  return *this;
}

template <typename T>
template <typename U>
Value<T>&
Value<T>::operator+=(U&& other)
{
  mInternal->mValue += other;
  mInternal->notify();
  return *this;
}

template <typename T>
template <typename U>
Value<T>&
Value<T>::operator-=(U&& other)
{
  mInternal->mValue -= other;
  mInternal->notify();
  return *this;
}

template <typename T>
template <typename U>
Value<T>&
Value<T>::operator*=(U&& other)
{
  mInternal->mValue *= other;
  mInternal->notify();
  return *this;
}

template <typename T>
template <typename U>
Value<T>&
Value<T>::operator/=(U&& other)
{
  mInternal->mValue /= other;
  mInternal->notify();
  return *this;
}

template <typename T>
Value<T>::operator T()
{
  return mInternal->mValue;
}

// ---------------------------------------------------------------------------
template <typename T>
Value<T>::Listener::Listener() : mNext(nullptr), mInternal(nullptr)
{
}

// ---------------------------------------------------------------------------
template <typename T>
Value<T>::ValueInternal::ValueInternal(T value) : mValue(value)
{
}

// ---------------------------------------------------------------------------
template <typename T>
void
Value<T>::ValueInternal::add(Listener* listener)
{
  mListeners.prepend(*listener);
}

template <typename T>
void
Value<T>::ValueInternal::remove(Listener* listener)
{
  mListeners.remove(*listener);
}

// ---------------------------------------------------------------------------
template <typename T>
void
Value<T>::ValueInternal::notify()
{
  auto iter = mListeners.begin();
  while (iter != mListeners.end())
  {
    iter->notify();
    ++iter;
  }
}

// } // namespace core

} // namespace protest
