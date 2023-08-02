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

#include "protest/coro/coroutine_base.h"

// #include "valgrind.h"

#include <iostream>
#include <cstdint>
#include <cassert>
#include <ucontext.h>

using namespace protest::coro;

// ---------------------------------------------------------------------------
static void
wrapper(int arg1, int arg2)
{
  void* thread = nullptr;
  auto* ptr = reinterpret_cast<uint32_t*>(&thread);
  ptr[0] = arg1;
  ptr[1] = arg2;

  auto* thd = reinterpret_cast<CoroutineBase*>(thread);
  thd->coroRun();
}

// ---------------------------------------------------------------------------
void
CoroutineBase::swapContext(CoroContext* first, CoroContext* second)
{
  swapcontext(first, second);
}

void
CoroutineBase::initialize(CoroContext* context)
{
  getcontext(context);
}

// ---------------------------------------------------------------------------
CoroutineBase::CoroutineBase(CoroContext* /* parent */) : mContext {0}
{
  ::memset(&mContext, 0, sizeof(mContext));
  getcontext(&mContext);
  mContext.uc_link = nullptr;
  // NOLINTNEXTLINE
  mContext.uc_stack.ss_sp = malloc(stackSize);
  assert(mContext.uc_stack.ss_sp);
  mContext.uc_stack.ss_size = stackSize;
  CoroutineBase* self = this;
  auto* ptr = reinterpret_cast<uint32_t*>(&self);
  makecontext(&this->mContext,
              reinterpret_cast<void (*)()>(wrapper),
              2,
              static_cast<int>(ptr[0]),
              static_cast<int>(ptr[1]));
}

CoroutineBase::~CoroutineBase()
{
  // NOLINTNEXTLINE
  free(mContext.uc_stack.ss_sp);
  mContext.uc_stack.ss_sp = nullptr;
}
