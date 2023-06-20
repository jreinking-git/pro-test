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

#include "protest/core/runner_raw.h"

namespace protest
{

namespace core
{

template <typename T>
class Signal;

}

namespace core
{

template <typename T>
class QueuePort;

template <typename T>
class SamplePort;

// ---------------------------------------------------------------------------
/**
 * @class PortTag
 */
struct PortTag
{
};

// ---------------------------------------------------------------------------
/**
 * @class AnyPortCreator
 * 
 * This class enables c++ to handle "function overloading by return value".
 * This allows protest to create queue and sample ports with the same creator
 * function.
 */
template <typename T>
class AnyPortCreator
{
public:
  friend class QueuePort<T>;
  friend class SamplePort<T>;

  explicit AnyPortCreator(core::Signal<T>& signal,
                          RunnerRaw* runner);

  AnyPortCreator(const AnyPortCreator&) = default;

  AnyPortCreator(AnyPortCreator&&) noexcept = default;

  AnyPortCreator&
  operator=(const AnyPortCreator&) = default;

  AnyPortCreator&
  operator=(AnyPortCreator&&) noexcept = default;

  ~AnyPortCreator() = default;

private:
  core::Signal<T>& mSignal;
  RunnerRaw* mRunner;
};

// ---------------------------------------------------------------------------
template <typename T>
AnyPortCreator<T>::AnyPortCreator(core::Signal<T>& signal,
                                  RunnerRaw* runner) :
  mSignal(signal),
  mRunner(runner)
{
}

} // namespace core

} // namespace protest
