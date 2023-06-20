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

#include "protest/mock/expectation.h"

using namespace protest::mock;
using namespace protest::mock::internal;
using namespace protest::log;

// ---------------------------------------------------------------------------
ExpectationBase::ExpectationBase(std::shared_ptr<FunctionMockerRaw> function,
                                 meta::ExpectCall& callContext) :
  mCardinalitySpecified(false),
  mRetiresOnSaturationCalled(false),
  mWillRepeatedlyCalled(false),
  mCallCounter(0),
  mCardinality(exactly(1)),
  mCallContext(callContext),
  mFunction(std::move(function)),
  mCondition(nullptr),
  mConditionContext(nullptr)
{
}

ExpectationBase::~ExpectationBase()
{
  delete mCondition;
  mCondition = nullptr;
}

// ---------------------------------------------------------------------------
void
ExpectationBase::after(std::shared_ptr<ExpectationBase>& expectation)
{
  return addPrerequisites(expectation);
}

void
ExpectationBase::addPrerequisites(
    std::shared_ptr<internal::ExpectationBase>& expectation)
{
  assert(expectation.get() != nullptr);
  mPrerequisites.insert(expectation);
}

bool
ExpectationBase::prerequisitesMet()
{
  bool all = true;
  auto iter = mPrerequisites.begin();
  while (iter != mPrerequisites.end())
  {
    if (!iter->get()->isSatisfied())
    {
      all = false;
    }
    ++iter;
  }
  return all;
}

void
ExpectationBase::retireAllPrerequisites(bool first)
{
  if (!first)
  {
    mRetiresOnSaturationCalled = true;
  }

  auto iter = mPrerequisites.begin();
  while (iter != mPrerequisites.end())
  {
    iter->get()->retireAllPrerequisites(false);
    ++iter;
  }
}

// ---------------------------------------------------------------------------
void
ExpectationBase::explain()
{
  auto* context = protest::core::Context::getCurrentContext();
  auto* runner = context->getCurrentVirtual();
  auto& logger = runner->getLogger();
  auto& stream = logger.getStream().getPlain();

  if (!mCardinality.isSatisfiedByCallCount(mCallCounter))
  {
    getCallContext().incrementNumberOfMissingCalls();
    logger.startLog("FAIL",
                    runner->getName(),
                    mCallContext.getUnit().getFileName(),
                    mCallContext.getLine(),
                    runner->now());
    stream << "Missing function call for '" << mFunction->getName() << "(";
    bool notFirst = false;
    for (int i = 0; i < mFunction->getNumberOfParams(); i++)
    {
      if (notFirst)
      {
        stream << ", ";
      }
      notFirst = true;
      stream << mCallContext.getArg(i);
    }
    stream << ")'\n";
    stream << "Expected: ";
    mCardinality.explain(stream);
    stream << "\n";
    stream << "     But: ";
    mCardinality.explain(stream, mCallCounter);
    stream << "\n";
    logger.flush();
  }
}

protest::meta::ExpectCall&
ExpectationBase::getCallContext()
{
  return mCallContext;
}

protest::mock::internal::Cardinality&
ExpectationBase::getCardinality()
{
  return mCardinality;
}

bool
ExpectationBase::isSaturated()
{
  return mCardinality.isSaturatedByCallCount(mCallCounter);
}

bool
ExpectationBase::isSatisfied()
{
  return mCardinality.isSatisfiedByCallCount(mCallCounter);
}

bool
ExpectationBase::retiresOnSaturation() const
{
  return mRetiresOnSaturationCalled;
}

void
ExpectationBase::incrementCallCounter()
{
  mCallCounter++;
}

// ---------------------------------------------------------------------------
bool
ExpectationBase::isWhenConditionFulfilled()
{
  return (!hasWhenClause()) || mCondition->isFulfilled();
}

bool
ExpectationBase::hasWhenClause()
{
  return mCondition != nullptr;
}

const char*
ExpectationBase::getWhenConditionAsString()
{
  assert(mConditionContext);
  return mConditionContext->getArg(0);
}