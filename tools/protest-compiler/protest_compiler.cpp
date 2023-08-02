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

#include "member_func_visitor.h"
#include "member_attr_visitor.h"
#include "static_attr_visitor.h"
#include "static_var_visitor.h"

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Driver/Options.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>

using namespace clang;
using namespace clang::tooling;
using namespace llvm::cl;
using namespace llvm;

bool
isInMainFile(clang::ASTContext* context, const clang::Decl* const decl)
{
  assert(decl);
  auto declFile = context->getSourceManager().getFileEntryForID(
      context->getSourceManager().getFileID(decl->getSourceRange().getBegin()));
  auto mainFile = context->getSourceManager().getFileEntryForID(
      context->getSourceManager().getMainFileID());
  return (declFile && mainFile) &&
         (declFile->tryGetRealPathName() == mainFile->tryGetRealPathName());
}

bool
isInMainFile(clang::ASTContext* context, const clang::Stmt* const expr)
{
  assert(expr);
  auto declFile = context->getSourceManager().getFileEntryForID(
      context->getSourceManager().getFileID(expr->getSourceRange().getBegin()));
  auto mainFile = context->getSourceManager().getFileEntryForID(
      context->getSourceManager().getMainFileID());
  return (declFile && mainFile) &&
         (declFile->tryGetRealPathName() == mainFile->tryGetRealPathName());
}

bool
isInMainFile(clang::ASTContext* context, clang::CallExpr* expr)
{
  assert(expr);
  auto declFile = context->getSourceManager().getFileEntryForID(
      context->getSourceManager().getFileID(expr->getSourceRange().getBegin()));
  auto mainFile = context->getSourceManager().getFileEntryForID(
      context->getSourceManager().getMainFileID());
  return (declFile && mainFile) &&
         (declFile->tryGetRealPathName() == mainFile->tryGetRealPathName());
}

std::string
escapeString(std::string const& s)
{
  std::string out;
  for (auto ch : s)
  {
    switch (ch)
    {
    case '\'':
      out += "\\'";
      break;
    case '\"':
      out += "\\\"";
      break;
    case '\?':
      out += "\\?";
      break;
    case '\\':
      out += "\\\\";
      break;
    case '\a':
      out += "\\a";
      break;
    case '\b':
      out += "\\b";
      break;
    case '\f':
      out += "\\f";
      break;
    case '\n':
      out += "\\n";
      break;
    case '\r':
      out += "\\r";
      break;
    case '\t':
      out += "\\t";
      break;
    case '\v':
      out += "\\v";
      break;
    default:
      out += ch;
    }
  }

  return out;
}

std::string
trim(const std::string& s)
{
  auto start = s.begin();
  while (start != s.end() && std::isspace(*start))
  {
    start++;
  }
  auto end = s.end();
  do
  {
    end--;
  }
  while (std::distance(start, end) > 0 && std::isspace(*end));
  auto copy = std::string(start, end + 1);
  while (copy.find("\n") != std::string::npos)
  {
    copy.replace(copy.find("\n"), 1, " ");
  }
  while (copy.find("  ") != std::string::npos)
  {
    copy.replace(copy.find("  "), 2, " ");
  }
  return copy;
}

size_t
lineNumber(ASTContext* context, const Expr* expr)
{
  return context->getSourceManager().getPresumedLineNumber(expr->getBeginLoc());
}

size_t
lineNumber(ASTContext* context, const Decl* expr)
{
  return context->getSourceManager().getPresumedLineNumber(expr->getBeginLoc());
}

CharSourceRange
getRange(const Expr* expr)
{
  return CharSourceRange::getTokenRange(expr->getBeginLoc(), expr->getEndLoc());
}

std::string
getSourceText(ASTContext* context, const Expr* expr)
{
  return Lexer::getSourceText(getRange(expr),
                              context->getSourceManager(),
                              context->getLangOpts())
      .str();
}

std::string
getSourceText(ASTContext* context, CharSourceRange range)
{
  return Lexer::getSourceText(range,
                              context->getSourceManager(),
                              context->getLangOpts())
      .str();
}

class ProtestVisitor : public RecursiveASTVisitor<ProtestVisitor>
{
public:
  explicit ProtestVisitor(ASTContext* Context,
                          clang::Rewriter& Rewriter,
                          llvm::raw_fd_ostream& outputFile) :
    Context(Context),
    Rewriter(Rewriter),
    mOutputFile(outputFile),
    mIdNext(0),
    mStaticVarVisitor(Context)
  {
  }

