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

#pragma once

#include "protest/log/universal_stream.h"

#include <cassert>

namespace protest
{

namespace matcher
{

// ---------------------------------------------------------------------------
template <typename Pointer>
inline const typename Pointer::element_type*
getRawPointer(const Pointer& ptr)
{
  return ptr.get();
}

template <typename Element>
inline Element*
getRawPointer(Element* ptr)
{
  return ptr;
}

inline void*
getRawPointer(std::nullptr_t)
{
  return NULL;
}

// ---------------------------------------------------------------------------
/**
 * @class MatcherInterface
 */
template <typename Opr>
class MatcherInterface
{
public:
  explicit MatcherInterface() = default;

  MatcherInterface(const MatcherInterface& other) = delete;

  MatcherInterface(MatcherInterface&& other) = delete;

  MatcherInterface&
  operator=(const MatcherInterface& other) = delete;

  MatcherInterface&
  operator=(MatcherInterface&& other) = delete;

  virtual ~MatcherInterface() = default;

// ---------------------------------------------------------------------------
  virtual bool
  check(Opr lhs) = 0;

  virtual void
  explain(const char* param, Opr lhs, log::UniversalStream& stream) = 0;

  virtual void
  explainNegative(const char* param, Opr lhs, log::UniversalStream& stream) = 0;

private:
};

// ---------------------------------------------------------------------------
/**
 * @class UnaryMatcherInterface
 */
template <typename M, typename Opr>
class UnaryMatcherInterface : public MatcherInterface<Opr>
{
public:
  explicit UnaryMatcherInterface() = default;

  UnaryMatcherInterface(const UnaryMatcherInterface& other) = delete;

  UnaryMatcherInterface(UnaryMatcherInterface&& other) = delete;

  UnaryMatcherInterface&
  operator=(const UnaryMatcherInterface& other) = delete;

  UnaryMatcherInterface&
  operator=(UnaryMatcherInterface&& other) = delete;

  ~UnaryMatcherInterface() = default;

// ---------------------------------------------------------------------------
  bool
  check(Opr opr) override;

  void
  explain(const char* param, Opr opr, log::UniversalStream& stream) override;

  void
  explainNegative(const char* param,
                  Opr opr,
                  log::UniversalStream& stream) override;

private:
  void
  explain(const char* param,
          Opr value,
          log::UniversalStream& stream,
          bool negative,
          bool check);
};

// ---------------------------------------------------------------------------
template <typename M, typename Opr>
bool
UnaryMatcherInterface<M, Opr>::check(Opr lhs)
{
  return M::checkValue(lhs);
}

template <typename M, typename Opr>
void
UnaryMatcherInterface<M, Opr>::explain(const char* param,
                                       Opr opr,
                                       log::UniversalStream& stream)
{
  explain(param, opr, stream, false, M::checkValue(opr));
}

template <typename M, typename Opr>
void
UnaryMatcherInterface<M, Opr>::explainNegative(const char* param,
                                               Opr opr,
                                               log::UniversalStream& stream)
{
  explain(param, opr, stream, true, M::checkValue(opr));
}

template <typename M, typename Opr>
void
UnaryMatcherInterface<M, Opr>::explain(const char* param,
                                       Opr value,
                                       log::UniversalStream& stream,
                                       bool negative,
                                       bool check)
{
  static constexpr size_t indent = 10;
  stream.mOutput << "Value of: " << param << "\n";
  stream.mOutput << "Expected: ";
  if (!negative)
  {
    stream.mOutput << M::description;
  }
  else
  {
    stream.mOutput << M::negativeDescription;
  }
  stream.mOutput << "\n";

  if (negative ? !check : check)
  {
    stream.mOutput << "  Actual: ";
  }
  else
  {
    stream.mOutput << "  But is: ";
  }

  stream.incrementIndent(indent);
  // stream.mOutput << asString;
  M::printValue(value, stream);
  stream.decrementIndent(indent);
  stream.mOutput << "\n";
}

// ---------------------------------------------------------------------------
/**
 * @class BinaryMatcherInterface
 */
template <typename M, typename Lhs, typename Rhs>
class BinaryMatcherInterface : public MatcherInterface<Lhs>
{
public:
  explicit BinaryMatcherInterface(Rhs rhs) : mRhs(rhs)
  {
  }

