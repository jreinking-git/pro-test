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

#include <protest/rtos/semaphore.h>
#include <protest/rtos/mutex.h>
#include <protest/time/duration.h>
#include <protest/rtos/thread.h>

#include <cstdint>
#include <cstddef>

// ---------------------------------------------------------------------------
// clang-format off
static constexpr size_t numberOfNodes = 4U;
static constexpr protest::time::Duration minSendTime = protest::time::Millisecond(1000);
static constexpr protest::time::Duration maxSendTime = protest::time::Millisecond(2000);
static constexpr protest::time::Duration maxTimeError = protest::time::Millisecond(1000);
static constexpr protest::time::Duration maxExecutionTime = maxTimeError * 5 + maxSendTime * 2;
// clang-format on

// ---------------------------------------------------------------------------
enum class Type
{
  initial,
  echo
};

// ---------------------------------------------------------------------------
/**
 * @class Message
 */
class Message
{
public:
  Message();

  Type
  getType();

  void
  setType(Type type);

  uint8_t
  getValue();

  void
  setValue(uint8_t value);

  uint8_t
  getFrom();

  void
  setFrom(uint8_t from);

  uint8_t
  getTo();

  void
  setTo(uint8_t to);

  uint8_t
  getSequence();

  void
  setSequence(uint8_t sequence);

private:
  Type mType;
  uint8_t mValue;
  uint8_t mFrom;
  uint8_t mTo;
  uint8_t mSequence;
};

// ---------------------------------------------------------------------------
/**
 * @class ReliableBroadcast
 */
class ReliableBroadcast : public protest::rtos::Thread
{
public:
  enum class State
  {
    blocking,
    acceptingEchos,
    acceptingInitials,
    waitingForInitials,
    waitingForEchos,
    delivered
  };

  class Communication
  {
  public:
    virtual void
    broadcast(Message& message) = 0;

    virtual void
    deliverResult(uint8_t result) = 0;

  private:
  };

// ---------------------------------------------------------------------------
  ReliableBroadcast(Communication& communication, const char* name);

// ---------------------------------------------------------------------------
  void
  run() override;

// ---------------------------------------------------------------------------
  /**
   * @brief propose
   * 
   * Propose a value to the other nodes. This method should be called by one
   * of the nodes. All other should call @c receive. This method is non
   * blocking.
   */
  void
  propose(uint8_t value);

  /**
   * @brief receive
   * 
   * This method should be called by all receiving nodes. This method is non
   * blocking.
   */
  void
  receive();

  /**
   * @brief handle
   * 
   * Handle the message according to the current state. This function is non
   * blocking.
   * 
   * @param message the message to handle
   */
  void
  handle(Message& message);

  void
  acquire();

  void
  release();

private:
  void
  process(uint8_t value = UINT8_MAX);

  uint8_t
  majority();

  void
  reset();

  Communication& mCommunication;
  uint8_t mInitials[numberOfNodes];
  uint8_t mEchos[numberOfNodes];
  uint8_t mSequence;
  State mState;
  uint8_t mValue;

  protest::rtos::Semaphore mSemaphore;
  protest::rtos::Mutex mMutex;
};
