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

#include "protest/log/logger.h"

#include <cstring>

using namespace protest::log;
using namespace protest::time;

// ---------------------------------------------------------------------------
// TODO (jreinking) should not be global
// NOLINTNEXTLINE
bool Logger::globalLastIsNewline = true;

// ---------------------------------------------------------------------------
StreamWrapper::StreamWrapper(std::ostream& stream) : mStream(stream)
{
}

StreamWrapper::~StreamWrapper()
{
  mStream.flush();
}

StreamWrapper::operator std::ostream&()
{
  return mStream;
}

// ---------------------------------------------------------------------------
Logger::Logger() :
  mBuffer(),
  mStatus(Status::indent),
  mStream(this),
  mUserStream(mStream),
  mNullStream(mNullStreamImpl),
  mLastIsNewline(true)
{
  ::memset(&mBuffer[0], 0, bufferSize);
  setp(&mBuffer[0], &mBuffer[sizeof(mBuffer) - 1]);
}

// ---------------------------------------------------------------------------
StreamWrapper
Logger::startLog(const char* tag,
                 const char* name,
                 const char* file,
                 size_t line,
                 protest::time::TimePoint now)
{
  flush();
  if (!globalLastIsNewline)
  {
    mStream << "\n";
  }
  flush();
  // assert(mLastIsNewline);
  mStatus = Status::normal;
  mStream << tag << " " << std::setw(timestampSize) << std::setfill('0')
          << now.milliseconds() << " "
          << " " << std::setw(nameSize) << std::setfill(' ') << name << " "
          << file << ":" << line << std::endl;
  return StreamWrapper(mStream);
}

StreamWrapper
Logger::startLog(const char* tag,
                 const char* name,
                 protest::time::TimePoint now)
{
  flush();
  if (!globalLastIsNewline)
  {
    mStream << "\n";
  }
  flush();
  // assert(mLastIsNewline);
  mStatus = Status::normal;
  mStream << tag << " " << std::setw(timestampSize) << std::setfill('0')
          << now.milliseconds() << " "
          << " " << std::setw(nameSize) << std::setfill(' ') << name << " ";
  return StreamWrapper(mStream);
}

StreamWrapper
Logger::startLog(const char* tag, const char* name)
{
  flush();
  if (!globalLastIsNewline)
  {
    mStream << "\n";
  }
  flush();
  // assert(mLastIsNewline);
  mStatus = Status::normal;
  mStream << tag << " " << std::setw(timestampSize) << std::setfill(' ') << " "
          << " "
          << " " << std::setw(nameSize) << std::setfill(' ') << name << " ";
  return StreamWrapper(mStream);
}

UniversalStream&
Logger::getStream()
{
  return mUserStream;
}

UniversalStream&
Logger::getNullStream()
{
  return mNullStream;
}

void
Logger::flush()
{
  mStream.flush();
}

// ---------------------------------------------------------------------------
std::streambuf::int_type
Logger::NullStreambuf::overflow(std::streambuf::int_type character)
{
  setp(&mDummyBuffer[0], &mDummyBuffer[sizeof(mDummyBuffer)]);
  return (character == EOF) ? '\0' : character;
}

Logger::NullOStream::NullOStream() : std::ostream(this)
{
}

// ---------------------------------------------------------------------------
std::streambuf::int_type
Logger::overflow(std::streambuf::int_type character)
{
  if (character != traits_type::eof())
  {
    *pptr() = static_cast<char>(character);
    pbump(1);
    writeToCOut();
  }
  else
  {
  }
  return character;
}

int
Logger::sync()
{
  writeToCOut();
  return 0;
}

void
Logger::writeToCOut()
{
  static bool newline = false;
  auto* iter = pbase();
  while (iter != pptr())
  {
    if (mLastIsNewline && mStatus == Status::indent)
    {
      for (int i = 0; i <= totalOffsetSize; i++)
      {
        std::cout << " ";
      }
    }
    else
    {
    }

    if (*iter == '\n')
    {
      mStatus = Status::indent;
      mLastIsNewline = true;
      globalLastIsNewline = true;
      assert(!newline);
      newline = true;
    }
    else
    {
      globalLastIsNewline = false;
      mLastIsNewline = false;
      newline = false;
    }

    std::cout.write(iter, 1);
    iter++;
  }
  // NOLINTNEXTLINE
  pbump(pbase() - pptr());
}

// ---------------------------------------------------------------------------
// NOLINTNEXTLINE
template <>
UniversalStream&
protest::log::operator<<(UniversalStream& stream, const char* value)
{
  stream.mOutput << "\"" << value << "\"";
  stream.flush();
  return stream;
}
