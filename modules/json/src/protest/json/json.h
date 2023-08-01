/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2023 Janosch Reinking
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

#include <string>

namespace protest
{

namespace json
{

/**
 * Since nlohmann::json header takes to long to compile this tiny wrapper is
 * used (not only this project, but also each test script would compile 
 * significant longer). It hides all dependencies from nlohmann::json by
 * using void pointer.
 */

class JsonParser;
class Value;

// ---------------------------------------------------------------------------
/**
 * @class Base
 */
class Base
{
public:
  explicit Base(void* handle);

  Base(const Base&) = delete;

  Base(Base&&) noexcept = delete;

  Base&
  operator=(const Base&) = delete;

  Base&
  operator=(Base&&) noexcept = delete;

  ~Base();

protected:
  void* mHandle;
};

// ---------------------------------------------------------------------------
/**
 * @class Object
 */
class Object : Base
{
public:
  explicit Object(void* object);

  Object(const Object&) = delete;

  Object(Object&&) noexcept = delete;

  Object&
  operator=(const Object&) = delete;

  Object&
  operator=(Object&&) noexcept = delete;

  ~Object() = default;

// ---------------------------------------------------------------------------
  bool
  hasKey(std::string key);

  Value
  get(std::string key);

private:
};

// ---------------------------------------------------------------------------
/**
 * @class Array
 */
class Array : Base
{
public:
  explicit Array(void* array);

  Array(const Array&) = delete;

  Array(Array&&) noexcept = delete;

  Array&
  operator=(const Array&) = delete;

  Array&
  operator=(Array&&) noexcept = delete;

  ~Array() = default;

// ---------------------------------------------------------------------------
  size_t
  numberOfElements();

  Value
  get(size_t index);

private:
};

// ---------------------------------------------------------------------------
/**
 * @class Value
 */
class Value : Base
{
public:
  explicit Value(void* value);

  Value(const Value&) = delete;

  Value(Value&&) noexcept = delete;

  Value&
  operator=(const Value&) = delete;

  Value&
  operator=(Value&&) noexcept = delete;

  ~Value() = default;

// ---------------------------------------------------------------------------
  bool
  isObject();

  Object
  getAsObject();

  bool
  isNumber();

  bool
  isInteger();

  int64_t
  getInteger();

  bool
  isArray();

  Array
  getAsArray();

private:
};

// ---------------------------------------------------------------------------
/**
 * @class JsonParser
 */
class JsonParser
{
public:
  explicit JsonParser();

  JsonParser(const JsonParser&) = delete;

  JsonParser(JsonParser&&) noexcept = delete;

  JsonParser&
  operator=(const JsonParser&) = delete;

  JsonParser&
  operator=(JsonParser&&) noexcept = delete;

  ~JsonParser();

// ---------------------------------------------------------------------------
  bool
  parse(std::string file);

  Value
  getValue();

private:
  void* mHandle;
};

} // namespace json

} // namespace protest
