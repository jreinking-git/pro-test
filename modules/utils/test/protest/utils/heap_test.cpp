#include <gtest/gtest.h>

#include "protest/utils/heap.h"

using namespace protest;

static constexpr int numberOfElements = 100;
static constexpr int anyValue = 42;

class ListElement
{
public:
  ListElement(int value) : mIndex(0), mValue(value)
  {
  }

  ~ListElement()
  {
  }

  bool
  operator<(ListElement& other)
  {
    return mValue < other.mValue;
  }

  size_t mIndex;
  int mValue;
};

TEST(heap, __empty__should_be_not_available)
{
  Heap<ListElement, numberOfElements> heap;

  ASSERT_FALSE(heap.isAvailable());
}

TEST(heap, __empty__should_be_empty)
{
  Heap<ListElement, numberOfElements> heap;

  ASSERT_TRUE(heap.isEmpty());
}

TEST(heap, __available__should_be_not_empty)
{
  Heap<ListElement, numberOfElements> heap;
  ListElement element(0);

  heap.push(&element);

  ASSERT_FALSE(heap.isEmpty());
}

TEST(heap, __available__should_peek)
{
  Heap<ListElement, numberOfElements> heap;
  ListElement element(0);

  heap.push(&element);
  auto pelem = heap.peek();
  auto elem = heap.pop();

  ASSERT_EQ(elem, pelem);
  ASSERT_TRUE(heap.isEmpty());
}

TEST(heap, __full__should_be_full)
{
  Heap<ListElement, numberOfElements> heap;

  for (int i = 0; i < numberOfElements; i++)
  {
    ListElement* elem = new ListElement(0);
    heap.push(elem);
  }

  ASSERT_TRUE(heap.isFull());
}

TEST(heap, __available__should_be_available)
{
  Heap<ListElement, numberOfElements> heap;
  ListElement element(0);

  heap.push(&element);

  ASSERT_TRUE(heap.isAvailable());
}

TEST(heap, __available__should_remove_value)
{
  Heap<ListElement, numberOfElements> heap;
  ListElement element(0);
  heap.push(&element);

  heap.remove(&element);

  ASSERT_FALSE(heap.isAvailable());
}

TEST(heap, __empty__should_throw_assert_exception_on_remove)
{
  Heap<ListElement, numberOfElements> heap;
  ListElement element(0);

  EXPECT_ANY_THROW(heap.remove(&element));
}

TEST(heap, __full__should_throw_assert_exception_on_push)
{
  Heap<ListElement, numberOfElements> heap;
  ListElement element(0);
  for (int i = 0; i < numberOfElements; i++)
  {
    ListElement* elem = new ListElement(0);
    heap.push(elem);
  }

  EXPECT_ANY_THROW(heap.push(&element));
}

TEST(heap, __empty__should_throw_assert_exception_on_peek)
{
  Heap<ListElement, numberOfElements> heap;

  EXPECT_ANY_THROW(heap.peek());
}

TEST(heap, __empty__should_push_values1)
{
  Heap<ListElement, numberOfElements> heap;
  ListElement element0(0);
  ListElement element1(1);
  ListElement element2(2);
  ListElement element3(3);

  heap.push(&element0);
  heap.push(&element1);
  heap.push(&element2);
  heap.push(&element3);

  int n = 0;
  while (heap.isAvailable())
  {
    ListElement* value = heap.pop();
    ASSERT_EQ(value->mValue, n);
    n++;
  }
  ASSERT_FALSE(heap.isAvailable());
}

TEST(heap, __empty__should_push_values2)
{
  Heap<ListElement, numberOfElements> heap;
  ListElement element0(0);
  ListElement element1(1);
  ListElement element2(2);
  ListElement element3(3);

  heap.push(&element1);
  heap.push(&element0);
  heap.push(&element2);
  heap.push(&element3);

  int n = 0;
  while (heap.isAvailable())
  {
    ListElement* value = heap.pop();
    ASSERT_EQ(value->mValue, n);
    n++;
  }
  ASSERT_FALSE(heap.isAvailable());
}

