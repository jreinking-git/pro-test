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

#include <clang/AST/DeclCXX.h>
#include "clang/AST/ASTContext.h"
#include <clang/Lex/Lexer.h>
#include "clang/Rewrite/Core/Rewriter.h"

#include <string>

namespace protest
{

// ---------------------------------------------------------------------------
std::string
forwardDeclarationOf(clang::CXXRecordDecl* decl);

// ---------------------------------------------------------------------------
template <typename T>
std::string
getArgument(clang::ASTContext* context, const T* call, unsigned int n)
{
  if (n < call->getNumArgs())
  {
    const clang::Expr* arg = call->getArg(n);
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
clang::CharSourceRange
getRange(const clang::Expr* expr);

// ---------------------------------------------------------------------------
std::string
getSourceText(clang::ASTContext* context, const clang::Expr* expr);

std::string
getSourceText(clang::ASTContext* context, clang::CharSourceRange range);

// ---------------------------------------------------------------------------
int
getNumParams(clang::CallExpr* expr);

std::string
getArgumentsAsString(clang::Rewriter& rewriter, clang::CallExpr* expr);

} // namespace protest
