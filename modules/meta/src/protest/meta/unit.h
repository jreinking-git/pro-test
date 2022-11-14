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

#include "protest/utils/static_linked_list.h"
#include "protest/meta/assertion.h"
#include "protest/meta/check.h"
#include "protest/meta/invariant.h"

#include <vector>

#define PROTEST_PATHS(path1, path2, path3) ("project:/" path3)

namespace protest
{

namespace meta
{

// ---------------------------------------------------------------------------
/**
 * @class Unit
 */
class Unit : public StaticLinkedList<Unit>
{
public:
  static Unit* listOfAllUnits;

// ---------------------------------------------------------------------------
  explicit Unit(const char* file);

  Unit(const Unit&) = delete;

  Unit(Unit&&) noexcept = delete;

  Unit&
  operator=(const Unit&) = delete;

  Unit&
  operator=(Unit&&) noexcept = delete;

  ~Unit() = default;

// ---------------------------------------------------------------------------
  void
  addAssertion(Assertion& assertion);

  const std::vector<Assertion*>&
  getAssertions() const;

  size_t
  getNumberOfFailedAssertions() const;

  size_t
  getNumberOfPassedAssertions() const;

  size_t
  getNumberOfNotExecutedAssertions() const;

// ---------------------------------------------------------------------------
  void
  addInvariant(Invariant& invariant);

  const std::vector<Invariant*>&
  getInvariants() const;

  size_t
  getNumberOfFailedInvariants() const;

  size_t
  getNumberOfNotExecutedInvariants() const;

// ---------------------------------------------------------------------------
  void
  addCheck(Check& check);

  const std::vector<Check*>&
  getChecks() const;

  size_t
  getNumberOfFailedChecks() const;

  size_t
  getNumberOfPassedChecks() const;

  size_t
  getNumberOfNotExecutedChecks() const;

// ---------------------------------------------------------------------------
  const char*
  getFileName() const;

private:
  const char* mFileName;
  std::vector<Assertion*> mAssertions;
  std::vector<Invariant*> mInvariants;
  std::vector<Check*> mChecks;
};

} // namespace meta

} // namespace protest
