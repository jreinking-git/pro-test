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

#include "protest/utils/debug.h"

#include <cstddef>
#include <cstdint>

namespace protest
{

template <typename T, size_t N>
class Heap
{
public:
  static_assert(N > 0);

  explicit Heap();

  ~Heap();

  Heap(const Heap&) = delete;

  Heap(const Heap&&) = delete;

  Heap&
  operator=(const Heap&) = delete;

  Heap&
  operator=(const Heap&&) = delete;

  void
  push(T* value);

  T*
  peek();

  T*
  pop();

  void
  remove(T* value);

  bool
  isAvailable();

  bool
  isEmpty();

  bool
  isFull();

  size_t
  numberOfElements();

public:// TODO
  void
  heapify(size_t index);

  size_t
  parent(size_t n);

  size_t
  left(size_t n);

  size_t
  right(size_t n);

  void
  swap(size_t n, size_t m);

  void
  checkTree();

  void
  checkTree(int i);

  void
  checkTreeLeft(int root, int i);

  void
  checkTreeRight(int root, int i);

  T* mBuffer[N];
  size_t mNumberOfElements;
};

template <typename T, size_t N>
Heap<T, N>::Heap() : mNumberOfElements(0u)
{
  for (size_t i = 0; i < N; i++)
  {
    mBuffer[i] = nullptr;
  }
}

template <typename T, size_t N>
Heap<T, N>::~Heap()
{
}

template <typename T, size_t N>
void
Heap<T, N>::push(T* value)
{
  PROTEST_ASSERT(mNumberOfElements < N);
  mBuffer[mNumberOfElements] = value;
  mBuffer[mNumberOfElements]->mIndex = mNumberOfElements;
  size_t n = mNumberOfElements;
  size_t p = 0;

  while (n > 0)
  {
    p = parent(n);

    if ((*mBuffer[n]) < (*mBuffer[p]))
    {
      // need to order
      swap(n, p);
    }
    else
    {
      // is fine
    }
    n = p;
  }

  mNumberOfElements++;
  checkTree();
}

template <typename T, size_t N>
T*
Heap<T, N>::peek()
{
  PROTEST_ASSERT(isAvailable());
  return mBuffer[0];
}

template <typename T, size_t N>
T*
Heap<T, N>::pop()
{
  PROTEST_ASSERT(isAvailable());
  T* value = mBuffer[0];
  heapify(0);
  checkTree();
  return value;
}

template <typename T, size_t N>
void
Heap<T, N>::remove(T* value)
{
  PROTEST_ASSERT(value->mIndex < mNumberOfElements);
  if (value->mIndex != 0)
  {
    size_t current = value->mIndex;
    size_t top = parent(current);
    while (current != 0)
    {
      mBuffer[current] = mBuffer[top];
      mBuffer[top]->mIndex = current;
      current = top;
      if (current != 0)
      {
        top = parent(current);
      }
    }
  }
  pop();
  checkTree();
}

template <typename T, size_t N>
bool
Heap<T, N>::isAvailable()
{
  return mNumberOfElements > 0;
}

template <typename T, size_t N>
bool
Heap<T, N>::isEmpty()
{
  return mNumberOfElements == 0;
}

template <typename T, size_t N>
bool
Heap<T, N>::isFull()
{
  return mNumberOfElements == N;
}

template <typename T, size_t N>
size_t
Heap<T, N>::numberOfElements()
{
  return mNumberOfElements;
}

template <typename T, size_t N>
void
Heap<T, N>::heapify(size_t index)
{
  PROTEST_ASSERT(index < mNumberOfElements);
  mBuffer[index] = mBuffer[mNumberOfElements - 1];
  mBuffer[index]->mIndex = index;
  mNumberOfElements--;
  mBuffer[mNumberOfElements] = nullptr;
  size_t n = index;

  bool done = false;
  while (!done)
  {
    size_t l = left(n);
    size_t r = right(n);

    bool hasLeft = (l < N && mBuffer[l] != nullptr);
    bool hasRight = (r < N && mBuffer[r] != nullptr);

    if (hasLeft && hasRight)
    {
      if ((*mBuffer[l]) < (*mBuffer[r]))
      {
        if ((*mBuffer[l]) < (*mBuffer[n]))
        {
          swap(l, n);
          n = l;
        }
        else
        {
          done = true;
        }
      }
      else
      {
        if ((*mBuffer[r]) < (*mBuffer[n]))
        {
          swap(r, n);
          n = r;
        }
        else
        {
          done = true;
        }
      }
    }
    else if (hasLeft)
    {
      if ((*mBuffer[l]) < (*mBuffer[n]))
      {
        swap(l, n);
        n = l;
      }
      else
      {
        done = true;
      }
    }
    else if (hasRight)
    {
      // this cannot happen
      // there must be a value on the left sight since if pop or remove
      // is called the place will be filled with the last element in the
      // array (either this value (since hasRight is true) or any other value)
      // from the left side of the array
      PROTEST_ASSERT(false);
    }
    else
    {
      done = true;
    }
  }
}

template <typename T, size_t N>
size_t
Heap<T, N>::parent(size_t n)
{
  PROTEST_ASSERT(n > 0);
  return (n - 1) / 2;
}

template <typename T, size_t N>
size_t
Heap<T, N>::left(size_t n)
{
  return (2 * n) + 1;
}

template <typename T, size_t N>
size_t
Heap<T, N>::right(size_t n)
{
  return (2 * n) + 2;
}

template <typename T, size_t N>
void
Heap<T, N>::swap(size_t n, size_t m)
{
  T* tmp = mBuffer[n];
  mBuffer[n] = mBuffer[m];
  mBuffer[m] = tmp;

  mBuffer[m]->mIndex = m;
  mBuffer[n]->mIndex = n;
}

template <typename T, size_t N>
void
Heap<T, N>::checkTree()
{
  for (int i = 1; i < mNumberOfElements; i++)
  {
    checkTree(i);
  }
}

template <typename T, size_t N>
void
Heap<T, N>::checkTree(int i)
{
  int p = parent(i);
  if (!(*mBuffer[p] <= *mBuffer[i]))
  {
    assert(false);
  }
}

} // namespace protest
