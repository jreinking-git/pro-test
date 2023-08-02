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

namespace internal
{

// ---------------------------------------------------------------------------
/**
 * @brief UnsignedTypeOf
 * 
 * Mapping from signed to unsigned types. E.g.: int8_t -> uint8_t
 * 
 * @tparam T
 *   type to map
 */
template <typename T>
struct UnsignedTypeOf
{
};

// ---------------------------------------------------------------------------
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
template <>
struct UnsignedTypeOf<uint8_t>
{
  using type = uint8_t;
};

template <>
struct UnsignedTypeOf<uint16_t>
{
  using type = uint16_t;
};

template <>
struct UnsignedTypeOf<uint32_t>
{
  using type = uint32_t;
};

template <>
struct UnsignedTypeOf<uint64_t>
{
  using type = uint64_t;
};

// ---------------------------------------------------------------------------
template <typename T>
inline void
toStringIntegralHex(std::stringstream& sstream, const T& value)
{
  sstream << "0x";
  sstream << std::setfill('0') << std::hex << std::setw(sizeof(T) * 2);
  sstream << static_cast<unsigned long long>(
      static_cast<typename UnsignedTypeOf<T>::type>(value));
}

} // namespace internal

// ---------------------------------------------------------------------------
/**
 * @brief toStringCustom
 * 
 * Custom printer function for integral types.
 * 
 * @tparam T
 *   type of the integral to print
 * 
 * @param stream0
 *   stream to print the integral
 * 
 * @param value 
 *   the value to print
 * 
 * @return the stream
 */
template <typename T>
inline std::enable_if_t<std::is_integral_v<T>, UniversalStream&>
toStringCustom(UniversalStream& stream0, const T& value)
{
  static_assert(sizeof(T) <= sizeof(long long));
  std::stringstream ss;
  internal::toStringIntegralHex(ss, value);
  stream0.mOutput << "{ " << static_cast<long long>(value) << ", " << ss.str()
                  << " }";
  stream0.flush();
  return stream0;
}

// ---------------------------------------------------------------------------
/**
 * @brief toStringCustom
 * 
 * Custom printer function for floating point types
 * 
 * @param stream0 
 *   stream to print the floating point number
 * 
 * @param value
 *   the value to print
 * 
 * @return the stream
 */
template <typename T>
inline std::enable_if_t<std::is_floating_point_v<T>, UniversalStream&>
toStringCustom(UniversalStream& stream0, const T& value)
{
  std::stringstream ss;
  stream0.mOutput << "{ " << value << "f }";
  stream0.flush();
  return stream0;
}

// ---------------------------------------------------------------------------
/**
 * @brief toStringCustom
 * 
 * Custom printer function for std::string
 * 
 * @param stream0 
 *   stream to print the std::string
 * 
 * @param value
 *   the value to print
 * 
 * @return the stream
 */
template <typename T>
inline std::enable_if_t<std::is_same_v<std::string, T>, UniversalStream&>
toStringCustom(UniversalStream& stream, const T& value)
{
  stream.getPlain() << "\"" << value << "\"";
  stream.flush();
  return stream;
}

// ---------------------------------------------------------------------------
template <typename T>
inline constexpr bool firstLevel =
    HasCustomOperator<std::remove_reference_t<T>>::value;

/**
 * @brief operator<<
 * 
 * Template specialization for types which have an toStringCustom function.
 * This specialization has the highest priority among the specializations.
 * 
 * @tparam T
 *   type of the value to print
 * 
 * @param stream 
 *   stream to print the value
 * 
 * @param value 
 *   the value to print
 * 
 * @return the stream
 */
template <typename T>
std::enable_if_t<firstLevel<T>, UniversalStream&>
operator<<(UniversalStream& stream, const T& value)
{
  toStringCustom(stream, value);
  stream.flush();
  return stream;
}

template <typename T>
std::enable_if_t<firstLevel<T>, UniversalStream&>
operator<<(UniversalStream& stream, const T&& value)
{
  toStringCustom(stream, value);
  stream.flush();
  return stream;
}

// ---------------------------------------------------------------------------
// pointers and array types can be printed via std::iostream
// -> HasOutputOperator is always true
// exclude them from this level so that they can be printed as array (every
// element) or as pointer to value (where the value might be printed via
// universal stream). This allows to use a toStringCustom or a generated
// printer function for dereferenced pointer type.
template <typename T>
inline constexpr bool secondLevel =
    HasOutputOperator<std::remove_reference_t<T>>::value &&
    !HasCustomOperator<std::remove_reference_t<T>>::value &&
    !std::is_pointer_v<std::remove_reference_t<T>> &&
    !std::is_enum_v<std::remove_reference_t<T>> &&
    !std::is_array_v<std::remove_reference_t<T>>;

/**
 * @brief operator<<
 * 
 * This specialization is used for types which has no custom printer function
 * but can be directly printed to a an std::iostream. Therefore if for the
 * given type a std::iostream& operator<<(std::iostream&, T value) exists, it
 * will be used. This specialisation has the second highest priority.
 * 
 * Arrays and pointer will be execuded since there get an extrac treatment.
 * 
 * @tparam T
 *   the type of the value to print
 * 
 * @param stream
 *   the stream to print to
 * 
 * @param value
 *   the value to print
 * 
 * @return the stream
 */
template <typename T>
std::enable_if_t<secondLevel<T>, UniversalStream&>
operator<<(UniversalStream& stream, const T& value)
{
  stream.mOutput << value;
  stream.flush();
  return stream;
}

template <typename T>
std::enable_if_t<secondLevel<T>, UniversalStream&>
operator<<(UniversalStream& stream, const T&& value)
{
  stream.mOutput << value;
  stream.flush();
  return stream;
}

