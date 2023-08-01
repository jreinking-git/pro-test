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

#include "protest/meta/call_context.h"
#include "protest/meta/unit.h"

#include <cassert>

using namespace protest::meta;

// ---------------------------------------------------------------------------
protest::meta::CallContext&
protest::meta::CallContext::defaultContext()
{
  static protest::meta::Unit unit("");
  static protest::meta::CallContext ctx(unit, 0, "", {}, {});
  return ctx;
}

// ---------------------------------------------------------------------------
CallContext::CallContext(Unit& unit,
                         size_t line,
                         const char* objectName,
                         std::vector<std::string>&& args,
                         std::map<std::string, std::string>&& comments) :
  mUnit(unit),
  mLine(line),
  mObjectName(objectName),
  mArgs(std::move(args)),
  mComments(std::move(comments))
{
}

// ---------------------------------------------------------------------------
Unit&
CallContext::getUnit()
{
  return mUnit;
}

size_t
CallContext::getLine() const
{
  return mLine;
}

const char*
CallContext::getArg(uint8_t arg)
{
  return mArgs.at(arg).c_str();
}

const char*
CallContext::getObjectName()
{
  return mObjectName;
}

bool
CallContext::hasComment(const char* comment)
{
  return mComments.count(comment) > 0;
}

const char*
CallContext::getComment(const char* comment)
{
  return mComments[comment].c_str();
}

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
                     const char* objectName,
                     std::vector<std::string>&& args,
                     std::map<std::string, std::string>&& comments) :
  CallContext(unit, line, objectName, std::move(args), std::move(comments)),
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
const char*
Assertion::getCondition()
{
  return CallContext::getArg(0);
}

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
             const char* objectName,
             std::vector<std::string>&& args,
             std::map<std::string, std::string>&& comments) :
  CallContext(unit, line, objectName, std::move(args), std::move(comments)),
  mNumberOfFailes(0),
  mExecuted(false)
{
  // TODO (jreinking) mUnit might not be initialized yet
  getUnit().addCheck(*this);
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
const char*
Check::getCondition()
{
  return CallContext::getArg(0);
}

// ---------------------------------------------------------------------------
ExpectCall&
ExpectCall::defaultContext()
{
  static protest::meta::Unit unit("");
  static protest::meta::ExpectCall ctx(unit, 0, "", {}, {});
  return ctx;
}

// ---------------------------------------------------------------------------
ExpectCall::ExpectCall(Unit& unit,
                       size_t line,
                       const char* objectName,
                       std::vector<std::string>&& args,
                       std::map<std::string, std::string>&& comments) :
  CallContext(unit, line, objectName, std::move(args), std::move(comments)),
  mNumberOfUnexpectedCalls(0),
  mNumberOfUnmetPrerequisites(0),
  mNumberOfMissingCalls(0),
  mWasExecuted(false)
{
  unit.addExpectCall(*this);
}

// ---------------------------------------------------------------------------
bool
ExpectCall::wasExecuted() const
{
  return mWasExecuted;
}

void
ExpectCall::markAsExecuted()
{
  mWasExecuted = true;
}

// ---------------------------------------------------------------------------
size_t
ExpectCall::getNumberOfUnexpectedCalls() const
{
  return mNumberOfUnexpectedCalls;
}

void
ExpectCall::incrementNumberOfUnexpectedCalls()
{
  mNumberOfUnexpectedCalls++;
}

size_t
ExpectCall::getNumberOfUnmetPrerequisites() const
{
  return mNumberOfUnmetPrerequisites;
}

void
ExpectCall::incrementNumberOfUnmetPrerequisites()
{
  mNumberOfUnmetPrerequisites++;
}

size_t
ExpectCall::getNumberOfMissingCalls() const
{
  return mNumberOfMissingCalls;
}

void
ExpectCall::incrementNumberOfMissingCalls()
{
  mNumberOfMissingCalls++;
}

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

// ---------------------------------------------------------------------------
MockCreation&
MockCreation::defaultContext()
{
  static protest::meta::Unit unit("");
  static protest::meta::MockCreation ctx(unit, 0, "", {}, {});
  return ctx;
}

// ---------------------------------------------------------------------------
MockCreation::MockCreation(Unit& unit,
                           size_t line,
                           const char* objectName,
                           std::vector<std::string>&& args,
                           std::map<std::string, std::string>&& comments) :
  CallContext(unit, line, objectName, std::move(args), std::move(comments)),
  mNumberOfUnexpectedCalls(0),
  mNumberOfCreations(0)
{
  // TODO (jreinking) mUnit might not be initialized yet
  getUnit().addMockCreation(*this);
}

// ---------------------------------------------------------------------------
size_t
MockCreation::getNumberOfCreations() const
{
  return mNumberOfCreations;
}

void
MockCreation::incrementNumberOfCreations()
{
  mNumberOfCreations++;
}

// ---------------------------------------------------------------------------
size_t
MockCreation::getNumberOfUnexpectedCalls() const
{
  return mNumberOfUnexpectedCalls;
}

void
MockCreation::incrementNumberOfUnexpectedCalls()
{
  mNumberOfUnexpectedCalls++;
}

// ---------------------------------------------------------------------------
Signal&
Signal::defaultContext()
{
  static protest::meta::Unit unit("");
  static protest::meta::Signal ctx(unit, 0, "", {}, {});
  return ctx;
}

// ---------------------------------------------------------------------------
Signal::Signal(Unit& unit,
               size_t line,
               const char* objectName,
               std::vector<std::string>&& args,
               std::map<std::string, std::string>&& comments) :
  CallContext(unit, line, objectName, std::move(args), std::move(comments))
{
}