  BinaryMatcherInterface(const BinaryMatcherInterface& other) = delete;

  BinaryMatcherInterface(BinaryMatcherInterface&& other) = delete;

  BinaryMatcherInterface&
  operator=(const BinaryMatcherInterface& other) = delete;

  BinaryMatcherInterface&
  operator=(BinaryMatcherInterface&& other) = delete;

  ~BinaryMatcherInterface() = default;

// ---------------------------------------------------------------------------
  bool
  check(Lhs lhs) override;

  void
  explain(const char* param, Lhs lhs, log::UniversalStream& stream) override;

  void
  explainNegative(const char* param,
                  Lhs lhs,
                  log::UniversalStream& stream) override;

private:
  void
  explain(const char* param,
          Lhs lhs,
          Rhs rhs,
          log::UniversalStream& stream,
          bool negative,
          bool check);

  Rhs mRhs;
};

// ---------------------------------------------------------------------------
template <typename M, typename Lhs, typename Rhs>
bool
BinaryMatcherInterface<M, Lhs, Rhs>::check(Lhs lhs)
{
  return M::checkValue(lhs, mRhs);
}

template <typename M, typename Lhs, typename Rhs>
void
BinaryMatcherInterface<M, Lhs, Rhs>::explain(const char* param,
                                             Lhs lhs,
                                             log::UniversalStream& stream)
{
  explain(param, lhs, mRhs, stream, false, M::checkValue(lhs, mRhs));
}

template <typename M, typename Lhs, typename Rhs>
void
BinaryMatcherInterface<M, Lhs, Rhs>::explainNegative(
    const char* param,
    Lhs lhs,
    log::UniversalStream& stream)
{
  explain(param, lhs, mRhs, stream, true, M::checkValue(lhs, mRhs));
}

template <typename M, typename Lhs, typename Rhs>
void
BinaryMatcherInterface<M, Lhs, Rhs>::explain(const char* param,
                                             Lhs lhs,
                                             Rhs rhs,
                                             log::UniversalStream& stream,
                                             bool negative,
                                             bool check)
{
  static constexpr size_t indent = 10;
  stream.mOutput << "Value of: " << param << "\n";
  stream.mOutput << "Expected: ";

  if (!negative)
  {
    stream.mOutput << M::description;
  }
  else
  {
    stream.mOutput << M::negativeDescription;
  }

  stream.mOutput << "\n";
  stream.incrementIndent(indent);
  stream.printIndent();
  M::printValue(rhs, stream);
  stream.decrementIndent(indent);
  stream.mOutput << "\n";

  if (negative ? !check : check)
  {
    stream.mOutput << "  Actual: ";
  }
  else
  {
    stream.mOutput << "  But is: ";
  }

  stream.incrementIndent(indent);
  M::printValue(lhs, stream);
  stream.decrementIndent(indent);
  stream.mOutput << "\n";
}

// ---------------------------------------------------------------------------
/**
 * @class Matcher
 */
template <typename Opr>
class Matcher
{
public:
  explicit Matcher(MatcherInterface<Opr>* interface);

  Matcher(const Matcher& other) = delete;

  Matcher(Matcher&& other);

  Matcher&
  operator=(const Matcher& other) = delete;

  Matcher&
  operator=(Matcher&& other);

  ~Matcher();

// ---------------------------------------------------------------------------
  bool
  check(Opr value);

  void
  explain(const char* param, Opr value, protest::log::UniversalStream& stream);

  void
  explainNegative(const char* param,
                  Opr value,
                  protest::log::UniversalStream& stream);