  bool
  VisitVarDecl(VarDecl* Expr)
  {
    auto comment = Context->getRawCommentForDeclNoCache(Expr);

    std::map<std::string, std::string> comments;
    if (comment)
    {
      std::stringstream ss(
          comment->getFormattedText(Context->getSourceManager(),
                                    Context->getDiagnostics()));

      std::string line;
      while (std::getline(ss, line, '\n'))
      {
        std::regex r("(.*)(@[a-zA-Z]*)(\\s)+(.*)(\\s)*");
        std::smatch sm;
        std::regex_match(line, sm, r);
        if (sm.size() >= 6)
        {
          comments[sm[2]] = sm[4];
        }
      }
    }

    // construction
    // auto var = MyType();
    if (Expr->getInit())
    {
      auto constructor = dyn_cast_or_null<CXXConstructExpr>(Expr->getInit());

      if (constructor)
      {
        auto decl = constructor->getConstructor();
        std::string type = "";
        if (decl && isProtestFunctionCall(decl, type))
        {

          type = type.substr(0, type.size() - 2);
          std::string functionName = decl->getQualifiedNameAsString();
          std::string metaObjectName = functionName;
          metaObjectName = toMetaObjectName(metaObjectName);

          std::string exprAsString = getArgumentsAsString(constructor, decl);

          std::vector<std::string> args;
          for (unsigned int i = 0; i < decl->getMinRequiredArguments(); i++)
          {
            args.push_back(getArgument(constructor, i));
          }

          std::string objectRef;
          std::string text = Expr->getNameAsString();

          const CharSourceRange range = getRange(constructor);
          const std::string newCall = Expr->getNameAsString() + "(" +
                                      exprAsString + metaObjectName +
                                      std::to_string(mIdNext) + ")";

          // already rewritten
          // just update the information
          if (mMetaInfos.count(std::pair(constructor->getBeginLoc(),
                                         constructor->getEndLoc())) == 0)
          {
            Rewriter.ReplaceText(range, newCall);
          }
          writeMetaInfos(constructor,
                         metaObjectName,
                         args,
                         type,
                         text,
                         comments);
          return true;
        }
      }
    }

    // assignment
    // existingVar = createFunction();
    auto* init = Expr->getInit();
    if (init != nullptr)
    {
      auto bind =
          dyn_cast_or_null<CXXBindTemporaryExpr>(init->IgnoreImpCasts());
      if (bind != nullptr)
      {
        auto call = dyn_cast_or_null<CallExpr>(bind->getSubExpr());

        if (call != nullptr)
        {
          auto decl = dyn_cast_or_null<FunctionDecl>(call->getCalleeDecl());
          std::string type = "";
          if (decl && isProtestFunctionCall(decl, type))
          {
            type = type.substr(0, type.size() - 2);
            std::string functionName = decl->getQualifiedNameAsString();
            std::string metaObjectName = functionName;
            metaObjectName = toMetaObjectName(metaObjectName);

            std::string exprAsString = getArgumentsAsString(call, decl);

            std::vector<std::string> args;
            for (unsigned int i = 0; i < decl->getMinRequiredArguments(); i++)
            {
              args.push_back(getArgument(call, i));
            }

            std::string objectRef = "";
            std::string text = Expr->getNameAsString();
            writeMetaInfos(call, metaObjectName, args, type, text);
            return true;
          }
        }
      }
    }
    return true;
  }

  bool
  VisitCXXConstructExpr(CXXConstructExpr* expr)
  {
    if (expr != nullptr)
    {
      auto* decl = expr->getConstructor();
      if (decl != nullptr)
      {
        auto ctx = decl->getDeclContext();
        if (ctx && ctx->isRecord())
        {
          auto* other = dyn_cast_or_null<CXXRecordDecl>(ctx);
          if (other != nullptr)
          {
            auto tpl = dyn_cast_or_null<ClassTemplateSpecializationDecl>(other);
            if (tpl != nullptr)
            {
              // traverse every method of the class
              auto iter = tpl->method_begin();
              while (iter != tpl->method_end())
              {
                auto method = *iter;
                if (method)
                {
                  auto funcDecl = dyn_cast_or_null<FunctionDecl>(method);
                  if (funcDecl && isInMainFile(Context, funcDecl))
                  {
                    if (mAlreadyTraversed.count(funcDecl) == 0)
                    {
                      mAlreadyTraversed.insert(funcDecl);
                      static int i = 0;
                      i++;
                      TraverseDecl(funcDecl);
                    }
                  }
                }
                ++iter;
              }
            }
          }
        }
      }
    }
    return true;
  }

