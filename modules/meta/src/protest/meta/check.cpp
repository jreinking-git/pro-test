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

#include "protest/meta/check.h"
#include "protest/meta/unit.h"

using namespace protest::meta;

// ---------------------------------------------------------------------------
protest::meta::Check&
protest::meta::Check::defaultContext()
{
  static protest::meta::Unit unit("");
  static protest::meta::Check ctx(unit, 0, "", {}, {});
  return ctx;
}

// ---------------------------------------------------------------------------
Check::Check(Unit& unit,
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
  mUnit.addCheck(*this);
}

// ---------------------------------------------------------------------------
bool
Check::wasExecuted() const
{
  return mExecuted;
}

void
Check::markAsExecuted()
{
  mExecuted = true;
}

// ---------------------------------------------------------------------------
uint32_t
Check::getNumberOfFailes() const
{
  return mNumberOfFailes;
}

void
Check::incrementNumberOfFailes()
{
  mNumberOfFailes++;
}

// ---------------------------------------------------------------------------
const Unit&
Check::getUnit()
{
  return mUnit;
}

size_t
Check::getLine() const
{
  return mLine;
}

const char*
Check::getCondition() const
{
  return mArgs.at(0).c_str();
}
