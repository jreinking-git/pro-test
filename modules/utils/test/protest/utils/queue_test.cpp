#include <gtest/gtest.h>

#include "protest/utils/queue.h"

using namespace protest;

static constexpr auto numberOfElements = 100u;
static constexpr int anyValue = 42;

class Element
{
public:
  Element() : mNext(nullptr), mValue(anyValue)
  {
  }

  Element(int value) : mNext(nullptr), mValue(value)
  {
  }

  Element* mNext;
  int mValue;
};

TEST(queue, __empty__should_be_not_available)
{
  Queue<Element> queue;

  ASSERT_FALSE(queue.isAvailable());
}

TEST(queue, __empty__should_be_available)
{
  Queue<Element> queue;
  Element element;

  queue.push(element);

  ASSERT_TRUE(queue.isAvailable());
}

TEST(queue, __empty__should_push_value)
{
  Queue<Element> queue;
  Element element;

  queue.push(element);
  auto& elem = queue.pop();

  ASSERT_EQ(&elem, &element);
}

TEST(queue, __available__should_pop_value)
{
  Queue<Element> queue;
  Element element;

  queue.push(element);
  auto& elem = queue.pop();

  ASSERT_EQ(&elem, &element);
}

TEST(queue, __empty__should_push_2_values)
{
  Queue<Element> queue;
  Element element1(1);
  Element element2(2);

  queue.push(element1);
  queue.push(element2);
  auto& elem1 = queue.pop();
  auto& elem2 = queue.pop();

  ASSERT_EQ(&elem1, &element1);
  ASSERT_EQ(&elem2, &element2);
}

TEST(queue, __empty__should_push_3_values)
{
  Queue<Element> queue;
  Element element1(1);
  Element element2(2);
  Element element3(3);

  queue.push(element1);
  queue.push(element2);
  queue.push(element3);
  auto& elem1 = queue.pop();
  auto& elem2 = queue.pop();
  auto& elem3 = queue.pop();

  ASSERT_EQ(&elem1, &element1);
  ASSERT_EQ(&elem2, &element2);
  ASSERT_EQ(&elem3, &element3);
}

TEST(queue, __empty__should_throw_assert_exception_on_pop)
{
  Queue<Element> queue;

  EXPECT_ANY_THROW(queue.pop());
}

TEST(queue, __already_inserted__should_throw_assert_exception_on_push)
{
  Queue<Element> queue;
  Element element1(1);

  queue.push(element1);

  EXPECT_ANY_THROW(queue.push(element1));
}

TEST(queue, __empty__should_push_already_inserted_element)
{
  Queue<Element> queue;
  Element element1(1);

  queue.push(element1);
  queue.pop();
  queue.push(element1);
  auto& elem = queue.pop();

  ASSERT_EQ(&elem, &element1);
}
