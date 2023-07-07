#pragma once

#include <iostream>
#include <cstddef>
#include <cstdint>

class MyClass
{
private:
  static uint32_t myvalue;
  static uint32_t value();
  int func(int) { return 44; }
  int other(int) { return 45; }
  int other(double) { return 46; }
  uint8_t mMember1 = 41;
  uint8_t mMember2 = 42;
  uint16_t mMember3 = 43;
};
