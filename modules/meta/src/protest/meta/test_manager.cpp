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

#include "protest/meta/test_manager.h"

using namespace protest::meta;

// ---------------------------------------------------------------------------
void
TestManager::initialize()
{
  auto* current = Unit::listOfAllUnits;
  while (current != nullptr)
  {
    mUnits.emplace_back(current);
    current = current->next();
  }
}

// ---------------------------------------------------------------------------
size_t
TestManager::getNumberOfFailedAssertions() const
{
  return sum(&Unit::getNumberOfFailedAssertions);
}

size_t
TestManager::getNumberOfPassedAssertions() const
{
  return sum(&Unit::getNumberOfPassedAssertions);
}

size_t
TestManager::getNumberOfNotExecutedAssertions() const
{
  return sum(&Unit::getNumberOfNotExecutedAssertions);
}

// ---------------------------------------------------------------------------
size_t
TestManager::getNumberOfFailedChecks() const
{
  return sum(&Unit::getNumberOfFailedChecks);
}

size_t
TestManager::getNumberOfPassedChecks() const
{
  return sum(&Unit::getNumberOfPassedChecks);
}

size_t
TestManager::getNumberOfNotExecutedChecks() const
{
  return sum(&Unit::getNumberOfNotExecutedChecks);
}

// ---------------------------------------------------------------------------
size_t
TestManager::getNumberOfPassedInvariants() const
{
  size_t number = 0;
  auto iter = mUnits.begin();
  while (iter != mUnits.end())
  {
    number += (*iter)->getInvariants().size();
    number -= (*iter)->getNumberOfNotExecutedInvariants();
    number -= (*iter)->getNumberOfFailedInvariants();
    ++iter;
  }
  return number;
}

size_t
TestManager::getNumberOfFailedInvariants() const
{
  return sum(&Unit::getNumberOfFailedInvariants);
}

size_t
TestManager::getNumberOfNotExecutedInvariants() const
{
  return sum(&Unit::getNumberOfNotExecutedInvariants);
}

// ---------------------------------------------------------------------------
size_t
TestManager::getNumberOfOversaturatedFunctionCalls() const
{
  return sum(&Unit::getNumberOfOversaturatedFunctionCalls);
}

size_t
TestManager::getNumberOfUnmetPrerequisties() const
{
  return sum(&Unit::getNumberOfUnmetPrerequisties);
}

size_t
TestManager::getNumberOfMissingFunctionCalls() const
{
  return sum(&Unit::getNumberOfMissingFunctionCalls);
}

size_t
TestManager::getNumberOfUnexpectedFunctionCalls() const
{
  return sum(&Unit::getNumberOfUnexpectedFunctionCalls);
}

size_t
TestManager::getNumberOfExecutedExpectCalls() const
{
  return sum(&Unit::getNumberOfExecutedExpectCall);
}

size_t
TestManager::getNumberOfMockFailures() const
{
  return getNumberOfOversaturatedFunctionCalls() +
         getNumberOfMissingFunctionCalls() +
         getNumberOfUnexpectedFunctionCalls() + getNumberOfUnmetPrerequisties();
}

size_t
TestManager::getNumberOfMocks() const
{
  return sum(&Unit::getNumberOfMocks);
}

// ---------------------------------------------------------------------------
size_t
TestManager::sum(size_t (Unit::*func)() const) const
{
  size_t number = 0;
  auto iter = mUnits.begin();
  while (iter != mUnits.end())
  {
    Unit* unit = *iter;
    number += (unit->*func)();
    ++iter;
  }
  return number;
}

std::vector<Unit*>&
TestManager::getUnits()
{
  return mUnits;
}
