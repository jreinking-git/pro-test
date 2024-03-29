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

namespace protest
{

namespace log
{

/**
 * Template specialization for types which have a @c toString function 
 * generated by the protest-compiler. Since the generated @c toStrings
 * functions are available only after the protest-compiler processes a source
 * file the header should not be directly include into the pt script. The
 * protest-compiler itself will add a '#include' to the compiled pt script, so
 * that it is available when acutally invoking the c++ compiler.
 */
template <typename T>
std::enable_if_t<!HasCustomOperator<T>::value &&
                     HasProtestOperator<std::remove_reference_t<T>>::value,
                 UniversalStream&>
operator<<(UniversalStream& stream1, const T& value)
{
  toString(stream1, value);
  return stream1;
}

template <typename T>
std::enable_if_t<!HasCustomOperator<T>::value &&
                     HasProtestOperator<std::remove_reference_t<T>>::value,
                 UniversalStream&>
operator<<(UniversalStream& stream1, const T&& value)
{
  toString(stream1, value);
  return stream1;
}

} // namespace log

} // namespace protest
