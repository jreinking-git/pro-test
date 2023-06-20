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

TEST(can_invoke, should_test_invoke_prefix_tuple_with_int_param)
{
  auto lambda = [](int a)
  {

  };

  ASSERT_TRUE((CanInvokePrefixTuple<decltype(lambda), std::tuple<int>>::value));
  ASSERT_TRUE(
      (CanInvokePrefixTuple<decltype(lambda), std::tuple<int, int>>::value));
  ASSERT_TRUE((CanInvokePrefixTuple<decltype(lambda),
                                    std::tuple<int, int, int>>::value));

  ASSERT_FALSE(
      (CanInvokePrefixTuple<decltype(lambda), std::tuple<std::string>>::value));
  ASSERT_FALSE((CanInvokePrefixTuple<decltype(lambda),
                                     std::tuple<std::string, int>>::value));
  ASSERT_FALSE(
      (CanInvokePrefixTuple<decltype(lambda),
                            std::tuple<std::string, int, int>>::value));
}

TEST(can_invoke, should_test_invoke_prefix_tuple_with_no_param)
{
  auto lambda = []()
  {

  };

  ASSERT_TRUE((CanInvokePrefixTuple<decltype(lambda), std::tuple<int>>::value));
  ASSERT_TRUE(
      (CanInvokePrefixTuple<decltype(lambda), std::tuple<int, int>>::value));
  ASSERT_TRUE((CanInvokePrefixTuple<decltype(lambda),
                                    std::tuple<int, int, int>>::value));

  ASSERT_TRUE(
      (CanInvokePrefixTuple<decltype(lambda), std::tuple<std::string>>::value));
  ASSERT_TRUE((CanInvokePrefixTuple<decltype(lambda),
                                    std::tuple<std::string, int>>::value));
  ASSERT_TRUE((CanInvokePrefixTuple<decltype(lambda),
                                    std::tuple<std::string, int, int>>::value));
}

TEST(can_invoke, should_test_invoke_prefix_tuple_with_std_string_and_int_param)
{
  auto lambda = [](std::string a, int)
  {

  };

  ASSERT_FALSE(
      (CanInvokePrefixTuple<decltype(lambda), std::tuple<int>>::value));
  ASSERT_FALSE(
      (CanInvokePrefixTuple<decltype(lambda), std::tuple<int, int>>::value));
  ASSERT_FALSE((CanInvokePrefixTuple<decltype(lambda),
                                     std::tuple<int, int, int>>::value));

  ASSERT_FALSE(
      (CanInvokePrefixTuple<decltype(lambda), std::tuple<std::string>>::value));
  ASSERT_TRUE((CanInvokePrefixTuple<decltype(lambda),
                                    std::tuple<std::string, int>>::value));
  ASSERT_TRUE((CanInvokePrefixTuple<decltype(lambda),
                                    std::tuple<std::string, int, int>>::value));
}
