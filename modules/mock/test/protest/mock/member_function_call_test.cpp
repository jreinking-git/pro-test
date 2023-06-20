#include <gtest/gtest.h>
#include <gmock/gmock.h>

class Turtle
{
public:
  Turtle() = default;
  virtual ~Turtle()
  {
  }
  virtual int
  myFunction(int, double) = 0;
};

class MockTurtle : public Turtle
{
public:
  MockTurtle() = default;
  virtual ~MockTurtle() = default;
  MOCK_METHOD2(myFunction, int(int, double));
};

TEST(FunctionMocker, willOnce0)
{
  MockTurtle turtle;

  testing::InSequence outerSeq;

  {
    testing::InSequence seq;
    EXPECT_CALL(turtle, myFunction(testing::Gt(40), 42));
    EXPECT_CALL(turtle, myFunction(43, 43));
  }
  EXPECT_CALL(turtle, myFunction(42, 42));

  turtle.myFunction(42, 42);
  turtle.myFunction(43, 43);
  turtle.myFunction(42, 42);
}