  void
  clear();

private:
  MatcherInterface<Opr>* mInterface;
};

// ---------------------------------------------------------------------------
template <typename Opr>
Matcher<Opr>::Matcher(MatcherInterface<Opr>* interface) : mInterface(interface)
{
}

template <typename Opr>
Matcher<Opr>::Matcher(Matcher&& other)
{
  mInterface = other.mInterface;
  other.mInterface = nullptr;
}

template <typename Opr>
Matcher<Opr>&
Matcher<Opr>::operator=(Matcher&& other)
{
  mInterface = other.mInterface;
  other.mInterface = nullptr;
  return *this;
}

template <typename Opr>
Matcher<Opr>::~Matcher()
{
  if (mInterface != nullptr)
  {
    delete mInterface;
    mInterface = nullptr;
  }
  else
  {
  }
}

// ---------------------------------------------------------------------------
template <typename Opr>
bool
Matcher<Opr>::check(Opr value)
{
  return mInterface->check(value);
}

template <typename Opr>
void
Matcher<Opr>::explain(const char* param,
                      Opr value,
                      protest::log::UniversalStream& stream)
{
  mInterface->explain(param, value, stream);
}

template <typename Opr>
void
Matcher<Opr>::explainNegative(const char* param,
                              Opr value,
                              protest::log::UniversalStream& stream)
{
  mInterface->explainNegative(param, value, stream);
}

template <typename Opr>
void
Matcher<Opr>::clear()
{
  delete mInterface;
  mInterface = nullptr;
}

// ---------------------------------------------------------------------------
/**
 * @class ComparisonBase
 */
template <typename Rhs, typename Comparison>
class ComparisonBase
{
public:
  ComparisonBase(Rhs value);

  ComparisonBase(const ComparisonBase& other) = default;

  ComparisonBase(ComparisonBase&& other) = default;

  ComparisonBase&
  operator=(const ComparisonBase& other) = delete;

  ComparisonBase&
  operator=(ComparisonBase&& other) = delete;

  ~ComparisonBase() = default;

// ---------------------------------------------------------------------------
  template <typename Lhs>
  operator Matcher<Lhs>();

private:
  /**
   * @class ComparisonImpl
   */
  template <typename Lhs>
  class ComparisonImpl :
    public BinaryMatcherInterface<ComparisonImpl<Lhs>, Lhs, Rhs>
  {
  public:
    static constexpr const char* description = Comparison::description;
    static constexpr const char* negativeDescription =
        Comparison::negativeDescription;

    explicit ComparisonImpl(Rhs rhs);

    ComparisonImpl(const ComparisonImpl& other) = delete;

    ComparisonImpl(ComparisonImpl&& other) = delete;

    ComparisonImpl&
    operator=(const ComparisonImpl& other) = delete;

    ComparisonImpl&
    operator=(ComparisonImpl&& other) = delete;

    ~ComparisonImpl() = default;

// ---------------------------------------------------------------------------
    static bool
    checkValue(Lhs lhs, Rhs rhs);

    template <typename Opr>
    static void
    printValue(Opr opr, log::UniversalStream& stream);

  private:
  };

  Rhs mValue;
};

// ---------------------------------------------------------------------------
template <typename Rhs, typename Comparison>
ComparisonBase<Rhs, Comparison>::ComparisonBase(Rhs value) : mValue(value)
{
}

template <typename Rhs, typename Comparison>
template <typename Lhs>
ComparisonBase<Rhs, Comparison>::operator Matcher<Lhs>()
{
  return Matcher<Lhs>(new ComparisonImpl<Lhs>(mValue));
}

// ---------------------------------------------------------------------------
template <typename Rhs, typename Comparison>
template <typename Lhs>
ComparisonBase<Rhs, Comparison>::ComparisonImpl<Lhs>::ComparisonImpl(Rhs rhs) :
  BinaryMatcherInterface<ComparisonImpl<Lhs>, Lhs, Rhs>(rhs)
{
}

template <typename Rhs, typename Comparison>
template <typename Lhs>
bool
ComparisonBase<Rhs, Comparison>::ComparisonImpl<Lhs>::checkValue(Lhs lhs,
                                                                 Rhs rhs)
{
  bool ret = Comparison::compare(lhs, rhs);
  return ret;
}

template <typename Rhs, typename Comparison>
template <typename Lhs>
template <typename Opr>
void
ComparisonBase<Rhs, Comparison>::ComparisonImpl<Lhs>::printValue(
    Opr opr,
    log::UniversalStream& stream)
{
  stream << opr;
}

// ---------------------------------------------------------------------------
#define BINARY_COMPARISON(NAME, OPERATOR, DESCRIPTION, NEGATIVE_DESCRIPTION)   \
  template <typename Rhs>                                                      \
  class NAME : public matcher::ComparisonBase<Rhs, NAME<Rhs>>                  \
  {                                                                            \
  public:                                                                      \
    static constexpr const char* description = DESCRIPTION;                    \
    static constexpr const char* negativeDescription = NEGATIVE_DESCRIPTION;   \
                                                                               \
    template <typename Lhs>                                                    \
    static bool                                                                \
    compare(Lhs lhs, Rhs rhs)                                                  \
    {                                                                          \
      return lhs OPERATOR rhs;                                                 \
    }                                                                          \
                                                                               \
    explicit NAME(Rhs rhs);                                                    \
                                                                               \
    NAME(const NAME& other) = default;                                         \
                                                                               \
    NAME(NAME&& other) = default;                                              \
                                                                               \
    NAME&                                                                      \
    operator=(const NAME& other) = delete;                                     \
                                                                               \
    NAME&                                                                      \
    operator=(NAME&& other) = delete;                                          \
                                                                               \
    ~NAME() = default;                                                         \
                                                                               \
  private:                                                                     \
  };                                                                           \
                                                                               \
  template <typename Rhs>                                                      \
  NAME<Rhs>::NAME(Rhs rhs) : matcher::ComparisonBase<Rhs, NAME<Rhs>>(rhs)      \
  {                                                                            \
  }

// ---------------------------------------------------------------------------
/**
 * @class NotMatcher
 */
template <typename Inner>
class NotMatcher
{
public:
  explicit NotMatcher(Inner inner);

