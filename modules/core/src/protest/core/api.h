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

#include "protest/core/runner_raw.h"
#include "protest/core/stopwatch.h"
#include "protest/core/timer.h"
#include "protest/core/port.h"
#include "protest/core/invariant.h"
#include "protest/core/signal.h"
#include "protest/core/value.h"

namespace protest
{

// ---------------------------------------------------------------------------
const char*
version();

// ---------------------------------------------------------------------------
using Context = protest::core::Context;

template <typename T>
using Signal = protest::core::Signal<T>;

template <typename T>
using SamplePort = protest::core::SamplePort<T>;

template <typename T>
using QueuePort = protest::core::QueuePort<T>;

using Logger = protest::log::Logger;

// ---------------------------------------------------------------------------
/**
 * @class Runner
 */
class Runner : private core::RunnerRaw
{
public:
  explicit Runner(core::Context& context, const char* name = "none");

  Runner(const Runner&) = delete;

  Runner&
  operator=(const Runner&) = delete;

  Runner&
  operator=(Runner&&) noexcept = delete;

  virtual ~Runner() = default;

private:
};

// ---------------------------------------------------------------------------
/**
 * @brief log
 */
protest::log::StreamWrapper
logger();

/**
 * @brief line
 */
void
line();

/**
 * @brief info
 */
protest::log::StreamWrapper
info(protest::meta::CallContext& callContext =
         protest::meta::CallContext::defaultContext());

/**
 * @brief warn
 */
protest::log::StreamWrapper
warn(protest::meta::CallContext& callContext =
         protest::meta::CallContext::defaultContext());

// ---------------------------------------------------------------------------
/**
 * @brief Enforce a printer function generation
 * 
 * Force the protest-compiler to generate a printer function for the given
 * type. This function can be used if the protest-compiler cannot see that
 * a printer function is needed for the given type. E.g.: 
 * 
 * struct Dummy
 * {
 *   std::shared_ptr<MyStruct> mPtr;
 * }
 * 
 * even if there is a custom printer function for std::shared_ptr<T> the
 * protest-compiler cannot see that MyStruct needs a printer function unless
 * the printer function for Dummy was generated. Therfore there is no call
 * to operator<<(UniversalStream&, const T&) with T is equals to MyStruct at
 * protest compile time. Adding
 * 
 * enforcePrinterGeneration<MyStruct>();
 * 
 * anywhere adds the operator<<(UniversalStream&, const MyStruct&) to the AST
 * even if it is not really called at runtime. But this triggers the printer
 * function generation of the protest compiler;
 * 
 * @tparam T
 *  type for which the printer function should be generated
 */
template <typename T>
void
enforcePrinterGeneration();

// ---------------------------------------------------------------------------
/**
 * @brief now
 */
time::TimePoint
now();

// ---------------------------------------------------------------------------
/**
 * @brief Wait
 * 
 * Let the runner sleep for the given duration.
 * 
 * @param duration the duration to sleep
 */
void
wait(time::Duration duration,
     protest::meta::CallContext& callContext =
         protest::meta::CallContext::defaultContext());

// ---------------------------------------------------------------------------

using Stopwatch = core::Stopwatch;

/**
 * @brief Create a Stopwatch
 * 
 * Use this function to create a stopwatch.
 * 
 * @return core::Stopwatch 
 */
core::Stopwatch
createStopwatch();

// ---------------------------------------------------------------------------
/**
 * @brief Create a Timer
 * 
 * @tparam Callback
 *  The type of the callback (auto deducted)
 * 
 * @param callback 
 *  The callback to call when timer expires.
 * 
 * @return core::Timer<Callback> 
 */
/* template <typename Callback>
core::Timer<Callback>
createTimer(Callback callback); */

// ---------------------------------------------------------------------------
/**
 * @brief Create a Port
 * 
 * @tparam T
 *  The type of the signal / port (auto deducted)
 * 
 * @param signal 
 *  The signal to listen on
 * 
 * @return
 *  The created port
 */
template <typename T>
core::AnyPortCreator<T>
createPort(core::Signal<T>& signal);

// ---------------------------------------------------------------------------
/**
 * @brief Create a Invariant
 * 
 * Create an invariant. The expression will be checked whenever one of the
 * sub expression change.
 * 
 * @tparam Expr
 *  Type of the expression (auto deducted)
 * 
 * @param expr
 *  The expression of the invariant
 * 
 * @return
 *  Handle to the invariant
 */
template <typename Expr>
core::Invariant
createInvariant(Expr expr,
                protest::meta::Invariant& context =
                    protest::meta::Invariant::defaultContext());

// ---------------------------------------------------------------------------
time::Duration
random(time::Duration from, time::Duration to);

time::Duration
random(time::Duration to);

bool
coinFlip(float probability = 0.5);

// ---------------------------------------------------------------------------
/**
 * @brief Wait for a condition
 * 
 * @tparam T
 *  Type of the condition
 * 
 * @param condition
 *  The condition to wait for
 */
template <typename T>
std::enable_if_t<!std::is_base_of_v<time::Duration, std::remove_const_t<T>>,
                 void>
wait(T condition,
     protest::meta::CallContext& context =
         protest::meta::CallContext::defaultContext());

// ---------------------------------------------------------------------------
/**
 * @brief Wait for a condition
 * 
 * @tparam T
 *  Type of the condition
 * 
 * @param condition 
 *  The condition to wait for
 * 
 * @param timeout 
 *  timeout for the wait call
 * 
 * @return true, on success
 * @return false, on timeout
 */
template <typename T>
std::enable_if_t<!std::is_base_of_v<time::Duration, std::remove_const_t<T>>,
                 bool>
wait(T condition,
     time::Duration timeout,
     protest::meta::CallContext& context =
         protest::meta::CallContext::defaultContext());

// ---------------------------------------------------------------------------
/**
 * @class Stream
 */
class Stream
{
public:
  explicit Stream(log::Logger& logger,
                  log::UniversalStream& stream,
                  bool result);

