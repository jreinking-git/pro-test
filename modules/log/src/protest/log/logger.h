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

#include "protest/time/time_point.h"
#include "protest/log/operator.h"

#include <iomanip>

#include <cstdio>
#include <cassert>

namespace protest
{

namespace log
{

class Logger;

// ---------------------------------------------------------------------------
/**
 * @class StreamWrapper
 * 
 * This is a wrapper arround an std::ostream&.This wrapper should be used when
 * delegating a std::ostream to other software parts so that flush will
 * automatically be called. This prevents the log output might from overlaping.
 */
class StreamWrapper
{
public:
  template <typename T>
  friend StreamWrapper
  operator<<(StreamWrapper&& wrapper, const T& value);

  template <typename T>
  friend StreamWrapper
  operator<<(StreamWrapper&& wrapper, const T&& value);

// ---------------------------------------------------------------------------
  explicit StreamWrapper(std::ostream& stream);

  StreamWrapper(const StreamWrapper&) = default;

  StreamWrapper(StreamWrapper&&) noexcept = default;

  StreamWrapper&
  operator=(const StreamWrapper&) = delete;

  StreamWrapper&
  operator=(StreamWrapper&&) noexcept = delete;

  ~StreamWrapper();

// ---------------------------------------------------------------------------
  operator std::ostream&();

private:
  std::ostream& mStream;
};

// ---------------------------------------------------------------------------
// TODO this is not so nice since it will call flush for every value streamed
template <typename T>
inline StreamWrapper
operator<<(StreamWrapper&& wrapper, const T& value)
{
  wrapper.mStream << value;
  wrapper.mStream.flush();
  return wrapper;
}

template <typename T>
inline StreamWrapper
operator<<(StreamWrapper&& wrapper, const T&& value)
{
  wrapper.mStream << value;
  wrapper.mStream.flush();
  return wrapper;
}

// ---------------------------------------------------------------------------
/**
 * @class Logger
 */
class Logger : public std::streambuf
{
public:
  static constexpr size_t bufferSize = 1024;
  static constexpr size_t timestampSize = 10U;
  static constexpr size_t nameSize = 4;
  static constexpr size_t totalOffsetSize =
      4 + 1 + timestampSize + 1 + nameSize + 1;

  explicit Logger();

  Logger(const Logger& other) = delete;

  Logger(Logger&& other) = delete;

  Logger&
  operator=(const Logger& other) = delete;

  Logger&
  operator=(Logger&& other) = delete;

  ~Logger() = default;

// ---------------------------------------------------------------------------
  StreamWrapper
  startLog(const char* tag,
           const char* name,
           const char* file,
           size_t line,
           protest::time::TimePoint now);

  StreamWrapper
  startLog(const char* tag, const char* name, protest::time::TimePoint now);

  StreamWrapper
  startLog(const char* tag, const char* name);

  UniversalStream&
  getStream();

  UniversalStream&
  getNullStream();

  void
  flush();

// ---------------------------------------------------------------------------
public:// TODO
  class NullStreambuf : public std::streambuf
  {
  public:
    explicit NullStreambuf() = default;

    explicit NullStreambuf(const NullStreambuf& other) = delete;

    NullStreambuf(NullStreambuf&& other) = delete;

    NullStreambuf&
    operator=(const NullStreambuf& other) = delete;

    NullStreambuf&
    operator=(NullStreambuf&& other) = delete;

  private:
    int_type
    overflow(int_type c) override;

    char mDummyBuffer[64];
  };

  class NullOStream : public NullStreambuf, public std::ostream
  {
  public:
    explicit NullOStream();

    NullOStream(const NullOStream& other) = delete;

    NullOStream(NullOStream&& other) = delete;

    NullOStream&
    operator=(const NullOStream& other) = delete;

    NullOStream&
    operator=(NullOStream&& other) = delete;
  };

  enum class Status
  {
    normal,
    indent
  };

  int_type
  overflow(int_type c) override;

  int
  sync() override;

  void
  writeToCOut();

  char mBuffer[bufferSize];
  Status mStatus;
  std::ostream mStream;
  UniversalStream mUserStream;
  UniversalStream mNullStream;
  NullOStream mNullStreamImpl;
  bool mLastIsNewline;
  static bool globalLastIsNewline;
};

} // namespace log

} // namespace protest