  NotMatcher(const NotMatcher& other) = default;

  NotMatcher(NotMatcher&& other) = delete;

  NotMatcher&
  operator=(const NotMatcher& other) = delete;

  NotMatcher&
  operator=(NotMatcher&& other) = delete;

  ~NotMatcher() = default;

  template <typename Lhs>
  operator matcher::Matcher<Lhs>();

private:
  /**
   * @class NotMatcherImpl
   */
  template <typename Lhs>
  class NotMatcherImpl : public matcher::MatcherInterface<Lhs>
  {
  public:
    explicit NotMatcherImpl(matcher::Matcher<Lhs>&& inner);

    NotMatcherImpl(const NotMatcherImpl& other) = delete;

    NotMatcherImpl(NotMatcherImpl&& other) = delete;

    NotMatcherImpl&
    operator=(const NotMatcherImpl& other) = delete;

    NotMatcherImpl&
    operator=(NotMatcherImpl&& other) = delete;

    ~NotMatcherImpl();

// ---------------------------------------------------------------------------
    bool
    check(Lhs lhs) override;

    void
    explain(const char* param, Lhs lhs, log::UniversalStream& stream) override;

    void
    explainNegative(const char* param,
                    Lhs lhs,
                    log::UniversalStream& stream) override;

  private:
    matcher::Matcher<Lhs> mInner;
  };

  Inner mInner;
};

// ---------------------------------------------------------------------------
template <typename Inner>
NotMatcher<Inner>::NotMatcher(Inner inner) : mInner(inner)
{
}

template <typename Inner>
template <typename Lhs>
NotMatcher<Inner>::operator matcher::Matcher<Lhs>()
{
  return matcher::Matcher<Lhs>(new NotMatcherImpl<Lhs>(mInner));
}

// ---------------------------------------------------------------------------
template <typename Inner>
template <typename Lhs>
NotMatcher<Inner>::NotMatcherImpl<Lhs>::NotMatcherImpl(
    matcher::Matcher<Lhs>&& inner) :
  mInner(std::move(inner))
{
}

template <typename Inner>
template <typename Lhs>
NotMatcher<Inner>::NotMatcherImpl<Lhs>::~NotMatcherImpl()
{
  mInner.clear();
}

template <typename Inner>
template <typename Lhs>
bool
NotMatcher<Inner>::NotMatcherImpl<Lhs>::check(Lhs lhs)
{
  return !mInner.check(lhs);
}

template <typename Inner>
template <typename Lhs>
void
NotMatcher<Inner>::NotMatcherImpl<Lhs>::explain(const char* param,
                                                Lhs lhs,
                                                log::UniversalStream& stream)
{
  mInner.explainNegative(param, lhs, stream);
}

template <typename Inner>
template <typename Lhs>
void
NotMatcher<Inner>::NotMatcherImpl<Lhs>::explainNegative(
    const char* param,
    Lhs lhs,
    log::UniversalStream& stream)
{
  mInner.explain(param, lhs, stream);
}

} // namespace matcher

// clang-format off
BINARY_COMPARISON(Eq, ==, "is equal to", "is not equal to");
BINARY_COMPARISON(Ne, !=, "is not equal to", "is equal to");
BINARY_COMPARISON(Lt, <, "is less then", "is not less then");
BINARY_COMPARISON(Gt, >, "is greater then", "is not greater then");
BINARY_COMPARISON(Le, <=, "is less or equals to", "is not less or equals to");
BINARY_COMPARISON(Ge, >=, "is greater or equals to", "is not greater or equals to");
// clang-format on

// ---------------------------------------------------------------------------
template <typename Inner>
inline matcher::NotMatcher<Inner>
Not(Inner inner)
{
  return matcher::NotMatcher<Inner>(inner);
}

// ---------------------------------------------------------------------------
/**
 * @class NotNull
 */
class NotNull
{
public:
  explicit NotNull() = default;

