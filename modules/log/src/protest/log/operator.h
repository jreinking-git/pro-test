// ---------------------------------------------------------------------------
/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2022 Janosch Reinking
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to 
 * deal in the Software without restriction, including without limitation the 
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#pragma once

#include "protest/log/universal_stream.h"
#include "protest/log/traits.h"

#include <iomanip>

namespace protest
{

namespace log
{

// ---------------------------------------------------------------------------
template <typename T>
struct UnsignedTypeOf
{
};

template <>
struct UnsignedTypeOf<int8_t>
{
  using type = uint8_t;
};

template <>
struct UnsignedTypeOf<int16_t>
{
  using type = uint16_t;
};

template <>
struct UnsignedTypeOf<int32_t>
{
  using type = uint32_t;
};

template <>
struct UnsignedTypeOf<int64_t>
{
  using type = uint64_t;
};

// ---------------------------------------------------------------------------
template <typename T>
std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T>,
                 UniversalStream&>
toStringCustom(UniversalStream& stream0, const T& value)
{
  static_assert(sizeof(T) <= sizeof(unsigned long long));
  std::stringstream ss;
  ss << std::setfill('0') << std::hex << std::setw(sizeof(T) * 2)
     << static_cast<unsigned long long>(value);
  stream0.mOutput << "{ " << static_cast<unsigned long long>(value) << ", 0x"
                  << ss.str() << " }";
  stream0.flush();
  return stream0;
}

template <typename T>
std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>, UniversalStream&>
toStringCustom(UniversalStream& stream0, const T& value)
{
  static_assert(sizeof(T) <= sizeof(long long));
  std::stringstream ss;
  ss << std::setfill('0') << std::hex << std::setw(sizeof(T) * 2)
     << static_cast<long long>(
            static_cast<typename UnsignedTypeOf<T>::type>(value));
  stream0.mOutput << "{ " << static_cast<long long>(value) << ", 0x" << ss.str()
                  << " }";
  stream0.flush();
  return stream0;
}

template <typename T>
std::enable_if_t<std::is_floating_point_v<T>, UniversalStream&>
toStringCustom(UniversalStream& stream0, const T& value)
{
  std::stringstream ss;
  stream0.mOutput << "{ " << value << "f }";
  stream0.flush();
  return stream0;
}

// ---------------------------------------------------------------------------
template <typename T>
std::enable_if_t<std::is_same_v<std::string, T>, UniversalStream&>
toStringCustom(UniversalStream& stream, const T& value)
{
  stream.getPlain() << "\"" << value << "\"";
  stream.flush();
  return stream;
}

// ---------------------------------------------------------------------------
/**
 * Template specialization for types which have an toStringCustom function.
 * This specialization has the highest priority among the specializations.
 */
template <typename T>
std::enable_if_t<HasCustomOperator<std::remove_reference_t<T>>::value,
                 UniversalStream&>
operator<<(UniversalStream& stream, const T& value)
{
  toStringCustom(stream, value);
  stream.flush();
  return stream;
}

template <typename T>
std::enable_if_t<HasCustomOperator<std::remove_reference_t<T>>::value,
                 UniversalStream&>
operator<<(UniversalStream& stream, const T&& value)
{
  toStringCustom(stream, value);
  stream.flush();
  return stream;
}

// ---------------------------------------------------------------------------
/**
 * Template specialization for types which have an toStringCustom function.
 * This specialization has the highest priority among the specializations.
 */
template <typename T>
std::enable_if_t<HasOutputOperator<std::remove_reference_t<T>>::value &&
                     !HasCustomOperator<std::remove_reference_t<T>>::value &&
                     !std::is_pointer_v<std::remove_reference_t<T>>,
                 UniversalStream&>
operator<<(UniversalStream& stream, const T& value)
{
  stream.mOutput << value;
  stream.flush();
  return stream;
}

template <typename T>
std::enable_if_t<HasOutputOperator<std::remove_reference_t<T>>::value &&
                     !HasCustomOperator<std::remove_reference_t<T>>::value &&
                     !std::is_pointer_v<std::remove_reference_t<T>>,
                 UniversalStream&>
operator<<(UniversalStream& stream, const T&& value)
{
  stream.mOutput << value;
  stream.flush();
  return stream;
}

// ---------------------------------------------------------------------------
/**
 * This template specialization has the lowest priority. It print any type
 * which could not use one of the previouse specializations. If the 
 * protest-compiler detects the usage of this operator it tries to generate a
 * @c toString funktion for the given type. If succesful, on the actual call
 * of the c++ compiler one of the other specialization will be used. Therefore
 * this specialization will only be used rarly in a test execution.
 * 
 * @warning the protest-compiler recognizes the usage by locking at the name of
 * the stream (E.g.: stream2). Therefore it should not be change!
 */

template <typename T>
UniversalStream&
operator<<(UniversalStream& stream, const T* value)
{
  if (value)
  {
    stream.mOutput << "@" << static_cast<const void*>(value) << " -> ";
    stream << (const T&) (*value);
  }
  else
  {
    stream.mOutput << "{ nullptr }";
  }
  stream.flush();
  return stream;
}

template <typename T>
UniversalStream&
operator<<(UniversalStream& stream, const T** value)
{
  if (value)
  {
    stream.mOutput << "{ object@" << (void*) value << " }";
  }
  else
  {
    stream.mOutput << "{ nullptr }";
  }
  stream.flush();
  return stream;
}

template <>
UniversalStream&
operator<<(UniversalStream& stream, const char* value);

template <typename T>
std::enable_if_t<!HasCustomOperator<std::remove_reference_t<T>>::value &&
                     !HasOutputOperator<std::remove_reference_t<T>>::value &&
                     !HasProtestOperator<std::remove_reference_t<T>>::value &&
                     !std::is_pointer_v<std::remove_reference_t<T>>,
                 UniversalStream&>

operator<<(UniversalStream& stream2, const T& value)
{
  stream2.mOutput << "{ object@" << &value << " }";
  stream2.flush();
  return stream2;
}

template <typename T>
std::enable_if_t<!HasCustomOperator<std::remove_reference_t<T>>::value &&
                     !HasOutputOperator<std::remove_reference_t<T>>::value &&
                     !HasProtestOperator<std::remove_reference_t<T>>::value &&
                     !std::is_pointer_v<std::remove_reference_t<T>>,
                 UniversalStream&>

operator<<(UniversalStream& stream2, const T&& value)
{
  stream2.mOutput << "{ object@" << &value << " }";
  stream2.flush();
  return stream2;
}

} // namespace log

} // namespace protest