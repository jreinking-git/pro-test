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

#include "protest/mock/in_sequence.h"
#include "protest/mock/expectation.h"

using namespace protest::mock;

// ---------------------------------------------------------------------------
ImplicitSequence::ImplicitSequence() :
  protest::core::RunnerRaw::Userdata(id),
  mLast(nullptr)
{
}

// ---------------------------------------------------------------------------
void
ImplicitSequence::addExpectation(
    std::shared_ptr<internal::ExpectationBase>& expectation)
{
  if (mLast != nullptr)
  {
    expectation->addPrerequisites(mLast);
  }
  else
  {
  }
  mLast = expectation;
}

// ---------------------------------------------------------------------------
InSequence::InSequence()
{
  auto* context = protest::core::Context::getCurrentContext();
  auto* runner = context->getCurrentVirtual();

  if (runner->getUserdataAs<ImplicitSequence>() == nullptr)
  {
    auto* sequenceList = &mImplicitSequence;
    runner->setUserdata(sequenceList);
  }
  else
  {
  }
}

InSequence::~InSequence()
{
  auto* context = protest::core::Context::getCurrentContext();
  auto* runner = context->getCurrentVirtual();
  auto* userdata = runner->getUserdataAs<ImplicitSequence>();

  // there must be at least one since this is a InSequence
  assert(userdata);

  if (&mImplicitSequence == userdata)
  {
    runner->setUserdata<ImplicitSequence>(nullptr);
  }
  else
  {
  }
}