  bool
  VisitFunctionDecl(FunctionDecl* Func)
  {
    if (Func != nullptr && isInMainFile(Context, Func))
    {
      if (Func->hasAttrs())
      {
        for (auto attr : Func->getAttrs())
        {
          std::string attr_name(attr->getSpelling());
          std::string attr_annotate("annotate");
          std::string attr_my("annotate(\"my_attr\")");

          if (attr_name == attr_annotate)
          {

            std::string SS;
            llvm::raw_string_ostream S(SS);
            static PrintingPolicy print_policy(Context->getLangOpts());
            print_policy.FullyQualifiedName = 1;
            print_policy.SuppressScope = 0;
            print_policy.PrintCanonicalTypes = 1;
            attr->printPretty(S, print_policy);
            std::string annotations = S.str();

            std::string result;
            while (true)
            {
              auto n = annotations.find("pt::");
              if (n != std::string::npos)
              {
                annotations = annotations.substr(n + 4);

                int start = annotations.find("(");
                int braces = 1;
                int i = 0;

                if (start != std::string::npos)
                {
                  for (i = start + 1; i < annotations.size() && braces > 0; i++)
                  {
                    if (annotations[i] == '(')
                    {
                      braces++;
                    }
                    else if (annotations[i] == ')')
                    {
                      braces--;
                    }
                  }
                }

                if (braces == 0)
                {
                  auto curr = annotations.substr(0, i - 1);
                  std::string metaObjectName =
                      std::string("protest_annotation");
                  result += std::string("auto ") + metaObjectName + "_guard" +
                            std::to_string(mIdNext) + " = protest::" + curr +
                            ", " + metaObjectName + std::to_string(mIdNext) +
                            ");";
                  writeMetaInfos(Func, metaObjectName, {});
                }
              }
              else
              {
                break;
              }
            }

            auto body = Func->getBody();
            auto range = body->getSourceRange();
            auto range2 = CharSourceRange::getTokenRange(range.getBegin(),
                                                         range.getBegin());
            std::string oldCall = Rewriter.getRewrittenText(range2);
            std::string newBody = std::string("{") + result + oldCall.substr(1);
            Rewriter.ReplaceText(range2, newBody);
          }
        }
      }
    }
    return true;
  }

