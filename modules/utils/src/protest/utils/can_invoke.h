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

#include <utility>

namespace protest
{

// ---------------------------------------------------------------------------
template <typename>
using Void = void;

template <typename Sig, typename = void>
struct CanInvoke2 : std::false_type
{
};

template <typename F, typename... Args>
struct CanInvoke2<F(Args...),
                  Void<decltype(std::declval<F>()(std::declval<Args>()...))>> :
  std::true_type
{
};

template <typename F, typename... Args>
struct CanInvoke : CanInvoke2<F(Args&...)>
{
};

// ---------------------------------------------------------------------------
template <typename F, typename Args>
struct CanInvokeTuple : std::false_type
{
};

template <typename F, typename... Args>
struct CanInvokeTuple<F, std::tuple<Args...>> : CanInvoke2<F(Args&...)>
{
};

// ---------------------------------------------------------------------------
template <typename, typename>
struct Concat
{
};

template <typename... Ts, typename... Us>
struct Concat<std::tuple<Ts...>, std::tuple<Us...>>
{
  using type = std::tuple<Ts..., Us...>;
};

// ---------------------------------------------------------------------------
template <class T, class... Args>
struct RemoveLast
{
};

template <class T>
struct RemoveLast<std::tuple<T>>
{
  using type = std::tuple<>;
};

template <class T, class... Args>
struct RemoveLast<std::tuple<T, Args...>>
{
  using type =
      typename Concat<std::tuple<T>,
                      typename RemoveLast<std::tuple<Args...>>::type>::type;
};

// ---------------------------------------------------------------------------
template <typename T1, typename T2, bool>
struct Ite;

template <typename T1, typename T2>
struct Ite<T1, T2, true> : T1
{
};

template <typename T1, typename T2>
struct Ite<T1, T2, false> : T2
{
};

// ---------------------------------------------------------------------------
template <typename F, typename Args>
struct CanInvokePrefixTupleInternal :
  Ite<std::true_type,
      CanInvokePrefixTupleInternal<F, typename RemoveLast<Args>::type>,
      CanInvokeTuple<F, Args>::value>
{
};

template <typename F>
struct CanInvokePrefixTupleInternal<F, std::tuple<>> :
  CanInvokeTuple<F, std::tuple<>>
{
};

// ---------------------------------------------------------------------------
template <typename F, typename Args>
struct CanInvokePrefixTuple : std::false_type
{
};

template <typename F, typename... Args>
struct CanInvokePrefixTuple<F, std::tuple<Args...>> :
  CanInvokePrefixTupleInternal<F, std::tuple<Args...>>
{
};

} // namespace protest
