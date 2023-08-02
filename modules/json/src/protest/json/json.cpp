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

#include "protest/json/json.h"

#include <nlohmann/json.hpp>

#include <fstream>
#include <iostream>

#include <cassert>

using namespace protest::json;

// ---------------------------------------------------------------------------
Base::Base(void* handle) : mHandle(nullptr)
{
  assert(handle);
  auto* input = reinterpret_cast<nlohmann::json*>(handle);
  auto* json = new nlohmann::json();
  assert(json);
  *json = *input;
  mHandle = json;
}

Base::~Base()
{
  auto* json = reinterpret_cast<nlohmann::json*>(mHandle);
  delete json;
}

// ---------------------------------------------------------------------------
Object::Object(void* object) : Base(object)
{
}

// ---------------------------------------------------------------------------
bool
Object::hasKey(std::string key)
{
  auto* json = reinterpret_cast<nlohmann::json*>(mHandle);
  return json->contains(key);
}

Value
Object::get(std::string key)
{
  auto* json = reinterpret_cast<nlohmann::json*>(mHandle);
  auto value = (*json)[key];
  assert(&value);
  return Value(&value);
}

// ---------------------------------------------------------------------------
Array::Array(void* array) : Base(array)
{
}

// ---------------------------------------------------------------------------
size_t
Array::numberOfElements()
{
  auto* json = reinterpret_cast<nlohmann::json*>(mHandle);
  return json->size();
}

Value
Array::get(size_t index)
{
  auto* json = reinterpret_cast<nlohmann::json*>(mHandle);
  auto child = (*json)[index];
  return Value(&child);
}

// ---------------------------------------------------------------------------
Value::Value(void* value) : Base(value)
{
  assert(value);
}

// ---------------------------------------------------------------------------
bool
Value::isObject()
{
  auto* json = reinterpret_cast<nlohmann::json*>(mHandle);
  return json->is_object();
}

Object
Value::getAsObject()
{
  return Object(mHandle);
}

bool
Value::isNumber()
{
  auto* json = reinterpret_cast<nlohmann::json*>(mHandle);
  return json->is_number();
}

bool
Value::isInteger()
{
  auto* json = reinterpret_cast<nlohmann::json*>(mHandle);
  return json->is_number_integer();
}

int64_t
Value::getInteger()
{
  auto* json = reinterpret_cast<nlohmann::json*>(mHandle);
  static_assert(std::is_same_v<nlohmann::json::number_integer_t, int64_t>);
  return *json;
}

bool
Value::isArray()
{
  auto* json = reinterpret_cast<nlohmann::json*>(mHandle);
  return json->is_array();
}

Array
Value::getAsArray()
{
  auto* json = reinterpret_cast<nlohmann::json*>(mHandle);
  return Array(json);
}

// ---------------------------------------------------------------------------
JsonParser::JsonParser() : mHandle(nullptr)
{
  auto* json = new nlohmann::json();
  mHandle = json;
  assert(json);
}

JsonParser::~JsonParser()
{
  auto* json = reinterpret_cast<nlohmann::json*>(mHandle);
  delete json;
}

// ---------------------------------------------------------------------------
bool
JsonParser::parse(std::string file)
{
  auto* json = reinterpret_cast<nlohmann::json*>(mHandle);
  std::ifstream input(file);
  if (input)
  {
    *json = nlohmann::json::parse(input);
    return true;
  }
  else
  {
    std::invalid_argument("Invalid path to file");
  }
  return false;
}

Value
JsonParser::getValue()
{
  auto* json = reinterpret_cast<nlohmann::json*>(mHandle);
  return Value(mHandle);
}
