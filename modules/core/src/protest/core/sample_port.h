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

#include "protest/core/signal_raw.h"
#include "protest/core/port.h"
#include "protest/utils/can_invoke.h"
#include "protest/utils/ref_counter.h"

namespace protest
{

namespace core
{

// ---------------------------------------------------------------------------
/**
 * @class SamplePort
 */
struct SamplePortTag : public PortTag
{
};

/**
 * @class SamplePort
 */
template <typename T>
class SamplePort : public SamplePortTag, public ConvertableToCopyExprTag
{
private:
  class SamplePortInternal;
  class Listener;
  class CallbackRaw;
  class FilterRaw;

public:
  friend class Signal<T>;

// ---------------------------------------------------------------------------
  /**
   * @class SamplePortExpr
   */
  class SamplePortExpr : public Listener, public CopyExprTag
  {
  public:
    using Type = T;

    explicit SamplePortExpr(SamplePortInternal* port);

    SamplePortExpr(const SamplePortExpr&);

    SamplePortExpr(SamplePortExpr&&) noexcept = delete;

    SamplePortExpr&
    operator=(const SamplePortExpr&) = delete;

    SamplePortExpr&
    operator=(SamplePortExpr&&) noexcept = delete;

    ~SamplePortExpr();

// ---------------------------------------------------------------------------
    void
    conditionEnable(RunnerRaw* runner, Condition* condition);

    void
    conditionDisable();

    void
    notify();

    Type
    getValue();

  private:
    SamplePortInternal* mInternal;
    Condition* mCondition;
  };

  using Type = SamplePortExpr;

// ---------------------------------------------------------------------------
  /**
   * @class Converter
   */
  struct Converter
  {
  public:
    // toCopyExpr must be public. But it's better to hide it from the user
    // in the auto completion -> use a extra class with static method.
    static Type
    toCopyExpr(SamplePort& port);
  };

// ---------------------------------------------------------------------------
  explicit SamplePort();

  SamplePort(const SamplePort& port);

  SamplePort(SamplePort&&) noexcept = delete;

  SamplePort&
  operator=(const SamplePort& port) = delete;

  SamplePort&
  operator=(SamplePort&&) noexcept = delete;

  ~SamplePort();

// ---------------------------------------------------------------------------
  SamplePort&
  operator=(const core::AnyPortCreator<T>& info);

// ---------------------------------------------------------------------------
  operator T();

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

  const char*
  getName();

private:
  /**
   * @class CallbackRaw
   */
  class CallbackRaw
  {
  public:
    friend class List<CallbackRaw>;
    friend class SamplePortInternal;

    explicit CallbackRaw();

    CallbackRaw(const CallbackRaw&) = delete;

    CallbackRaw&
    operator=(const CallbackRaw&) = delete;

    CallbackRaw(CallbackRaw&&) noexcept = delete;

    CallbackRaw&
    operator=(CallbackRaw&&) noexcept = delete;

    virtual ~CallbackRaw() = default;

  protected:
    const char* mFile;
    size_t mLine;

  private:
    virtual void
    handle(T& value) = 0;

    CallbackRaw* mNext;
  };

  /**
   * @class Callback
   */
  template <typename Function>
  class Callback : public CallbackRaw
  {
  public:
    friend class List<Callback>;
    friend class SamplePortInternal;

    explicit Callback(Function function,
                      SamplePortInternal* mInternal,
                      protest::meta::CallContext& context =
                          protest::meta::CallContext::defaultContext());

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
    SamplePortInternal* mInternal;
    meta::CallContext& mContext;
  };

  /**
   * @class FilterRaw
   */
  class FilterRaw
  {
  public:
    friend class List<FilterRaw>;

    explicit FilterRaw();

    FilterRaw(const FilterRaw&) = default;

    FilterRaw&
    operator=(const FilterRaw&) = delete;

    FilterRaw(FilterRaw&&) noexcept = delete;

    FilterRaw&
    operator=(FilterRaw&&) noexcept = delete;

    virtual ~FilterRaw() = default;

    virtual bool
    handle(T& value) = 0;

  private:
    FilterRaw* mNext;
  };

  /**
   * @class Filter
   */
  template <typename Function>
  class Filter : public FilterRaw
  {
  public:
    friend class List<Filter>;

    explicit Filter(Function function);

    Filter(const Filter&) = default;

    Filter&
    operator=(const Filter&) = delete;

    Filter(Filter&&) noexcept = delete;

    Filter&
    operator=(Filter&&) noexcept = delete;

    virtual ~Filter() = default;

    bool
    handle(T& value);

  private:
    Function mFunction;
  };

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
    SamplePortInternal* mInternal;
    Condition* mCondition;
  };

  /**
   * @class SamplePortInternal
   */
  class SamplePortInternal : public RefCounter
  {
  public:
    friend class List<SamplePort<T>::SamplePortInternal>;