TEST(heap, __empty__should_push_values3)
{
  Heap<ListElement, numberOfElements> heap;
  ListElement element0(0);
  ListElement element1(1);
  ListElement element2(2);
  ListElement element3(3);

  heap.push(&element1);
  heap.push(&element2);
  heap.push(&element0);
  heap.push(&element3);

  int n = 0;
  while (heap.isAvailable())
  {
    ListElement* value = heap.pop();
    ASSERT_EQ(value->mValue, n);
    n++;
  }
  ASSERT_FALSE(heap.isAvailable());
}

TEST(heap, __empty__should_push_values4)
{
  Heap<ListElement, numberOfElements> heap;
  ListElement element0(0);
  ListElement element2(2);
  ListElement element1(1);
  ListElement element3(3);

  heap.push(&element1);
  heap.push(&element2);
  heap.push(&element3);
  heap.push(&element0);

  int n = 0;
  while (heap.isAvailable())
  {
    ListElement* value = heap.pop();
    ASSERT_EQ(value->mValue, n);
    n++;
  }
  ASSERT_FALSE(heap.isAvailable());
}

TEST(heap, __empty__should_push_values5)
{
  Heap<ListElement, numberOfElements> heap;
  ListElement element0(0);
  ListElement element2(2);
  ListElement element1(1);
  ListElement element3(3);

  heap.push(&element0);
  heap.push(&element2);
  heap.push(&element1);
  heap.push(&element3);

  int n = 0;
  while (heap.isAvailable())
  {
    ListElement* value = heap.pop();
    ASSERT_EQ(value->mValue, n);
    n++;
  }
  ASSERT_FALSE(heap.isAvailable());
}

TEST(heap, __empty__should_push_values6)
{
  Heap<ListElement, numberOfElements> heap;
  ListElement element0(0);
  ListElement element2(2);
  ListElement element1(1);
  ListElement element3(3);

  heap.push(&element0);
  heap.push(&element1);
  heap.push(&element2);
  heap.push(&element3);

  int n = 0;
  while (heap.isAvailable())
  {
    ListElement* value = heap.pop();
    ASSERT_EQ(value->mValue, n);
    n++;
  }
  ASSERT_FALSE(heap.isAvailable());
}

TEST(heap, __empty__should_push_values7)
{
  Heap<ListElement, numberOfElements> heap;
  ListElement element0(0);
  ListElement element2(2);
  ListElement element1(1);
  ListElement element3(3);

  heap.push(&element0);
  heap.push(&element1);
  heap.push(&element3);
  heap.push(&element2);

  int n = 0;
  while (heap.isAvailable())
  {
    ListElement* value = heap.pop();
    ASSERT_EQ(value->mValue, n);
    n++;
  }
  ASSERT_FALSE(heap.isAvailable());
}

TEST(heap, __empty__should_push_values8)
{
  Heap<ListElement, numberOfElements> heap;
  ListElement element0(0);
  ListElement element2(2);
  ListElement element1(1);
  ListElement element3(3);

  heap.push(&element2);
  heap.push(&element0);
  heap.push(&element1);
  heap.push(&element3);

  int n = 0;
  while (heap.isAvailable())
  {
    ListElement* value = heap.pop();
    ASSERT_EQ(value->mValue, n);
    n++;
  }
  ASSERT_FALSE(heap.isAvailable());
}

TEST(heap, __empty__should_push_and_pop_random_sequences)
{
  Heap<ListElement, numberOfElements> heap;

  for (int i = 0; i < 10000; i++)
  {
    std::vector<ListElement*> zeros;

    int r = random() % (numberOfElements - heap.numberOfElements() + 1);
    for (int j = 0; j < r; j++)
    {
      int value = random() % 100;
      ListElement* elem = new ListElement(value);
      heap.push(elem);
      if (value == 0)
      {
        zeros.push_back(elem);
      }
    }

    for (auto ptr : zeros)
    {
      heap.remove(ptr);
    }

    int rr = random() % (heap.numberOfElements() + 1);
    int ii = 0;
    for (int j = 0; j < rr; j++)
    {
      ListElement* elem = heap.pop();
      ASSERT_TRUE(elem->mValue >= ii);
      ASSERT_TRUE(elem->mValue != 0);
      ii = elem->mValue;
      delete elem;
    }
  }
}
