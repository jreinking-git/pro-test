#pragma once

#include <iostream>
#include <cstddef>
#include <cstdint>

struct MyStruct
{
  uint8_t mValue = 45;
};

class MyClass
{
public:
  struct MyStruct2
  {
    uint8_t mValue = 46;
  };

  enum Mode
  {
    flight,
    ground,
    idle
  };

private:
  struct MyStruct3
  {
    uint8_t mValue1 = 49;
    uint16_t mValue2 = 50;
  };

  static uint32_t myvalue;
  static uint32_t
  value();
  void
  func()
  {
    return;
  }
  int
  func(int)
  {
    return 44;
  }
  int
  other(int)
  {
    return 45;
  }
  int
  other(double)
  {
    return 46;
  }
  uint8_t mMember1 = 41;
  uint8_t mMember2 = 42;
  uint16_t mMember3 = 43;
  uint8_t& mMember4 = mMember1;
  const uint8_t mMember5 = 44;
  MyStruct mMember6;
  MyStruct2 mMember7;
  MyStruct3 mMember8;
  Mode mMode = Mode::ground;
};

namespace test
{

class MyClass
{
private:
  static uint32_t myvalue;
  static uint32_t
  value();

  int
  func(int)
  {
    return 44;
  }
  uint8_t mMember1 = 41;
};

namespace test2
{

class MyClass
{
private:
  static uint32_t myvalue;
  static uint32_t
  value();

  int
  func(int)
  {
    return 44;
  }
  uint8_t mMember1 = 41;
};

} // namespace test2

} // namespace test