  Stream(const Stream&) = default;

  Stream(Stream&&) noexcept = default;

  Stream&
  operator=(const Stream&) = delete;

  Stream&
  operator=(Stream&&) noexcept = delete;

  ~Stream() = default;

// ---------------------------------------------------------------------------
  operator bool() const;

public:
  log::Logger& mLogger;
  log::UniversalStream& mStream;
  bool mResult;
};

// ---------------------------------------------------------------------------
template <typename T>
Stream
operator<<(Stream stream, T& value)
{
  stream.mStream.getPlain() << value;
  return stream;
}

template <typename T>
Stream
operator<<(Stream stream, T&& value)
{
  stream.mStream.getPlain() << value;
  return stream;
}

// ---------------------------------------------------------------------------
/**
 * @class Section
 */
class Section
{
public:
  explicit Section(const char* name);

  Section(const Section& other) = delete;
  
  Section(Section&& other) = default;

  Section& operator=(const Section& other) = delete;

  Section& operator=(Section&& other) = delete;

  ~Section();

private:
};

Section
section(const char* name = nullptr);

// ---------------------------------------------------------------------------
/**
 * @brief assert a condition
 * 
 * @param condition
 *  the condition to check
 * 
 * @return true 
 *  when the condition is true
 * 
 * @return false 
 *  when the condition is false
 */
Stream
assertThat(bool condition,
           protest::meta::Assertion& assertion =
               protest::meta::Assertion::defaultContext());

// ---------------------------------------------------------------------------
/**
 * @brief check a condition
 * 
 * @param condition 
 *  the condition to check
 * 
 * @return true
 *  when the condition is true
 *
 * @return false 
 *  when the condition is false
 */
Stream
checkThat(bool condition,
          protest::meta::Check& check = protest::meta::Check::defaultContext());

} // namespace protest

// ---------------------------------------------------------------------------
#include "protest/core/api_impl.h"