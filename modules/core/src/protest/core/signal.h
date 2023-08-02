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

#include "protest/core/queue_port.h"
#include "protest/core/signal_raw.h"
#include "protest/core/sample_port.h"
#include "protest/utils/list.h"
#include "protest/core/context.h"
#include "protest/meta.h"

#include <type_traits>

namespace protest
{

namespace core
{

// ---------------------------------------------------------------------------
/**
 * @class Signal
 */
template <typename T>
class Signal : public protest::core::SignalRaw
{
public:
  explicit Signal(
      Context& context,
      protest::meta::Signal& signal = protest::meta::Signal::defaultContext());

  Signal(const Signal&) = delete;

  Signal(Signal&&) noexcept = delete;

  Signal&
  operator=(const Signal&) = delete;

  Signal&
  operator=(Signal&&) noexcept = delete;

  ~Signal();

// ---------------------------------------------------------------------------
  void
  push(T value,
       protest::meta::CallContext& push =
           protest::meta::CallContext::defaultContext());

private:
  void
  bindSamplePort(void* port) override;

  void
  bindQueuePort(void* port) override;

  void
  bindPort(typename core::SamplePort<T>::SamplePortInternal& port);

  void
  bindPort(typename core::QueuePort<T>::QueuePortInternal& port);

  void
  removeSamplePort(void* port) override;

  void
  removeQueuePort(void* port) override;

  void
  pushValue(void* value) override;

  protest::List<typename core::SamplePort<T>::SamplePortInternal> mSamplePorts;
  protest::List<typename core::QueuePort<T>::QueuePortInternal> mQueuePorts;
  Context& mContext;
  // meta::Signal& mSignalInfo;
};

// ---------------------------------------------------------------------------
template <typename T>
Signal<T>::Signal(Context& context, protest::meta::Signal& signal) :
  SignalRaw(signal),
  mContext(context)
{
}

template <typename T>
Signal<T>::~Signal()
{
  // PROTEST_ASSERT(mSamplePorts.numberOfElements() == 0);
  // PROTEST_ASSERT(mQueuePorts.numberOfElements() == 0);
}

// ---------------------------------------------------------------------------
template <typename T>
void
Signal<T>::push(T value, meta::CallContext& context)
{
  auto runner = mContext.getCurrent();
  {
    auto stream = runner->getLogger().startLog("PUSH",
                                               runner->getName(),
                                               context.getUnit().getFileName(),
                                               context.getLine(),
                                               runner->now());
    stream.operator std::ostream&()
        << "Push value to '" << getSignalInfo().getObjectName() << "':\n";
    runner->getLogger().getStream() << value;
  }

  auto n = mSamplePorts.numberOfElements();
  auto iter1 = mSamplePorts.begin();
  while (iter1 != mSamplePorts.end())
  {
    // yield here so that if there are two or more coroutines are runable
    // there can mix the delivery ot the message instead of that each
    // coroutine is pushing it in a raw
    mContext.getCurrent()->coroYield();
    iter1->insertValue(value);
    ++iter1;
  }
  PROTEST_ASSERT(mSamplePorts.numberOfElements() == n);

  n = mQueuePorts.numberOfElements();
  auto iter2 = mQueuePorts.begin();
  while (iter2 != mQueuePorts.end())
  {
    // yield here so that if there are two or more coroutines runable
    // there can mix the delivery ot the message instead of that each
    // coroutine is pushing it in a row
    mContext.getCurrent()->coroYield();
    iter2->insertValue(value);
    ++iter2;
  }
  PROTEST_ASSERT(mQueuePorts.numberOfElements() == n);
}

// ---------------------------------------------------------------------------
template <typename T>
void
Signal<T>::bindSamplePort(void* port)
{
  using Internal = typename core::SamplePort<T>::SamplePortInternal;
  bindPort(*reinterpret_cast<Internal*>(port));
}

template <typename T>
void
Signal<T>::bindQueuePort(void* port)
{
  using Internal = typename core::QueuePort<T>::QueuePortInternal;
  bindPort(*reinterpret_cast<Internal*>(port));
}

template <typename T>
void
Signal<T>::bindPort(typename core::SamplePort<T>::SamplePortInternal& port)
{
  mSamplePorts.prepend(port);
}

template <typename T>
void
Signal<T>::bindPort(typename core::QueuePort<T>::QueuePortInternal& port)
{
  mQueuePorts.prepend(port);
}

template <typename T>
void
Signal<T>::removeSamplePort(void* port)
{
  using Internal = typename SamplePort<T>::SamplePortInternal;
  mSamplePorts.remove(*reinterpret_cast<Internal*>(port));
}

template <typename T>
void
Signal<T>::removeQueuePort(void* port)
{
  using Internal = typename QueuePort<T>::QueuePortInternal;
  mQueuePorts.remove(*reinterpret_cast<Internal*>(port));
}

template <typename T>
void
Signal<T>::pushValue(void* value)
{
  assert(false);
  // push(*reinterpret_cast<T*>(value));
}

} // namespace core

} // namespace protest
