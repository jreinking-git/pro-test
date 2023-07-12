/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2022 Janosch Reinking
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "member_func_visitor.h"
#include "utils.h"

#include "clang/AST/AST.h"

#include <sstream>

using namespace protest;

// ---------------------------------------------------------------------------
void
MemberFuncVisitor::handle(clang::FunctionDecl* decl)
{
  if (decl->getQualifiedNameAsString() == "protest::getMemberFunction")
  {
    auto* record = getCxxRecord(decl);
    if (record && !alreadyHandled(record))
    {
      std::vector<clang::CXXMethodDecl*> methods;
      for (auto* function : record->methods())
      {
        methods.push_back(function);
      }

      std::stringstream output;
      while (!methods.empty())
      {
        auto* function = methods.back();
        methods.pop_back();

        if (function->getNameAsString() != record->getNameAsString())
        {
          // TODO (jreinking) forward decl of record so that it can be used for
          // types which are defined in the pt script itself
          storeDeclaration(record, function);
          output << startImplementation(record, function);
          output << addMember(record, function, true);

          bool again = true;
          while (again)
          {
            int index = 0;
            again = false;
            for (auto* other : methods)
            {
              bool eq = compareSignature(function, other);

              if (eq)
              {
                output << addMember(record, other, false);
                methods.erase(std::next(methods.begin(), index));
                again = true;
                break;
              }
              index++;
            }
          }
          output << endImplementation(record, function);
        }
      }
      auto key = std::pair(record->getBeginLoc(), record->getEndLoc());
      mImplementation[key] = output.str();
    }
  }
}

void
MemberFuncVisitor::writeDeclarations(llvm::raw_fd_ostream& stream)
{
  for (auto& decl : mDeclarations)
  {
    stream << decl.second;
  }
}

void
MemberFuncVisitor::writeImplementation(llvm::raw_fd_ostream& stream)
{
  for (auto& impl : mImplementation)
  {
    stream << impl.second;
  }
}

// ---------------------------------------------------------------------------
bool
MemberFuncVisitor::alreadyHandled(clang::CXXRecordDecl* cls)
{
  auto key = std::pair(cls->getBeginLoc(), cls->getEndLoc());
  return mImplementation.count(key) != 0;
}

bool
MemberFuncVisitor::compareSignature(clang::CXXMethodDecl* first,
                                    clang::CXXMethodDecl* second)
{
  bool equal = false;
  if (first->getReturnType() == second->getReturnType() &&
      first->getNumParams() == second->getNumParams() &&
      first->isStatic() == second->isStatic())
  {
    auto iter1 = first->param_begin();
    auto iter2 = second->param_begin();
    equal = true;
    while (iter1 != first->param_end())
    {
      if ((*iter1)->getType() != (*iter2)->getType())
      {
        equal = false;
      }
      ++iter1;
      ++iter2;
    }
  }
  return equal;
}

clang::CXXRecordDecl*
MemberFuncVisitor::getCxxRecord(clang::FunctionDecl* decl)
{
  auto* param = decl->parameters()[0];
  auto* record =
      param->getType().getNonReferenceType().getTypePtr()->getAsCXXRecordDecl();
  return record;
}

std::string
MemberFuncVisitor::getParameters(clang::CXXMethodDecl* member)
{
  std::string params;
  bool first = true;
  for (auto* param : member->parameters())
  {
    std::string extra;
    if (!first)
    {
      extra = ", ";
    }
    first = false;
    params += extra + param->getType().getAsString();
  }
  return params;
}

std::string
MemberFuncVisitor::getPlaceholders(int n)
{
  std::string placeholders;
  bool first = true;
  for (int i = 1; i < n + 1; i++)
  {
    first = false;
    placeholders += ", std::placeholders::_" + std::to_string(i);
  }
  return placeholders;
}

std::string
MemberFuncVisitor::startImplementation(clang::CXXRecordDecl* cls,
                                       clang::CXXMethodDecl* member)
{
  std::string retValue = member->getReturnType().getAsString();
  std::string params = getParameters(member);
  std::string placeholders = getPlaceholders(member->getNumParams());

  std::stringstream output;
  output << "namespace protest { template <>";
  output << "std::function<" << retValue << "(" << params << ")>";
  output << "protest::getMemberFunction(";
  if (!member->isStatic())
  {
    output << cls->getQualifiedNameAsString() << "& obj, ";
  }
  output << "const char* name";
  if (member->isStatic())
  {
    output << ", " << cls->getQualifiedNameAsString() << "*";
  }
  output << ")";
  output << "{";
  return output.str();
}

std::string
MemberFuncVisitor::endImplementation(clang::CXXRecordDecl* cls,
                                     clang::CXXMethodDecl* member)
{
  std::string retValue = member->getReturnType().getAsString();
  std::string params = getParameters(member);
  std::stringstream output;
  output << "return std::function<" << retValue << "(" << params << ")"
         << ">();}}\n";
  return output.str();
}

std::string
MemberFuncVisitor::addMember(clang::CXXRecordDecl* cls,
                             clang::CXXMethodDecl* member,
                             bool first)
{
  std::string retValue = member->getReturnType().getAsString();
  std::string params = getParameters(member);
  std::string placeholders = getPlaceholders(member->getNumParams());

  std::string extra;
  if (!first)
  {
    extra = "else ";
  }

  std::stringstream output;
  output << extra << "if (name == std::string(\"" << member->getNameAsString()
         << "\")) { return std::bind((" << retValue << "(";
  if (member->isStatic())
  {
    output << "*)(" << params << ")) &";
  }
  else
  {
    output << cls->getQualifiedNameAsString() << "::*)(" << params << ")) &";
  }
  output << cls->getQualifiedNameAsString() << "::" << member->getNameAsString();
  
  if (!member->isStatic())
  {
    output << ", obj";
  }

  output << placeholders << ");}\n";
  return output.str();
}

void
MemberFuncVisitor::storeDeclaration(clang::CXXRecordDecl* cls,
                                    clang::CXXMethodDecl* member)
{
  std::string retValue = member->getReturnType().getAsString();
  std::string params = getParameters(member);

  std::stringstream output;
  output << forwardDeclarationOf(cls);
  output << "namespace protest { template <>";
  output << "std::function<" << retValue << "(" << params << ")>";
  output << "getMemberFunction(";
  if (!member->isStatic())
  {
    output << cls->getQualifiedNameAsString() << "& obj, ";
  }
  output << "const char* name";
  if (member->isStatic())
  {
    output << ", " << cls->getQualifiedNameAsString() << "*";
  }
  output << "); }\n";

  auto key = std::pair(member->getBeginLoc(), member->getEndLoc());
  mDeclarations[key] = output.str();
}