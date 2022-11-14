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

#include "protest/meta/test_manager.h"

using namespace protest::meta;

// ---------------------------------------------------------------------------
const Unit&
TestManager::getCurrentUnit(protest::meta::CallContext& context)
{
  return context.getUnit();
}

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
  size_t number = 0;
  auto iter = mUnits.begin();
  while (iter != mUnits.end())
  {
    number += (*iter)->getNumberOfFailedAssertions();
    ++iter;
  }
  return number;
}

size_t
TestManager::getNumberOfPassedAssertions() const
{
  size_t number = 0;
  auto iter = mUnits.begin();
  while (iter != mUnits.end())
  {
    number += (*iter)->getNumberOfPassedAssertions();
    ++iter;
  }
  return number;
}

size_t
TestManager::getNumberOfNotExecutedAssertions() const
{
  size_t number = 0;
  auto iter = mUnits.begin();
  while (iter != mUnits.end())
  {
    number += (*iter)->getNumberOfNotExecutedAssertions();
    ++iter;
  }
  return number;
}

// ---------------------------------------------------------------------------
size_t
TestManager::getNumberOfFailedChecks() const
{
  size_t number = 0;
  auto iter = mUnits.begin();
  while (iter != mUnits.end())
  {
    number += (*iter)->getNumberOfFailedChecks();
    ++iter;
  }
  return number;
}

size_t
TestManager::getNumberOfPassedChecks() const
{
  size_t number = 0;
  auto iter = mUnits.begin();
  while (iter != mUnits.end())
  {
    number += (*iter)->getNumberOfPassedChecks();
    ++iter;
  }
  return number;
}

size_t
TestManager::getNumberOfNotExecutedChecks() const
{
  size_t number = 0;
  auto iter = mUnits.begin();
  while (iter != mUnits.end())
  {
    number += (*iter)->getNumberOfNotExecutedChecks();
    ++iter;
  }
  return number;
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
  size_t number = 0;
  auto iter = mUnits.begin();
  while (iter != mUnits.end())
  {
    number += (*iter)->getNumberOfFailedInvariants();
    ++iter;
  }
  return number;
}

size_t
TestManager::getNumberOfNotExecutedInvariants() const
{
  size_t number = 0;
  auto iter = mUnits.begin();
  while (iter != mUnits.end())
  {
    number += (*iter)->getNumberOfNotExecutedInvariants();
    ++iter;
  }
  return number;
}

// ---------------------------------------------------------------------------
std::vector<Unit*>&
TestManager::getUnits()
{
  return mUnits;
}
