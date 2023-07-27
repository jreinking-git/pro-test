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

#include <clang/Basic/SourceLocation.h>
#include <clang/AST/Decl.h>
#include <clang/AST/DeclCXX.h>
#include <llvm/Support/raw_ostream.h>

#include <string>
#include <map>

namespace protest
{

// ---------------------------------------------------------------------------
/**
 * @class StaticAttrVisitor
 */
class StaticAttrVisitor
{
public:
  explicit StaticAttrVisitor() = default;

  StaticAttrVisitor(const StaticAttrVisitor&) = delete;

  StaticAttrVisitor(StaticAttrVisitor&&) noexcept = delete;

  StaticAttrVisitor&
  operator=(const StaticAttrVisitor&) = delete;

  StaticAttrVisitor&
  operator=(StaticAttrVisitor&&) noexcept = delete;

  ~StaticAttrVisitor() = default;

// ---------------------------------------------------------------------------
  void
  handle(clang::FunctionDecl* decl);

  void
  writeDeclarations(llvm::raw_fd_ostream& stream);

  void
  writeImplementation(llvm::raw_fd_ostream& stream);

private:
  bool
  alreadyHandled(clang::CXXRecordDecl* cls);

  bool
  compareSignature(clang::VarDecl* first, clang::VarDecl* second);

  clang::CXXRecordDecl*
  getCxxRecord(clang::FunctionDecl* decl);

  std::string
  startImplementation(clang::CXXRecordDecl* cls, clang::VarDecl* member);

  std::string
  endImplementation(clang::CXXRecordDecl* cls, clang::VarDecl* member);

  std::string
  addMember(clang::CXXRecordDecl* cls,
            clang::VarDecl* member,
            bool first);

  void
  storeDeclaration(clang::CXXRecordDecl* cls, clang::VarDecl* member);

  using Key = std::pair<clang::SourceLocation, clang::SourceLocation>;

  std::map<Key, std::string> mDeclarations;
  std::map<Key, std::string> mImplementation;
};

}// namespace protest
