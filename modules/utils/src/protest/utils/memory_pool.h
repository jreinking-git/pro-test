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
#include <cstring>

namespace protest
{

template <uint8_t numberOfBlocks, size_t blockSize>
class MemoryPool
{
public:
  explicit MemoryPool();

  ~MemoryPool();

  MemoryPool(const MemoryPool&) = delete;

  MemoryPool(const MemoryPool&&) = delete;

  MemoryPool&
  operator=(const MemoryPool&) = delete;

  MemoryPool&
  operator=(const MemoryPool&&) = delete;

  uint8_t*
  allocate();

  template <typename T, typename... Args>
  T*
  newObject(Args&&... args);

  void
  free(uint8_t*& block);

  template <typename T>
  void
  deleteObject(T*& object);

  bool
  isAvailable();

private:
  uint8_t*
  index2Block(const size_t index);

  uint8_t
  block2index(const uint8_t* block);

  uint8_t mAvailableBlocks;
  uint8_t mNextFreeBlockIndex;
  uint8_t mBuffer[numberOfBlocks * blockSize];
};

template <typename T, size_t N>
using ObjectPool = MemoryPool<N, sizeof(T)>;

template <uint8_t numberOfBlocks, size_t blockSize>
MemoryPool<numberOfBlocks, blockSize>::MemoryPool() :
  mAvailableBlocks(numberOfBlocks),
  mNextFreeBlockIndex(0u)
{
  std::memset(mBuffer, 0, numberOfBlocks * blockSize);

  // let each element point to the next (n + 1) one
  for (uint8_t i = 0u; i < numberOfBlocks; i++)
  {
    uint8_t* block = index2Block(i);
    // this might assign 0 (255 + 1) to the last element but it's ok
    *block = i + 1;
  }

  // point to the first free element
  mNextFreeBlockIndex = 0u;
}

template <uint8_t numberOfBlocks, size_t blockSize>
MemoryPool<numberOfBlocks, blockSize>::~MemoryPool()
{
  // PROTEST_ASSERT(mAvailableBlocks == numberOfBlocks);
}

template <uint8_t numberOfBlocks, size_t blockSize>
uint8_t*
MemoryPool<numberOfBlocks, blockSize>::allocate()
{
  PROTEST_ASSERT(isAvailable());
  uint8_t* freeBlock = index2Block(mNextFreeBlockIndex);
  mNextFreeBlockIndex = *freeBlock;
  mAvailableBlocks--;
  return freeBlock;
}

template <uint8_t numberOfBlocks, size_t blockSize>
template <typename T, typename... Args>
T*
MemoryPool<numberOfBlocks, blockSize>::newObject(Args&&... args)
{
  static_assert(sizeof(T) <= blockSize,
                "Cannot alloc objects that are greater than a block!");
  uint8_t* block = allocate();
  return new (block) T(std::forward<Args>(args)...);
}

template <uint8_t numberOfBlocks, size_t blockSize>
void
MemoryPool<numberOfBlocks, blockSize>::free(uint8_t*& block)
{
  *block = mNextFreeBlockIndex;
  mNextFreeBlockIndex = block2index(block);
  mAvailableBlocks++;
  block = nullptr;
}

template <uint8_t numberOfBlocks, size_t blockSize>
template <typename T>
void
MemoryPool<numberOfBlocks, blockSize>::deleteObject(T*& object)
{
  object->~T();
  // since all types of object are freed the same way we need to cast them
  // to uint8_t pointer.
  uint8_t* block = reinterpret_cast<uint8_t*>(object);
  free(block);
  object = nullptr;
}

template <uint8_t numberOfBlocks, size_t blockSize>
bool
MemoryPool<numberOfBlocks, blockSize>::isAvailable()
{
  return mAvailableBlocks > 0u;
}

// ---------------------------------------------------------------------------
template <uint8_t numberOfBlocks, size_t blockSize>
uint8_t*
MemoryPool<numberOfBlocks, blockSize>::index2Block(const size_t index)
{
  PROTEST_ASSERT(index < numberOfBlocks);
  return &mBuffer[index * blockSize];
}

template <uint8_t numberOfBlocks, size_t blockSize>
uint8_t
MemoryPool<numberOfBlocks, blockSize>::block2index(const uint8_t* block)
{
  PROTEST_ASSERT(block >= mBuffer &&
                 block < &mBuffer[numberOfBlocks * blockSize]);// is in mBuffer?
  PROTEST_ASSERT((block - mBuffer) % blockSize ==
                 0u);// points to start of block?
  return (block - mBuffer) / blockSize;
}

} // namespace protest

/* PROTEST_UTILS_MEMORY_POOL_H */