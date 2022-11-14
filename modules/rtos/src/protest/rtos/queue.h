#pragma once

#include "protest/rtos/mutex.h"
#include "protest/rtos/semaphore.h"
#include "protest/utils/ring_buffer.h"

namespace protest
{

namespace rtos
{

// ---------------------------------------------------------------------------
/**
 * @class Queue
 */
template <typename T, size_t N>
class Queue
{
public:
  explicit Queue();

  Queue(const Queue&) = delete;

  Queue(Queue&&) noexcept = delete;

  Queue&
  operator=(const Queue&) = delete;

  Queue&
  operator=(Queue&&) noexcept = delete;

  ~Queue() = default;

  bool
  push(T& value, time::Duration timeout = time::Duration::infinity());

  bool
  pop(T& value, time::Duration timeout = time::Duration::infinity());

  void
  acquire();

  void
  release();

  bool
  isAvailable();

  bool
  isFull();

  size_t
  numberOfElements();

private:
  RingBuffer<T, N> mRingBuffer;
  rtos::Mutex mMutex;
  rtos::Semaphore mSemaphoreBlockOnEmpty;
  rtos::Semaphore mSemaphoreBlockOnFull;
};

// ---------------------------------------------------------------------------
template <typename T, size_t N>
Queue<T, N>::Queue()
{
  for (size_t i = 0; i < N; i++)
  {
    mSemaphoreBlockOnFull.release();
  }
}

template <typename T, size_t N>
bool
Queue<T, N>::push(T& value, time::Duration timeout)
{
  mMutex.release();
  bool gotTimeout = mSemaphoreBlockOnFull.acquire(timeout);
  mMutex.acquire();
  if (!gotTimeout)
  {
    PROTEST_ASSERT(!mRingBuffer.isFull());
    mRingBuffer.push(value);
    mSemaphoreBlockOnEmpty.release();
  }
  else
  {
  }
  return gotTimeout;
}

template <typename T, size_t N>
bool
Queue<T, N>::pop(T& value, time::Duration timeout)
{
  mMutex.release();
  bool gotTimeout = mSemaphoreBlockOnEmpty.acquire(timeout);
  mMutex.acquire();
  if (!gotTimeout)
  {
    value = mRingBuffer.pop();
    mSemaphoreBlockOnFull.release();
  }
  else
  {
    // timeout
  }
  return gotTimeout;
}

template <typename T, size_t N>
void
Queue<T, N>::acquire()
{
  mMutex.acquire();
}

template <typename T, size_t N>
void
Queue<T, N>::release()
{
  mMutex.release();
}

template <typename T, size_t N>
bool
Queue<T, N>::isAvailable()
{
  return mRingBuffer.isAvailable();
}

template <typename T, size_t N>
bool
Queue<T, N>::isFull()
{
  return mRingBuffer.isFull();
}

template <typename T, size_t N>
size_t
Queue<T, N>::numberOfElements()
{
  return mRingBuffer.numberOfElements();
}

} // namespace rtos

} // namespace protest
