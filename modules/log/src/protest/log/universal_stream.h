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

#include <type_traits>
#include <iostream>

namespace protest
{

namespace log
{

// ---------------------------------------------------------------------------
class Logger;

// ---------------------------------------------------------------------------
/**
 * @class UniversalStream
 * 
 * The universal stream will be used to print any object of any type.
 * Therefore the usage of operator<<(UniversalStream&, const T&) will never
 * lead to compile error. This allows the protest-compiler to compile a 
 * protest-script which uses the universal stream to print values of user
 * defined types. If the protest-compile detects the usage of 
 * operator<<(UniversalStream&, const T&) for T then it tries to generate a
 * printer function which will be injected into the code via template
 * specialisation. These specialisations will be prevert over the default
 * implementation when actually compiling the generate *.protest.cpp file
 * with a c++ compiler.
 */
class UniversalStream
{
public:
  static constexpr size_t maxDepth = 1;

  explicit UniversalStream(std::ostream& ostream);

  UniversalStream(const UniversalStream&) = delete;

  UniversalStream(UniversalStream&&) noexcept = delete;

  UniversalStream&
  operator=(const UniversalStream&) = delete;

  UniversalStream&
  operator=(UniversalStream&&) noexcept = delete;

  ~UniversalStream() = default;

// ---------------------------------------------------------------------------
  /**
   * @brief incrementIndent
   * 
   * Increase the indent.
   * Utility to add indents to nested structures. For example:
   * A printer function for JSON could use it for any nested step of printing
   * a JSON object:
   * 
   * {
   *   "main": {
   *     "test": 43
   *   }
   * }
   */
  void
  incrementIndent();

  void
  incrementIndent(size_t indent);

  /**
   * @brief decreaseIndent
   * 
   * Decreases the indent.
   */
  void
  decrementIndent();

  void
  decrementIndent(size_t indent);

// ---------------------------------------------------------------------------
  /**
   * @brief incrementDepth
   * 
   * Increment depth. The depth will be used to recognize recursion or deeply
   * nested structures when printing.
   */
  void
  incrementDepth();

  /**
   * @brief decrementDepth
   * 
   * Decrement depth. Depth can be negative to allow printers to manipulate the
   * allowed depth.
   */
  void
  decrementDepth();

  /**
   * @brief Get the current depth
   * @return the depth
   */
  uint8_t
  getDepth() const;

// ---------------------------------------------------------------------------
  /**
   * @brief flush
   * 
   * Use this method to flush a coherent block of text. This method should be
   * called at least once before the logger of any other runner is used to
   * prevent that the log output overlaps.
   */
  void
  flush();

  /**
   * @brief Get access to the underlying plain stream
   * 
   * This allows the user to use the stream directly. This will be usally used
   * to by pass the @c UniversalStream. E.g.: for exmaple if the user wants to
   * print a type which has a generated or custom printer function
   * (std::string).
   * 
   * @return the underlying plain stream
   */
  std::ostream&
  getPlain();

// ---------------------------------------------------------------------------
  /**
   * @brief printRecord
   * 
   * This method will be used by the generated printer function (or custom
   * printer function). This method will take care of incrementing and 
   * checking the depth. This prevents endless recursion and to many log
   * output when printing large nested records (class or structs).
   * 
   * @tparam T
   *  type of value to print
   * 
   * @param value 
   *  the value to print
   */
  template <typename T>
  inline void
  printRecord(const T& value);

  /**
   * @brief printIndent
   * 
   * This method will be used by the generated printer function (or custom
   * printer function) to print the indent on the current level.
   */
  void
  printIndent();

public:
  std::ostream& mOutput;
  uint8_t mIndent;
  int8_t mDepth;
};

// ---------------------------------------------------------------------------
template <typename T>
void
UniversalStream::printRecord(const T& value)
{
  if (getDepth() < maxDepth)
  {
    incrementDepth();
    *this << value;
    decrementDepth();
  }
  else
  {
    mOutput << "object@" << reinterpret_cast<const void*>(&value);
  }
  flush();
}

} // namespace log

} // namespace protest
