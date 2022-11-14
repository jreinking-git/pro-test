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

#include <map>
#include <vector>
#include <string>

#include <cstdint>
#include <cstddef>

namespace protest
{

namespace meta
{

class Unit;

// ---------------------------------------------------------------------------
/**
 * @class CallContext
 */
class CallContext
{
public:
  static CallContext&
  defaultContext();

// ---------------------------------------------------------------------------
  explicit CallContext(Unit& unit,
                       size_t line,
                       const char* objectName,
                       std::vector<std::string>&& args,
                       std::map<std::string, std::string>&& comments = {});

  CallContext(const CallContext&) = delete;

  CallContext(CallContext&&) noexcept = delete;

  CallContext&
  operator=(const CallContext&) = delete;

  CallContext&
  operator=(CallContext&&) noexcept = delete;

  ~CallContext() = default;

// ---------------------------------------------------------------------------
  const Unit&
  getUnit() const;

  size_t
  getLine() const;

  const char*
  getArg(uint8_t arg);

  const char*
  getObjectName();

  bool
  hasComment(const char* comment);

  const char*
  getComment(const char* comment);

private:
  Unit& mUnit;
  size_t mLine;
  const char* mObjectName;
  std::vector<std::string> mArgs;
  std::map<std::string, std::string> mComments;
};

} // namespace meta

} // namespace protest