  bool
  VisitCallExpr(CallExpr* Expr)
  {
    // TODO (jreinking)
    // static std::set<CallExpr*> visted;
    // if (visted.find(Expr) != visted.end())
    // {
    //   return false;
    // }
    // else
    // {
    //   visted.insert(Expr);
    // }

    auto cxxcall = dyn_cast_or_null<CXXMemberCallExpr>(Expr);
    FunctionDecl* decl = dyn_cast_or_null<FunctionDecl>(Expr->getCalleeDecl());

    if (Expr != nullptr)
    {
      if (cxxcall)
      {
        // this is necessary to get all calls of an function call, even if it is
        // used in a template class member function:
        //
        // template <typename T>
        // class MyClass {
        // public:
        //   void myFunc(T value) {
        //     func(value);
        //   }
        // };
        //
        // MyClass<int> cls;
        // cls.myFunc(32);
        if (decl && isInMainFile(Context, decl))
        {
          auto other = cxxcall->getRecordDecl();
          if (other != nullptr)
          {
            auto tpl = dyn_cast_or_null<ClassTemplateSpecializationDecl>(other);
            if (tpl != nullptr)
            {
              // traverse every method of the class
              auto iter = tpl->method_begin();
              while (iter != tpl->method_end())
              {
                auto method = *iter;
                if (method)
                {
                  auto funcDecl = dyn_cast_or_null<FunctionDecl>(method);
                  if (funcDecl && isInMainFile(Context, funcDecl))
                  {
                    if (mAlreadyTraversed.count(funcDecl) == 0)
                    {
                      mAlreadyTraversed.insert(funcDecl);
                      static int i = 0;
                      i++;
                      TraverseDecl(funcDecl);
                    }
                  }
                }
                ++iter;
              }
            }
          }
        }
        else
        {
        }
      }
      else
      {
      }
    }

    if (Expr != nullptr)
    {
      if (decl != nullptr)
      {
        // this is necessary to get all calls of an function call, even if it is
        // used in a template function:
        //
        // template <typename T>
        // void myFunc(T value) {
        //   func(value);
        // }
        //
        // myFunc<int>(32);
        if (decl->isFunctionTemplateSpecialization())
        {
          TraverseDecl(decl);
        }
      }
      else
      {
      }
    }

    std::string type;
    if (decl != nullptr)
    {
      for (auto& param : decl->parameters())
      {
        if (param->getNameAsString() == "stream2")
        {
          auto type = decl->getTemplateSpecializationArgs()->get(0).getAsType();
          auto t = dyn_cast_or_null<EnumType>(type.getTypePtr());
          auto tt = dyn_cast_or_null<RecordType>(type.getTypePtr());
          auto ttt = dyn_cast_or_null<PointerType>(type.getTypePtr());
          if (t)
          {
            auto edecl = t->getDecl();
            createEnumPrinter(edecl);
          }
          else if (tt && isRecord(tt))
          {
            if (isInMainFile(Context, tt->getDecl()))
            {
              createRecordPrinter(tt);
            }
          }
          else if (ttt && isRecord(ttt->getPointeeCXXRecordDecl()))
          {
            if (isInMainFile(Context, tt->getPointeeCXXRecordDecl()))
            {
              auto tttt = ttt->getPointeeCXXRecordDecl();
              createRecordPrinter(tttt);
            }
          }
        }
      }
    }
    if (decl && isProtestFunctionCall(decl, type))
    {
      if (mMetaInfos.count(std::pair(Expr->getBeginLoc(), Expr->getEndLoc())) ==
          0)
      {
        type = type.substr(0, type.size() - 2);
        std::string functionName = decl->getQualifiedNameAsString();
        bool isWhen = functionName.find("when") != std::string::npos;
        std::string metaObjectName = functionName;
        metaObjectName = toMetaObjectName(metaObjectName);

        std::string exprAsString = getArgumentsAsString(Expr, decl);

        std::vector<std::string> args;
        for (unsigned int i = 0; i < decl->getMinRequiredArguments(); i++)
        {
          args.push_back(getArgument(Expr, i));
        }

        std::string templateArgs = "";

        std::string text = "";
        if (cxxcall)
        {
          auto* object = cxxcall->getImplicitObjectArgument();
          text = getSourceText(Context, object);
        }

        std::string newCall = "";
        CharSourceRange range = getRange(Expr);

        if (isWhen)
        {
          // TODO (jreinking) special handling for 'when' is not so nice
          std::string oldCall = Rewriter.getRewrittenText(range);
          oldCall = oldCall.substr(0, oldCall.find("when("));
          newCall = oldCall + "when(" + exprAsString + metaObjectName +
                    std::to_string(mIdNext) + ")";
        }
        else
        {
          std::string oldCall = getSourceText(Context, range);
          oldCall = oldCall.substr(0, oldCall.find('('));
          newCall = oldCall + "(" + exprAsString + metaObjectName +
                    std::to_string(mIdNext) + ")";
        }

        Rewriter.ReplaceText(range, newCall);

        writeMetaInfos(Expr, metaObjectName, args, type, text);
      }
    }
    if (decl)
    {
      mMemberFuncVisitor.handle(decl);
      mMemberAttrVisitor.handle(decl);
      mStaticAttrVisitor.handle(decl);
      mStaticVarVisitor.handle(Expr);
    }
    return true;
  }

  bool
  shouldTraversePostOrder()
  {
    return true;
  }

  protest::MemberFuncVisitor mMemberFuncVisitor;
  protest::MemberAttrVisitor mMemberAttrVisitor;
  protest::StaticAttrVisitor mStaticAttrVisitor;
  protest::StaticVarVisitor mStaticVarVisitor;

  std::map<std::pair<SourceLocation, SourceLocation>,
           std::pair<std::string, int>>
      mMetaInfos;
  std::map<std::pair<SourceLocation, SourceLocation>, std::string> mImpls;
  // std::map<std::pair<SourceLocation, SourceLocation>, std::string> mMemberImpls;
  std::set<clang::Decl*> mAlreadyTraversed;

private:
  std::string
  toMetaObjectName(std::string name)
  {
    while (name.find("::") != std::string::npos)
    {
      name.replace(name.find("::"), 2, "_");
    }
    while (name.find("<") != std::string::npos)
    {
      name.replace(name.find("<"), 1, "_");
    }
    while (name.find(">") != std::string::npos)
    {
      name.replace(name.find(">"), 1, "_");
    }
    while (name.find(" ") != std::string::npos)
    {
      name.replace(name.find(" "), 1, "_");
    }
    while (name.find("*") != std::string::npos)
    {
      name.replace(name.find("*"), 1, "");
    }
    while (name.find("\n") != std::string::npos)
    {
      name.replace(name.find("\n"), 1, "");
    }
    while (name.find(",") != std::string::npos)
    {
      name.replace(name.find(","), 1, "_");
    }
    return name;
  }