    template <typename Function>
    friend class Callback;

    explicit SamplePortInternal();

    SamplePortInternal(const SamplePortInternal&) = default;

    SamplePortInternal(SamplePortInternal&&) noexcept = default;

    SamplePortInternal&
    operator=(const SamplePortInternal&) = default;

    SamplePortInternal&
    operator=(SamplePortInternal&&) noexcept = default;

    ~SamplePortInternal();

// ---------------------------------------------------------------------------
    void
    bindTo(core::Signal<T>& signal, RunnerRaw* runner);

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

    const char*
    getName();

// ---------------------------------------------------------------------------
    void
    add(Listener* listener);

    void
    remove(Listener* listener);

  private:
    T mValue;
    core::SignalRaw* mSignal;
    SamplePortInternal* mNext;
    RunnerRaw* mRunner;
    const char* mName;
    List<Listener> mListeners;
    List<CallbackRaw> mCallbacks;
    List<FilterRaw> mFilters;

    void (*mPrinter)(std::ostream&, T&);
  };

  SamplePort(core::Signal<T>& signal, RunnerRaw* coroutine);

  SamplePortInternal* mInternal;
};

// ---------------------------------------------------------------------------
template <typename T>
SamplePort<T>::SamplePortExpr::SamplePortExpr(SamplePortInternal* port) :
  mInternal(port),
  mCondition(nullptr)
{
  assert(mInternal);
  assert(port);
  mInternal->increment();
}

template <typename T>
SamplePort<T>::SamplePortExpr::SamplePortExpr(const SamplePortExpr& other) :
  mInternal(other.mInternal),
  mCondition(other.mCondition)
{
  assert(mInternal);
  mInternal->increment();
}

template <typename T>
SamplePort<T>::SamplePortExpr::~SamplePortExpr()
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
SamplePort<T>::SamplePortExpr::conditionEnable(RunnerRaw* runner,
                                               Condition* condition)
{
  assert(mInternal);
  mCondition = condition;
  mInternal->add(this);
}

template <typename T>
void
SamplePort<T>::SamplePortExpr::conditionDisable()
{
  assert(mInternal);
  mInternal->remove(this);
}

template <typename T>
void
SamplePort<T>::SamplePortExpr::notify()
{
  assert(mCondition);
  mCondition->notifyListener();
}

template <typename T>
typename SamplePort<T>::SamplePortExpr::Type
SamplePort<T>::SamplePortExpr::getValue()
{
  assert(mInternal);
  return mInternal->getValue();
}

// ---------------------------------------------------------------------------
template <typename T>
typename SamplePort<T>::SamplePortExpr
SamplePort<T>::Converter::toCopyExpr(SamplePort<T>& port)
{
  return SamplePort<T>::SamplePortExpr(port.mInternal);
}

// ---------------------------------------------------------------------------
template <typename T>
SamplePort<T>::SamplePort() : mInternal(nullptr)
{
}

template <typename T>
SamplePort<T>::SamplePort(const SamplePort& port) : mInternal(port.mInternal)
{
  mInternal->increment();
}

template <typename T>
SamplePort<T>&
SamplePort<T>::operator=(const core::AnyPortCreator<T>& info)
{
  assert(mInternal == nullptr);
  mInternal = new SamplePortInternal();
  mInternal->increment();
  assert(mInternal);
  mInternal->bindTo(info.mSignal, info.mRunner);
  return *this;
}

template <typename T>
SamplePort<T>::~SamplePort()
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
SamplePort<T>::operator T()
{
  assert(mInternal);
  return mInternal->getValue();
}

template <typename T>
template <typename Function>
void
SamplePort<T>::setPrinterFunction(Function& function)
{
  assert(mInternal);
  return mInternal->setPrinterFunction(function);
}

template <typename T>
template <typename Function>
typename SamplePort<T>::CallbackRaw*
SamplePort<T>::addListener(Function function,
                           protest::meta::CallContext& context)
{
  assert(mInternal);
  return mInternal->addListener(function, context);
}

template <typename T>
void
SamplePort<T>::removeListener(CallbackRaw* callback)
{
  assert(mInternal);
  return mInternal->removeListener(callback);
}

template <typename T>
template <typename Function>
typename SamplePort<T>::FilterRaw*
SamplePort<T>::addFilter(Function function)
{
  assert(mInternal);
  return mInternal->addFilter(function);
}

template <typename T>
void
SamplePort<T>::removeFilter(FilterRaw* filter)
{
  assert(mInternal);
  return mInternal->removeFilter(filter);
}

template <typename T>
const char*
SamplePort<T>::getName()
{
  assert(mInternal);
  return mInternal->getName();
}

// ---------------------------------------------------------------------------
template <typename T>
SamplePort<T>::CallbackRaw::CallbackRaw() : mNext(nullptr)
{
}

// ---------------------------------------------------------------------------
template <typename T>
template <typename Function>
SamplePort<T>::Callback<Function>::Callback(
    Function function,
    SamplePortInternal* internal,
    protest::meta::CallContext& context) :
  mFunction(function),
  mInternal(internal),
  mContext(context)
{
}

template <typename T>
template <typename Function>
void
SamplePort<T>::Callback<Function>::handle(T& value)
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
    logger.getStream().mOutput
        << "Handle value of '"
        << mInternal->mSignal->getSignalInfo().getObjectName() << "':\n";
    logger.getStream() << value;
    // logger.getStream().mOutput << "\n";
  }
  logger.getStream().flush();

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
SamplePort<T>::FilterRaw::FilterRaw() : mNext(nullptr)
{
}

