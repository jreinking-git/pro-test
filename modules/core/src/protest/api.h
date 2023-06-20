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

// PROTEST_HIDE is used to hide certain parts of the code which are used
// by the protest-compiler but can cause problems in the last compile stage
// (I.e.: when creating a object file). E.g.:
// 
// PROTEST_HIDE(__attribute__("annotation")) void function() {}
// 
// the __attribute__("annotation") is only visible for the protest-compiler
#ifdef PROTEST_COMPILE_STAGE
#define PROTEST_HIDE(X) X
#else
#define PROTEST_HIDE(X)
#endif

#include "protest/core/api.h"
#include "protest/matcher/matcher.h"
#include "protest/log/operator.h"
#include "protest/mock/traits.h"
#include "protest/mock/mock_base.h"
#include "protest/mock/expectation.h"
#include "protest/mock/in_sequence.h"