  /**
   * @brief isProtestFunctionCall
   * 
   * All function that have a default parameter like:
   * 
   * void
   * function(protest::meta::CallContext context =
   *              protest::meta::CallContext::defaultContext())
   * {
   *   // ..
   * }
   * 
   * will be treated as a protest function. This means that the default
   * parameter will be replaced by the actual call context. E .g.:
   * 
   * function();
   * 
   * becomes:
   * 
   * function(callContext);
   * 
   * @param decl
   *  declaration to check
   * 
   * @param type
   *  prameter to store the type of the defaultContext
   * 
   * @return true, if it is a protest function
   *         false , otherwise
   */
  bool
  isProtestFunctionCall(FunctionDecl* decl, std::string& type)
  {
    for (auto& params : decl->parameters())
    {
      if (params->hasDefaultArg() && !params->hasUninstantiatedDefaultArg())
      {
        type = QualType::getAsString(params->getType().split(),
                                     PrintingPolicy {{}});
        clang::Expr* defaultValue = params->getDefaultArg();
        auto defaultValueCall = dyn_cast_or_null<CallExpr>(defaultValue);
        if (defaultValueCall)
        {
          auto defaultFunc =
              dyn_cast_or_null<FunctionDecl>(defaultValueCall->getCalleeDecl());
          if (defaultFunc)
          {
            if (defaultFunc->getQualifiedNameAsString().find(
                    "protest::meta::") == 0 &&
                defaultFunc->getQualifiedNameAsString().find(
                    "defaultContext") != std::string::npos)
            {
              return true;
            }
          }
        }
      }
    }
    return false;
  }

  template <typename T>
  std::string
  getArgumentsAsString(T* expr, FunctionDecl* decl)
  {
    std::string exprAsString = "";
    for (unsigned int i = 0; i < decl->getMinRequiredArguments(); i++)
    {
      const clang::Expr* arg = expr->getArg(i);
      CharSourceRange conditionRange =
          CharSourceRange::getTokenRange(arg->getBeginLoc(), arg->getEndLoc());
      exprAsString += Rewriter.getRewrittenText(conditionRange) + ", ";
    }
    return exprAsString;
  }

  void
  writeMetaInfos(Expr* expr,
                 std::string name,
                 std::vector<std::string> args,
                 std::string metaType = "protest::meta::CallContext",
                 std::string objectName = "",
                 std::map<std::string, std::string> comments = {})
  {
    int id = 0;
    if (mMetaInfos.count(std::pair(expr->getBeginLoc(), expr->getEndLoc())) > 0)
    {
      id = mMetaInfos[std::pair(expr->getBeginLoc(), expr->getEndLoc())].second;
    }
    else
    {
      id = mIdNext;
      mIdNext++;
    }
    std::stringstream output;
    output << "static " << metaType << " " << name << id << "(protest_unit, "
           << lineNumber(Context, expr) << ", \""
           << trim(escapeString(objectName)) << "\"";

    output << ", { ";
    bool first = true;
    for (auto& arg : args)
    {
      if (first)
      {
        output << "\"" << escapeString(trim(arg)) << "\"";
        first = false;
      }
      else
      {
        output << ", \"" << escapeString(trim(arg)) << "\"";
      }
    }
    output << " }";
    output << ", { ";
    for (auto& comment : comments)
    {
      output << "{ \"" << comment.first << "\", ";
      output << "\"" << comment.second << "\" },";
    }
    output << " }";
    output << ");"
           << "\n";

    mMetaInfos[std::pair(expr->getBeginLoc(), expr->getEndLoc())] =
        std::pair<std::string, int>(output.str(), id);
  }

  // TODO (jreinking) code duplication
  void
  writeMetaInfos(Decl* expr,
                 std::string name,
                 std::vector<std::string> args,
                 std::string metaType = "protest::meta::CallContext",
                 std::string objectName = "",
                 std::map<std::string, std::string> comments = {})
  {
    int id = 0;
    // if (mMetaInfos.count(std::pair(expr->getBeginLoc(), expr->getEndLoc())) > 0)
    // {
    //   id = mMetaInfos[std::pair(expr->getBeginLoc(), expr->getEndLoc())].second;
    // }
    // else
    // {
    id = mIdNext;
    mIdNext++;
    // }
    std::stringstream output;
    output << "static " << metaType << " " << name << id << "(protest_unit, "
           << lineNumber(Context, expr) << ", \""
           << trim(escapeString(objectName)) << "\"";

    output << ", { ";
    bool first = true;
    for (auto& arg : args)
    {
      if (first)
      {
        output << "\"" << escapeString(trim(arg)) << "\"";
        first = false;
      }
      else
      {
        output << ", \"" << escapeString(trim(arg)) << "\"";
      }
    }
    output << " }";
    output << ", { ";
    for (auto& comment : comments)
    {
      output << "{ \"" << comment.first << "\", ";
      output << "\"" << comment.second << "\" },";
    }
    output << " }";
    output << ");"
           << "\n";

    std::string other;
    if (mMetaInfos.count(std::pair(expr->getBeginLoc(), expr->getEndLoc())) > 0)
    {
      other =
          mMetaInfos[std::pair(expr->getBeginLoc(), expr->getEndLoc())].first;
    }
    mMetaInfos[std::pair(expr->getBeginLoc(), expr->getEndLoc())] =
        std::pair<std::string, int>(other + output.str(), id);
  }

