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

#include "protest/time/time_point.h"
#include "protest/utils/debug.h"

#include <cstdint>
#include <cstddef>

using namespace protest::time;

// NOLINTNEXTLINE
TimePoint
TimePoint::operator+(const Duration rhs) const
{
  return TimePoint(this->mNanoseconds + rhs.nanoseconds());
}

// NOLINTNEXTLINE
TimePoint
TimePoint::operator-(const Duration rhs) const
{
  return TimePoint(this->mNanoseconds - rhs.nanoseconds());
}

// NOLINTNEXTLINE
Duration
TimePoint::operator-(const TimePoint rhs) const
{
  const uint64_t diff = (mNanoseconds > rhs.mNanoseconds)
                            ? (mNanoseconds - rhs.mNanoseconds)
                            : (rhs.mNanoseconds - mNanoseconds);
  PROTEST_ASSERT(diff <= INT64_MAX);
  return Duration((mNanoseconds > rhs.mNanoseconds)
                      ? static_cast<int64_t>(diff)
                      : -static_cast<int64_t>(diff));
}

// NOLINTNEXTLINE
uint64_t
TimePoint::nanoseconds() const
{
  return mNanoseconds;
}

// NOLINTNEXTLINE
uint64_t
TimePoint::milliseconds() const
{
  return mNanoseconds / (nanoToMilliseconds);
}

// NOLINTNEXTLINE
bool
TimePoint::operator==(const TimePoint other) const
{
  return other.mNanoseconds == this->mNanoseconds;
}

// NOLINTNEXTLINE
bool
TimePoint::operator!=(const TimePoint other) const
{
  return other.mNanoseconds != this->mNanoseconds;
}

// NOLINTNEXTLINE
bool
TimePoint::operator>=(const TimePoint other) const
{
  return this->mNanoseconds >= other.mNanoseconds;
}

// NOLINTNEXTLINE
bool
TimePoint::operator>(const TimePoint other) const
{
  return this->mNanoseconds > other.mNanoseconds;
}

// NOLINTNEXTLINE
bool
TimePoint::operator<(const TimePoint& rhs) const
{
  return this->mNanoseconds < rhs.mNanoseconds;
}

// NOLINTNEXTLINE
bool
TimePoint::operator<=(const TimePoint& rhs) const
{
  return this->mNanoseconds <= rhs.mNanoseconds;
}
