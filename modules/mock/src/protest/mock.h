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

#define _STRINGIFY(X) _STRINGIFY2(X)
#define _STRINGIFY2(X) #X

#define _CAT(X, Y) _CAT2(X, Y)
#define _CAT2(X, Y) X##_##Y
#define _CAT_2 _CAT

#define _INCLUDE_FILE(HEAD, TAIL) _STRINGIFY(_CAT_2(HEAD, TAIL))

#include _INCLUDE_FILE(PROTEST_SOURCE_FILE, mocks.hpp)

namespace protest
{

template <typename T>
std::enable_if_t<!protest::mock::HasMockImpl<T>::value, T&>
createMock(protest::meta::CallContext& callContext =
               protest::meta::CallContext::defaultContext())
{
  // should be replaced via template specialization
  assert(false);
  void* ptr = nullptr;
  return *reinterpret_cast<T*>(ptr);
}

template <typename T>
typename protest::mock::ExpectationHandle<typename T::Function>
expectCall(T&& expectation)
{
  auto ptr = expectation.operator std::shared_ptr<internal::ExpectationBase>();
  ptr->getCallContext().markAsExecuted();
  return expectation;
}

namespace mock
{

} // namespace mock

} // namespace protest