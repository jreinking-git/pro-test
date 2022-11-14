#include <gtest/gtest.h>

#include "protest/utils/ring_buffer.h"

using namespace protest;

static constexpr auto numberOfElements = 100u;
static constexpr int anyValue = 42;

TEST(ring_buffer, __empty__should_push_value)
{
  RingBuffer<int, numberOfElements> ringBuffer;

  ringBuffer.push(anyValue);
  int value = ringBuffer.pop();

  ASSERT_EQ(value, anyValue);
}

TEST(ring_buffer, __available__should_pop_value)
{
  RingBuffer<int, numberOfElements> ringBuffer;

  ringBuffer.push(anyValue);
  int value = ringBuffer.pop();

  ASSERT_EQ(value, anyValue);
}

TEST(ring_buffer, __available__should_pop_value_twice)
{
  RingBuffer<int, numberOfElements> ringBuffer;

  ringBuffer.push(anyValue);
  ringBuffer.push(anyValue + 1);
  int value1 = ringBuffer.pop();
  int value2 = ringBuffer.pop();

  ASSERT_EQ(value1, anyValue);
  ASSERT_EQ(value2, anyValue + 1);
}

TEST(ring_buffer, __empty__should_be_empty)
{
  RingBuffer<int, numberOfElements> ringBuffer;

  ASSERT_TRUE(ringBuffer.isEmpty());
}

TEST(ring_buffer, __empty__should_be_not_available)
{
  RingBuffer<int, numberOfElements> ringBuffer;

  ASSERT_FALSE(ringBuffer.isAvailable());
}

TEST(ring_buffer, __available__should_be_available)
{
  RingBuffer<int, numberOfElements> ringBuffer;

  ringBuffer.push(anyValue);

  ASSERT_TRUE(ringBuffer.isAvailable());
}

TEST(ring_buffer, __empty__should_have_no_elements)
{
  RingBuffer<int, numberOfElements> ringBuffer;

  ASSERT_EQ(ringBuffer.numberOfElements(), 0);
}

TEST(ring_buffer, __available__should_have_one_elements)
{
  RingBuffer<int, numberOfElements> ringBuffer;

  ringBuffer.push(anyValue);

  ASSERT_EQ(ringBuffer.numberOfElements(), 1);
}

TEST(ring_buffer, __full__should_have_100_elements)
{
  RingBuffer<int, numberOfElements> ringBuffer;

  for (int i = 0; i < 100; i++)
  {
    ringBuffer.push(anyValue);
  }

  ASSERT_EQ(ringBuffer.numberOfElements(), 100);
}

TEST(ring_buffer, __full__should_be_full)
{
  RingBuffer<int, numberOfElements> ringBuffer;

  for (int i = 0; i < 100; i++)
  {
    ringBuffer.push(anyValue);
  }

  ASSERT_TRUE(ringBuffer.isFull());
}

TEST(ring_buffer, __available__should_push_value)
{
  RingBuffer<int, numberOfElements> ringBuffer;

  ringBuffer.push(anyValue);
  ringBuffer.push(anyValue);

  ASSERT_EQ(ringBuffer.numberOfElements(), 2);
}

TEST(ring_buffer, __available__should_peek_value)
{
  RingBuffer<int, numberOfElements> ringBuffer;

  ringBuffer.push(anyValue);

  ASSERT_EQ(ringBuffer.peek(), anyValue);
  ASSERT_EQ(ringBuffer.numberOfElements(), 1);
}

TEST(ring_buffer, __full__should_throw_assert_exeception_on_push)
{
  RingBuffer<int, numberOfElements> ringBuffer;
  for (int i = 0; i < 100; i++)
  {
    ringBuffer.push(anyValue);
  }

  EXPECT_ANY_THROW(ringBuffer.push(anyValue));
}

TEST(ring_buffer, __full__should_pop_and_push_value)
{
  RingBuffer<int, numberOfElements> ringBuffer;

  for (int i = 0; i < 100; i++)
  {
    ringBuffer.push(i);
  }
  int value = ringBuffer.pop();
  ringBuffer.push(42);

  ASSERT_EQ(value, 0);
}

TEST(ring_buffer, __empty__should_throw_assert_exeception_on_pop)
{
  RingBuffer<int, numberOfElements> ringBuffer;

  EXPECT_ANY_THROW(ringBuffer.pop());
}

TEST(ring_buffer, __empty__should_throw_assert_exeception_on_peek)
{
  RingBuffer<int, numberOfElements> ringBuffer;

  EXPECT_ANY_THROW(ringBuffer.peek());
}

TEST(ring_buffer, __full__should_pop_value)
{
  RingBuffer<int, numberOfElements> ringBuffer;
  for (int i = 0; i < 100; i++)
  {
    ringBuffer.push(anyValue);
  }

  ASSERT_EQ(ringBuffer.pop(), anyValue);
}

TEST(ring_buffer, should_keep_sequence)
{
  RingBuffer<int, numberOfElements> ringBuffer;

  ringBuffer.push(1);
  ringBuffer.push(2);
  ringBuffer.push(3);

  ASSERT_EQ(ringBuffer.pop(), 1);
  ASSERT_EQ(ringBuffer.pop(), 2);
  ASSERT_EQ(ringBuffer.pop(), 3);
}
