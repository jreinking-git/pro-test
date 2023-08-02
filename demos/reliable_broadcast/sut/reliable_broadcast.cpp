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

#include "reliable_broadcast.h"

#include <protest/rtos/log.h>

#include <iostream>
#include <cstring>

using namespace protest::rtos;

// ---------------------------------------------------------------------------
Message::Message() :
  mType(Type::echo),
  mValue(0),
  mFrom(0),
  mTo(0),
  mSequence(0)
{
}

Type
Message::getType()
{
  return mType;
}

void
Message::setType(Type type)
{
  mType = type;
}

uint8_t
Message::getValue()
{
  return mValue;
}

void
Message::setValue(uint8_t value)
{
  mValue = value;
}

uint8_t
Message::getFrom()
{
  return mFrom;
}

void
Message::setFrom(uint8_t from)
{
  mFrom = from;
}

uint8_t
Message::getTo()
{
  return mTo;
}

void
Message::setTo(uint8_t to)
{
  mTo = to;
}

uint8_t
Message::getSequence()
{
  return mSequence;
}

void
Message::setSequence(uint8_t sequence)
{
  mSequence = sequence;
}

// ---------------------------------------------------------------------------
ReliableBroadcast::ReliableBroadcast(Communication& communication,
                                     const char* name) :
  protest::rtos::Thread(0, 0, name),
  mCommunication(communication),
  mState(State::blocking),
  mSequence(0),
  mValue(0)
{
  reset();
}

// ---------------------------------------------------------------------------
void
ReliableBroadcast::run()
{
  mMutex.acquire();
  while (true)
  {
    mMutex.release();
    mSemaphore.acquire();
    mMutex.acquire();
    process(mValue);
  }
  mMutex.release();
}

// ---------------------------------------------------------------------------
void
ReliableBroadcast::propose(uint8_t value)
{
  mValue = value;
  mSemaphore.release();
}

void
ReliableBroadcast::receive()
{
  mValue = UINT8_MAX;
  mSemaphore.release();
}

void
ReliableBroadcast::handle(Message& message)
{
  if (message.getSequence() == mSequence)
  {
    if (message.getType() == Type::initial &&
        (mState == State::acceptingInitials ||
         mState == State::waitingForInitials))
    {
      if (mInitials[message.getFrom()] == UINT8_MAX)
      {
        mInitials[message.getFrom()] = message.getValue();
        message.setType(Type::echo);
        mCommunication.broadcast(message);
      }
      else
      {
        // duplicated initial message
        PT_WARN("Failure detected: Duplicated message!");
      }
    }
    else if (mState == State::acceptingEchos ||
             mState == State::acceptingInitials ||
             mState == State::waitingForInitials ||
             mState == State::waitingForEchos)
    {
      if (mEchos[message.getFrom()] == UINT8_MAX)
      {
        mEchos[message.getFrom()] = message.getValue();
      }
      else
      {
        // duplicated echo message
        PT_WARN("Failure detected: Duplicated message!");
      }
    }
    else
    {
      // can not accept message in current state
      PT_WARN("Failure detected: Invalid message type!");
    }
  }
  else
  {
    // invalid sequence
    PT_WARN("Failure detected: Invalid sequence! (%d != %d)",
            (int) mSequence,
            (int) message.getSequence());
  }
}

void
ReliableBroadcast::acquire()
{
  mMutex.acquire();
}

void
ReliableBroadcast::release()
{
  mMutex.release();
}

void
ReliableBroadcast::process(uint8_t value)
{
  /**
   * Start accepting echos. Accepting initials in this state can cause
   * errors since not all nodes might accept echos yet. Therefore
   * forwarded message (from a non-faulty node) might be ignored by
   * non-faulty nodes. Which is obviously wrong. Therefore after entering
   * this state, all non-faulty nodes has to wait @c maxTimeError to make
   * sure that all non-faulty nodes can accept initials.
   */
  mState = State::acceptingEchos;
  mMutex.release();
  protest::rtos::Thread::sleep(maxTimeError);
  mMutex.acquire();

  /**
   * Start accepting initials also. Wait another @c maxTimeError to make
   * sure that all non-faulty nodes are atleast in the state
   * @c acceptingInitials. Only then it is safe broadcast the actual value.
   */
  mState = State::acceptingInitials;
  mMutex.release();
  protest::rtos::Thread::sleep(maxTimeError);
  mMutex.acquire();

  /**
   * Now all non-faulty nodes accept echos and initals. Send the initial 
   * message if this is a sender.
   */
  if (value != UINT8_MAX)
  {
    Message message;
    message.setSequence(mSequence);
    message.setValue(value);
    message.setType(Type::initial);
    mCommunication.broadcast(message);
  }

  /**
   * After @c maxTimeError and @c maxSendTime all non-faulty nodes should
   * have received the initial message (if send by an non-faulty node)
   */
  mState = State::waitingForInitials;
  mMutex.release();
  protest::rtos::Thread::sleep(maxTimeError + maxSendTime);
  mMutex.acquire();

  /**
   * After all non-faulty nodes received the initial message. The nodes has
   * to wait for all echos of non-faulty nodes to be delivered. This takes
   * another @c maxTimeError + @c maxSendTime. Initial message are not
   * accepted any more, to make sure that no echo messages from non-faulty
   * nodes are ony the way after leaving this state.
   */
  mState = State::waitingForEchos;
  mMutex.release();
  protest::rtos::Thread::sleep(maxTimeError + maxSendTime);
  mMutex.acquire();

  /**
   * After that there should be a majority among the delivered echo messages.
   * Or UINT8_MAX if no value can be delivered (sender is faulty).
   */
  mState = State::delivered;
  uint8_t result = majority();

  reset();
  mSequence++;

  /**
   * Wait after all nodes has reset there module and increment the sequence
   * counter.
   */
  mMutex.release();
  protest::rtos::Thread::sleep(maxTimeError);
  mMutex.acquire();

  /**
   * When returning from this method certain invariants must hold:
   * 1. Agreement: all non-faulty nodes delivere the same value
   * 2. Modularity: No echo- or initial-message from a non-faulty node is 
   *    beeing send. In other words, all resources are free again so that
   *    the module can be reused. Concider the following situation: a non
   *    faulty node sends a inital message to the other nodes. These nodes
   *    now sending echo messages according to the protocol. When the next
   *    sequence begins before these echo messages are delivered, they block
   *    resources which are needed by the already started sequence. Under
   *    this circumstances @c maxSendTime might not be an upper limit.
   * 3. All non-faulty nodes are still within the expected time error of
   *    @c maxTimeError
   */
  mCommunication.deliverResult(result);
}

uint8_t
ReliableBroadcast::majority()
{
  uint8_t result = UINT8_MAX;
  for (int8_t i = 0; i < numberOfNodes; i++)
  {
    uint8_t n = 0;
    for (int8_t j = 0; j < numberOfNodes; j++)
    {
      if (mEchos[i] == mEchos[j])
      {
        n++;
      }
    }
    if (n >= (numberOfNodes / 2) + 1)
    {
      result = mEchos[i];
    }
    else
    {
    }
  }
  return result;
}

void
ReliableBroadcast::reset()
{
  memset(&mInitials[0], UINT8_MAX, numberOfNodes);
  memset(&mEchos[0], UINT8_MAX, numberOfNodes);
  mState = State::blocking;
}
