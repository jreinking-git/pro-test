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

#include "protest/meta/test_manager.h"
#include "protest/log/logger.h"
// #include "protest/core/context.h"

namespace protest
{

namespace core
{

class Context;

}

namespace doc
{

// ---------------------------------------------------------------------------
/**
 * @class DocManager
 */
class DocManager
{
public:
  static constexpr size_t lengthOfSeperator = 79;
  static constexpr size_t maxLengthOfKey = 10;
  static constexpr size_t maxLengthOfKeyHeader = 18;

  explicit DocManager(core::Context& context);

  DocManager(const DocManager&) = delete;

  DocManager(DocManager&&) noexcept = delete;

  DocManager&
  operator=(const DocManager&) = delete;

  DocManager&
  operator=(DocManager&&) noexcept = delete;

  ~DocManager() = default;

// ---------------------------------------------------------------------------
  void
  printPreamble();

  void
  printPostamble();

private:
  void
  printSeperator();

  meta::TestManager& mTestManager;
  core::Context& mContext;
  log::Logger mLogger;
};

} // namespace doc

} // namespace protest
