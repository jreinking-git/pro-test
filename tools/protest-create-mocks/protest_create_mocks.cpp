// ---------------------------------------------------------------------------
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

#include <inja/inja.hpp>

#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>

using namespace clang;
using namespace clang::tooling;
using namespace llvm::cl;
using namespace llvm;

std::string tmpl = std::string(
    R"(

class {{ class_id }}_Mocker;

class {{ class_id }}_Mock : public {{ interface }} { 
public:
  {{ class_id }}_Mock({{ class_id }}_Mocker& mocker) :
    mMocker(mocker)
  {
  }

## for method in methods
  {{ method.return_type }}
  {{ method.name }}({% for param in method.params %}{% if not loop.is_first %}, {% endif %}{{ param.type }} {{ param.name }}{% endfor %});

## endfor 
private:
  {{ class_id }}_Mocker& mMocker;
};

class {{ class_id }}_Mocker : public protest::mock::internal::MockBase {
  public:
    {{ class_id }}_Mocker(protest::meta::MockCreation& callContext) :
      protest::mock::internal::MockBase(callContext),
      mMock(*this)
    {

    }

    {{ class_id }}_Mocker(MyInterface_Mocker& other) :
      protest::mock::internal::MockBase(protest::meta::MockCreation::defaultContext()),
      mMock(*this)
    {
      assert(false);
    }

## for method in methods
  struct FunctionTraits{{ loop.index }}
  {
    using ReturnType = {{ method.return_type }};
    using Matchers = std::tuple<{% for param in method.params %}{% if not loop.is_first %}, {% endif %}protest::matcher::Matcher<{{ param.type }}>{% endfor %}>;
    using Args = std::tuple<{% for param in method.params %}{% if not loop.is_first %}, {% endif %}{{ param.type }}{% endfor %}>;
    using FunctionType = ReturnType({% for param in method.params %}{% if not loop.is_first %}, {% endif %}{{ param.type }}{% endfor %});
  };

## endfor

## for method in methods
  class Function{{ loop.index }}  : public protest::mock::internal::FunctionMockerBase<FunctionTraits{{ loop.index }}>
  {
  public:
    Function{{ loop.index }}(protest::mock::internal::MockBase& mock) :
      protest::mock::internal::FunctionMockerBase<FunctionTraits{{ loop.index }}>(mock,
                                        name,
                                        &paramsTypes[0],
                                        &params[0],
                                        numberOfParameters)
      {
        mock.addFunctionMocker(this);
      }
      static constexpr const char* name = "{{ method.name }}";
      static constexpr size_t numberOfParameters = {{ length(method.params) }};
      static constexpr const char* const paramsTypes[numberOfParameters] = {
## for param in method.params
        "{{ param.type }}"{% if not loop.is_last %}, {% endif %}
## endfor
      };
      static constexpr const char* const params[numberOfParameters] = {
## for param in method.params
        "{{ param.name }}"{% if not loop.is_last %}, {% endif %}
## endfor
      };
    };

    std::shared_ptr<Function{{ loop.index }}> function{{ loop.index }} = std::make_shared<Function{{ loop.index }}>(*this);

## endfor

## for method in methods
  protest::mock::ExpectationHandle<FunctionTraits{{ loop.index }}>
  {{ method.name }}({% for param in method.params %}protest::matcher::Matcher<{{ param.type }}>&& {{ param.name }}, {% endfor %}
        protest::meta::ExpectCall& callContext = protest::meta::ExpectCall::defaultContext())
  {
    auto matchers = std::make_tuple({% for param in method.params %}{% if not loop.is_first %}, {% endif %}std::move({{ param.name }}){% endfor %});
    auto expectation = std::make_shared<protest::mock::Expectation<FunctionTraits{{ loop.index }}>>(function{{ loop.index }},
                                                                std::move(matchers),
                                                                callContext);
    function{{ loop.index }}->addExpectation(expectation);
    return ExpectationHandle<FunctionTraits{{ loop.index }}>(expectation);
  }

  {{ method.return_type }}
  {{ method.name }}(Function{{ loop.index }}::Args& args)
  {
    return function{{ loop.index }}->evaluatedCall(args);
  }

## endfor

  operator {{ interface }}&()
  {
    return mMock;
  }

  ~{{ class_id }}_Mocker()
  {
    protest::mock::internal::MockBase::checkMissingCalls();
## for method in methods
    function{{ loop.index }}->enterPassiveMode();
    function{{ loop.index }} = nullptr;
## endfor
  }

private:
  {{ class_id }}_Mock mMock;
};

## for method in methods
  {{ method.return_type }}
  {{ class_id }}_Mock::{{ method.name }}({% for param in method.params %}{% if not loop.is_first %}, {% endif %}{{ param.type }} {{ param.name }}{% endfor %})
  {
    auto args = std::make_tuple({% for param in method.params %}{% if not loop.is_first %}, {% endif %}{{ param.name }}{% endfor %});
    return mMocker.{{ method.name }}(args);
  }
## endfor

namespace protest
{

namespace mock
{

template <>
struct HasMockImpl<{{ interface }}> : std::true_type
{
};

} // namespace mock

template <typename T>
std::enable_if_t<std::is_same_v<T, {{ interface }}>, {{ class_id }}_Mocker>
createMock(protest::meta::MockCreation& callContext =
               protest::meta::MockCreation::defaultContext())
{
  callContext.incrementNumberOfCreations();
  return {{ class_id }}_Mocker(callContext);
}

} // namespace protest

)");

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
    mIdNext(0)
  {
  }

  std::string
  qualifiedNameToIdentifier(clang::CXXRecordDecl* decl)
  {
    return qualifiedNameToIdentifier(decl->getQualifiedNameAsString());
  }

  std::string
  qualifiedNameToIdentifier(std::string name)
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

  bool
  VisitCallExpr(CallExpr* Expr)
  {
    auto cxxcall = dyn_cast_or_null<CXXMemberCallExpr>(Expr);
    FunctionDecl* decl = dyn_cast_or_null<FunctionDecl>(Expr->getCalleeDecl());

    std::string type;
    if (decl && isProtestFunctionCall(decl, type))
    {
      if (decl->getQualifiedNameAsString() == "protest::createMock")
      {
        std::stringstream ss;

        assert(decl->isFunctionTemplateSpecialization());
        auto* targs = decl->getTemplateSpecializationArgs();
        if (targs)
        {
          for (auto arg : targs->asArray())
          {
            auto* record = arg.getAsType().getTypePtr()->getAsCXXRecordDecl();
            if (record)
            {
              nlohmann::json data = nlohmann::json::object();
              std::string classIdentifier = qualifiedNameToIdentifier(record);
              data["interface"] = record->getQualifiedNameAsString();
              data["class_id"] = classIdentifier;
              nlohmann::json methodsAsJson = nlohmann::json::array();

              for (auto method : record->methods())
              {
                if (method->isVirtual())
                {
                  nlohmann::json methodAsJson = nlohmann::json::object();
                  methodAsJson["return_type"] =
                      getSourceText(Context,
                                    CharSourceRange::getTokenRange(
                                        method->getReturnTypeSourceRange()));
                  methodAsJson["name"] = method->getNameAsString();
                  nlohmann::json paramsAsJson = nlohmann::json::array();
                  int i = 1;
                  for (auto param : method->parameters())
                  {
                    nlohmann::json paramAsJson = nlohmann::json::object();
                    paramAsJson["type"] = param->getType().getAsString();
                    if (param->getNameAsString() == "")
                    {
                      paramAsJson["name"] =
                          std::string("arg") + std::to_string(i);
                    }
                    else
                    {
                      paramAsJson["name"] = param->getNameAsString();
                    }
                    i++;
                    paramsAsJson.push_back(paramAsJson);
                  }
                  methodAsJson["params"] = paramsAsJson;
                  methodsAsJson.push_back(methodAsJson);
                }
              }
              data["methods"] = methodsAsJson;

              auto constructor = inja::render(tmpl, data);
              ss << constructor;
            }
          }
          mMocks[decl] = ss.str();
        }
      }
    }
    return true;
  }

  bool
  shouldTraversePostOrder()
  {
    return true;
  }

  std::set<clang::Decl*> mAlreadyTraversed;
  std::map<clang::Decl*, std::string> mMocks;

private:
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
    for (auto mock : Visitor.mMocks)
    {
      mOutputFile << mock.second << "\n";
    }
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
    // ignore result since ther might be compile error
    // mock will be generated anyway
    return 0;
  }
  return 1;
}