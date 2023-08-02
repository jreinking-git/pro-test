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

#include "protest/time/duration.h"

#include <cstdint>
#include <cstddef>

namespace protest
{

namespace time
{

class Clock;

// ---------------------------------------------------------------------------
/**
 * @class TimePoint
 */
class TimePoint
{
public:
  friend class Clock;

  constexpr static TimePoint
  startOfEpoche()
  {
    return TimePoint(0u);
  }

  constexpr static TimePoint
  endOfEpoche()
  {
    // TODO max<uint62_t>::value
    return TimePoint(0xFFFFFFFFFFFFFFFF);
  }

// ---------------------------------------------------------------------------
  TimePoint() : mNanoseconds(0u)
  {
  }

  TimePoint(const TimePoint&) = default;

  TimePoint&
  operator=(const TimePoint&) = default;

  ~TimePoint() = default;

// ---------------------------------------------------------------------------
  TimePoint
  operator+(const Duration rhs) const;

  TimePoint
  operator-(const Duration rhs) const;

  Duration
  operator-(const TimePoint rhs) const;

// ---------------------------------------------------------------------------
  uint64_t
  nanoseconds() const;

  uint64_t
  milliseconds() const;

// ---------------------------------------------------------------------------
  bool
  operator==(const TimePoint other) const;

  bool
  operator!=(const TimePoint other) const;

  bool
  operator>=(const TimePoint other) const;

  bool
  operator>(const TimePoint other) const;

  bool
  operator<(const TimePoint& rhs) const;

  bool
  operator<=(const TimePoint& rhs) const;

private:
  static constexpr uint64_t nanoToMilliseconds = 1000 * 1000;

  constexpr explicit TimePoint(uint64_t nanoseconds) : mNanoseconds(nanoseconds)
  {
  }

  uint64_t mNanoseconds;
};

} // namespace time

} // namespace protest
