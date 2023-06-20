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

#include <map>
#include <vector>
#include <string>

#include <cstdint>
#include <cstddef>

namespace protest
{

namespace meta
{

class Unit;

// ---------------------------------------------------------------------------
/**
 * @class CallContext
 */
class CallContext
{
public:
  static CallContext&
  defaultContext();

// ---------------------------------------------------------------------------
  explicit CallContext(Unit& unit,
                       size_t line,
                       const char* objectName,
                       std::vector<std::string>&& args,
                       std::map<std::string, std::string>&& comments = {});

  CallContext(const CallContext&) = delete;

  CallContext(CallContext&&) noexcept = delete;

  CallContext&
  operator=(const CallContext&) = delete;

  CallContext&
  operator=(CallContext&&) noexcept = delete;

  ~CallContext() = default;

// ---------------------------------------------------------------------------
  Unit&
  getUnit();

  size_t
  getLine() const;

  const char*
  getArg(uint8_t arg);

  const char*
  getObjectName();

  bool
  hasComment(const char* comment);

  const char*
  getComment(const char* comment);

private:
  Unit& mUnit;
  size_t mLine;
  const char* mObjectName;
  std::vector<std::string> mArgs;
  std::map<std::string, std::string> mComments;
};

// ---------------------------------------------------------------------------
/**
 * @class Assertion
 */
class Assertion : public CallContext
{
public:
  static Assertion&
  defaultContext();

// ---------------------------------------------------------------------------
  explicit Assertion(Unit& unit,
                     size_t line,
                     const char* objectName,
                     std::vector<std::string>&& args,
                     std::map<std::string, std::string>&& comments);

  Assertion(const Assertion&) = delete;

  Assertion(Assertion&&) noexcept = delete;

  Assertion&
  operator=(const Assertion&) = delete;

  Assertion&
  operator=(Assertion&&) noexcept = delete;

  ~Assertion() = default;

// ---------------------------------------------------------------------------
  bool
  wasExecuted() const;

  void
  markAsExecuted();

// ---------------------------------------------------------------------------
  uint32_t
  getNumberOfFailes() const;

  void
  incrementNumberOfFailes();

// ---------------------------------------------------------------------------
  const char*
  getCondition();

private:
  uint32_t mNumberOfFailes;
  bool mExecuted;
};

// ---------------------------------------------------------------------------
/**
 * @class Check
 */
class Check : public CallContext
{
public:
  static Check&
  defaultContext();

// ---------------------------------------------------------------------------
  explicit Check(Unit& unit,
                 size_t line,
                 const char* objectName,
                 std::vector<std::string>&& args,
                 std::map<std::string, std::string>&& comments);

  Check(const Check&) = delete;

  Check(Check&&) noexcept = delete;

  Check&
  operator=(const Check&) = delete;

  Check&
  operator=(Check&&) noexcept = delete;

  ~Check() = default;

// ---------------------------------------------------------------------------
  bool
  wasExecuted() const;

  void
  markAsExecuted();

// ---------------------------------------------------------------------------
  uint32_t
  getNumberOfFailes() const;

  void
  incrementNumberOfFailes();

// ---------------------------------------------------------------------------
  const char*
  getCondition();

private:
  uint32_t mNumberOfFailes;
  bool mExecuted;
};

// ---------------------------------------------------------------------------
/**
 * @class ExpectCall
 */
class ExpectCall : public CallContext
{
public:
  static ExpectCall&
  defaultContext();

// ---------------------------------------------------------------------------
  explicit ExpectCall(Unit& unit,
                      size_t line,
                      const char* objectName,
                      std::vector<std::string>&& args,
                      std::map<std::string, std::string>&& comments);

  ExpectCall(const ExpectCall&) = delete;

  ExpectCall(ExpectCall&&) noexcept = delete;

  ExpectCall&
  operator=(const ExpectCall&) = delete;

  ExpectCall&
  operator=(ExpectCall&&) noexcept = delete;

  ~ExpectCall() = default;

// ---------------------------------------------------------------------------
  bool
  wasExecuted() const;

  void
  markAsExecuted();

// ---------------------------------------------------------------------------
  size_t
  getNumberOfUnexpectedCalls() const;

  void
  incrementNumberOfUnexpectedCalls();

  size_t
  getNumberOfUnmetPrerequisites() const;

  void
  incrementNumberOfUnmetPrerequisites();

  size_t
  getNumberOfMissingCalls() const;

  void
  incrementNumberOfMissingCalls();

private:
  size_t mNumberOfUnexpectedCalls;
  size_t mNumberOfUnmetPrerequisites;
  size_t mNumberOfMissingCalls;
  bool mWasExecuted;
};

// ---------------------------------------------------------------------------
/**
 * @class Invariant
 */
class Invariant : public CallContext
{
public:
  static Invariant&
  defaultContext();

// ---------------------------------------------------------------------------
  explicit Invariant(Unit& unit,
                     size_t line,
                     const char* condition,
                     std::vector<std::string>&& args,
                     std::map<std::string, std::string>&& comments);

  Invariant(const Invariant&) = delete;

  Invariant(Invariant&&) noexcept = delete;

  Invariant&
  operator=(const Invariant&) = delete;

  Invariant&
  operator=(Invariant&&) noexcept = delete;

  ~Invariant() = default;

// ---------------------------------------------------------------------------
  bool
  wasCreated() const;

  void
  markAsCreated();

  bool
  hold() const;

  void
  markAsNotHold();

private:
  bool mWasCreated;
  bool mHold;
};

// ---------------------------------------------------------------------------
/**
 * @class MockCreation
 */
class MockCreation : public CallContext
{
public:
  static MockCreation&
  defaultContext();

// ---------------------------------------------------------------------------
  explicit MockCreation(Unit& unit,
                        size_t line,
                        const char* objectName,
                        std::vector<std::string>&& args,
                        std::map<std::string, std::string>&& comments);

  MockCreation(const MockCreation&) = delete;

  MockCreation(MockCreation&&) noexcept = delete;

  MockCreation&
  operator=(const MockCreation&) = delete;

  MockCreation&
  operator=(MockCreation&&) noexcept = delete;

  ~MockCreation() = default;

// ---------------------------------------------------------------------------
  size_t
  getNumberOfCreations() const;

  void
  incrementNumberOfCreations();

// ---------------------------------------------------------------------------
  size_t
  getNumberOfUnexpectedCalls() const;

  void
  incrementNumberOfUnexpectedCalls();

private:
  size_t mNumberOfUnexpectedCalls;
  size_t mNumberOfCreations;
};

// ---------------------------------------------------------------------------
/**
 * @class Signal
 */
class Signal : public CallContext
{
public:
  static Signal&
  defaultContext();

// ---------------------------------------------------------------------------
  explicit Signal(Unit& unit,
                  size_t line,
                  const char* objectName,
                  std::vector<std::string>&& args,
                  std::map<std::string, std::string>&& comments);

  Signal(const Signal&) = delete;

  Signal(Signal&&) noexcept = delete;

  Signal&
  operator=(const Signal&) = delete;

  Signal&
  operator=(Signal&&) noexcept = delete;

  ~Signal() = default;

private:
};

} // namespace meta

} // namespace protest
