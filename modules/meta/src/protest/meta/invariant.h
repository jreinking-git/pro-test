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

#include "protest/meta/call_context.h"

namespace protest
{

namespace meta
{

class Unit;

// ---------------------------------------------------------------------------
/**
 * @class Invariant
 */
class Invariant : public CallContext
{
public:
  static Invariant&
  defaultContext();

// ---------------------------------------------------------------------------
  explicit Invariant(Unit& unit,
                     size_t line,
                     const char* condition,
                     std::vector<std::string>&& args,
                     std::map<std::string, std::string>&& comments);

  Invariant(const Invariant&) = delete;

  Invariant(Invariant&&) noexcept = delete;

  Invariant&
  operator=(const Invariant&) = delete;

  Invariant&
  operator=(Invariant&&) noexcept = delete;

  ~Invariant() = default;

// ---------------------------------------------------------------------------
  bool
  wasCreated() const;

  void
  markAsCreated();

  bool
  hold() const;

  void
  markAsNotHold();

private:
  bool mWasCreated;
  bool mHold;
};

} // namespace meta

} // namespace protest
