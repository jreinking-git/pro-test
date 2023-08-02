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

#include <cstddef>
#include <cstdint>

namespace protest
{

namespace time
{

class TimePoint;
class Millisecond;

// ---------------------------------------------------------------------------
/**
 * @class Duration
 */
class Duration
{
public:
// ---------------------------------------------------------------------------
  friend class TimePoint;
  friend class Nanoseconds;
  friend class Microseconds;
  friend class Millisecond;
  friend class Seconds;
  friend class Minutes;
  friend class Hours;
  friend class Days;

  static constexpr Duration
  infinity();

  static constexpr Duration
  zero();

  static constexpr Duration
  smallestNonZero();

// ---------------------------------------------------------------------------
  explicit Duration() = default;

  Duration(const Duration&) = default;

  Duration&
  operator=(const Duration&) = default;

  ~Duration() = default;

// ---------------------------------------------------------------------------
  constexpr bool
  operator!=(const Duration other) const;

  constexpr bool
  operator==(const Duration other) const;

  constexpr bool
  operator<(const Duration other) const;

  constexpr bool
  operator>(const Duration other) const;

  constexpr bool
  operator<=(const Duration other) const;

  constexpr bool
  operator>=(const Duration other) const;

  constexpr Duration
  operator+(const Duration other) const;

  constexpr Duration
  operator-(const Duration other) const;

  constexpr Duration
  operator+=(const Duration other);

  constexpr Duration
  operator-=(const Duration other);

  constexpr Duration
  operator*(int64_t factor) const;

// ---------------------------------------------------------------------------
  constexpr int64_t
  nanoseconds() const;

  constexpr int64_t
  microseconds() const;

  constexpr int64_t
  milliseconds() const;

  constexpr int64_t
  seconds() const;

  constexpr int64_t
  minutes() const;

  constexpr int64_t
  hours() const;

  constexpr int64_t
  days() const;

// ---------------------------------------------------------------------------
private:
  constexpr explicit Duration(int64_t nanoseconds);

  int64_t mNanoseconds;
};

// ---------------------------------------------------------------------------
constexpr Duration
Duration::infinity()
{
  // TODO Limit<int64_t>::max
  return Duration(0x7FFFFFFFFFFFFFFF);
}

constexpr Duration
Duration::zero()
{
  return Duration(0);
}

constexpr Duration
Duration::smallestNonZero()
{
  return Duration(1);
}

// ---------------------------------------------------------------------------
constexpr bool
Duration::operator!=(const Duration other) const
{
  return other.mNanoseconds != this->mNanoseconds;
}

constexpr bool
Duration::operator==(const Duration other) const
{
  return other.mNanoseconds == this->mNanoseconds;
}

constexpr bool
Duration::operator<(const Duration other) const
{
  return this->mNanoseconds < other.mNanoseconds;
}

constexpr bool
Duration::operator>(const Duration other) const
{
  return this->mNanoseconds > other.mNanoseconds;
}

constexpr bool
Duration::operator<=(const Duration other) const
{
  return this->mNanoseconds <= other.mNanoseconds;
}

constexpr bool
Duration::operator>=(const Duration other) const
{
  return this->mNanoseconds >= other.mNanoseconds;
}

constexpr Duration
Duration::operator+(const Duration other) const
{
  return Duration(this->mNanoseconds + other.mNanoseconds);
}

constexpr Duration
Duration::operator-(const Duration other) const
{
  return Duration(this->mNanoseconds - other.mNanoseconds);
}

constexpr Duration
Duration::operator+=(const Duration other)
{
  this->mNanoseconds += other.mNanoseconds;
  return Duration(this->mNanoseconds);
}

constexpr Duration
Duration::operator-=(const Duration other)
{
  this->mNanoseconds -= other.mNanoseconds;
  return Duration(this->mNanoseconds);
}

constexpr Duration
Duration::operator*(int64_t factor) const
{
  return Duration(this->mNanoseconds * factor);
}

// ---------------------------------------------------------------------------
constexpr int64_t
Duration::nanoseconds() const
{
  return mNanoseconds;
}

constexpr int64_t
Duration::microseconds() const
{
  return mNanoseconds / (1000u);
}

constexpr int64_t
Duration::milliseconds() const
{
  return mNanoseconds / (1000u * 1000u);
}

constexpr int64_t
Duration::seconds() const
{
  return mNanoseconds / (1000u * 1000u * 1000u);
}

constexpr int64_t
Duration::minutes() const
{
  return ((((mNanoseconds / 1000u) / 1000u) / 1000u) / 60u);
}

constexpr int64_t
Duration::hours() const
{
  return (((((mNanoseconds / 1000u) / 1000u) / 1000u) / 60u) / 60u);
}

constexpr int64_t
Duration::days() const
{
  return ((((((mNanoseconds / 1000u) / 1000u) / 1000u) / 60u) / 60u) / 24u);
}

// ---------------------------------------------------------------------------
constexpr Duration::Duration(int64_t nanoseconds) : mNanoseconds(nanoseconds)
{
}

// ---------------------------------------------------------------------------
/**
 * @class Nanoseconds
 */
class Nanoseconds : public Duration
{
public:
  constexpr explicit Nanoseconds(int64_t nanoseconds);

