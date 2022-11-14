#include <gtest/gtest.h>

#include "protest/time/duration.h"

using namespace protest::time;

TEST(duration, should_create_infinity)
{
  auto inf = Duration::infinity();
  ASSERT_TRUE(inf > Duration::zero());
  inf += Duration::smallestNonZero();
  ASSERT_TRUE(inf < Duration::zero());
}

TEST(duration, should_create_zero)
{
  ASSERT_EQ(Duration::zero().nanoseconds(), 0);
}

TEST(duration, should_create_smallest_non_zero)
{
  ASSERT_EQ(Duration::zero().nanoseconds(), 0);
}

TEST(duration, should_not_be_unequal)
{
  ASSERT_FALSE(Nanoseconds(1) != Nanoseconds(1));
}

TEST(duration, should_be_unequal)
{
  ASSERT_TRUE(Nanoseconds(1) != Nanoseconds(2));
}

TEST(duration, should_be_equal)
{
  ASSERT_TRUE(Nanoseconds(1) == Nanoseconds(1));
}

TEST(duration, should_not_be_equal)
{
  ASSERT_FALSE(Nanoseconds(1) == Nanoseconds(2));
}

TEST(duration, should_be_less)
{
  ASSERT_TRUE(Nanoseconds(1) < Nanoseconds(2));
}

TEST(duration, should_not_be_less)
{
  ASSERT_FALSE(Nanoseconds(1) < Nanoseconds(1));
}

TEST(duration, should_be_greater)
{
  ASSERT_TRUE(Nanoseconds(2) > Nanoseconds(1));
}

TEST(duration, should_not_be_greater)
{
  ASSERT_FALSE(Nanoseconds(1) > Nanoseconds(1));
}

TEST(duration, should_be_less_or_equals1)
{
  ASSERT_TRUE(Nanoseconds(1) <= Nanoseconds(2));
}

TEST(duration, should_be_less_or_equals2)
{
  ASSERT_TRUE(Nanoseconds(1) <= Nanoseconds(1));
}

TEST(duration, should_not_be_less_or_equals)
{
  ASSERT_FALSE(Nanoseconds(1) <= Nanoseconds(0));
}

TEST(duration, should_be_greater_or_equals1)
{
  ASSERT_TRUE(Nanoseconds(2) >= Nanoseconds(1));
}

TEST(duration, should_be_greater_or_equals2)
{
  ASSERT_TRUE(Nanoseconds(1) >= Nanoseconds(1));
}

TEST(duration, should_not_be_greater_or_equals)
{
  ASSERT_FALSE(Nanoseconds(0) >= Nanoseconds(1));
}

TEST(duration, should_add)
{
  ASSERT_TRUE(Nanoseconds(1) + Nanoseconds(1) == Nanoseconds(2));
}

TEST(duration, should_minus)
{
  ASSERT_TRUE(Nanoseconds(1) - Nanoseconds(1) == Nanoseconds(0));
}

TEST(duration, should_add_and_assign)
{
  auto duration = Nanoseconds(1);
  duration += Nanoseconds(1);
  ASSERT_TRUE(duration == Nanoseconds(2));
}

TEST(duration, should_minus_and_assign)
{
  auto duration = Nanoseconds(1);
  duration -= Nanoseconds(1);
  ASSERT_TRUE(duration == Nanoseconds(0));
}

TEST(duration, should_convert)
{
  auto duration = Nanoseconds(1000000000000000);
  ASSERT_EQ(duration.nanoseconds(), 1000000000000000);
  ASSERT_EQ(duration.microseconds(), 1000000000000);
  ASSERT_EQ(duration.milliseconds(), 1000000000);
  ASSERT_EQ(duration.seconds(), 1000000);
  ASSERT_EQ(duration.minutes(), 16666);
  ASSERT_EQ(duration.hours(), 277);
  ASSERT_EQ(duration.days(), 11);
}

TEST(duration, should_create)
{
  ASSERT_EQ(Nanoseconds(1u).nanoseconds(), 1u);
  ASSERT_EQ(Microseconds(1), Nanoseconds(1000));
  ASSERT_EQ(Millisecond(1), Microseconds(1000));
  ASSERT_EQ(Seconds(1), Millisecond(1000));
  ASSERT_EQ(Minutes(1), Seconds(60));
  ASSERT_EQ(Hours(1), Minutes(60));
  ASSERT_EQ(Days(1), Hours(24));
}

TEST(duration, should_create2)
{
  ASSERT_EQ(1_ns, Nanoseconds(1));
  ASSERT_EQ(1_us, Nanoseconds(1000));
  ASSERT_EQ(1_ms, Microseconds(1000));
  ASSERT_EQ(1_s, Millisecond(1000));
  ASSERT_EQ(1_min, Seconds(60));
  ASSERT_EQ(1_h, Minutes(60));
  ASSERT_EQ(1_days, Hours(24));
}
