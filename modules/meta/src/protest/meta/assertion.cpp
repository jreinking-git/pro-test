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

#include "protest/meta/assertion.h"
#include "protest/meta/unit.h"

using namespace protest::meta;

// ---------------------------------------------------------------------------
protest::meta::Assertion&
protest::meta::Assertion::defaultContext()
{
  static protest::meta::Unit unit("");
  static protest::meta::Assertion ctx(unit, 0, "", {}, {});
  return ctx;
}

// ---------------------------------------------------------------------------
Assertion::Assertion(Unit& unit,
                     size_t line,
                     const char* /* objectName */,
                     std::vector<std::string>&& args,
                     std::map<std::string, std::string>&& /* comments */) :
  mUnit(unit),
  mLine(line),
  mArgs(std::move(args)),
  mNumberOfFailes(0),
  mExecuted(false)
{
  // TODO (jreinking) mUnit might not be initialized yet
  unit.addAssertion(*this);
}

// ---------------------------------------------------------------------------
bool
Assertion::wasExecuted() const
{
  return mExecuted;
}

void
Assertion::markAsExecuted()
{
  mExecuted = true;
}

// ---------------------------------------------------------------------------
uint32_t
Assertion::getNumberOfFailes() const
{
  return mNumberOfFailes;
}

void
Assertion::incrementNumberOfFailes()
{
  mNumberOfFailes++;
}

// ---------------------------------------------------------------------------
const Unit&
Assertion::getUnit() const
{
  return mUnit;
}

size_t
Assertion::getLine() const
{
  return mLine;
}

const char*
Assertion::getCondition() const
{
  return mArgs.at(0).c_str();
}
