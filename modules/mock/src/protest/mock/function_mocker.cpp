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

#include "protest/mock/function_mocker.h"
#include "protest/mock/mock_base.h"
#include "protest/mock/expectation.h"
#include "protest/mock/sequence.h"
#include "protest/mock/in_sequence.h"

using namespace protest::mock;
using namespace protest::core;
using namespace protest::mock::internal;
using namespace protest::log;

// ---------------------------------------------------------------------------
bool
FunctionMockerRaw::isSaturated(ExpectationBase* expectation)
{
  return expectation->isSaturated();
}

// ---------------------------------------------------------------------------
FunctionMockerRaw::FunctionMockerRaw(MockBase& mock,
                                     const char* name,
                                     const char* const* paramTypes,
                                     const char* const* params,
                                     size_t numParams) :
  mMock(mock),
  mName(name),
  mParamTypes(paramTypes),
  mParams(params),
  mNumberOfParameter(numParams)
{
}

// ---------------------------------------------------------------------------
protest::meta::MockCreation&
FunctionMockerRaw::getCallContext()
{
  return mMock.getCallContext();
}

void
FunctionMockerRaw::addExpectation(
    std::shared_ptr<internal::ExpectationBase>& expectation)
{
  assert(expectation.get() != nullptr);
  mExpectations.push_back(expectation);

  auto* context = protest::core::Context::getCurrentContext();
  auto* runner = context->getCurrentVirtual();
  auto* userdata = runner->getUserdataAs<ImplicitSequence>();

  if (userdata != nullptr)
  {
    // in sequence -> add last to prerequsites
    userdata->addExpectation(expectation);
  }
  else
  {
  }
}

void
FunctionMockerRaw::checkMissingCalls()
{
  for (auto& expectation : mExpectations)
  {
    assert(expectation != nullptr);
    expectation->explain();
  }
}

void
FunctionMockerRaw::reportUnexpectedCall()
{
  mMock.getCallContext().incrementNumberOfUnexpectedCalls();
}

// ---------------------------------------------------------------------------
const char*
FunctionMockerRaw::getName() const
{
  return mName;
}

size_t
FunctionMockerRaw::getNumberOfParams() const
{
  return mNumberOfParameter;
}

const char*
FunctionMockerRaw::getParamType(size_t index)
{
  return mParamTypes[index];
}

const char*
FunctionMockerRaw::getParam(size_t index)
{
  return mParams[index];
}

void
FunctionMockerRaw::enterPassiveMode()
{
  // when the mock get destroyed the function must go into the passive mode.
  // This means that it cannot be used to created new expectation, but it
  // might be refered by already existing expectation. The own expectation of
  // this function will be removed, so that no cyclic dependencies exits due
  // shared_ptr.
  mExpectations.clear();
}

// ---------------------------------------------------------------------------
void
FunctionMockerRaw::printFunction(std::ostream& ostream)
{
  ostream << getName() << "(";
  bool notFirst = false;
  for (int i = 0; i < getNumberOfParams(); i++)
  {
    if (notFirst)
    {
      ostream << ", ";
    }
    notFirst = true;
    ostream << getParamType(i) << " " << getParam(i);
  }
  ostream << ")";
}

void
FunctionMockerRaw::printFunctionWithExpectation(
    std::ostream& ostream,
    ExpectationBase* expectation) const
{
  ostream << getName() << "(";
  bool notFirst = false;
  for (int i = 0; i < getNumberOfParams(); i++)
  {
    if (notFirst)
    {
      ostream << ", ";
    }
    notFirst = true;
    ostream << expectation->getCallContext().getArg(i);
  }
  ostream << ")";
}

void
FunctionMockerRaw::printCallToMockFunction(ExpectationBase* expectation)
{
  auto* runner = Context::getCurrentContext()->getCurrentVirtual();
  auto& logger = runner->getLogger();
  auto& stream = logger.getStream().getPlain();

  logger.startLog("MOCK",
                  runner->getName(),
                  expectation->getCallContext().getUnit().getFileName(),
                  expectation->getCallContext().getLine(),
                  runner->now());

  stream << "Call to mock function '";
  printFunction(stream);
  stream << "'";

  if (getNumberOfParams() > 0)
  {
    stream << " with:";
  }

  stream << "\n";
}

void
FunctionMockerRaw::printUnexpectedFunctionCall(ExpectationBase* expectation)
{
  auto* runner = Context::getCurrentContext()->getCurrentVirtual();
  auto& logger = runner->getLogger();
  auto& stream = logger.getStream().getPlain();

  logger.startLog("FAIL",
                  runner->getName(),
                  expectation->getCallContext().getUnit().getFileName(),
                  expectation->getCallContext().getLine(),
                  runner->now());

  stream << "Unexpected function call for '";
  printFunctionWithExpectation(stream, expectation);
  stream << "'\n";
  if (expectation->isSaturated())
  {
    stream << "Expected: ";
    expectation->getCardinality().explain(stream);
    stream << "\n";
  }
  else
  {
  }
}

void
FunctionMockerRaw::printUnexpectedFunctionCall()
{
  auto* runner = Context::getCurrentContext()->getCurrentVirtual();
  auto& logger = runner->getLogger();
  auto& stream = logger.getStream().getPlain();

  logger.startLog("FAIL",
                  runner->getName(),
                  getCallContext().getUnit().getFileName(),
                  getCallContext().getLine(),
                  runner->now());

  stream << "Unexpected call to mock function '";
  printFunction(stream);
  stream << "'";

  if (getNumberOfParams() > 0)
  {
    logger.getStream().getPlain() << " with:";
  }
  stream << "\n";
}

void
FunctionMockerRaw::printUnmetPrerequisite(ExpectationBase* expectation)
{
  auto* runner = Context::getCurrentContext()->getCurrentVirtual();
  auto& logger = runner->getLogger();
  auto& stream = logger.getStream().getPlain();

  logger.startLog("FAIL",
                  runner->getName(),
                  expectation->getCallContext().getUnit().getFileName(),
                  expectation->getCallContext().getLine(),
                  runner->now());

  stream << "Unmet prerequisite '";
  printFunctionWithExpectation(stream, expectation);
  stream << "'\n";
  stream << "Expected: ";
  expectation->getCardinality().explain(stream);
  stream << "\n";
}

void
FunctionMockerRaw::printWhenClause(ExpectationBase* expectation)
{
  auto* runner = Context::getCurrentContext()->getCurrentVirtual();
  auto& logger = runner->getLogger();
  auto& stream = logger.getStream().getPlain();

  if (expectation->hasWhenClause())
  {
    stream << "    when: '" << expectation->getWhenConditionAsString()
           << "' not fulfilled!" << std::endl;
  }
}
