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

#include <cstdint>
#include <cstddef>

namespace protest
{

// ---------------------------------------------------------------------------
/**
 * @class List
 */
template <typename T>
class List
{
public:
  explicit List();

  List(const List&) = delete;

  List(List&&) noexcept = delete;

  List&
  operator=(const List&) = delete;

  List&
  operator=(List&&) noexcept = delete;

  ~List() = default;

  /**
   * @class Iterator
   */
  class Iterator
  {
  public:
    friend class List;

    Iterator(const Iterator&) = delete;

    Iterator(Iterator&&) noexcept = delete;

    Iterator&
    operator=(const Iterator&) = delete;

    Iterator&
    operator=(Iterator&&) noexcept = delete;

    ~Iterator() = default;

    Iterator&
    operator++();

    bool
    operator==(const Iterator& other);

    T*
    operator->();

    bool
    operator!=(const Iterator& other);

    T&
    operator*();

  private:
    explicit Iterator(T* current);

    T* mCurrent;
  };

  Iterator
  begin();

  Iterator
  end();

  T&
  first();

  T&
  removeFirst();

  void
  prepend(T& obj);

  void
  append(T& obj);

  void
  remove(T& obj);

  size_t
  numberOfElements();

  bool
  isAvailable();

private:
  T* mHead;
};

// ---------------------------------------------------------------------------
template <typename T>
List<T>::List() : mHead(nullptr)
{
}

// ---------------------------------------------------------------------------
template <typename T>
typename List<T>::Iterator&
List<T>::Iterator::operator++()
{
  mCurrent = mCurrent->mNext;
  return *this;
}

template <typename T>
bool
List<T>::Iterator::operator==(const Iterator& other)
{
  return mCurrent == other.mCurrent;
}

template <typename T>
T*
List<T>::Iterator::operator->()
{
  return mCurrent;
}

template <typename T>
bool
List<T>::Iterator::operator!=(const Iterator& other)
{
  return mCurrent != other.mCurrent;
}

template <typename T>
T&
List<T>::Iterator::operator*()
{
  return *mCurrent;
}

template <typename T>
List<T>::Iterator::Iterator(T* current) : mCurrent(current)
{
}

// ---------------------------------------------------------------------------
template <typename T>
typename List<T>::Iterator
List<T>::begin()
{
  return Iterator(mHead);
}

template <typename T>
typename List<T>::Iterator
List<T>::end()
{
  return Iterator(nullptr);
}

// ---------------------------------------------------------------------------
template <typename T>
T&
List<T>::first()
{
  PROTEST_ASSERT(isAvailable());
  return *mHead;
}

template <typename T>
T&
List<T>::removeFirst()
{
  PROTEST_ASSERT(isAvailable());
  T* head = mHead;
  mHead = head->mNext;
  head->mNext = nullptr;
  return *head;
}

template <typename T>
void
List<T>::prepend(T& obj)
{
  PROTEST_ASSERT(obj.mNext == nullptr);
  if (mHead == nullptr)
  {
    mHead = &obj;
    obj.mNext = nullptr;
  }
  else
  {
    obj.mNext = mHead;
    mHead = &obj;
  }
}

template <typename T>
void
List<T>::append(T& obj)
{
  PROTEST_ASSERT(obj.mNext == nullptr);
  T* current = mHead;
  if (current == nullptr)
  {
    mHead = &obj;
  }
  else
  {
    while (current->mNext)
    {
      current = current->mNext;
    }
    current->mNext = &obj;
  }
}

template <typename T>
void
List<T>::remove(T& obj)
{
  PROTEST_ASSERT(isAvailable());
  T* last = nullptr;
  T* current = mHead;

  while (current)
  {
    if (current == &obj)
    {
      if (last == nullptr)
      {
        mHead = obj.mNext;
      }
      else
      {
        last->mNext = obj.mNext;
      }
      current = nullptr;
    }
    else
    {
      last = current;
      current = current->mNext;
      PROTEST_ASSERT(current != nullptr);
    }
  }

  obj.mNext = nullptr;
}

template <typename T>
size_t
List<T>::numberOfElements()
{
  size_t n = 0u;
  T* current = mHead;
  while (current)
  {
    n++;
    current = current->mNext;
  }
  return n;
}

template <typename T>
bool
List<T>::isAvailable()
{
  return mHead;
}

} // namespace protest
