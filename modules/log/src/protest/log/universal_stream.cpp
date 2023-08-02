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

#include "protest/log/operator.h"
#include "protest/log/logger.h"

using namespace protest::log;

// ---------------------------------------------------------------------------
UniversalStream::UniversalStream(std::ostream& ostream) :
  mOutput(ostream),
  mIndent(0),
  mDepth(0)
{
}

// ---------------------------------------------------------------------------
void
UniversalStream::incrementIndent()
{
  mIndent += 2;
  assert(mIndent >= 2);
}

void
UniversalStream::incrementIndent(size_t indent)
{
  mIndent += indent;
  assert(mIndent >= indent);
}

void
UniversalStream::decrementIndent()
{
  assert(mIndent >= 2);
  mIndent -= 2;
}

void
UniversalStream::decrementIndent(size_t indent)
{
  assert(mIndent >= indent);
  mIndent -= indent;
}

size_t
UniversalStream::getIndent() const
{
  return mIndent;
}

// ---------------------------------------------------------------------------
void
UniversalStream::incrementDepth()
{
  mDepth += 1;
}

void
UniversalStream::decrementDepth()
{
  mDepth -= 1;
}

uint8_t
UniversalStream::getDepth() const
{
  return mDepth;
}

// ---------------------------------------------------------------------------
void
UniversalStream::flush()
{
  mOutput.flush();
}

std::ostream&
UniversalStream::getPlain()
{
  return mOutput;
}

// ---------------------------------------------------------------------------
void
UniversalStream::printIndent()
{
  mOutput << std::string(mIndent, ' ');
}