  template <typename T>
  std::string
  getArgument(const T* call, unsigned int n)
  {
    if (n < call->getNumArgs())
    {
      const Expr* arg = call->getArg(0);
      bool invalid = false;
      CharSourceRange conditionRange =
          CharSourceRange::getTokenRange(arg->getBeginLoc(), arg->getEndLoc());
      StringRef exprAsString = Lexer::getSourceText(conditionRange,
                                                    Context->getSourceManager(),
                                                    Context->getLangOpts(),
                                                    &invalid);
      assert(!invalid);
      return exprAsString.str();
    }
    return "";
  }

  bool
  isRecord(const RecordType* tt)
  {
    auto record = tt->getAsCXXRecordDecl();
    return record;
  }

  bool
  isRecord(const RecordDecl* tt)
  {
    return tt;
  }

  bool
  createRecordPrinter(const RecordType* tt)
  {
    auto record = tt->getAsCXXRecordDecl();
    if (record)
    {
      return createRecordPrinter(record);
    }
    else
    {
      return false;
    }
  }

  bool
  createRecordPrinter(const RecordDecl* tt)
  {
    if (tt)
    {
      if (mImpls.count(std::pair(tt->getBeginLoc(), tt->getEndLoc())) == 0)
      {
        // just to prevent recursion
        mImpls[std::pair(tt->getBeginLoc(), tt->getEndLoc())] = "";
        std::stringstream output;
        if (isInMainFile(Context, tt))
        {
          int n = 0;
          auto parent = tt->getParent();
          while (parent && parent->isNamespace())
          {
            // TODO error when inner type since it cannot be forwarded
            auto ns = dyn_cast_or_null<NamespaceDecl>(parent);
            if (ns)
            {
              n++;
              output << "namespace " << ns->getNameAsString() << " { ";
              parent = parent->getParent();
            }
            else
            {
              parent = nullptr;
            }
          }

          if (tt->isStruct())
          {
            output << "struct " << tt->getNameAsString() << ";";
          }
          else
          {
            output << "class " << tt->getNameAsString() << ";";
          }

          for (int i = 0; i < n; i++)
          {
            output << " } ";
          }
        }

        output << "void toString(protest::log::UniversalStream& output, const "
               << tt->getQualifiedNameAsString() << "& value); ";

        // template <>
        output << "template <> struct protest::log::HasProtestOperator<"
               << tt->getQualifiedNameAsString() << "> : std::true_type"
               << "{"
               << "};\n";

        // TODO check if there is already a meta info for the type
        mMetaInfos[std::pair(tt->getBeginLoc(), tt->getEndLoc())] = {
            output.str(),
            0};
        size_t largestFiled = 0;
        for (auto field : tt->fields())
        {
          if (field->getNameAsString().size() >= largestFiled)
          {
            largestFiled = field->getNameAsString().size();
          }
        }
        std::stringstream impl;
        impl << "void toString(protest::log::UniversalStream& output, const "
             << tt->getQualifiedNameAsString() << "& value) {"
             << " output.mOutput << \"";
        if (tt->isStruct())
        {
          impl << "struct " << tt->getQualifiedNameAsString();
        }
        else
        {
          impl << "class " << tt->getQualifiedNameAsString();
        }

        impl << " {\\n\"; output.incrementIndent();";
        for (auto field : tt->fields())
        {
          bool isARecord = false;
          auto* t = dyn_cast_or_null<EnumType>(field->getType().getTypePtr());
          auto* tt =
              dyn_cast_or_null<RecordType>(field->getType().getTypePtr());
          auto ttt =
              dyn_cast_or_null<PointerType>(field->getType().getTypePtr());
          if (t)
          {
            createEnumPrinter(t->getDecl());
          }
          else if (tt && isRecord(tt))
          {
            if (isInMainFile(Context, tt->getAsRecordDecl()))
            {
              createRecordPrinter(tt);
            }
            isARecord = true;
          }
          else if (ttt && isRecord(ttt->getPointeeCXXRecordDecl()))
          {
            if (isInMainFile(Context, ttt->getPointeeCXXRecordDecl()))
            {
              createRecordPrinter(ttt->getPointeeCXXRecordDecl());
            }
            isARecord = true;
          }
          else
          {
          }

          impl << "output.printIndent(); ";

          if (isARecord)
          {
            impl << "output.mOutput << \"" << std::setfill(' ') << std::left
                 << std::setw(largestFiled + 1) << field->getNameAsString()
                 << ": \"; output.printRecord(value."
                 << field->getNameAsString() << "); output.mOutput << \"\\n\";";
          }
          else
          {
            impl << "output.mOutput << \"" << std::setfill(' ') << std::left
                 << std::setw(largestFiled + 1) << field->getNameAsString()
                 << ": \"; ";
            impl << "output << value." << field->getNameAsString() << ";";
            impl << "output.mOutput << \"\\n\";";
          }
        }
        impl << "output.decrementIndent(); output.printIndent(); "
                "output.mOutput << "
                "\"}\";";
        impl << "}\n";
        mImpls[std::pair(tt->getBeginLoc(), tt->getEndLoc())] = impl.str();
      }
      return true;
    }
    else
    {
      return false;
    }
  }

