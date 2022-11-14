#include <gtest/gtest.h>

#include "protest/utils/list.h"

using namespace protest;

static constexpr int anyValue = 42;

class ListElement
{
public:
  ListElement() : mNext(nullptr), mValue(anyValue)
  {
  }

  ListElement(int value) : mNext(nullptr), mValue(value)
  {
  }

  ~ListElement()
  {
  }

  ListElement* mNext;
  int mValue;
};

TEST(list, __empty__should_be_not_available)
{
  List<ListElement> list;
  ListElement Listelement;

  ASSERT_FALSE(list.isAvailable());
}

TEST(list, __available__should_be_available)
{
  List<ListElement> list;
  ListElement Listelement;

  list.prepend(Listelement);

  ASSERT_TRUE(list.isAvailable());
}

TEST(list, __empty__should_prepend)
{
  List<ListElement> list;
  ListElement element;

  list.prepend(element);
  ListElement& elem = list.first();

  ASSERT_EQ(&elem, &element);
}

TEST(list, __available__should_return_first)
{
  List<ListElement> list;
  ListElement element;

  list.prepend(element);
  ListElement& elem = list.first();

  ASSERT_EQ(&elem, &element);
}

TEST(list, __available__should_remove_first)
{
  List<ListElement> list;
  ListElement element;

  list.prepend(element);
  ListElement& elem = list.removeFirst();

  ASSERT_EQ(&elem, &element);
  ASSERT_FALSE(list.isAvailable());
}

TEST(list, __empty__should_prepend_and_remove_2_elements)
{
  List<ListElement> list;
  ListElement element1;
  ListElement element2;

  list.prepend(element1);
  list.prepend(element2);
  ListElement& elem1 = list.removeFirst();
  ListElement& elem2 = list.removeFirst();

  ASSERT_EQ(&elem2, &element1);
  ASSERT_EQ(&elem1, &element2);
  ASSERT_FALSE(list.isAvailable());
}

TEST(list, __empty__should_prepend_in_a_empty_but_already_used_list)
{
  List<ListElement> list;
  ListElement element1;
  ListElement element2;
  list.prepend(element1);
  list.prepend(element2);
  list.removeFirst();
  list.removeFirst();

  ListElement element3;
  list.prepend(element3);
  ListElement& elem3 = list.removeFirst();

  ASSERT_EQ(&elem3, &element3);
  ASSERT_FALSE(list.isAvailable());
}

TEST(list, __empty__should_throw_assert_exception_on_remove_first)
{
  List<ListElement> list;

  EXPECT_ANY_THROW(list.removeFirst());
}

TEST(
    list,
    __available__should_throw_assert_exception_when_removing_none_inserted_element)
{
  List<ListElement> list;
  ListElement element;
  ListElement element2;

  list.prepend(element);

  EXPECT_ANY_THROW(list.remove(element2));
}

TEST(list,
     __empty__should_throw_assert_exception_when_removing_none_inserted_element)
{
  List<ListElement> list;
  ListElement element2;

  EXPECT_ANY_THROW(list.remove(element2));
}

TEST(list, __empty__should_remove_single_element)
{
  List<ListElement> list;
  ListElement element;
  list.prepend(element);

  list.remove(element);

  ASSERT_FALSE(list.isAvailable());
}

TEST(list, __empty__should_remove_first_element)
{
  List<ListElement> list;
  ListElement element;
  ListElement element2;
  list.prepend(element);
  list.prepend(element2);

  list.remove(element2);

  ASSERT_EQ(list.numberOfElements(), 1);
  ASSERT_EQ(element2.mNext, nullptr);
}

TEST(list, __empty__should_remove_last_element)
{
  List<ListElement> list;
  ListElement element;
  ListElement element2;
  list.prepend(element);
  list.prepend(element2);

  list.remove(element);

  ASSERT_EQ(list.numberOfElements(), 1);
  ASSERT_EQ(element.mNext, nullptr);
}

TEST(list, __empty__should_remove_element_in_middle)
{
  List<ListElement> list;
  ListElement element;
  ListElement element2;
  ListElement element3;
  list.prepend(element);
  list.prepend(element2);
  list.prepend(element3);

  list.remove(element2);

  ASSERT_EQ(list.numberOfElements(), 2);
  ASSERT_EQ(element2.mNext, nullptr);
}

TEST(list, __empty__should_iterate)
{
  List<ListElement> list;
  auto iter = list.begin();
  bool reached = false;
  while (iter != list.end())
  {
    reached = true;
  }
  ASSERT_FALSE(reached);
}

TEST(list, __1_element__should_iterate)
{
  List<ListElement> list;
  int reached = 0;

  ListElement element;
  list.prepend(element);

  auto iter = list.begin();
  while (iter != list.end())
  {
    if (reached == 0)
    {
      ASSERT_EQ(&*iter, &element);
    }
    else
    {
    }
    reached++;
    ++iter;
  }

  ASSERT_EQ(reached, list.numberOfElements());
}

TEST(list, __2_element__should_iterate)
{
  List<ListElement> list;
  int reached = 0;

  ListElement element;
  ListElement element2;
  list.prepend(element);
  list.prepend(element2);

  auto iter = list.begin();
  while (iter != list.end())
  {
    if (reached == 0)
    {
      ASSERT_EQ(&*iter, &element2);
    }
    else if (reached == 1)
    {
      ASSERT_EQ(&*iter, &element);
    }
    else
    {
    }
    reached++;
    ++iter;
  }

  ASSERT_EQ(reached, list.numberOfElements());
}

TEST(list, __3_element__should_iterate)
{
  List<ListElement> list;
  int reached = 0;

  ListElement element;
  ListElement element2;
  ListElement element3;
  list.prepend(element);
  list.prepend(element2);
  list.prepend(element3);

  auto iter = list.begin();
  while (iter != list.end())
  {
    if (reached == 0)
    {
      ASSERT_EQ(&*iter, &element3);
    }
    else if (reached == 1)
    {
      ASSERT_EQ(&*iter, &element2);
    }
    else if (reached == 2)
    {
      ASSERT_EQ(&*iter, &element);
    }
    else
    {
    }
    reached++;
    ++iter;
  }

  ASSERT_EQ(reached, list.numberOfElements());
}
