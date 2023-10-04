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

#include "utils.h"

#include <clang/AST/Decl.h>

#include <iostream>

using namespace clang;
using namespace protest;

// ---------------------------------------------------------------------------
std::string
protest::forwardDeclarationOf(clang::CXXRecordDecl* decl)
{
  std::string forwardDeclaration;
  auto* parent = decl->getParent();
  int n = 0;
  while (parent && parent->isNamespace())
  {
    // TODO error when inner type since it cannot be forwarded
    auto ns = clang::dyn_cast_or_null<clang::NamespaceDecl>(parent);
    if (ns)
    {
      forwardDeclaration =
          "namespace " + ns->getNameAsString() + " { " + forwardDeclaration;
      parent = parent->getParent();
      n++;
    }
    else
    {
      parent = nullptr;
    }
  }

  if (decl->isStruct())
  {
    forwardDeclaration += "struct ";
  }
  else
  {
    forwardDeclaration += "class ";
  }

  forwardDeclaration += decl->getNameAsString() + "; ";

  for (int i = 0; i < n; i++)
  {
    forwardDeclaration += "} ";
  }

  return forwardDeclaration;
}

// ---------------------------------------------------------------------------
clang::CharSourceRange
protest::getRange(const clang::Expr* expr)
{
  return CharSourceRange::getTokenRange(expr->getBeginLoc(), expr->getEndLoc());
}

// ---------------------------------------------------------------------------
std::string
protest::getSourceText(clang::ASTContext* context, const clang::Expr* expr)
{
  return Lexer::getSourceText(getRange(expr),
                              context->getSourceManager(),
                              context->getLangOpts())
      .str();
}

std::string
protest::getSourceText(ASTContext* context, CharSourceRange range)
{
  return Lexer::getSourceText(range,
                              context->getSourceManager(),
                              context->getLangOpts())
      .str();
}

// ---------------------------------------------------------------------------
int
protest::getNumParams(clang::CallExpr* expr)
{
  int n = 0;
  for (unsigned int i = 0; i < expr->getNumArgs(); i++)
  {
    const clang::Expr* arg = expr->getArg(i);
    clang::CharSourceRange conditionRange =
        clang::CharSourceRange::getTokenRange(arg->getBeginLoc(), arg->getEndLoc());
    if (conditionRange.isValid())
    {
      n++;
    }
    else
    {
      break;
    }
  }
  return n;
}

std::string
protest::getArgumentsAsString(clang::Rewriter& rewriter, clang::CallExpr* expr)
{
  std::string exprAsString = "";
  for (unsigned int i = 0; i < expr->getNumArgs(); i++)
  {
    const clang::Expr* arg = expr->getArg(i);
    clang::CharSourceRange conditionRange =
        clang::CharSourceRange::getTokenRange(arg->getBeginLoc(), arg->getEndLoc());
    if (conditionRange.isValid())
    {
      exprAsString += rewriter.getRewrittenText(conditionRange) + ", ";
    }
    else
    {
      break;
    }
  }
  return exprAsString;
}
