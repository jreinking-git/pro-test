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

#include "protest/time/time_point.h"

namespace protest
{

namespace rtos
{

// ---------------------------------------------------------------------------
/**
 * @class SystemClock
 */
class SystemClock
{
public:
  explicit SystemClock();

  SystemClock(const SystemClock&) = delete;

  SystemClock(SystemClock&&) noexcept = delete;

  SystemClock&
  operator=(const SystemClock&) = delete;

  SystemClock&
  operator=(SystemClock&&) noexcept = delete;

  ~SystemClock();

// ---------------------------------------------------------------------------
  time::TimePoint
  now();

private:
  void* mUserdata;
};

} // namespace rtos

} // namespace protest
