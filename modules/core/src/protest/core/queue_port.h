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

#include "protest/core/signal.h"
#include "protest/core/signal_raw.h"
#include "protest/core/expression.h"
#include "protest/core/port.h"
#include "protest/utils/ring_buffer.h"
#include "protest/utils/ref_counter.h"
#include "protest/meta.h"
#include "protest/utils/can_invoke.h"

namespace protest
{

namespace core
{

template <typename T>
class QueuePort;

// ---------------------------------------------------------------------------
/**
 * @class QueuePortTag
 */
struct QueuePortTag : public PortTag
{
};

// ---------------------------------------------------------------------------
/**
 * @class QueuePort
 */
template <typename T>
class QueuePort : public QueuePortTag
{
private:
  class QueuePortInternal;
  class Listener;
  class CallbackRaw;
  class FilterRaw;

public:
  friend class Signal<T>;

  using Type = T;

// ---------------------------------------------------------------------------
  /**
   * @class Size
   */
  class Size : public Listener, public CopyExprTag
  {
  public:
    template <typename>
    friend struct ExprCondition;

    using Type = size_t;

    explicit Size(QueuePortInternal* port);

    explicit Size() = delete;

    Size(const Size&);

    Size(Size&&) noexcept = delete;

    Size&
    operator=(const Size&) = delete;

    Size&
    operator=(Size&&) noexcept = delete;

    ~Size();

// ---------------------------------------------------------------------------
    operator size_t();

// ---------------------------------------------------------------------------
    size_t
    getValue();

    void
    conditionEnable(RunnerRaw* runner, Condition* condition);

    void
    conditionDisable();

  private:
    void
    notify();

    QueuePortInternal* mInternal;
    Condition* mCondition;
  };

// ---------------------------------------------------------------------------
  /**
   * @class IsAvailable
   */
  class IsAvailable : public Listener, public CopyExprTag
  {
  public:
    template <typename>
    friend struct ExprCondition;

    using Type = bool;

    explicit IsAvailable(QueuePortInternal* port);

    IsAvailable(const IsAvailable&);

    IsAvailable(IsAvailable&&) noexcept = delete;

    IsAvailable&
    operator=(const IsAvailable&) = delete;

    IsAvailable&
    operator=(IsAvailable&&) noexcept = delete;

    ~IsAvailable();

// ---------------------------------------------------------------------------
    operator bool();

// ---------------------------------------------------------------------------
    bool
    getValue();

    void
    conditionEnable(RunnerRaw* runner, Condition* condition);

    void
    conditionDisable();

  private:
    void
    notify();

    QueuePortInternal* mInternal;
    Condition* mCondition;
  };

// ---------------------------------------------------------------------------
  explicit QueuePort();

  QueuePort&
  operator=(const QueuePort&) = delete;

  QueuePort(QueuePort&&) noexcept = delete;

  QueuePort&
  operator=(QueuePort&&) noexcept = delete;

  QueuePort&
  operator=(const core::AnyPortCreator<T>& info);

  ~QueuePort();

// ---------------------------------------------------------------------------
  T
  pop(protest::meta::CallContext& context =
          protest::meta::CallContext::defaultContext());

  template <typename Function>
  void
  setPrinterFunction(Function& printer);

  template <typename Function>
  CallbackRaw*
  addListener(Function function,
              protest::meta::CallContext& context =
                  protest::meta::CallContext::defaultContext());

  void
  removeListener(CallbackRaw* callback);

  template <typename Function>
  FilterRaw*
  addFilter(Function function);

  void
  removeFilter(FilterRaw* filter);

  Size
  size();

  IsAvailable
  isAvailable();

  void
  clear();

private:
// ---------------------------------------------------------------------------
  /**
   * @class CallbackRaw
   */
  class CallbackRaw
  {
  public:
    friend class List<CallbackRaw>;
    friend class QueuePortInternal;

    explicit CallbackRaw();

    CallbackRaw(const CallbackRaw&) = default;

    CallbackRaw&
    operator=(const CallbackRaw&) = delete;

    CallbackRaw(CallbackRaw&&) noexcept = delete;

