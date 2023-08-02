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

#include "protest/core/api.h"
#include "protest/core/context.h"
#include "protest/core/version.h"

using namespace protest;
using namespace protest::core;
using namespace protest::time;

// ---------------------------------------------------------------------------
const char*
protest::version()
{
  return protest::versionAsString;
}

// ---------------------------------------------------------------------------
Runner::Runner(core::Context& context, const char* name) :
  RunnerRaw(context, name)
{
  assert(strlen(name) <= 4);
  assert(strlen(name) > 0);
}

// ---------------------------------------------------------------------------
protest::log::StreamWrapper
protest::logger()
{
  auto* context = core::Context::getCurrentContext();
  auto* runner = context->getCurrentVirtual();
  return runner->getLogger().startLog("INFO", runner->getName());
}

void
protest::line()
{
  auto* context = core::Context::getCurrentContext();
  auto* runner = context->getCurrentVirtual();
  // TODO (jreinking) it is spread arround the code
  static constexpr size_t seperatorLength = 79;
  runner->getLogger().getStream().getPlain()
      << std::string(seperatorLength, '-') << "\n";
  runner->getLogger().flush();
}

protest::log::StreamWrapper
protest::info(meta::CallContext& callContext)
{
  auto* context = core::Context::getCurrentContext();
  auto* runner = context->getCurrentVirtual();
  return runner->getLogger().startLog("INFO",
                                      runner->getName(),
                                      callContext.getUnit().getFileName(),
                                      callContext.getLine(),
                                      runner->now());
}

protest::log::StreamWrapper
protest::warn(meta::CallContext& callContext)
{
  auto* context = core::Context::getCurrentContext();
  auto* runner = context->getCurrentVirtual();
  return runner->getLogger().startLog("WARN",
                                      runner->getName(),
                                      callContext.getUnit().getFileName(),
                                      callContext.getLine(),
                                      runner->now());
}

// ---------------------------------------------------------------------------
// TODO (jreinking) now could also be used like: wait(now() >= myTimePoint);
TimePoint
protest::now()
{
  auto* context = core::Context::getCurrentContext();
  return context->getCurrentVirtual()->now();
}

void
protest::wait(time::Duration duration, meta::CallContext& callContext)
{
  auto* context = core::Context::getCurrentContext();
  auto* runner = context->getCurrentVirtual();
  {
    auto stream =
        runner->getLogger().startLog("WAIT",
                                     runner->getName(),
                                     callContext.getUnit().getFileName(),
                                     callContext.getLine(),
                                     runner->now());
    stream.operator std::ostream&()
        << std::string("Wait for ") << std::to_string(duration.milliseconds())
        << " ms\n";
  }

  runner->RunnerRaw::waitInternal(duration);

  {
    auto stream =
        runner->getLogger().startLog("WAIT",
                                     runner->getName(),
                                     callContext.getUnit().getFileName(),
                                     callContext.getLine(),
                                     runner->now());
    stream.operator std::ostream&()
        << "Timeout of " << std::to_string(duration.milliseconds())
        << " ms elapsed!\n";
  }
}

// ---------------------------------------------------------------------------
Stopwatch
protest::createStopwatch()
{
  auto* context = core::Context::getCurrentContext();
  auto* runner = context->getCurrentVirtual();
  return Stopwatch(runner);
}

// ---------------------------------------------------------------------------
protest::time::Duration
protest::random(Duration durationFrom, Duration durationTo)
{
  PROTEST_ASSERT(durationFrom <= durationTo);
  auto diff = (durationTo - durationFrom);
  // clang-format off
  // random is often online a int32_t
  // NOLINTNEXTLINE
  static_assert(sizeof(decltype(rand())) == 4U);
  // NOLINTNEXTLINE
  auto randomDuration = Nanoseconds((((long long) rand() << 32) | rand()) % (diff.nanoseconds() + 1));
  // clang-format on
  return durationFrom + randomDuration;
}

protest::time::Duration
protest::random(Duration durationTo)
{
  return random(Duration::zero(), durationTo);
}

bool
protest::coinFlip(float probability)
{
  static constexpr int32_t promilleFactor = 1000;
  PROTEST_ASSERT(probability <= 1.0);
  PROTEST_ASSERT(probability >= 0.0);
  // NOLINTNEXTLINE
  return ((rand() % promilleFactor) <= probability * promilleFactor);
}

// ---------------------------------------------------------------------------
Stream::Stream(log::Logger& logger, log::UniversalStream& stream, bool result) :
  mLogger(logger),
  mStream(stream),
  mResult(result)
{
}

Stream::operator bool() const
{
  return mResult;
}

// ---------------------------------------------------------------------------
Section
protest::section(const char* name)
{
  return Section(name);
}

// ---------------------------------------------------------------------------
protest::Stream
protest::assertThat(bool condition, protest::meta::Assertion& assertion)
{
  auto* context = core::Context::getCurrentContext();
  auto* runner = context->getCurrentVirtual();

  assertion.markAsExecuted();
  if (!condition)
  {
    assertion.incrementNumberOfFailes();
    auto stream =
        runner->getLogger().startLog("FAIL",
                                     runner->getName(),
                                     assertion.getUnit().getFileName(),
                                     assertion.getLine(),
                                     runner->now());
    stream.operator std::ostream&()
        << "FAIL: the condition evaluates to false:\n'"
        << assertion.getCondition() << "'\n";
  }
  else
  {
    auto stream =
        runner->getLogger().startLog("PASS",
                                     runner->getName(),
                                     assertion.getUnit().getFileName(),
                                     assertion.getLine(),
                                     runner->now());
    stream.operator std::ostream&()
        << "PASS: the condition evaluates to true:\n'"
        << assertion.getCondition() << "'\n";
    runner->getLogger().flush();
  }
  return Stream(runner->getLogger(),
                runner->getLogger().getStream(),
                condition);
}

// ---------------------------------------------------------------------------
protest::Stream
protest::checkThat(bool condition, meta::Check& check)
{
  auto* context = core::Context::getCurrentContext();
  auto* runner = context->getCurrentVirtual();
  check.markAsExecuted();
  if (!condition)
  {
    check.incrementNumberOfFailes();

    auto stream = runner->getLogger().startLog("WARN",
                                               runner->getName(),
                                               check.getUnit().getFileName(),
                                               check.getLine(),
                                               runner->now());
    stream.operator std::ostream&()
        << "WARN: the condition evaluates to false:\n'" << check.getCondition()
        << "'\n";
  }
  else
  {
  }

  if (!condition)
  {
    return Stream(runner->getLogger(),
                  runner->getLogger().getStream(),
                  condition);
  }
  return Stream(runner->getLogger(),
                runner->getLogger().getNullStream(),
                condition);
}
