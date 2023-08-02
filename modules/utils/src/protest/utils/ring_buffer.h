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

#include "protest/utils/debug.h"

#include <cstddef>
#include <cstring>

namespace protest
{

template <typename T, size_t N>
class RingBuffer
{
public:
  explicit RingBuffer();

  virtual ~RingBuffer() = default;

  RingBuffer(const RingBuffer&) = delete;

  RingBuffer(RingBuffer&&) noexcept = delete;

  RingBuffer&
  operator=(const RingBuffer&) = delete;

  RingBuffer&
  operator=(RingBuffer&&) noexcept = delete;

  void
  push(T value);

  T
  pop();

  T
  peek();

  bool
  isEmpty();

  bool
  isFull();

  bool
  isAvailable();

  size_t
  numberOfElements();

  void
  clear();

private:
  T mBuffer[N];
  size_t mNumberOfElements;
  size_t mCurrentIndex;
};

template <typename T, size_t N>
RingBuffer<T, N>::RingBuffer() : mNumberOfElements(0u), mCurrentIndex(0u)
{
  memset(mBuffer, 0u, sizeof(T) * N);
}

template <typename T, size_t N>
void
RingBuffer<T, N>::push(T value)
{
  PROTEST_ASSERT(mNumberOfElements < N);
  PROTEST_ASSERT(mCurrentIndex < N);
  mBuffer[mCurrentIndex] = value;
  mCurrentIndex = (mCurrentIndex + 1) % N;
  if (!isFull())
  {
    mNumberOfElements++;
  }
  else
  {
    // no new element
  }
}

template <typename T, size_t N>
T
RingBuffer<T, N>::pop()
{
  PROTEST_ASSERT(mNumberOfElements > 0);
  // + N to make sure that mCurrentIndex - mNumberOfElemensts will be greater
  // than zero. Would be O.K but static code analysation could complane.
  // + N is not a problem because of % N
  PROTEST_ASSERT((((mCurrentIndex + N) - mNumberOfElements) % N) < N);
  T value = mBuffer[((mCurrentIndex + N) - mNumberOfElements) % N];
  mNumberOfElements--;
  return value;
}

template <typename T, size_t N>
T
RingBuffer<T, N>::peek()
{
  PROTEST_ASSERT(mNumberOfElements > 0);
  // + N to make sure that mCurrentIndex - mNumberOfElemensts will be greater
  // than zero. Would be O.K but static code analysation could complane.
  // + N is not a problem because of % N
  PROTEST_ASSERT((((mCurrentIndex + N) - mNumberOfElements) % N) < N);
  T value = mBuffer[((mCurrentIndex + N) - mNumberOfElements) % N];
  return value;
}

template <typename T, size_t N>
bool
RingBuffer<T, N>::isEmpty()
{
  return mNumberOfElements == 0;
}

template <typename T, size_t N>
bool
RingBuffer<T, N>::isFull()
{
  return mNumberOfElements == N;
}

template <typename T, size_t N>
bool
RingBuffer<T, N>::isAvailable()
{
  return mNumberOfElements > 0;
}

template <typename T, size_t N>
size_t
RingBuffer<T, N>::numberOfElements()
{
  return mNumberOfElements;
}

template <typename T, size_t N>
void
RingBuffer<T, N>::clear()
{
  while (numberOfElements())
  {
    pop();
  }
}

} // namespace protest