    CallbackRaw&
    operator=(CallbackRaw&&) noexcept = delete;

    virtual ~CallbackRaw() = default;

// ---------------------------------------------------------------------------
  private:
    virtual void
    handle(T& value) = 0;

    CallbackRaw* mNext;
  };

// ---------------------------------------------------------------------------
  /**
   * @class Callback
   */
  template <typename Function>
  class Callback : public CallbackRaw
  {
  public:
    friend class QueuePortInternal;

    explicit Callback(Function function,
                      QueuePortInternal* internal,
                      protest::meta::CallContext& context);

    Callback(const Callback&) = default;

    Callback&
    operator=(const Callback&) = delete;

    Callback(Callback&&) noexcept = delete;

    Callback&
    operator=(Callback&&) noexcept = delete;

    virtual ~Callback() = default;

  private:
    void
    handle(T& value);

    Function mFunction;
    QueuePortInternal* mInternal;
    meta::CallContext& mContext;
  };

// ---------------------------------------------------------------------------
  /**
   * @class FilterRaw
   */
  class FilterRaw
  {
  public:
    friend class List<FilterRaw>;
    friend class QueuePortInternal;

    explicit FilterRaw();

    FilterRaw(const FilterRaw&) = default;

    FilterRaw&
    operator=(const FilterRaw&) = delete;

    FilterRaw(FilterRaw&&) noexcept = delete;

    FilterRaw&
    operator=(FilterRaw&&) noexcept = delete;

    virtual ~FilterRaw() = default;

// ---------------------------------------------------------------------------
  private:
    virtual bool
    handle(T& value) = 0;

    FilterRaw* mNext;
  };

// ---------------------------------------------------------------------------
  /**
   * @class Filter
   */
  template <typename Function>
  class Filter : public FilterRaw
  {
  public:
    friend class QueuePortInternal;

    explicit Filter(Function function);

    Filter(const Filter&) = default;

    Filter&
    operator=(const Filter&) = delete;

    Filter(Filter&&) noexcept = delete;

    Filter&
    operator=(Filter&&) noexcept = delete;

    virtual ~Filter() = default;

// ---------------------------------------------------------------------------
  private:
    bool
    handle(T& value);

    Function mFunction;
  };

// ---------------------------------------------------------------------------
  /**
   * @class Listener
   */
  class Listener
  {
  public:
    friend class List<Listener>;
    friend class QueuePortInternal;

    explicit Listener();

    Listener(const Listener&) = default;

    Listener&
    operator=(const Listener&) = delete;

    Listener(Listener&&) noexcept = delete;

    Listener&
    operator=(Listener&&) noexcept = delete;

    virtual ~Listener() = default;

// ---------------------------------------------------------------------------
  private:
    virtual void
    notify() = 0;

    Listener* mNext;
    QueuePortInternal* mInternal;
    Condition* mCondition;
  };

// ---------------------------------------------------------------------------
  /**
   * @class QueuePortInternal
   */
  class QueuePortInternal : public RefCounter
  {
  public:
    friend class List<QueuePortInternal>;

    explicit QueuePortInternal();

    QueuePortInternal(const QueuePortInternal&) = delete;

    QueuePortInternal(QueuePortInternal&&) noexcept = delete;

    QueuePortInternal&
    operator=(const QueuePortInternal&) = delete;

    QueuePortInternal&
    operator=(QueuePortInternal&&) noexcept = delete;

    ~QueuePortInternal();

// ---------------------------------------------------------------------------
    void
    bindTo(core::Signal<T>& signal, RunnerRaw* coroutine);

    void
    setValue(T value);

    T
    getValue();

    bool
    insertValue(T value);

    template <typename Function>
    void
    setPrinterFunction(Function& printer);

    template <typename Function>
    CallbackRaw*
    addListener(Function function, meta::CallContext& context);

    void
    removeListener(CallbackRaw* callback);

    template <typename Function>
    FilterRaw*
    addFilter(Function function);

    void
    removeFilter(FilterRaw* filter);

    bool
    isAvailable();

    size_t
    size();

    void
    clear();

    void
    add(Listener* listener);

