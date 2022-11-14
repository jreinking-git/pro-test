#include <gtest/gtest.h>

#include "protest/utils/memory_pool.h"

using namespace protest;

static constexpr auto numberOfElements = 100u;
static constexpr auto blockSize = 100u;
static constexpr int anyValue = 42;

class Element
{
public:
  static bool destructorCalled;

  Element() : mNext(nullptr), mValue(anyValue)
  {
  }

  Element(int value) : mNext(nullptr), mValue(value)
  {
  }

  ~Element()
  {
    destructorCalled = true;
  }

  Element* mNext;
  int mValue;
};

bool Element::destructorCalled = false;

TEST(memory_pool, __available__should_be_available)
{
  MemoryPool<numberOfElements, blockSize> pool;

  ASSERT_TRUE(pool.isAvailable());
}

TEST(memory_pool, __empty__should_be_not_available)
{
  MemoryPool<numberOfElements, blockSize> pool;

  for (int i = 0; i < numberOfElements; i++)
  {
    pool.allocate();
  }

  ASSERT_FALSE(pool.isAvailable());
}

TEST(memory_pool, __empty__should_alloc_block)
{
  MemoryPool<numberOfElements, blockSize> pool;

  uint8_t* mem = pool.allocate();

  ASSERT_TRUE(mem != nullptr);
}

TEST(memory_pool, __empty__should_free_block)
{
  MemoryPool<numberOfElements, blockSize> pool;

  uint8_t* mem = pool.allocate();
  pool.free(mem);

  ASSERT_TRUE(mem == nullptr);
}

TEST(memory_pool, __full__should_free_and_alloc_block)
{
  MemoryPool<numberOfElements, blockSize> pool;

  uint8_t* mem = nullptr;

  mem = pool.allocate();
  for (int i = 0; i < numberOfElements - 1; i++)
  {
    pool.allocate();
  }
  pool.free(mem);
  mem = pool.allocate();

  ASSERT_TRUE(mem != nullptr);
}

TEST(memory_pool, should_free_all_allocated_blocks)
{
  std::vector<uint8_t*> blocks;
  MemoryPool<numberOfElements, blockSize> pool;
  for (int i = 0; i < numberOfElements; i++)
  {
    blocks.push_back(pool.allocate());
  }

  for (auto& ptr : blocks)
  {
    pool.free(ptr);
  }
}

TEST(memory_pool, should_alloc_and_free_object)
{
  std::vector<uint8_t*> blocks;
  ObjectPool<Element, 1> pool;

  auto e = pool.newObject<Element>(anyValue);
  int value = e->mValue;
  Element::destructorCalled = false;
  pool.deleteObject(e);

  ASSERT_EQ(value, anyValue);
  ASSERT_TRUE(Element::destructorCalled);
}

TEST(memory_pool, should_perform_sequence_of_allocs_and_free)
{
  MemoryPool<numberOfElements, 5> pool;

  uint8_t* ptr0 = pool.allocate();
  uint8_t* ptr1 = pool.allocate();
  uint8_t* ptr2 = pool.allocate();
  uint8_t* ptr3 = pool.allocate();
  uint8_t* ptr4 = pool.allocate();

  pool.free(ptr0);
  pool.free(ptr4);

  ptr0 = pool.allocate();
  ptr4 = pool.allocate();

  pool.free(ptr4);
  pool.free(ptr3);
  pool.free(ptr2);
  pool.free(ptr1);
  pool.free(ptr0);

  ptr0 = pool.allocate();
  pool.free(ptr0);

  ASSERT_EQ(ptr0, nullptr);
  ASSERT_EQ(ptr1, nullptr);
  ASSERT_EQ(ptr2, nullptr);
  ASSERT_EQ(ptr3, nullptr);
  ASSERT_EQ(ptr4, nullptr);
}