// ---------------------------------------------------------------------------
template <typename T>
template <typename Function>
SamplePort<T>::Filter<Function>::Filter(Function function) : mFunction(function)
{
}

template <typename T>
template <typename Function>
bool
SamplePort<T>::Filter<Function>::handle(T& value)
{
  return mFunction(value);
}

// ---------------------------------------------------------------------------
template <typename T>
SamplePort<T>::Listener::Listener() :
  mNext(nullptr),
  mInternal(nullptr),
  mCondition(nullptr)
{
}

// ---------------------------------------------------------------------------
template <typename T>
SamplePort<T>::SamplePortInternal::SamplePortInternal() :
  mValue(),
  mSignal(nullptr),
  mNext(nullptr),
  mRunner(nullptr),
  mName(nullptr),
  mPrinter(nullptr)
{
}

template <typename T>
SamplePort<T>::SamplePortInternal::~SamplePortInternal()
{
  mSignal->removeSamplePort(this);

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
SamplePort<T>::SamplePortInternal::bindTo(core::Signal<T>& signal,
                                          RunnerRaw* runner)
{
  mSignal = &signal;
  mSignal->bindSamplePort(this);
  mRunner = runner;
}

template <typename T>
void
SamplePort<T>::SamplePortInternal::setValue(T value)
{
  assert(mSignal);
  mSignal->pushValue(reinterpret_cast<void*>(&value));
}

template <typename T>
T
SamplePort<T>::SamplePortInternal::getValue()
{
  return mValue;
}

template <typename T>
bool
SamplePort<T>::SamplePortInternal::insertValue(T value)
{
  bool hasChanged = false;// TODO remove?
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
    mValue = value;
    auto iter = mListeners.begin();
    while (iter != mListeners.end())
    {
      iter->notify();
      ++iter;
    }
    auto callbacks = mCallbacks.begin();
    while (callbacks != mCallbacks.end())
    {
      callbacks->handle(mValue);
      ++callbacks;
    }
  }
  else
  {
  }

  context.setCurrentVirtual(nullptr);

  return hasChanged;
}

template <typename T>
template <typename Function>
void
SamplePort<T>::SamplePortInternal::setPrinterFunction(Function& printer)
{
  mPrinter = printer;
}

template <typename T>
template <typename Function>
typename SamplePort<T>::CallbackRaw*
SamplePort<T>::SamplePortInternal::addListener(
    Function function,
    protest::meta::CallContext& context)
{
  auto* callback = new Callback<Function>(function, this, context);
  callback->mLine = context.getLine();
  callback->mFile = context.getUnit().getFileName();
  mCallbacks.append(*callback);
  return callback;
}

template <typename T>
void
SamplePort<T>::SamplePortInternal::removeListener(CallbackRaw* callback)
{
  mCallbacks.remove(*callback);
  delete callback;
}

template <typename T>
template <typename Function>
typename SamplePort<T>::FilterRaw*
SamplePort<T>::SamplePortInternal::addFilter(Function function)
{
  auto* filter = new Filter<Function>(function);
  mFilters.append(*filter);
  return filter;
}

template <typename T>
void
SamplePort<T>::SamplePortInternal::removeFilter(FilterRaw* filter)
{
  mFilters.remove(*filter);
  delete filter;
}

template <typename T>
const char*
SamplePort<T>::SamplePortInternal::getName()
{
  return mName;
}

template <typename T>
void
SamplePort<T>::SamplePortInternal::add(Listener* listener)
{
  mListeners.prepend(*listener);
}

template <typename T>
void
SamplePort<T>::SamplePortInternal::remove(Listener* listener)
{
  mListeners.remove(*listener);
}

// ---------------------------------------------------------------------------
template <typename T>
SamplePort<T>::SamplePort(core::Signal<T>& signal, RunnerRaw* runner) :
  mInternal(nullptr)
{
  assert(mInternal == nullptr);
  mInternal = new SamplePortInternal();
  mInternal->increment();
  assert(mInternal);
  mInternal->bindTo(signal, runner);
}

} // namespace core

} // namespace protest
