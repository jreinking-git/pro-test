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
#include "utils.h"

#include "clang/Lex/Lexer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Mangle.h"

#include <iostream>

using namespace protest;
using namespace clang;

// ---------------------------------------------------------------------------
const char* StaticVarVisitor::staticVariable = "protest::getStaticVariable";
const char* StaticVarVisitor::staticFunction = "protest::getStaticFunction";

// ---------------------------------------------------------------------------
StaticVarVisitor::StaticVarVisitor(Rewriter& rewriter, ASTContext* context) :
  mContext(context),
  mRewriter(rewriter)
{
}

void
StaticVarVisitor::handle(CallExpr* expr)
{
  auto* decl = dyn_cast_or_null<FunctionDecl>(expr->getCalleeDecl());
  if (decl)
  {
    if (decl->getQualifiedNameAsString() == staticVariable)
    {
      auto* targs = decl->getTemplateSpecializationArgs();
      if (targs->size() == 3)
      {
        auto argType = targs->get(2);
        std::string function = getArgumentWithoutQuotes(expr, 0);
        std::string name = getArgumentWithoutQuotes(expr, 1);

        auto nm = buildNameForVariableInFunction(function, name, argType.getAsType(), true);

        auto range = getRange(expr);
        std::string oldCall = getSourceText(mContext, range);

        // TODO code duplication
        int tpl = 0;
        for (int i = 0; i < oldCall.size(); i++)
        {
          if (oldCall[i] == '(' &&
              tpl == 0)
          {
            oldCall = oldCall.substr(0, i);
            break;
          }
          else if (oldCall[i] == '<')
          {
            tpl++;
          }
          else if (oldCall[i] == '>')
          {
            tpl--;
          }
        }

        std::string extra;
        if (protest::getNumParams(expr) == 2)
        {
          extra = std::string("0, ");
        }
        else
        {
        }

        std::string newCall = oldCall + "(" + getArgumentsAsString(mRewriter, expr) + extra + "\"" + nm  + "\"" + ")";
        mRewriter.ReplaceText(range, newCall);

        // TODO only last param is realy used
        std::string str = "// @protest::getStaticVariable2(\"" + function + "\", \"" + name + "\", \"" + nm + "\");";
        mDeclarations.push_back(str);
      }
      else
      {
        auto argType = targs->get(0);
        std::string argAsString = getArgumentWithoutQuotes(expr, 0);
        auto nm = buildNameForVariable(argAsString, argType.getAsType());


        auto range = getRange(expr);
        std::string oldCall = getSourceText(mContext, range);

        // TODO code duplication
        int tpl = 0;
        for (int i = 0; i < oldCall.size(); i++)
        {
          if (oldCall[i] == '(' &&
              tpl == 0)
          {
            oldCall = oldCall.substr(0, i);
            break;
          }
          else if (oldCall[i] == '<')
          {
            tpl++;
          }
          else if (oldCall[i] == '>')
          {
            tpl--;
          }
        }

        std::string extra;
        if (protest::getNumParams(expr) == 1)
        {
          extra = std::string("0, ");
        }
        else
        {
        }

        std::string newCall = oldCall + "(" + getArgumentsAsString(mRewriter, expr) + extra + "\"" + nm  + "\"" + ")";
        mRewriter.ReplaceText(range, newCall);

        std::string str = "// @protest::getStaticVariable(\"" + nm + "\");";
        mDeclarations.push_back(str);
      }
    }
    else if (decl->getQualifiedNameAsString() == staticFunction)
    {
      std::string argAsString = getArgumentWithoutQuotes(expr, 0);
      auto* targs = decl->getTemplateSpecializationArgs();
      assert(targs);
      auto argType = targs->get(1);
      auto name = buildNameForFunction(argAsString, argType.getAsType());
      std::string str = "// @protest::getStaticFunction(\"" + name + "\", \"" + name + "\");";


      auto range = getRange(expr);
      std::string oldCall = getSourceText(mContext, range);

      // TODO code duplication
      int tpl = 0;
      for (int i = 0; i < oldCall.size(); i++)
      {
        if (oldCall[i] == '(' &&
            tpl == 0)
        {
          oldCall = oldCall.substr(0, i);
          break;
        }
        else if (oldCall[i] == '<')
        {
          tpl++;
        }
        else if (oldCall[i] == '>')
        {
          tpl--;
        }
      }

      std::string extra;
      if (protest::getNumParams(expr) == 1)
      {
        extra = std::string("0, ");
      }
      else
      {
      }

      std::string newCall = oldCall + "(" + getArgumentsAsString(mRewriter, expr) + extra + "\"" + name  + "\"" + ")";
      mRewriter.ReplaceText(range, newCall);

      mDeclarations.push_back(str);
    }
  }
}

void
StaticVarVisitor::writeDeclarations(llvm::raw_fd_ostream& stream)
{
  for (auto& decl : mDeclarations)
  {
    stream << decl << "\n";
  }
}

// ---------------------------------------------------------------------------
std::string
StaticVarVisitor::getArgumentWithoutQuotes(clang::CallExpr *expr, size_t index)
{
  std::string name = getArgument(mContext, expr, index);
  name = name.substr(1, name.size() - 2); // remove quotes
  return name;
}

std::string
StaticVarVisitor::mangledName(clang::QualType type)
{
  auto mangleContext = mContext->createMangleContext();
  std::string mangledName;
  llvm::raw_string_ostream ostream(mangledName);
  mangleContext->mangleTypeName(type, ostream);
  mangledName = mangledName.substr(6); // remove prefix
  mangledName = mangledName.substr(0, mangledName.size() - 1); // remove trailing E
  return mangledName;
}

std::string
StaticVarVisitor::buildNameForFunction(std::string name, clang::QualType signature)
{
  auto mname = mangledName(signature);
  mname = std::string("_ZL") + std::to_string(name.size()) + name + mname;
  return mname;
}

std::string
StaticVarVisitor::buildNameForVariable(std::string name, clang::QualType type)
{
  // 0000000000075010 d _ZL8my_value
  // 000000000007501c d _ZN4demoL8my_valueE
  auto mname = std::string("_ZL") + std::to_string(name.size()) + name;
  return mname;
}

std::string
StaticVarVisitor::buildNameForVariableInFunction(std::string function, std::string name, clang::QualType signature, bool isStatic)
{
  auto mname = mangledName(signature);
  mname = std::string("_ZZL\\\\?") + std::to_string(function.size()) + function + mname + "E" + std::to_string(name.size()) + name;
  return mname;
}