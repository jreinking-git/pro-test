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

namespace protest
{

namespace matcher
{

// ---------------------------------------------------------------------------
template <typename Pointer>
inline const typename Pointer::element_type*
getRawPointer(const Pointer& p)
{
  return p.get();
}

template <typename Element>
inline Element*
getRawPointer(Element* p)
{
  return p;
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
template <typename Lhs>
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
  check(Lhs lhs) = 0;

  virtual void
  explain(const char* param, Lhs lhs, log::UniversalStream& stream) = 0;

  virtual void
  explainNegative(const char* param, Lhs lhs, log::UniversalStream& stream) = 0;

private:
};

// ---------------------------------------------------------------------------
/**
 * @class Matcher
 */
template <typename Lhs>
class Matcher
{
public:
  explicit Matcher(MatcherInterface<Lhs>* interface);

  Matcher(const Matcher& other) = default;

  Matcher(Matcher&& other) = delete;

  Matcher&
  operator=(const Matcher& other) = delete;

  Matcher&
  operator=(Matcher&& other) = delete;

  ~Matcher() = default;

// ---------------------------------------------------------------------------
  bool
  check(Lhs value);

  void
  explain(const char* param, Lhs value, protest::log::UniversalStream& stream);

  void
  explainNegative(const char* param,
                  Lhs value,
                  protest::log::UniversalStream& stream);

  void
  clear();

private:
  MatcherInterface<Lhs>* mInterface;
};

// ---------------------------------------------------------------------------
template <typename Lhs>
Matcher<Lhs>::Matcher(MatcherInterface<Lhs>* interface) : mInterface(interface)
{
}

template <typename Lhs>
bool
Matcher<Lhs>::check(Lhs value)
{
  return mInterface->check(value);
}

template <typename Lhs>
void
Matcher<Lhs>::explain(const char* param,
                      Lhs value,
                      protest::log::UniversalStream& stream)
{
  mInterface->explain(param, value, stream);
}

template <typename Lhs>
void
Matcher<Lhs>::explainNegative(const char* param,
                              Lhs value,
                              protest::log::UniversalStream& stream)
{
  mInterface->explainNegative(param, value, stream);
}

template <typename Lhs>
void
Matcher<Lhs>::clear()
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
  class ComparisonImpl : public MatcherInterface<Lhs>
  {
  public:
    explicit ComparisonImpl(Rhs rhs);

    ComparisonImpl(const ComparisonImpl& other) = delete;

    ComparisonImpl(ComparisonImpl&& other) = delete;

    ComparisonImpl&
    operator=(const ComparisonImpl& other) = delete;

    ComparisonImpl&
    operator=(ComparisonImpl&& other) = delete;

    ~ComparisonImpl() = default;

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
    Rhs mValue;
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
  mValue(rhs)
{
}

template <typename Rhs, typename Comparison>
template <typename Lhs>
bool
ComparisonBase<Rhs, Comparison>::ComparisonImpl<Lhs>::check(Lhs lhs)
{
  return Comparison::compare(lhs, mValue);
}

template <typename Rhs, typename Comparison>
template <typename Lhs>
void
ComparisonBase<Rhs, Comparison>::ComparisonImpl<Lhs>::explain(
    const char* param,
    Lhs lhs,
    log::UniversalStream& stream)
{
  static constexpr size_t indent = 10;
  stream.mOutput << "Value of: " << param << "\n";
  stream.mOutput << "Expected: " << Comparison::description << "\n";
  stream.incrementIndent(indent);
  stream.printIndent();
  stream << mValue;
  stream.decrementIndent(indent);
  stream.mOutput << "\n";

  if (check(lhs))
  {
    stream.mOutput << "  Actual: ";
  }
  else
  {
    stream.mOutput << "  But is: ";
  }

  stream.incrementIndent(indent);
  stream << lhs;
  stream.decrementIndent(indent);
  stream.mOutput << "\n";
}

template <typename Rhs, typename Comparison>
template <typename Lhs>
void
ComparisonBase<Rhs, Comparison>::ComparisonImpl<Lhs>::explainNegative(
    const char* param,
    Lhs lhs,
    log::UniversalStream& stream)
{
  static constexpr size_t indent = 10;
  stream.mOutput << "Value of: " << param << "\n";
  stream.mOutput << "Expected: " << Comparison::negativeDescription << "\n";
  stream.incrementIndent(indent);
  stream.printIndent();
  stream << mValue;
  stream.decrementIndent(indent);
  stream.mOutput << "\n";

  if (check(lhs))
  {
    stream.mOutput << "  But is: ";
  }
  else
  {
    stream.mOutput << "  Actual: ";
  }

  stream.incrementIndent(indent);
  stream << lhs;
  stream.decrementIndent(indent);
  stream.mOutput << "\n";
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
    explicit NotMatcherImpl(matcher::Matcher<Lhs> inner);

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
    matcher::Matcher<Lhs> inner) :
  mInner(inner)
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
  template <typename Lhs>
  class NotNullImpl : public matcher::MatcherInterface<Lhs>
  {
  public:
    static_assert(std::is_pointer_v<std::remove_reference_t<Lhs>> ||
                  std::is_same_v<std::remove_reference_t<Lhs>, std::nullptr_t>);

