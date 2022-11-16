#include <gtest/gtest.h>
#include <gmock/gmock.h>

struct A
{
  bool operator ==(const A& other) const
  {
    return this->a == other.a;
  }

  int a;
};

TEST(asdf, asdf)
{
  A a;
  a.a = 42;
  A b;
  b.a = 40;
  ASSERT_THAT(42, testing::Ne(32));
  ASSERT_THAT(a, testing::Eq(b));
}

int main(int argc, char ** argv)
{

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

