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

namespace protest
{

// ---------------------------------------------------------------------------
/**
 * @class Queue
 */
template <typename T>
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

  void
  push(T& obj);

  T&
  pop();

  bool
  isAvailable();

private:
  T* mHead;
  T* mTail;
};

// ---------------------------------------------------------------------------
template <typename T>
Queue<T>::Queue() : mHead(nullptr), mTail(nullptr)
{
}

template <typename T>
void
Queue<T>::push(T& obj)
{
  PROTEST_ASSERT(obj.mNext == nullptr);
  obj.mNext = &obj;
  if (mHead == nullptr)
  {
    mHead = &obj;
    mTail = &obj;
  }
  else
  {
    mTail->mNext = &obj;
    mTail = &obj;
  }
}

template <typename T>
T&
Queue<T>::pop()
{
  PROTEST_ASSERT(isAvailable());
  T& obj = *mHead;
  mHead = obj.mNext;
  obj.mNext = nullptr;
  if (mHead == &obj)
  {
    mTail = nullptr;
    mHead = nullptr;
  }
  else
  {
  }
  obj.mNext = nullptr;
  return obj;
}

template <typename T>
bool
Queue<T>::isAvailable()
{
  return mHead;
}

} // namespace protest