  NotNull(const NotNull& other) = default;

  NotNull(NotNull&& other) = delete;

  NotNull&
  operator=(const NotNull& other) = delete;

  NotNull&
  operator=(NotNull&& other) = delete;

  ~NotNull() = default;

  template <typename Lhs>
  operator matcher::Matcher<Lhs>();

private:
  /**
   * @class NotNullImpl
   */
  template <typename Opr>
  class NotNullImpl :
    public matcher::UnaryMatcherInterface<NotNullImpl<Opr>, Opr>
  {
  public:
    static_assert(std::is_pointer_v<std::remove_reference_t<Opr>> ||
                  std::is_same_v<std::remove_reference_t<Opr>, std::nullptr_t>);

    static constexpr const char* description = "is not Null";
    static constexpr const char* negativeDescription = "is Null";

// ---------------------------------------------------------------------------
    static void
    printValue(Opr opr, log::UniversalStream& stream);

    static bool
    checkValue(Opr opr);

  private:
  };
};

// ---------------------------------------------------------------------------
template <typename Opr>
NotNull::operator matcher::Matcher<Opr>()
{
  return matcher::Matcher<Opr>(new NotNullImpl<Opr>());
}

// ---------------------------------------------------------------------------
template <typename Opr>
void
NotNull::NotNullImpl<Opr>::printValue(Opr opr, log::UniversalStream& stream)
{
  stream.mOutput << "@"
                 << static_cast<const void*>(matcher::getRawPointer(opr));
}

template <typename Opr>
bool
NotNull::NotNullImpl<Opr>::checkValue(Opr opr)
{
  return matcher::getRawPointer(opr) != nullptr;
}

// ---------------------------------------------------------------------------
inline auto
IsNull()
{
  return Not(NotNull());
}

// ---------------------------------------------------------------------------
/**
 * @class Ref
 */
template <typename T>
class Ref
{
public:
  explicit Ref(T& ref);

  Ref(const Ref& other) = default;

  Ref(Ref&& other) = delete;

  Ref&
  operator=(const Ref& other) = delete;

  Ref&
  operator=(Ref&& other) = delete;

  ~Ref() = default;

  template <typename Lhs>
  operator matcher::Matcher<Lhs>();

private:
  using Rhs = T&;

  /**
   * @class RefImpl
   */
  template <typename Lhs>
  class RefImpl : public matcher::BinaryMatcherInterface<RefImpl<Lhs>, Lhs, Rhs>
  {
  public:
    static_assert(std::is_reference_v<Lhs>);

    static constexpr const char* description = "is a reference to value";
    static constexpr const char* negativeDescription =
        "is not a reference to value";

    explicit RefImpl(T& ref);

    static bool
    checkValue(Lhs lhs, Rhs rhs);

    template <typename Opr>
    static void
    printValue(Opr opr, log::UniversalStream& stream);

  private:
  };

