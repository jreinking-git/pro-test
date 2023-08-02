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

#include "protest/meta/unit.h"

#include <iostream>

using namespace protest::meta;

// NOLINTNEXTLINE
Unit* Unit::listOfAllUnits = nullptr;

// ---------------------------------------------------------------------------
Unit::Unit(const char* file) :
  StaticLinkedList(this, listOfAllUnits),
  mFileName(file)
{
}

// ---------------------------------------------------------------------------
void
Unit::addAssertion(Assertion& assertion)
{
  mAssertions.emplace_back(&assertion);
}

const std::vector<Assertion*>&
Unit::getAssertions() const
{
  return mAssertions;
}

size_t
Unit::getNumberOfFailedAssertions() const
{
  size_t number = 0;
  for (const auto& assertion : mAssertions)
  {
    if (assertion->getNumberOfFailes() > 0)
    {
      number++;
    }
  }
  return number;
}

size_t
Unit::getNumberOfPassedAssertions() const
{
  size_t number = 0;
  for (const auto& assertion : mAssertions)
  {
    if (assertion->wasExecuted() && assertion->getNumberOfFailes() == 0)
    {
      number++;
    }
  }
  return number;
}

size_t
Unit::getNumberOfNotExecutedAssertions() const
{
  size_t number = 0;
  for (const auto& assertion : mAssertions)
  {
    if (!assertion->wasExecuted())
    {
      number++;
    }
  }
  return number;
}

// ---------------------------------------------------------------------------
void
Unit::addInvariant(Invariant& invariant)
{
  mInvariants.emplace_back(&invariant);
}

const std::vector<Invariant*>&
Unit::getInvariants() const
{
  return mInvariants;
}

size_t
Unit::getNumberOfFailedInvariants() const
{
  size_t number = 0;
  for (const auto& invariant : mInvariants)
  {
    if (!invariant->hold())
    {
      number++;
    }
  }
  return number;
}

size_t
Unit::getNumberOfNotExecutedInvariants() const
{
  size_t number = 0;
  for (const auto& invariant : mInvariants)
  {
    if (!invariant->wasCreated())
    {
      number++;
    }
  }
  return number;
}

// ---------------------------------------------------------------------------
void
Unit::addCheck(Check& check)
{
  mChecks.emplace_back(&check);
}

const std::vector<Check*>&
Unit::getChecks() const
{
  return mChecks;
}

size_t
Unit::getNumberOfFailedChecks() const
{
  size_t number = 0;
  for (const auto& invariant : mChecks)
  {
    if (invariant->getNumberOfFailes() > 0)
    {
      number++;
    }
  }
  return number;
}

size_t
Unit::getNumberOfPassedChecks() const
{
  size_t number = 0;
  for (const auto& check : mChecks)
  {
    if (check->wasExecuted() && check->getNumberOfFailes() == 0)
    {
      number++;
    }
  }
  return number;
}

size_t
Unit::getNumberOfNotExecutedChecks() const
{
  size_t number = 0;
  for (const auto& check : mChecks)
  {
    if (!check->wasExecuted())
    {
      number++;
    }
  }
  return number;
}

// ---------------------------------------------------------------------------
void
Unit::addExpectCall(ExpectCall& call)
{
  mExpectCalls.push_back(&call);
}

const std::vector<ExpectCall*>&
Unit::getExpectCalls() const
{
  return mExpectCalls;
}

void
Unit::addMockCreation(MockCreation& call)
{
  mMockCreations.push_back(&call);
}

const std::vector<MockCreation*>&
Unit::getMockCreations() const
{
  return mMockCreations;
}

size_t
Unit::getNumberOfOversaturatedFunctionCalls() const
{
  size_t number = 0;
  auto iter = mExpectCalls.begin();
  while (iter != mExpectCalls.end())
  {
    number += (*iter)->getNumberOfUnexpectedCalls();
    ++iter;
  }
  return number;
}

size_t
Unit::getNumberOfUnmetPrerequisties() const
{
  size_t number = 0;
  auto iter = mExpectCalls.begin();
  while (iter != mExpectCalls.end())
  {
    number += (*iter)->getNumberOfUnmetPrerequisites();
    ++iter;
  }
  return number;
}

size_t
Unit::getNumberOfMissingFunctionCalls() const
{
  size_t number = 0;
  auto iter = mExpectCalls.begin();
  while (iter != mExpectCalls.end())
  {
    number += (*iter)->getNumberOfMissingCalls();
    ++iter;
  }
  return number;
}

size_t
Unit::getNumberOfUnexpectedFunctionCalls() const
{
  size_t number = 0;
  auto iter = mMockCreations.begin();
  while (iter != mMockCreations.end())
  {
    number += (*iter)->getNumberOfUnexpectedCalls();
    ++iter;
  }
  return number;
}

size_t
Unit::getNumberOfExecutedExpectCall() const
{
  size_t number = 0;
  auto iter = mExpectCalls.begin();
  while (iter != mExpectCalls.end())
  {
    number += static_cast<size_t>((*iter)->wasExecuted());
    ++iter;
  }
  return number;
}

size_t
Unit::getNumberOfMocks() const
{
  size_t number = 0;
  auto iter = mMockCreations.begin();
  while (iter != mMockCreations.end())
  {
    number += (*iter)->getNumberOfCreations();
    ++iter;
  }
  return number;
}

// ---------------------------------------------------------------------------
const char*
Unit::getFileName() const
{
  return mFileName;
}