// ---------------------------------------------------------------------------

namespace internal
{

template <typename T, size_t N>
void
hexDumpArray(UniversalStream& stream, const T* value)
{
  static constexpr size_t maxNumberBytes = 128;

  stream.mOutput << "[" << N << "] {\n";
  stream.incrementIndent();
  auto indent = stream.getIndent();
  int numberPerLine = ((79 - indent) + 1 /* no space for the last one */) /
                      ((2 * sizeof(T)) + 2 /* 0x */ + 1 /* space */);
  for (int i = numberPerLine; i >= (numberPerLine / 2) && i > 0; i--)
  {
    if (N % i == 0)
    {
      numberPerLine = i;
      break;
    }
    else
    {
    }
  }

  bool firstLine = true;
  bool firstPerLine = true;
  for (int i = 0; i < N; i++)
  {
    if ((i % numberPerLine == 0))
    {
      if (firstLine)
      {
        stream.printIndent();
        firstLine = false;
      }
      else
      {
        stream.mOutput << "\n";
        stream.printIndent();
      }
      firstPerLine = true;
    }
    else
    {
    }

    if (firstPerLine)
    {
      firstPerLine = false;
    }
    else
    {
      stream.mOutput << " ";
    }

    if (i == maxNumberBytes && N > maxNumberBytes)
    {
      stream.mOutput << "...";
    }
    else
    {
      std::stringstream sstream;
      toStringIntegralHex(sstream, value[i]);
      stream.mOutput << sstream.str();
    }
  }
  stream.decrementIndent();
  stream.mOutput << "\n";
  stream.printIndent();
  stream.mOutput << "}";
}

// ---------------------------------------------------------------------------
template <typename T, size_t N>
std::enable_if_t<!std::is_integral_v<T>, UniversalStream&>
toStringArray(UniversalStream& stream, const T (&value)[N])
{
  stream.mOutput << "{\n";
  stream.incrementIndent();
  bool first = true;
  for (int i = 0; i < N; i++)
  {
    if (first)
    {
      first = false;
    }
    else
    {
      stream.mOutput << ",\n";
    }
    stream.printIndent();
    stream << value[i];
  }
  stream.decrementIndent();
  stream.printIndent();
  stream.mOutput << "}\n";
  return stream;
}

template <typename T, size_t N>
std::enable_if_t<std::is_integral_v<T>, UniversalStream&>
toStringArray(UniversalStream& stream, const T (&value)[N])
{
  hexDumpArray<T, N>(stream, value);
  return stream;
}

// ---------------------------------------------------------------------------
template <typename T>
std::enable_if_t<!std::is_same_v<std::remove_reference_t<T>, const char*>,
                 UniversalStream&>
toStringPointer(UniversalStream& stream, const T* value)
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
std::enable_if_t<std::is_same_v<std::remove_reference_t<T>, const char*>,
                 UniversalStream&>
toStringPointer(UniversalStream& stream, T& value)
{
  stream.mOutput << "\"" << value << "\"";
  stream.flush();
  return stream;
}

// ---------------------------------------------------------------------------
template <typename T>
UniversalStream&
printUnknownObject(UniversalStream& stream2, const T& value)
{
  stream2.mOutput << "object@" << &value;
  auto objectAsPointer = reinterpret_cast<const uint8_t*>(&value);
  hexDumpArray<uint8_t, sizeof(T)>(stream2, objectAsPointer);
  return stream2;
}

} // namespace internal

// ---------------------------------------------------------------------------
/**
 * @brief operator<<
 * 
 * The specialization with the lowest priority. It will be used for all types
 * which cannot be printed to a std::iostream and has no custom printer
 * function. If the protest-compiler sees a call to this function in the AST
 * it tries to generated a printer function for the given type (expect for
 * pointer and array types).
 * 
 * Pointers and array types are explicit handled in this function. Without
 * this special handling it is not possible for C++ to distinguish between
 * pointer and arrays since arrays can be implicit convertable to the
 * corresponding pointer type.
 * 
 * @tparam T
 *   the type of the value to print
 * 
 * @param stream 
 *   the stream to print to
 * 
 * @param value 
 *   the value to print
 * 
 * @return the stream
 */
template <typename T>
std::enable_if_t<(!HasProtestOperator<std::remove_reference_t<T>>::value &&
                  !HasCustomOperator<std::remove_reference_t<T>>::value),
                 UniversalStream&>
operator<<(UniversalStream& stream, const T& value)
{
  if constexpr (std::is_array_v<T>)
  {
    internal::toStringArray(stream, value);
  }
  else if constexpr (std::is_pointer_v<T>)
  {
    internal::toStringPointer(stream, value);
  }
  else
  {
    // try to generate printer funiction via the protest-compiler
    // if not possible -> print hex dump
    internal::printUnknownObject(stream, value);
  }
  return stream;
}

template <typename T>
std::enable_if_t<(!HasProtestOperator<std::remove_reference_t<T>>::value &&
                  !HasCustomOperator<std::remove_reference_t<T>>::value),
                 UniversalStream&>
operator<<(UniversalStream& stream, const T&& value)
{
  if constexpr (std::is_array_v<T>)
  {
    internal::toStringArray(stream, value);
  }
  else if constexpr (std::is_pointer_v<T>)
  {
    internal::toStringPointer(stream, value);
  }
  else
  {
    // try to generate printer funiction via the protest-compiler
    // if not possible -> print hex dump
    internal::printUnknownObject(stream, value);
  }
  return stream;
}

} // namespace log

} // namespace protest