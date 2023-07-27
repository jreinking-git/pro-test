/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2023 Janosch Reinking
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

#include "static_var_visitor.h"

#include "clang/Lex/Lexer.h"
#include "clang/AST/ASTContext.h"

#include <iostream>

using namespace protest;

// ---------------------------------------------------------------------------
// TODO (jreinking) code duplication
template <typename T>
std::string
getArgument(clang::ASTContext* context, const T* call, unsigned int n)
{
  if (n < call->getNumArgs())
  {
    const clang::Expr* arg = call->getArg(0);
    bool invalid = false;
    clang::CharSourceRange conditionRange =
        clang::CharSourceRange::getTokenRange(arg->getBeginLoc(),
                                              arg->getEndLoc());
    clang::StringRef exprAsString =
        clang::Lexer::getSourceText(conditionRange,
                                    context->getSourceManager(),
                                    context->getLangOpts(),
                                    &invalid);
    assert(!invalid);
    return exprAsString.str();
  }
  return "";
}

// ---------------------------------------------------------------------------
StaticVarVisitor::StaticVarVisitor(clang::ASTContext* context) :
  mContext(context)
{
}

void
StaticVarVisitor::handle(clang::CallExpr* expr)
{
  auto* decl =
      clang::dyn_cast_or_null<clang::FunctionDecl>(expr->getCalleeDecl());
  if (decl && decl->getQualifiedNameAsString() == "protest::getStaticVariable")
  {
    auto* arg = expr->getArg(0);
    std::string argAsString = getArgument(mContext, expr, 0);
    if (argAsString[0] == '\"')
    {
      std::string str = "// @protest::getStaticVariable(" + argAsString + ");";
      mDeclarations.push_back(str);
    }
  }
}

void
StaticVarVisitor::writeDeclarations(llvm::raw_fd_ostream& stream)
{
  for (auto& decl : mDeclarations)
  {
    stream << decl <<"\n";
  }
}