  T& mRef;
};

// ---------------------------------------------------------------------------
template <typename T>
Ref<T>::Ref(T& ref) : mRef(ref)
{
}

template <typename T>
template <typename Lhs>
Ref<T>::operator matcher::Matcher<Lhs>()
{
  return matcher::Matcher<Lhs>(new RefImpl<Lhs>(mRef));
}

// ---------------------------------------------------------------------------
template <typename T>
template <typename Lhs>
Ref<T>::RefImpl<Lhs>::RefImpl(T& ref) :
  matcher::BinaryMatcherInterface<RefImpl<Lhs>, Lhs, Rhs>(ref)
{
}

template <typename T>
template <typename Lhs>
bool
Ref<T>::RefImpl<Lhs>::checkValue(Lhs lhs, typename Ref<T>::Rhs rhs)
{
  bool ret = &lhs == &rhs;
  return ret;
}

template <typename T>
template <typename Lhs>
template <typename Opr>
void
Ref<T>::RefImpl<Lhs>::printValue(Opr opr, log::UniversalStream& stream)
{
  stream.mOutput << "@" << static_cast<const void*>(&opr);
}

// ---------------------------------------------------------------------------
/**
 * @class IsTrue
 */
class IsTrue
{
public:
  explicit IsTrue() = default;

  IsTrue(const IsTrue& other) = default;

  IsTrue(IsTrue&& other) = delete;

  IsTrue&
  operator=(const IsTrue& other) = delete;

  IsTrue&
  operator=(IsTrue&& other) = delete;

  ~IsTrue() = default;

  template <typename Lhs>
  operator matcher::Matcher<Lhs>();

private:
  /**
   * @class IsTrueImpl
   */
  template <typename Opr>
  class IsTrueImpl : public matcher::UnaryMatcherInterface<IsTrueImpl<Opr>, Opr>
  {
  public:
    static constexpr const char* description = "to be true";
    static constexpr const char* negativeDescription = "to be false";

    static void
    printValue(Opr opr, log::UniversalStream& stream);

    static bool
    checkValue(Opr opr);

  private:
  };
};

// ---------------------------------------------------------------------------
template <typename Opr>
IsTrue::operator matcher::Matcher<Opr>()
{
  return matcher::Matcher<Opr>(new IsTrueImpl<Opr>());
}

// ---------------------------------------------------------------------------
template <typename Opr>
void
IsTrue::IsTrueImpl<Opr>::printValue(Opr opr, log::UniversalStream& stream)
{
  stream.mOutput << (checkValue(opr) ? "true" : "false");
}

template <typename Opr>
bool
IsTrue::IsTrueImpl<Opr>::checkValue(Opr opr)
{
  return static_cast<bool>(opr);
}

// ---------------------------------------------------------------------------
inline auto
IsFalse()
{
  return Not(IsTrue());
}

// ---------------------------------------------------------------------------
/**
 * @class Anything
 */
class Anything
{
public:
  explicit Anything() = default;

  Anything(const Anything& other) = default;

  Anything(Anything&& other) = delete;

  Anything&
  operator=(const Anything& other) = delete;

  Anything&
  operator=(Anything&& other) = delete;

  ~Anything() = default;

  template <typename Lhs>
  operator matcher::Matcher<Lhs>();

private:
  /**
   * @class AnythingImpl
   */
  template <typename Opr>
  class AnythingImpl :
    public matcher::UnaryMatcherInterface<AnythingImpl<Opr>, Opr>
  {
  public:
    static constexpr const char* description = "is anything";
    static constexpr const char* negativeDescription = "is nothing";

    static void
    printValue(Opr opr, log::UniversalStream& stream);

    static bool
    checkValue(Opr opr);

  private:
  };
};

// ---------------------------------------------------------------------------
template <typename Opr>
Anything::operator matcher::Matcher<Opr>()
{
  return matcher::Matcher<Opr>(new AnythingImpl<Opr>());
}

// ---------------------------------------------------------------------------
template <typename Opr>
void
Anything::AnythingImpl<Opr>::printValue(Opr opr, log::UniversalStream& stream)
{
  stream.getPlain() << "is something";
}

template <typename Opr>
bool
Anything::AnythingImpl<Opr>::checkValue(Opr opr)
{
  return true;
}

// ---------------------------------------------------------------------------
inline auto
Any()
{
  return Anything();
}

static Anything _;

} // namespace protest
