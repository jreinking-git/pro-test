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

#include "static_attr_visitor.h"

#include "clang/AST/AST.h"

#include <iostream>
#include <sstream>

using namespace protest;

// ---------------------------------------------------------------------------
void
StaticAttrVisitor::handle(clang::FunctionDecl* decl)
{
  if (decl->getQualifiedNameAsString() == "getMemberAttr")
  {
    auto* record = getCxxRecord(decl);

    if (record && !alreadyHandled(record))
    {
      std::vector<clang::VarDecl*> attrs;
      for (auto* d : record->decls())
      {
        clang::VarDecl* v = clang::dyn_cast<clang::VarDecl>(d);
        if (v && v->isStaticDataMember())
        {
          attrs.push_back(v);
        }
      }

      std::stringstream output;
      while (!attrs.empty())
      {
        auto* attr = attrs.back();
        attrs.pop_back();

        // TODO (jreinking) forward decl of record so that it can be used for
        // types which are defined in the pt script itself
        storeDeclaration(record, attr);
        output << startImplementation(record, attr);
        output << addMember(record, attr, true);

        while (true)
        {
          int index = 0;
          for (auto* other : attrs)
          {
            bool eq = compareSignature(attr, other);

            if (eq)
            {
              output << addMember(record, other, false);
              attrs.erase(std::next(attrs.begin(), index));
              break;
            }
            index++;
          }
          output << endImplementation(record, attr);
          break;
        }
      }
      auto key = std::pair(record->getBeginLoc(), record->getEndLoc());
      mImplementation[key] = output.str();
    }
  }
}

void
StaticAttrVisitor::writeDeclarations(llvm::raw_fd_ostream& stream)
{
  for (auto& decl : mDeclarations)
  {
    stream << decl.second;
  }
}

void
StaticAttrVisitor::writeImplementation(llvm::raw_fd_ostream& stream)
{
  for (auto& impl : mImplementation)
  {
    stream << impl.second;
  }
}

// ---------------------------------------------------------------------------
bool
StaticAttrVisitor::alreadyHandled(clang::CXXRecordDecl* cls)
{
  auto key = std::pair(cls->getBeginLoc(), cls->getEndLoc());
  return mImplementation.count(key) != 0;
}

bool
StaticAttrVisitor::compareSignature(clang::VarDecl* first,
                                    clang::VarDecl* second)
{
  return first->getType() == second->getType();
}

clang::CXXRecordDecl*
StaticAttrVisitor::getCxxRecord(clang::FunctionDecl* decl)
{
  auto* param = decl->parameters()[0];
  auto* record =
      param->getType().getNonReferenceType().getTypePtr()->getAsCXXRecordDecl();
  return record;
}

std::string
StaticAttrVisitor::startImplementation(clang::CXXRecordDecl* cls,
                                       clang::VarDecl* member)
{
  std::string retValue = member->getType().getNonReferenceType().getAsString();

  std::stringstream output;
  output << "template <>";
  output << retValue << "& ";
  output << "getMemberAttr("
         << "const char* name, " << cls->getQualifiedNameAsString() << "*)";
  output << "{";
  return output.str();
}

std::string
StaticAttrVisitor::endImplementation(clang::CXXRecordDecl* cls,
                                     clang::VarDecl* member)
{
  std::stringstream output;
  output << "assert(false); }\n";
  return output.str();
}

std::string
StaticAttrVisitor::addMember(clang::CXXRecordDecl* cls,
                             clang::VarDecl* member,
                             bool first)
{
  std::string retValue = member->getType().getNonReferenceType().getAsString();

  std::string extra;
  if (!first)
  {
    extra = "else ";
  }

  std::stringstream output;
  output << extra << "if (name == std::string(\"" << member->getNameAsString()
         << "\")) { return " << cls->getQualifiedNameAsString() << "::" << member->getNameAsString() << ";}\n";
  return output.str();
}

void
StaticAttrVisitor::storeDeclaration(clang::CXXRecordDecl* cls,
                                    clang::VarDecl* member)
{
  std::string retValue = member->getType().getNonReferenceType().getAsString();

  std::stringstream output;
  output << "template <>";
  output << retValue << "& ";
  output << "getMemberAttr(const char* name, " << cls->getQualifiedNameAsString() << "*)";
  output << ";\n";

  auto key = std::pair(member->getBeginLoc(), member->getEndLoc());
  mDeclarations[key] = output.str();
}