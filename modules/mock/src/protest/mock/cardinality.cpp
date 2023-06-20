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

#include "protest/mock/cardinality.h"

#include <limits>
#include <sstream>

#include <cstdint>
#include <cstddef>
#include <cassert>

using namespace protest::mock;
using namespace protest::mock::internal;

// ---------------------------------------------------------------------------
Cardinality
protest::mock::any()
{
  return Cardinality(new BetweenImpl(0, std::numeric_limits<size_t>::max()));
}

Cardinality
protest::mock::exactly(size_t numberOfCalls)
{
  return Cardinality(new BetweenImpl(numberOfCalls, numberOfCalls));
}

Cardinality
protest::mock::atMost(size_t max)
{
  return Cardinality(new BetweenImpl(0, max));
}

Cardinality
protest::mock::atLeast(size_t min)
{
  return Cardinality(new BetweenImpl(min, std::numeric_limits<size_t>::max()));
}

Cardinality
protest::mock::between(size_t min, size_t max)
{
  return Cardinality(new BetweenImpl(min, max));
}

// ---------------------------------------------------------------------------
Cardinality::Cardinality(CardinalityInterface* interface) :
  mInterface(interface)
{
}

Cardinality::Cardinality(Cardinality&& other) noexcept
{
  delete mInterface;
  // NOLINTNEXTLINE
  mInterface = other.mInterface;
  other.mInterface = nullptr;
}

Cardinality&
Cardinality::operator=(Cardinality&& other) noexcept
{
  delete mInterface;
  mInterface = other.mInterface;
  other.mInterface = nullptr;
  return *this;
}

Cardinality::~Cardinality()
{
  delete mInterface;
  mInterface = nullptr;
}

// ---------------------------------------------------------------------------
bool
Cardinality::isSatisfiedByCallCount(size_t numberOfCalls) const
{
  return mInterface->isSatisfiedByCallCount(numberOfCalls);
}

bool
Cardinality::isSaturatedByCallCount(size_t numberOfCalls) const
{
  return mInterface->isSaturatedByCallCount(numberOfCalls);
}

void
Cardinality::explain(std::ostream& stream) const
{
  return mInterface->explain(stream);
}

void
Cardinality::explain(std::ostream& stream, size_t callCounter) const
{
  return mInterface->explain(stream, callCounter);
}

// ---------------------------------------------------------------------------
BetweenImpl::BetweenImpl(size_t min, size_t max) : mMin(min), mMax(max)
{
  assert(min <= max);
}

// ---------------------------------------------------------------------------
bool
BetweenImpl::isSatisfiedByCallCount(size_t numberOfCalls) const
{
  return numberOfCalls <= mMax && numberOfCalls >= mMin;
}

bool
BetweenImpl::isSaturatedByCallCount(size_t numberOfCalls) const
{
  return numberOfCalls >= mMax;
}

void
BetweenImpl::explain(std::ostream& stream)
{
  if (mMin == 0)
  {
    if (mMax == 0)
    {
      stream << "never called";
    }
    else if (mMax == std::numeric_limits<size_t>::max())
    {
      stream << "called any number of times";
    }
    else
    {
      stream << "called at most " << format(mMax);
    }
  }
  else if (mMin == mMax)
  {
    stream << "called " << format(mMin);
  }
  else if (mMax == std::numeric_limits<size_t>::max())
  {
    stream << "called at least " << format(mMin);
  }
  else
  {
    stream << "called between " << mMin << " and " << mMax << " times";
  }
}

void
BetweenImpl::explain(std::ostream& stream, size_t callCounter)
{
  if (callCounter > 0)
  {
    stream << "called " << format(callCounter);
  }
  else
  {
    stream << "never called";
  }
}

std::string
BetweenImpl::format(size_t count)
{
  if (count == 0)
  {
    return "never";
  }
  if (count == 1)
  {
    return "once";
  }
  if (count == 2)
  {
    return "twice";
  }
  std::stringstream sstream;
  sstream << count << " times";
  return sstream.str();
}
