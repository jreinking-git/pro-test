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

#include <iostream>

#include <cstdint>
#include <cstddef>

namespace protest
{

namespace mock
{

namespace internal
{

class Cardinality;

} // namespace internal

// ---------------------------------------------------------------------------
/**
 * @brief any
 * 
 * Expect any number of calls.
 * 
 * @return Cardinality
 */
internal::Cardinality
any();

/**
 * @brief exactly
 * 
 * Expect exactly the number of calls.
 * 
 * @param numberOfCalls
 *  the number of calls to expect.
 * 
 * @return Cardinality
 */
internal::Cardinality
exactly(size_t numberOfCalls);

/**
 * @brief atMost
 * 
 * Expect that calls have an upper limit.
 * 
 * @param numberOfCalls
 *  expect at most numberOfCalls calls
 * 
 * @return Cardinality 
 */
internal::Cardinality
atMost(size_t max);

/**
 * @brief atLeast
 * 
 * Expect that calls have a lower limit
 * 
 * @param numberOfCalls
 *  expect that there are at least numberOfCalls calls
 * 
 * @return Cardinality 
 */
internal::Cardinality
atLeast(size_t min);

/**
 * @brief between
 * 
 * Expect number of calls to be between an upper and a lower bound.
 * 
 * @param min
 *  expect at least min number of calls
 * 
 * @param max
 *  expect at most max number of calls
 * 
 * @return Cardinality 
 */
internal::Cardinality
between(size_t min, size_t max);

namespace internal
{

// ---------------------------------------------------------------------------
/**
 * @class CardinalityInterface
 */
class CardinalityInterface
{
public:
  explicit CardinalityInterface() = default;

  CardinalityInterface(const CardinalityInterface& other) = delete;

  CardinalityInterface(CardinalityInterface&& other) = delete;

  CardinalityInterface&
  operator=(const CardinalityInterface& other) = delete;

  CardinalityInterface&
  operator=(CardinalityInterface&& other) = delete;

  virtual ~CardinalityInterface() = default;

// ---------------------------------------------------------------------------
  virtual bool
  isSatisfiedByCallCount(size_t numberOfCalls) const = 0;

  virtual bool
  isSaturatedByCallCount(size_t numberOfCalls) const = 0;

  virtual void
  explain(std::ostream& stream) = 0;

  virtual void
  explain(std::ostream& stream, size_t callCounter) = 0;

private:
};

// ---------------------------------------------------------------------------
/**
 * @class Cardinality
 * 
 * This class is used to specifie the number of calls to an mock function. This
 * is a wrapper class which holds an pointer to the actual implementation. A
 * Cardinality can be for example: atMost, atLeast, between, even, odd, etc.
 * Cardinalities can have diffrent properties. I.e.: 'atMost' has an upper limit
 * an can therefore saturated. Whereas 'even' will never be saturated. An
 * cardinality must further implement function which explain what the expectation
 * of a cardinality is, and why a given call counter does not match the
 * expectation.
 */
class Cardinality
{
public:
  explicit Cardinality(CardinalityInterface* interface);

  Cardinality(const Cardinality& other) = delete;

  Cardinality(Cardinality&& other) noexcept;

  Cardinality&
  operator=(const Cardinality& other) = delete;

  Cardinality&
  operator=(Cardinality&& other) noexcept;

  ~Cardinality();

// ---------------------------------------------------------------------------
  /**
   * @brief isSatisfiedByCallCount
   * 
   * If the cardinality is satisfied, no failure message are generated when the
   * mock gets destroyed. 
   * 
   * @param numberOfCalls
   *  check if the numberOfCalls satisfy this cardinality
   * 
   * @return true, if the numberOfCalls is satisfied
   * @return false, otherwise
   */
  bool
  isSatisfiedByCallCount(size_t numberOfCalls) const;

  /**
   * @brief isSaturatedByCallCount
   * 
   * A cardinality is saturated if any further call would yield in an failure.
   * 
   * @param numberOfCalls 
   * @return true, if saturated
   * @return false, otherwise
   */
  bool
  isSaturatedByCallCount(size_t numberOfCalls) const;

  /**
   * @brief explain
   * 
   * Explain what the expectaion of the cardinality is.
   * 
   * @param stream
   *  stream to use for explaination
   */
  void
  explain(std::ostream& stream) const;

  /**
   * @brief explain
   * 
   * Explain why the given call counter does not fulfill the cardinality.
   * 
   * @param stream
   *  stream to use for explaination
   * 
   * @param callCounter
   *  the call counter to explain
   */
  void
  explain(std::ostream& stream, size_t callCounter) const;

private:
public:
  CardinalityInterface* mInterface;
};

// ---------------------------------------------------------------------------
/**
 * @class BetweenImpl
 */
class BetweenImpl : public CardinalityInterface
{
public:
  explicit BetweenImpl(size_t min, size_t max);

  BetweenImpl(const BetweenImpl& other) = delete;

  BetweenImpl(BetweenImpl&& other) = delete;

  BetweenImpl&
  operator=(const BetweenImpl& other) = delete;

  BetweenImpl&
  operator=(BetweenImpl&& other) = delete;

  ~BetweenImpl() = default;

// ---------------------------------------------------------------------------
  bool
  isSatisfiedByCallCount(size_t numberOfCalls) const override;

  bool
  isSaturatedByCallCount(size_t numberOfCalls) const override;

  void
  explain(std::ostream& stream) override;

  void
  explain(std::ostream& stream, size_t callCounter) override;

private:
  static std::string
  format(size_t count);

  size_t mMin;
  size_t mMax;
};

} // namespace internal

} // namespace mock

} // namespace protest