  void
  createEnumPrinter(EnumDecl* edecl)
  {
    if (mImpls.count(std::pair(edecl->getBeginLoc(), edecl->getEndLoc())) == 0)
    {
      std::stringstream output;
      if (isInMainFile(Context, edecl))
      {
        int n = 0;
        auto parent = edecl->getParent();
        while (parent && parent->isNamespace())
        {
          auto ns = dyn_cast_or_null<NamespaceDecl>(parent);
          if (ns)
          {
            n++;
            output << "namespace " << ns->getNameAsString() << " { ";
            parent = parent->getParent();
          }
          else
          {
            parent = nullptr;
          }
        }

        output << "enum class " << edecl->getNameAsString() << ";";

        for (int i = 0; i < n; i++)
        {
          output << " } ";
        }
      }

      output << "void toString(protest::log::UniversalStream& output, const "
             << edecl->getQualifiedNameAsString() << "& value); ";

      // template <>
      output << "template <> struct protest::log::HasProtestOperator<"
             << edecl->getQualifiedNameAsString() << "> : std::true_type"
             << "{"
             << "};\n";

      // TODO check if there is already a meta info for the type
      mMetaInfos[std::pair(edecl->getBeginLoc(), edecl->getEndLoc())] = {
          output.str(),
          0};
      std::stringstream impl;
      impl << "void toString(protest::log::UniversalStream& output, const "
           << edecl->getQualifiedNameAsString() << "& value) {";
      for (auto e : edecl->enumerators())
      {
        impl << "if (value == " << edecl->getQualifiedNameAsString()
             << "::" << e->getNameAsString()
             << ") { std::stringstream ss1; ss1 << std::setfill('0') << "
                "std::setw(sizeof(value) * 2) << std::hex << "
                "static_cast<int>(value); output.getPlain() << \"{ "
             << edecl->getQualifiedNameAsString()
             << "::" << e->getNameAsString() << "\" << \", \" << "
             << "std::to_string(static_cast<int>(value)) << \", 0x\" << "
                "ss1.str() << \" }\";}";
      }
      impl << "; }\n";
      mImpls[std::pair(edecl->getBeginLoc(), edecl->getEndLoc())] = impl.str();
    }
  }

  ASTContext* Context;
  clang::Rewriter& Rewriter;
  llvm::raw_fd_ostream& mOutputFile;
  size_t mIdNext;
};

class ProtestConsumer : public clang::ASTConsumer
{
public:
  explicit ProtestConsumer(ASTContext* Context,
                           llvm::raw_fd_ostream& outputFile) :
    Visitor(Context, Rewriter, outputFile),
    mOutputFile(outputFile)
  {
    Rewriter.setSourceMgr(Context->getSourceManager(), Context->getLangOpts());
  }

