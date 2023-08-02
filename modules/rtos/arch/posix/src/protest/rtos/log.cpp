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

#include "protest/rtos/log.h"

#include <iostream>

#include <cstdarg>
#include <cassert>

using namespace protest::rtos;

void// NOLINTNEXTLINE
protest::rtos::_info(const char* /* file */,
                     size_t /* line */,
                     const char* format,
                     ...)
{
  va_list argp;
  va_start(&argp[0], format);
  printf("INFO: ");
  const int ret = vprintf(format, &argp[0]);
  assert(ret >= 0);
  printf("\n");
  va_end(&argp[0]);
}

void// NOLINTNEXTLINE
protest::rtos::_warn(const char* /* file */,
                     size_t /* line */,
                     const char* format,
                     ...)
{
  va_list argp;
  va_start(&argp[0], format);
  printf("WARN: ");
  const int ret = vprintf(format, &argp[0]);
  assert(ret >= 0);
  printf("\n");
  va_end(&argp[0]);
}
