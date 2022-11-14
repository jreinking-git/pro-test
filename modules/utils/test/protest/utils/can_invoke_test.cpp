#include <gtest/gtest.h>

#include "protest/utils/can_invoke.h"

using namespace protest;

TEST(can_invoke, should_be_invokable)
{
  auto lambda = [](int a)
  {

  };

  ASSERT_TRUE((CanInvoke<decltype(lambda), int>::value));
  ASSERT_TRUE((CanInvoke<decltype(lambda), double>::value));
}

TEST(can_invoke, should_not_be_invokable)
{
  auto lambda = [](int a)
  {

  };

  ASSERT_FALSE((CanInvoke<decltype(lambda), void*>::value));
}
