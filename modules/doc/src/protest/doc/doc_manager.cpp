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

#include "protest/doc/doc_manager.h"
#include "protest/core/context.h"

#include <iostream>
#include <iomanip>

#include <cassert>
#include <ctime>

using namespace protest::doc;

// ---------------------------------------------------------------------------
DocManager::DocManager(core::Context& context) :
  mTestManager(context.getTestManager()),
  mContext(context)
{
}

// ---------------------------------------------------------------------------
void
DocManager::printPreamble()
{
  auto& callContext = mContext.getCallContext();

  auto stream = mLogger.startLog("    ", "    ");
  printSeperator();
  if (callContext.hasComment("@test"))
  {
    std::stringstream strstream;
    strstream << std::left << std::setw(maxLengthOfKeyHeader) << "* @test"
              << callContext.getComment("@test") << "\n";
    stream.operator std::ostream&() << strstream.str();
  }
  if (callContext.hasComment("@author"))
  {
    std::stringstream strstream;
    strstream << std::left << std::setw(maxLengthOfKeyHeader) << "* @author"
              << callContext.getComment("@author") << "\n";
    stream.operator std::ostream&() << strstream.str();
  }
  if (callContext.hasComment("@version"))
  {
    std::stringstream strstream;
    strstream << std::left << std::setw(maxLengthOfKeyHeader) << "* @version"
              << callContext.getComment("@version") << "\n";
    stream.operator std::ostream&() << strstream.str();
  }

  auto nowTime = std::time(nullptr);
  // NOLINTNEXTLINE
  auto currentTime = *std::localtime(&nowTime);
  std::stringstream strstream;
  strstream << std::left << std::setw(maxLengthOfKeyHeader) << "* @date"
            << std::put_time(&currentTime, "%d-%m-%Y %H-%M-%S") << "\n";
  stream.operator std::ostream&() << strstream.str();
  printSeperator();
}

