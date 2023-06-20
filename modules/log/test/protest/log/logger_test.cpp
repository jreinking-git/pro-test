#include <protest/time/time_point.h>

#include <gtest/gtest.h>

#include <type_traits>
#include <iostream>
#include <iomanip>

enum class Number
{
  first,
  second,
  third
};

template <typename T, typename = void>
struct has_output_operator : std::false_type
{
};

template <typename T>
struct has_output_operator<
    T,
    std::void_t<decltype(std::cout << std::declval<T>())>> : std::true_type
{
};

/**
 * @class OStream
 */
class OStream
{
public:
  explicit OStream() = default;

  OStream(const OStream& other) = delete;

  OStream(OStream&& other) = delete;

  OStream&
  operator=(const OStream& other) = delete;

  OStream&
  operator=(OStream&& other) = delete;

  ~OStream() = default;

private:
};

template <typename T>
OStream&
operator<<(OStream& out, const T& value)
{
  if constexpr (has_output_operator<T>::value)
  {
    std::cout << value << std::endl;
  }
  else
  {
    std::cout << "default" << std::endl;
  }
  return out;
}

template <>
OStream&
operator<< <Number>(OStream& out, const Number& value)
{
  if (value == Number::first)
  {
    std::cout << "first" << std::endl;
  }
  else if (value == Number::second)
  {
    std::cout << "second" << std::endl;
  }
  else if (value == Number::third)
  {
    std::cout << "third" << std::endl;
  }
  else
  {
    assert(false);
  }
  return out;
}

TEST(logger, init)
{
  OStream os;
  os << 4;
  os << Number::first;
  os << Number::second;
  assert(false);
}