    explicit NotNullImpl() = default;

    NotNullImpl(const NotNullImpl& other) = delete;

    NotNullImpl(NotNullImpl&& other) = delete;

    NotNullImpl&
    operator=(const NotNullImpl& other) = delete;

    NotNullImpl&
    operator=(NotNullImpl&& other) = delete;

    ~NotNullImpl() = default;

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
  };
};

// ---------------------------------------------------------------------------
template <typename Lhs>
NotNull::operator matcher::Matcher<Lhs>()
{
  return matcher::Matcher<Lhs>(new NotNullImpl<Lhs>());
}

template <typename Lhs>
bool
NotNull::NotNullImpl<Lhs>::check(Lhs lhs)
{
  return matcher::getRawPointer(lhs) != nullptr;
}

template <typename Lhs>
void
NotNull::NotNullImpl<Lhs>::explain(const char* param,
                                   Lhs lhs,
                                   log::UniversalStream& stream)
{
  static constexpr size_t indent = 10;
  stream.mOutput << "Value of: " << param << "\n";
  stream.mOutput << "Expected: "
                 << "is not nullptr"
                 << "\n";

  if (check(lhs))
  {
    stream.mOutput << "  Actual: ";
  }
  else
  {
    stream.mOutput << "  But is: ";
  }

  stream.incrementIndent(indent);
  stream.mOutput << "@" << static_cast<const void*>(lhs);
  stream.decrementIndent(indent);
  stream.mOutput << "\n";
}

template <typename Lhs>
void
NotNull::NotNullImpl<Lhs>::explainNegative(const char* param,
                                           Lhs lhs,
                                           log::UniversalStream& stream)
{
  static constexpr size_t indent = 10;
  stream.mOutput << "Value of: " << param << "\n";
  stream.mOutput << "Expected: "
                 << "is nullptr"
                 << "\n";

  if (check(lhs))
  {
    stream.mOutput << "  But is: ";
  }
  else
  {
    stream.mOutput << "  Actual: ";
  }

  stream.incrementIndent(indent);
  stream.mOutput << "@" << static_cast<const void*>(lhs);
  stream.decrementIndent(indent);
  stream.mOutput << "\n";
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
  /**
   * @class RefImpl
   */
  template <typename Lhs>
  class RefImpl : public matcher::MatcherInterface<Lhs>
  {
  public:
    static_assert(std::is_reference_v<Lhs>);

    explicit RefImpl(T& ref);

    RefImpl(const RefImpl& other) = delete;

    RefImpl(RefImpl&& other) = delete;

    RefImpl&
    operator=(const RefImpl& other) = delete;

    RefImpl&
    operator=(RefImpl&& other) = delete;

    ~RefImpl() = default;

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
    T& mRef;
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
Ref<T>::RefImpl<Lhs>::RefImpl(T& ref) : mRef(ref)
{
}

template <typename T>
template <typename Lhs>
bool
Ref<T>::RefImpl<Lhs>::check(Lhs lhs)
{
  return &lhs == &mRef;
}

template <typename T>
template <typename Lhs>
void
Ref<T>::RefImpl<Lhs>::explain(const char* param,
                              Lhs lhs,
                              log::UniversalStream& stream)
{
  static constexpr size_t indent = 10;
  stream.mOutput << "Value of: " << param << "\n";
  stream.mOutput << "Expected: "
                 << "is a reference to value"
                 << "\n";
  stream.incrementIndent(indent);
  stream.printIndent();
  stream.mOutput << "@" << static_cast<const void*>(&mRef);
  stream.decrementIndent(indent);
  stream.mOutput << "\n";

  if (check(lhs))
  {
    stream.mOutput << "  Actual: ";
  }
  else
  {
    stream.mOutput << "  But is: ";
  }

  stream.incrementIndent(indent);
  stream.mOutput << "@" << static_cast<const void*>(&lhs);
  stream.decrementIndent(indent);
  stream.mOutput << "\n";
}

template <typename T>
template <typename Lhs>
void
Ref<T>::RefImpl<Lhs>::explainNegative(const char* param,
                                      Lhs lhs,
                                      log::UniversalStream& stream)
{
  static constexpr size_t indent = 10;
  stream.mOutput << "Value of: " << param << "\n";
  stream.mOutput << "Expected: "
                 << "is not a reference to value"
                 << "\n";
  stream.incrementIndent(indent);
  stream.printIndent();
  stream.mOutput << "@" << static_cast<const void*>(&mRef);
  stream.decrementIndent(indent);
  stream.mOutput << "\n";

  if (check(lhs))
  {
    stream.mOutput << "  But is: ";
  }
  else
  {
    stream.mOutput << "  Actual: ";
  }

  stream.incrementIndent(indent);
  stream.mOutput << "@" << static_cast<const void*>(&lhs);
  stream.decrementIndent(indent);
  stream.mOutput << "\n";
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
  template <typename Lhs>
  class IsTrueImpl : public matcher::MatcherInterface<Lhs>
  {
  public:
    explicit IsTrueImpl() = default;

    IsTrueImpl(const IsTrueImpl& other) = delete;

    IsTrueImpl(IsTrueImpl&& other) = delete;

    IsTrueImpl&
    operator=(const IsTrueImpl& other) = delete;

    IsTrueImpl&
    operator=(IsTrueImpl&& other) = delete;

    ~IsTrueImpl() = default;

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
  };
};

// ---------------------------------------------------------------------------
template <typename Lhs>
IsTrue::operator matcher::Matcher<Lhs>()
{
  return matcher::Matcher<Lhs>(new IsTrueImpl<Lhs>());
}

template <typename Lhs>
bool
IsTrue::IsTrueImpl<Lhs>::check(Lhs lhs)
{
  return static_cast<bool>(lhs);
}

template <typename Lhs>
void
IsTrue::IsTrueImpl<Lhs>::explain(const char* param,
                                   Lhs lhs,
                                   log::UniversalStream& stream)
{
  static constexpr size_t indent = 10;
  stream.mOutput << "Value of: " << param << "\n";
  stream.mOutput << "Expected: "
                 << "to be true"
                 << "\n";

  if (check(lhs))
  {
    stream.mOutput << "  Actual: ";
  }
  else
  {
    stream.mOutput << "  But is: ";
  }

  stream.incrementIndent(indent);
  stream.mOutput << (check(lhs) ? "true" : "false");
  stream.decrementIndent(indent);
  stream.mOutput << "\n";
}

template <typename Lhs>
void
IsTrue::IsTrueImpl<Lhs>::explainNegative(const char* param,
                                           Lhs lhs,
                                           log::UniversalStream& stream)
{
  static constexpr size_t indent = 10;
  stream.mOutput << "Value of: " << param << "\n";
  stream.mOutput << "Expected: "
                 << "to be false"
                 << "\n";

  if (check(lhs))
  {
    stream.mOutput << "  But is: ";
  }
  else
  {
    stream.mOutput << "  Actual: ";
  }

  stream.incrementIndent(indent);
  stream.mOutput << (check(lhs) ? "true" : "false");
  stream.decrementIndent(indent);
  stream.mOutput << "\n";
}

// ---------------------------------------------------------------------------
inline auto
IsFalse()
{
  return Not(IsTrue());
}

} // namespace protest