    void
    remove(Listener* listener);

  private:
    // TODO size is fixed here
    RingBuffer<T, 100u> mRingBuffer;
    SignalRaw* mSignal;
    RunnerRaw* mRunner;
    QueuePortInternal* mNext;
    const char* mName;
    List<Listener> mListeners;
    List<CallbackRaw> mCallbacks;
    List<FilterRaw> mFilters;

    void (*mPrinter)(std::ostream&, T&);
  };

  QueuePort(core::Signal<T>& signal, RunnerRaw* coroutine);

  QueuePort(const QueuePort&);

  QueuePortInternal* mInternal;
};

// ---------------------------------------------------------------------------
template <typename T>
QueuePort<T>::Size::Size(QueuePortInternal* port) :
  mInternal(port),
  mCondition(nullptr)
{
  mInternal->increment();
}

template <typename T>
QueuePort<T>::Size::Size(const Size& other) :
  mInternal(other.mInternal),
  mCondition(other.mCondition)
{
  mInternal->increment();
}

template <typename T>
QueuePort<T>::Size::~Size()
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
QueuePort<T>::Size::operator size_t()
{
  return getValue();
}

template <typename T>
size_t
QueuePort<T>::Size::getValue()
{
  return mInternal->size();
}

// ---------------------------------------------------------------------------
template <typename T>
void
QueuePort<T>::Size::conditionEnable(RunnerRaw* runner, Condition* condition)
{
  assert(mInternal);
  mCondition = condition;
  mInternal->add(this);
}

template <typename T>
void
QueuePort<T>::Size::conditionDisable()
{
  assert(mInternal);
  mInternal->remove(this);
}

template <typename T>
void
QueuePort<T>::Size::notify()
{
  assert(mCondition);
  mCondition->notifyListener();
}

// ---------------------------------------------------------------------------
template <typename T>
QueuePort<T>::IsAvailable::IsAvailable(QueuePortInternal* port) :
  mInternal(port),
  mCondition(nullptr)
{
  mInternal->increment();
}

template <typename T>
QueuePort<T>::IsAvailable::IsAvailable(const IsAvailable& other) :
  mInternal(other.mInternal),
  mCondition(other.mCondition)
{
  mInternal->increment();
}

template <typename T>
QueuePort<T>::IsAvailable::~IsAvailable()
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
QueuePort<T>::IsAvailable::operator bool()
{
  return getValue();
}

template <typename T>
bool
QueuePort<T>::IsAvailable::getValue()
{
  return mInternal->isAvailable();
}

// ---------------------------------------------------------------------------
template <typename T>
void
QueuePort<T>::IsAvailable::conditionEnable(RunnerRaw* /* runner */,
                                           Condition* condition)
{
  assert(mInternal);
  mCondition = condition;
  mInternal->add(this);
}

template <typename T>
void
QueuePort<T>::IsAvailable::conditionDisable()
{
  assert(mInternal);
  mInternal->remove(this);
}

template <typename T>
void
QueuePort<T>::IsAvailable::notify()
{
  assert(mCondition);
  mCondition->notifyListener();
}

// ---------------------------------------------------------------------------
template <typename T>
QueuePort<T>::QueuePort() : mInternal(nullptr)
{
}

template <typename T>
QueuePort<T>&
QueuePort<T>::operator=(const core::AnyPortCreator<T>& info)
{
  assert(mInternal == nullptr);
  mInternal = new QueuePortInternal();
  mInternal->increment();
  assert(mInternal);
  mInternal->bindTo(info.mSignal, info.mRunner);
  return *this;
}

template <typename T>
QueuePort<T>::~QueuePort()
{
  if (mInternal && mInternal->decrement())
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
T
QueuePort<T>::pop(protest::meta::CallContext& context)
{
  assert(mInternal);
  auto value = mInternal->getValue();
  auto runner = mInternal->mRunner;
  auto& logger = runner->getLogger();
  logger.startLog("POP ",
                  runner->getName(),
                  context.getUnit().getFileName(),
                  context.getLine(),
                  runner->now());
  logger.getStream() << "Pop value from '" << context.getObjectName() << "':\n"
                     << value << "\n";
  return value;
}

template <typename T>
template <typename Function>
void
QueuePort<T>::setPrinterFunction(Function& function)
{
  assert(mInternal);
  return mInternal->setPrinterFunction(function);
}

template <typename T>
template <typename Function>
typename QueuePort<T>::CallbackRaw*
QueuePort<T>::addListener(Function function,
                          protest::meta::CallContext& context)
{
  assert(mInternal);
  return mInternal->addListener(function, context);
}

template <typename T>
void
QueuePort<T>::removeListener(CallbackRaw* callback)
{
  assert(mInternal);
  return mInternal->removeListener(callback);
}

template <typename T>
template <typename Function>
typename QueuePort<T>::FilterRaw*
QueuePort<T>::addFilter(Function function)
{
  assert(mInternal);
  return mInternal->addFilter(function);
}

template <typename T>
void
QueuePort<T>::removeFilter(FilterRaw* filter)
{
  assert(mInternal);
  return mInternal->removeFilter(filter);
}

template <typename T>
typename QueuePort<T>::Size
QueuePort<T>::size()
{
  return Size(mInternal);
}

template <typename T>
typename QueuePort<T>::IsAvailable
QueuePort<T>::isAvailable()
{
  return IsAvailable(mInternal);
}

template <typename T>
void
QueuePort<T>::clear()
{
  return mInternal->clear();
}

// ---------------------------------------------------------------------------
template <typename T>
QueuePort<T>::CallbackRaw::CallbackRaw() : mNext(nullptr)
{
}

// ---------------------------------------------------------------------------
template <typename T>
template <typename Function>
QueuePort<T>::Callback<Function>::Callback(Function function,
                                           QueuePortInternal* internal,
                                           meta::CallContext& context) :
  mFunction(function),
  mContext(context),
  mInternal(internal)
{
}

template <typename T>
template <typename Function>
void
QueuePort<T>::Callback<Function>::handle(T& value)
{
  auto runner = mInternal->mRunner;
  auto& logger = runner->getLogger();
  if (mInternal->mPrinter)
  {
    logger.startLog("HDL ",
                    runner->getName(),
                    mContext.getUnit().getFileName(),
                    mContext.getLine(),
                    runner->now());
    mInternal->mPrinter(logger.getStream().getPlain(), value);
  }
  else
  {
    logger.startLog("HDL ",
                    runner->getName(),
                    mContext.getUnit().getFileName(),
                    mContext.getLine(),
                    runner->now());
    logger.getStream() << "Handle value of '"
                       << mInternal->mSignal->getSignalInfo().getObjectName()
                       << "':\n"
                       << value << "\n";
  }

  if constexpr (CanInvoke<Function, T>::value)
  {
    mFunction(value);
  }
  else
  {
    mFunction();
  }
}

// ---------------------------------------------------------------------------
template <typename T>
QueuePort<T>::FilterRaw::FilterRaw() : mNext(nullptr)
{
}

// ---------------------------------------------------------------------------
template <typename T>
template <typename Function>
QueuePort<T>::Filter<Function>::Filter(Function function) : mFunction(function)
{
}

template <typename T>
template <typename Function>
bool
QueuePort<T>::Filter<Function>::handle(T& value)
{
  return mFunction(value);
}

// ---------------------------------------------------------------------------
template <typename T>
QueuePort<T>::Listener::Listener() :
  mNext(nullptr),
  mInternal(nullptr),
  mCondition(nullptr)
{
}

// ---------------------------------------------------------------------------
template <typename T>
QueuePort<T>::QueuePortInternal::QueuePortInternal() :
  mSignal(nullptr),
  mNext(nullptr),
  mPrinter(nullptr)
{
}

template <typename T>
QueuePort<T>::QueuePortInternal::~QueuePortInternal()
{
  mSignal->removeQueuePort(this);

  // should not be deleted if someone has still a ref to this object
  // PROTEST_ASSERT(mListeners.numberOfElements() == 0);

  {
    auto iter = mCallbacks.begin();
    while (iter != mCallbacks.end())
    {
      auto* tmp = &*iter;
      ++iter;
      delete tmp;
    }
  }

  {
    auto iter = mFilters.begin();
    while (iter != mFilters.end())
    {
      auto* tmp = &*iter;
      ++iter;
      delete tmp;
    }
  }
}

template <typename T>
void
QueuePort<T>::QueuePortInternal::bindTo(core::Signal<T>& signal,
                                        RunnerRaw* coroutine)
{
  mSignal = &signal;
  mSignal->bindQueuePort(this);
  mRunner = coroutine;
}

template <typename T>
void
QueuePort<T>::QueuePortInternal::setValue(T value)
{
  assert(mSignal);
  mSignal->pushValue(reinterpret_cast<void*>(&value));
}

template <typename T>
T
QueuePort<T>::QueuePortInternal::getValue()
{
  return mRingBuffer.pop();
}

template <typename T>
bool
QueuePort<T>::QueuePortInternal::insertValue(T value)
{
  bool lostAElement = false;
  bool accepted = true;

  auto& context = mRunner->getContext();
  context.setCurrentVirtual(mRunner);

  auto filter = mFilters.begin();
  while (filter != mFilters.end())
  {
    if (!filter->handle(value))
    {
      accepted = false;
    }
    else
    {
      // fine
    }
    ++filter;
  }
  if (accepted)
  {
    lostAElement = mRingBuffer.isFull();
    mRingBuffer.push(value);
    auto iter = mListeners.begin();
    while (iter != mListeners.end())
    {
      iter->notify();
      ++iter;
    }
    auto callbacks = mCallbacks.begin();
    while (callbacks != mCallbacks.end())
    {
      callbacks->handle(value);
      ++callbacks;
    }
  }
  else
  {
  }

  context.setCurrentVirtual(nullptr);

  return lostAElement;
}

template <typename T>
template <typename Function>
void
QueuePort<T>::QueuePortInternal::setPrinterFunction(Function& printer)
{
  mPrinter = printer;
}

template <typename T>
template <typename Function>
typename QueuePort<T>::CallbackRaw*
QueuePort<T>::QueuePortInternal::addListener(
    Function function,
    protest::meta::CallContext& context)
{
  auto* callback = new Callback<Function>(function, this, context);
  mCallbacks.append(*callback);
  return callback;
}

template <typename T>
void
QueuePort<T>::QueuePortInternal::removeListener(CallbackRaw* callback)
{
  mCallbacks.remove(*callback);
  delete callback;
}

template <typename T>
template <typename Function>
typename QueuePort<T>::FilterRaw*
QueuePort<T>::QueuePortInternal::addFilter(Function function)
{
  auto* filter = new Filter<Function>(function);
  mFilters.append(*filter);
  return filter;
}

template <typename T>
void
QueuePort<T>::QueuePortInternal::removeFilter(FilterRaw* filter)
{
  mFilters.remove(*filter);
  delete filter;
}

template <typename T>
bool
QueuePort<T>::QueuePortInternal::isAvailable()
{
  return mRingBuffer.isAvailable();
}

template <typename T>
size_t
QueuePort<T>::QueuePortInternal::size()
{
  return mRingBuffer.numberOfElements();
}

template <typename T>
void
QueuePort<T>::QueuePortInternal::clear()
{
  return mRingBuffer.clear();
}

template <typename T>
void
QueuePort<T>::QueuePortInternal::add(Listener* listener)
{
  mListeners.prepend(*listener);
}

template <typename T>
void
QueuePort<T>::QueuePortInternal::remove(Listener* listener)
{
  mListeners.remove(*listener);
}

// ---------------------------------------------------------------------------
template <typename T>
QueuePort<T>::QueuePort(core::Signal<T>& signal, RunnerRaw* coroutine) :
  mInternal(nullptr)
{
  assert(mInternal == nullptr);
  mInternal = new QueuePortInternal();
  mInternal->increment();
  assert(mInternal);
  mInternal->bindTo(signal, coroutine);
}

template <typename T>
QueuePort<T>::QueuePort(const QueuePort&)
{
  // RVO
  assert(false);
}

} // namespace core

} // namespace protest
