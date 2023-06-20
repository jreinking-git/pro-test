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

#include "protest/rtos/thread.h"
#include "protest/coro/scheduler.h"

#include <unistd.h>

using namespace protest::coro;

// ---------------------------------------------------------------------------
Scheduler::Scheduler() :
  mMain {0},
  mCurrent(nullptr),
  mNumberOfSleepingCoroutines(0)
{
  CoroutineBase::initialize(&mMain);
}

void
Scheduler::addThread(Coroutine* thread)
{
  mRunQueue.push(*thread);
}

// ---------------------------------------------------------------------------
void
Scheduler::yield(Coroutine* thread)
{
  // can only be called from thread itself
  PROTEST_ASSERT(getCurrent() == thread);
  mRunQueue.push(*thread);
  executeNext(thread);
}

void
Scheduler::sleep(Coroutine* thread, time::Duration duration)
{
  // can only be called from thread itself
  PROTEST_ASSERT(getCurrent() == thread);
  if (duration != time::Duration::infinity())
  {
    thread->mSleepUntil = now() + duration;
    pushToSleepQueue(thread);
    executeNext(thread);
  }
  else
  {
    // sleep for ever (or til the next wakeup)
    // since it sleeps to infinity it cannot wake just form letting time pass
    // there must be a call to wakeup
    mNumberOfSleepingCoroutines++;
    executeNext(thread);
  }
}

void
Scheduler::wakeup(Coroutine* thread)
{
  // can not be called from thread itself
  PROTEST_ASSERT(getCurrent() != thread);
  if (thread->mIsInSleepQueue)
  {
    removeFromSleepQueue(thread);
    thread->mIsInSleepQueue = false;
  }
  else
  {
    // sleeping for ever -> not in sleep queue
    // just push it into run queue again
  }
  mNumberOfSleepingCoroutines--;
  mRunQueue.push(*thread);
}

void
Scheduler::exit(Coroutine* thread)
{
  // can only be called from thread itself
  PROTEST_ASSERT(getCurrent() == thread);
  while (true)
  {
    executeNext(thread);
  }
  PROTEST_ASSERT(false);
}

protest::time::TimePoint
Scheduler::now(Coroutine* /* thread */)
{
  return mClock.now();
}

protest::time::TimePoint
Scheduler::now()
{
  return mClock.now();
}

// ---------------------------------------------------------------------------
void
Scheduler::run()
{
  if (mRunQueue.isAvailable())
  {
    Coroutine& next = mRunQueue.pop();
    mCurrent = &next;
  }

  while (mCurrent != nullptr)
  {
    mCurrent->contextSwitch(&mMain);
    mCurrent = getNext();
  }

  PROTEST_ASSERT(!mRunQueue.isAvailable());
  PROTEST_ASSERT(!mSleepQueue.isAvailable());
  if (mNumberOfSleepingCoroutines > 0)
  {
    // there are some infinty sleeping coroutines
    mClock.moveToEndOfEpoche();
  }
  else
  {
    // all coroutines called exit
  }
}

Coroutine*
Scheduler::getCurrent()
{
  PROTEST_ASSERT(mCurrent);
  return mCurrent;
}

// ---------------------------------------------------------------------------
void
Scheduler::executeNext(Coroutine* prev)
{
  CoroutineBase::swapContext(&prev->mContext, &mMain);
}

Coroutine*
Scheduler::getNext()
{
  Coroutine* next = nullptr;
  if (mRunQueue.isAvailable())
  {
    next = &mRunQueue.pop();
  }
  else
  {
    if (mSleepQueue.isAvailable())
    {
      Coroutine* current = mSleepQueue.peek();
      if (current->mSleepUntil == time::TimePoint::endOfEpoche())
      {
        next = nullptr;
        mClock.moveToEndOfEpoche();
      }
      else
      {
        const time::Duration sleepTime = current->mSleepUntil - mClock.now();
        // protest::rtos::Thread::sleep(sleepTime);
        mClock.moveForward(sleepTime);
        next = current;
      }

      if (current != nullptr)
      {
        Coroutine* tmp = popFromSleepQueue();
        assert(current == tmp);
      }
      else
      {
      }
    }
    else
    {
      // mCurrent = nullptr;
      // all are done
      // just return to the current thread
      next = nullptr;
    }
  }
  return next;
}

Coroutine*
Scheduler::popFromSleepQueue()
{
  Coroutine* thread = mSleepQueue.pop();
  PROTEST_ASSERT(thread->mIsInSleepQueue);
  thread->mIsInSleepQueue = false;
  mNumberOfSleepingCoroutines--;
  return thread;
}

void
Scheduler::removeFromSleepQueue(Coroutine* thread)
{
  PROTEST_ASSERT(thread->mIsInSleepQueue);
  mSleepQueue.remove(thread);
  thread->mIsInSleepQueue = false;
}

void
Scheduler::pushToSleepQueue(Coroutine* thread)
{
  PROTEST_ASSERT(thread->mIsInSleepQueue == false);
  PROTEST_ASSERT(thread->mSleepUntil >= now());
  mNumberOfSleepingCoroutines++;
  mSleepQueue.push(thread);
  thread->mIsInSleepQueue = true;
}