void
DocManager::printPostamble()
{
  size_t numberOfFailedInvariants = 0;
  for (auto* unit : mTestManager.getUnits())
  {
    assert(unit);
    for (meta::Invariant* inv : unit->getInvariants())
    {
      if (!inv->hold())
      {
        numberOfFailedInvariants++;
      }
      else
      {
      }
    }
  }

  if (numberOfFailedInvariants > 0)
  {
    auto stream = mLogger.startLog("    ", "    ");
    printSeperator();
    stream.operator std::ostream&()
        << "* NUMBER OF VIOLATED INVARIANTS: "
        << std::to_string(numberOfFailedInvariants) << "\n";

    for (auto* unit : mTestManager.getUnits())
    {
      assert(unit);
      for (meta::Invariant* inv : unit->getInvariants())
      {
        if (!inv->hold())
        {
          stream.operator std::ostream&()
              << "* " << unit->getFileName() << ":"
              << std::to_string(inv->getLine()) << "\n";
        }
        else
        {
        }
      }
    }
  }
  else
  {
  }

  if (mTestManager.getNumberOfFailedChecks() != 0)
  {
    auto stream = mLogger.startLog("    ", "    ");
    printSeperator();
    stream.operator std::ostream&()
        << "* NUMBER OF WARNINGS: "
        << std::to_string(mTestManager.getNumberOfFailedChecks()) << "\n";

    for (auto* unit : mTestManager.getUnits())
    {
      for (meta::Check* check : unit->getChecks())
      {
        if (check->getNumberOfFailes() != 0)
        {
          stream.operator std::ostream&()
              << "* " << unit->getFileName() << ":"
              << std::to_string(check->getLine()) << "\n";
        }
        else
        {
        }
      }
    }
  }
  else
  {
  }

  if (mTestManager.getNumberOfFailedAssertions() != 0)
  {
    auto stream = mLogger.startLog("    ", "    ");
    printSeperator();
    stream.operator std::ostream&()
        << "* NUMBER OF FAILED ASSERTIONS: "
        << std::to_string(mTestManager.getNumberOfFailedAssertions()) << "\n";

    for (auto* unit : mTestManager.getUnits())
    {
      for (meta::Assertion* assertion : unit->getAssertions())
      {
        if (assertion->getNumberOfFailes() != 0)
        {
          stream.operator std::ostream&()
              << "* " << unit->getFileName() << ":"
              << std::to_string(assertion->getLine()) << "\n";
        }
        else
        {
        }
      }
    }
  }
  else
  {
  }

  if (mTestManager.getNumberOfNotExecutedAssertions() != 0)
  {
    auto stream = mLogger.startLog("    ", "    ");
    printSeperator();
    stream.operator std::ostream&()
        << "* NUMBER OF NOT EXECUTED ASSERTIONS: "
        << std::to_string(mTestManager.getNumberOfNotExecutedAssertions())
        << "\n";

    for (auto* unit : mTestManager.getUnits())
    {
      for (meta::Assertion* assertion : unit->getAssertions())
      {
        if (!assertion->wasExecuted())
        {
          stream.operator std::ostream&()
              << "* " << unit->getFileName() << ":"
              << std::to_string(assertion->getLine()) << "\n";
        }
        else
        {
        }
      }
    }
  }
  else
  {
  }

  if (mTestManager.getNumberOfOversaturatedFunctionCalls() != 0)
  {
    auto stream = mLogger.startLog("    ", "    ");
    printSeperator();
    stream.operator std::ostream&()
        << "* NUMBER OF OVERSATURATED FUNCTION CALLS: "
        << std::to_string(mTestManager.getNumberOfOversaturatedFunctionCalls())
        << "\n";

    for (auto* unit : mTestManager.getUnits())
    {
      for (meta::ExpectCall* assertion : unit->getExpectCalls())
      {
        if (assertion->getNumberOfUnexpectedCalls() > 0)
        {
          stream.operator std::ostream&()
              << "* " << unit->getFileName() << ":"
              << std::to_string(assertion->getLine()) << "\n";
        }
        else
        {
        }
      }
    }
  }
  else
  {
  }

  if (mTestManager.getNumberOfMissingFunctionCalls() != 0)
  {
    auto stream = mLogger.startLog("    ", "    ");
    printSeperator();
    stream.operator std::ostream&()
        << "* NUMBER OF MISSING FUNCTION CALLS: "
        << std::to_string(mTestManager.getNumberOfMissingFunctionCalls())
        << "\n";

    for (auto* unit : mTestManager.getUnits())
    {
      for (meta::ExpectCall* assertion : unit->getExpectCalls())
      {
        if (assertion->getNumberOfMissingCalls() > 0)
        {
          stream.operator std::ostream&()
              << "* " << unit->getFileName() << ":"
              << std::to_string(assertion->getLine()) << "\n";
        }
        else
        {
        }
      }
    }
  }
  else
  {
  }

  if (mTestManager.getNumberOfUnexpectedFunctionCalls() != 0)
  {
    auto stream = mLogger.startLog("    ", "    ");
    printSeperator();
    stream.operator std::ostream&()
        << "* NUMBER OF UNEXPECTED FUNCTION CALLS: "
        << std::to_string(mTestManager.getNumberOfUnexpectedFunctionCalls())
        << "\n";

    for (auto* unit : mTestManager.getUnits())
    {
      for (meta::ExpectCall* assertion : unit->getExpectCalls())
      {
        if (assertion->getNumberOfUnexpectedCalls() > 0)
        {
          stream.operator std::ostream&()
              << "* " << unit->getFileName() << ":"
              << std::to_string(assertion->getLine()) << "\n";
        }
        else
        {
        }
      }
    }
  }
  else
  {
  }

  if (mTestManager.getNumberOfUnmetPrerequisties() != 0)
  {
    auto stream = mLogger.startLog("    ", "    ");
    printSeperator();
    stream.operator std::ostream&()
        << "* NUMBER OF MISSING PREREQUISITES: "
        << std::to_string(mTestManager.getNumberOfUnmetPrerequisties()) << "\n";

    for (auto* unit : mTestManager.getUnits())
    {
      for (meta::ExpectCall* assertion : unit->getExpectCalls())
      {
        if (assertion->getNumberOfUnmetPrerequisites() > 0)
        {
          stream.operator std::ostream&()
              << "* " << unit->getFileName() << ":"
              << std::to_string(assertion->getLine()) << "\n";
        }
        else
        {
        }
      }
    }
  }
  else
  {
  }

  const bool notTested = (mTestManager.getNumberOfPassedAssertions() == 0 &&
                          mTestManager.getNumberOfFailedAssertions() == 0) &&
                         (mTestManager.getNumberOfPassedInvariants() == 0 &&
                          mTestManager.getNumberOfFailedInvariants() == 0) &&
                         mTestManager.getNumberOfExecutedExpectCalls() == 0;

  const bool totalVerdict =
      (mTestManager.getNumberOfFailedAssertions() == 0 &&
       mTestManager.getNumberOfFailedInvariants() == 0 &&
       mTestManager.getNumberOfUnexpectedFunctionCalls() == 0 &&
       mTestManager.getNumberOfMissingFunctionCalls() == 0 &&
       mTestManager.getNumberOfUnmetPrerequisties() == 0 &&
       mTestManager.getNumberOfOversaturatedFunctionCalls() == 0);

  auto stream = mLogger.startLog("    ", "    ");
  printSeperator();
  // TODO (jreinking) whould be nice to have a 'not tested' in the case that there
  // was no executed assertion.
  stream.operator std::ostream&() << "* FAILURES:       ";
  std::stringstream tmp0;
  tmp0 << std::setw(maxLengthOfKey);
  tmp0 << mTestManager.getNumberOfFailedAssertions();
  stream.operator std::ostream&()
      << tmp0.str() << "                                               "
      << (mTestManager.getNumberOfFailedAssertions() != 0 ? "FAIL" : "PASS")
      << "\n"
      << "* WARNINGS:       ";
  std::stringstream tmp1;
  tmp1 << std::setw(maxLengthOfKey);
  tmp1 << mTestManager.getNumberOfFailedChecks();
  stream.operator std::ostream&()
      << tmp1.str() << "                                               "
      << (mTestManager.getNumberOfFailedChecks() != 0 ? "WARN" : "PASS ")
      << "\n"
      << "* INV VIOLATION:  ";
  std::stringstream tmp2;
  tmp2 << std::setw(maxLengthOfKey);
  tmp2 << mTestManager.getNumberOfFailedInvariants();
  stream.operator std::ostream&()
      << tmp2.str() << "                                         "
      << (mTestManager.getNumberOfFailedInvariants() != 0 ? "      FAIL"
                                                          : "      PASS ");
  if (mTestManager.getNumberOfMocks() > 0)
  {
    stream.operator std::ostream&() << "\n"
                                    << "* MOCK VIOLATION: ";
    std::stringstream tmp3;
    tmp3 << std::setw(maxLengthOfKey);
    tmp3 << mTestManager.getNumberOfMockFailures();
    stream.operator std::ostream&()
        << tmp3.str() << "                                         "
        << (mTestManager.getNumberOfMockFailures() != 0 ? "      FAIL"
                                                        : "      PASS ");
  }

  stream.operator std::ostream&()
      << "\n"
      << "*"
      << "\n"
      << "* TOTAL:                                      "
         "                       "
      << (notTested ? &"NOT TESTED"[0]
                    : (totalVerdict ? "      PASS" : "      FAIL"))
      << "\n";
  printSeperator();
}

void
DocManager::printSeperator()
{
  mLogger.getStream().getPlain() << std::string(lengthOfSeperator, '*') << "\n";
  mLogger.flush();
}
