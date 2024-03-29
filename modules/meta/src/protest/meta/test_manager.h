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

#include "protest/meta/unit.h"

#include <vector>

namespace protest
{

namespace meta
{

// ---------------------------------------------------------------------------
/**
 * @class TestManager
 */
class TestManager
{
public:
  explicit TestManager() = default;

  TestManager(const TestManager&) = delete;

  TestManager(TestManager&&) noexcept = delete;

  TestManager&
  operator=(const TestManager&) = delete;

  TestManager&
  operator=(TestManager&&) noexcept = delete;

  ~TestManager() = default;

// ---------------------------------------------------------------------------
  void
  initialize();

// ---------------------------------------------------------------------------
  size_t
  getNumberOfFailedAssertions() const;

  size_t
  getNumberOfPassedAssertions() const;

  size_t
  getNumberOfNotExecutedAssertions() const;

// ---------------------------------------------------------------------------
  size_t
  getNumberOfFailedChecks() const;

  size_t
  getNumberOfPassedChecks() const;

  size_t
  getNumberOfNotExecutedChecks() const;

// ---------------------------------------------------------------------------
  size_t
  getNumberOfPassedInvariants() const;

  size_t
  getNumberOfFailedInvariants() const;

  size_t
  getNumberOfNotExecutedInvariants() const;

// ---------------------------------------------------------------------------
  size_t
  getNumberOfOversaturatedFunctionCalls() const;

  size_t
  getNumberOfUnmetPrerequisties() const;

  size_t
  getNumberOfMissingFunctionCalls() const;

  size_t
  getNumberOfUnexpectedFunctionCalls() const;

  size_t
  getNumberOfExecutedExpectCalls() const;

  size_t
  getNumberOfMockFailures() const;

  size_t
  getNumberOfMocks() const;

// ---------------------------------------------------------------------------
  std::vector<Unit*>&
  getUnits();

private:
  size_t
  sum(size_t (Unit::*func)() const) const;

  std::vector<Unit*> mUnits;
};

} // namespace meta

} // namespace protest
