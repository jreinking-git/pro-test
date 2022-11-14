#include <gtest/gtest.h>

#include "protest/coro/logical_clock.h"

using namespace protest::coro;

TEST(logical_clock, should_be_start_of_epoch)
{
  LogicalClock clock;

  ASSERT_EQ(clock.now(), protest::time::TimePoint::startOfEpoche());
}

TEST(logical_clock, should_move)
{
  LogicalClock clock;
  clock.moveForward(protest::time::Millisecond(1000u));
  ASSERT_EQ(clock.now().milliseconds(), 1000u);
}

TEST(logical_clock, should_move_to_end_of_epoch)
{
  LogicalClock clock;
  clock.moveToEndOfEpoche();
  ASSERT_EQ(clock.now(), protest::time::TimePoint::endOfEpoche());
}