  Nanoseconds(const Nanoseconds&) = default;

  Nanoseconds&
  operator=(const Nanoseconds&) = delete;

  Nanoseconds&
  operator=(Nanoseconds&&) noexcept = delete;

  ~Nanoseconds() = default;

private:
};

// ---------------------------------------------------------------------------
constexpr Nanoseconds::Nanoseconds(int64_t nanoseconds) : Duration(nanoseconds)
{
}

// ---------------------------------------------------------------------------
/**
 * @class Microseconds
 */
class Microseconds : public Duration
{
public:
  constexpr explicit Microseconds(int64_t microseconds);

  Microseconds(const Microseconds&) = default;

  Microseconds&
  operator=(const Microseconds&) = delete;

  Microseconds&
  operator=(Microseconds&&) noexcept = delete;

  ~Microseconds() = default;

private:
};

// ---------------------------------------------------------------------------
constexpr Microseconds::Microseconds(int64_t microseconds) :
  Duration(microseconds * 1000u)
{
}

// ---------------------------------------------------------------------------
/**
 * @class Millisecond
 */
class Millisecond : public Duration
{
public:
  constexpr explicit Millisecond(int64_t milliseconds);

  Millisecond(const Millisecond&) = default;

  Millisecond&
  operator=(const Millisecond&) = delete;

  Millisecond&
  operator=(Millisecond&&) noexcept = delete;

  ~Millisecond() = default;

private:
};

// ---------------------------------------------------------------------------
constexpr Millisecond::Millisecond(int64_t milliseconds) :
  Duration(milliseconds * 1000u * 1000u)
{
}

// ---------------------------------------------------------------------------
/**
 * @class Seconds
 */
class Seconds : public Duration
{
public:
  constexpr explicit Seconds(int64_t s);

  Seconds(const Seconds&) = default;

  Seconds&
  operator=(const Seconds&) = delete;

  Seconds&
  operator=(Seconds&&) noexcept = delete;

  ~Seconds() = default;

private:
};

// ---------------------------------------------------------------------------
constexpr Seconds::Seconds(int64_t s) : Duration(s * 1000u * 1000u * 1000u)
{
}

// ---------------------------------------------------------------------------
/**
 * @class Minutes
 */
class Minutes : public Duration
{
public:
  constexpr explicit Minutes(int64_t min);

  Minutes(const Minutes&) = default;

  Minutes&
  operator=(const Minutes&) = delete;

  Minutes&
  operator=(Minutes&&) noexcept = delete;

  ~Minutes() = default;

private:
};

// ---------------------------------------------------------------------------
constexpr Minutes::Minutes(int64_t min) :
  Duration(min * 60 * 1000u * 1000u * 1000u)
{
}

// ---------------------------------------------------------------------------
/**
 * @class Hours
 */
class Hours : public Duration
{
public:
  constexpr explicit Hours(int64_t hours);

  Hours(const Hours&) = default;

  Hours&
  operator=(const Hours&) = delete;

  Hours&
  operator=(Hours&&) noexcept = delete;

  ~Hours() = default;

private:
};

// ---------------------------------------------------------------------------
constexpr Hours::Hours(int64_t hours) :
  Duration(hours * 60 * 60 * 1000u * 1000u * 1000u)
{
}

// ---------------------------------------------------------------------------
/**
 * @class Days
 */
class Days : public Duration
{
public:
  constexpr explicit Days(int64_t days);

  Days(const Days&) = default;

  Days&
  operator=(const Days&) = default;

  Days&
  operator=(Days&&) noexcept = default;

  ~Days() = default;

private:
};

// ---------------------------------------------------------------------------
constexpr Days::Days(int64_t days) :
  Duration(days * 24 * 60 * 60 * 1000u * 1000u * 1000u)
{
}

// ---------------------------------------------------------------------------
constexpr Duration operator"" _ns(unsigned long long n)
{
  return Nanoseconds(n);
}

constexpr Duration operator"" _us(unsigned long long n)
{
  return Microseconds(n);
}

constexpr Duration operator"" _ms(unsigned long long ms)
{
  return Millisecond(ms);
}

constexpr Duration operator"" _s(unsigned long long s)
{
  return Seconds(s);
}

constexpr Duration operator"" _min(unsigned long long min)
{
  return Minutes(min);
}

constexpr Duration operator"" _h(unsigned long long hours)
{
  return Hours(hours);
}

constexpr Duration operator"" _days(unsigned long long days)
{
  return Days(days);
}

} // namespace time

} // namespace protest
