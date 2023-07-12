#include "my_class.h"

uint32_t MyClass::myvalue = 42;
uint32_t test::MyClass::myvalue = 42;
uint32_t test::test2::MyClass::myvalue = 42;

uint32_t MyClass::value()
{
  return 40;
}

uint32_t test::MyClass::value()
{
  return 40;
}

uint32_t test::test2::MyClass::value()
{
  return 40;
}