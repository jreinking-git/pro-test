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

#include "protest/meta/invariant.h"
#include "protest/meta/unit.h"

using namespace protest::meta;

// ---------------------------------------------------------------------------
Invariant&
Invariant::defaultContext()
{
  static protest::meta::Unit unit("");
  static protest::meta::Invariant ctx(unit, 0, "", {}, {});
  return ctx;
}

// ---------------------------------------------------------------------------
Invariant::Invariant(Unit& unit,
                     size_t line,
                     const char* condition,
                     std::vector<std::string>&& args,
                     std::map<std::string, std::string>&& /* comments */) :
  CallContext(unit, line, condition, std::move(args)),
  mWasCreated(false),
  mHold(true)
{
  // TODO (jreinking) might not be initialized when allocated statically
  unit.addInvariant(*this);
}

// ---------------------------------------------------------------------------
bool
Invariant::wasCreated() const
{
  return mWasCreated;
}

void
Invariant::markAsCreated()
{
  mWasCreated = true;
}

bool
Invariant::hold() const
{
  return mHold;
}

void
Invariant::markAsNotHold()
{
  mHold = false;
}