  virtual void
  HandleTranslationUnit(clang::ASTContext& Context)
  {
    auto file =
        Context.getSourceManager()
            .getFileEntryForID(Context.getSourceManager().getMainFileID())
            ->getName();
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    std::string ostr;
    raw_string_ostream stringstream(ostr);
    Rewriter.getEditBuffer(Rewriter.getSourceMgr().getMainFileID())
        .write(stringstream);
    auto n = ostr.rfind("// PROTEST: end of file");
    auto nn = ostr.find("\n", n);
    // place the template specialization after the last header, so that all
    // user defined class definition are visible.
    if (nn != std::string::npos)
    {
      auto prefix = ostr.substr(0, nn + 1);
      auto suffix = ostr.substr(nn + 1);
      mOutputFile << prefix;
      mOutputFile
          << "static protest::meta::Unit protest_unit(PROTEST_ORIGIN_FILE);\n";

      for (auto& meta : Visitor.mMetaInfos)
      {
        mOutputFile << meta.second.first;
      }

      Visitor.mMemberAttrVisitor.writeDeclarations(mOutputFile);
      Visitor.mMemberFuncVisitor.writeDeclarations(mOutputFile);
      Visitor.mStaticAttrVisitor.writeDeclarations(mOutputFile);
      Visitor.mStaticVarVisitor.writeDeclarations(mOutputFile);

      // operator2 needs template specialization from above. Therefore it must
      // be include at last #include
      mOutputFile << "#include <protest/log/operator2.h>\n\n";
      mOutputFile << "\n#line "
                  << std::count(prefix.begin(), prefix.end(), '\n') << " \""
                  << file << "\"\n";
      mOutputFile << suffix;
    }
    else
    {
      llvm::errs() << "LAST INCLUDE NOT FOUND\n";
    }

    for (auto& impl : Visitor.mImpls)
    {
      mOutputFile << impl.second;
    }

    Visitor.mMemberFuncVisitor.writeImplementation(mOutputFile);
    Visitor.mMemberAttrVisitor.writeImplementation(mOutputFile);
    Visitor.mStaticAttrVisitor.writeImplementation(mOutputFile);
  }

public:
  ProtestVisitor Visitor;
  clang::Rewriter Rewriter;
  llvm::raw_fd_ostream& mOutputFile;
};

class ProtestCallAction : public clang::ASTFrontendAction
{
public:
  ProtestCallAction(llvm::raw_fd_ostream& outputFile) : mOutputFile(outputFile)
  {
  }

  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance& Compiler, llvm::StringRef InFile)
  {
    return std::unique_ptr<clang::ASTConsumer>(
        new ProtestConsumer(&Compiler.getASTContext(), mOutputFile));
  }

private:
  llvm::raw_fd_ostream& mOutputFile;
};

std::unique_ptr<FrontendActionFactory>
protestFrontendActionFactory(llvm::raw_fd_ostream& outputFile)
{
  class SimpleFrontendActionFactory : public FrontendActionFactory
  {
  public:
    SimpleFrontendActionFactory(llvm::raw_fd_ostream& outputFile) :
      mOutputFile(outputFile)
    {
    }

    std::unique_ptr<FrontendAction>
    create() override
    {
      return std::make_unique<ProtestCallAction>(mOutputFile);
    }

  private:
    llvm::raw_fd_ostream& mOutputFile;
  };

  return std::unique_ptr<FrontendActionFactory>(
      new SimpleFrontendActionFactory(outputFile));
}

static OptionCategory PtOptions("protest-compiler options");
static cl::opt<std::string> OutputPath("o", desc("Output file"), cl::Required);
static cl::opt<std::string> RootPath("r", desc("Root path"), cl::Optional);

int
main(int argc, const char** argv)
{
  int argcCopy = argc + 1;
  const char* argvCopy[argcCopy];
  for (int i = 0; i < argc; i++)
  {
    argvCopy[i] = argv[i];
  }
  // add clang default include dir so that the binary is relocateable
  argvCopy[argc] = "-I" PATH_TO_STD_CLANG_INCLUDE;

  llvm::Expected<tooling::CommonOptionsParser> OptionsParser =
      CommonOptionsParser::create(argcCopy, argvCopy, PtOptions);

  if (OptionsParser)
  {
    ClangTool Tool(OptionsParser.get().getCompilations(),
                   OptionsParser.get().getSourcePathList());

    StringRef outputFile(OutputPath.c_str());

    // Open the output file
    std::error_code errorCode;
    llvm::raw_fd_ostream HOS(outputFile.str(),
                             errorCode,
                             llvm::sys::fs::OF_None);
    if (errorCode)
    {
      llvm::errs() << "while opening '" << outputFile.str()
                   << "': " << errorCode.message() << '\n';
      return 1;
    }

    auto af = protestFrontendActionFactory(HOS);
    int result = Tool.run(af.get());
    return result;
  }
  return 1;
}