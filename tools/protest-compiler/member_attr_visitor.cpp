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

#include "member_attr_visitor.h"
#include "utils.h"

#include "clang/AST/AST.h"

#include <iostream>
#include <sstream>

using namespace protest;

// ---------------------------------------------------------------------------
void
MemberAttrVisitor::handle(clang::FunctionDecl* decl)
{
  if (decl->getQualifiedNameAsString() == "protest::getMemberAttr")
  {
    auto* record = getCxxRecord(decl);

    if (record && !alreadyHandled(record))
    {
      std::vector<clang::FieldDecl*> attrs;
      
      std::stringstream output;
      storeDeclarationRaw(record);

      output << startImplementationRaw(record);

      bool first = true;
      for (auto* attr : record->fields())
      {
        output << addMemberRaw(record, attr, first);
        attrs.push_back(attr);
        first = false;
      }
      output << "assert(false); }}\n";

      while (!attrs.empty())
      {
        auto* attr = attrs.back();
        attrs.pop_back();

        storeDeclaration(record, attr);
        output << startImplementation(record, attr);
        output << addMember(record, attr, true);

        bool again = true;
        while (again)
        {
          int index = 0;
          again = false;
          for (auto* other : attrs)
          {
            bool eq = compareSignature(attr, other);

            if (eq)
            {
              output << addMember(record, other, false);
              attrs.erase(std::next(attrs.begin(), index));
              again = true;
              break;
            }
            index++;
          }
        }
        output << endImplementation(record, attr);
      }
      auto key = std::pair(record->getBeginLoc(), record->getEndLoc());
      mImplementation[key] = output.str();
    }
  }
}

void
MemberAttrVisitor::writeDeclarations(llvm::raw_fd_ostream& stream)
{
  for (auto& decl : mDeclarations)
  {
    stream << decl.second;
  }
}

void
MemberAttrVisitor::writeImplementation(llvm::raw_fd_ostream& stream)
{
  for (auto& impl : mImplementation)
  {
    stream << impl.second;
  }
}

// ---------------------------------------------------------------------------
bool
MemberAttrVisitor::alreadyHandled(clang::CXXRecordDecl* cls)
{
  auto key = std::pair(cls->getBeginLoc(), cls->getEndLoc());
  return mImplementation.count(key) != 0;
}

bool
MemberAttrVisitor::compareSignature(clang::FieldDecl* first,
                                    clang::FieldDecl* second)
{
  return first->getType().getNonReferenceType() == second->getType().getNonReferenceType();
}

clang::CXXRecordDecl*
MemberAttrVisitor::getCxxRecord(clang::FunctionDecl* decl)
{
  auto* param = decl->parameters()[0];
  auto* record =
      param->getType().getNonReferenceType().getTypePtr()->getAsCXXRecordDecl();
  return record;
}

std::string
MemberAttrVisitor::startImplementation(clang::CXXRecordDecl* cls,
                                       clang::FieldDecl* member)
{
  std::string retValue = member->getType().getNonReferenceType().getAsString();

  std::stringstream output;
  output << "namespace protest { template <>";
  output << retValue << "& ";
  output << "protest::getMemberAttr(" << cls->getQualifiedNameAsString()
         << "& obj, const char* name)";
  output << "{";
  return output.str();
}

std::string
MemberAttrVisitor::startImplementationRaw(clang::CXXRecordDecl* cls)
{
  std::stringstream output;
  output << "namespace protest { template <>";
  output << "const void*";
  output << "protest::getMemberAttrRaw(" << cls->getQualifiedNameAsString()
         << "& obj, const char* name, size_t size)";
  output << "{";
  return output.str();
}

std::string
MemberAttrVisitor::endImplementation(clang::CXXRecordDecl* cls,
                                     clang::FieldDecl* member)
{
  std::stringstream output;
  output << "assert(false); }}\n";
  return output.str();
}

std::string
MemberAttrVisitor::addMember(clang::CXXRecordDecl* cls,
                             clang::FieldDecl* member,
                             bool first)
{
  std::string extra;
  if (!first)
  {
    extra = "else ";
  }

  std::stringstream output;
  output << extra << "if (name == std::string(\"" << member->getNameAsString()
         << "\")) { return obj." << member->getNameAsString() << ";}\n";
  return output.str();
}

std::string
MemberAttrVisitor::addMemberRaw(clang::CXXRecordDecl* cls,
                                clang::FieldDecl* member,
                                bool first)
{
  std::string extra;
  if (!first)
  {
    extra = "else ";
  }

  std::stringstream output;
  output << extra << "if (name == std::string(\"" << member->getNameAsString()
         << "\")) { assert(sizeof(obj." << member->getNameAsString() << ") == size); return reinterpret_cast<const void*>(&obj." << member->getNameAsString() << ");}\n";
  return output.str();
}

void
MemberAttrVisitor::storeDeclaration(clang::CXXRecordDecl* cls,
                                    clang::FieldDecl* member)
{
  std::string retValue = member->getType().getNonReferenceType().getAsString();

  std::stringstream output;
  output << forwardDeclarationOf(cls);
  output << "namespace protest { template <>";
  output << retValue << "& ";
  output << "getMemberAttr(" << cls->getQualifiedNameAsString();
  output << "& obj, const char* name)";
  output << "; }\n";

  auto key = std::pair(member->getBeginLoc(), member->getEndLoc());
  mDeclarations[key] = output.str();
}

void
MemberAttrVisitor::storeDeclarationRaw(clang::CXXRecordDecl* cls)
{
  std::stringstream output;
  output << forwardDeclarationOf(cls);
  output << "namespace protest { template <>";
  output << "const void* ";
  output << "getMemberAttrRaw(" << cls->getQualifiedNameAsString();
  output << "& obj, const char* name, size_t size)";
  output << "; }\n";

  auto key = std::pair(cls->getBeginLoc(), cls->getEndLoc());
  mDeclarations[key] = output.str();
}