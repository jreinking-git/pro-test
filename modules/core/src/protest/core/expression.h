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

#include <utility>

namespace protest
{

namespace core
{

class RunnerRaw;
class Condition;

// ---------------------------------------------------------------------------
/**
 * @class Plus
 */
struct Plus
{
};

/**
 * @class Minus
 */
struct Minus
{
};

/**
 * @class Minus
 */
struct Mul
{
};

/**
 * @class Minus
 */
struct Div
{
};

/**
 * @class Equals
 */
struct Equals
{
};

/**
 * @class NotEquals
 */
struct NotEquals
{
};

/**
 * @class Greater
 */
struct Greater
{
};

/**
 * @class GreaterOrEquals
 */
struct GreaterOrEquals
{
};

/**
 * @class Less
 */
struct Less
{
};

/**
 * @class LessOrEquals
 */
struct LessOrEquals
{
};

/**
 * @class And
 */
struct And
{
};

/**
 * @class Or
 */
struct Or
{
};

/**
 * @class Not
 */
struct Not
{
};

// ---------------------------------------------------------------------------
struct ExprTag
{
};

// ---------------------------------------------------------------------------
/**
 * @class CopyExprTag
 * 
 * This is tag for all types that can be used to create conditions. Its quite
 * simple to use but in the most cases not sufficent enought. It is for
 * example used by @c size() of @c QueuePort. @c size() returns an object of
 * the class @c Size which itself is not directly used to build protest scripts.
 * Therefore it does not need to hide methods and does not need an internal
 * representation.
 */
struct CopyExprTag : public ExprTag
{
};

// ---------------------------------------------------------------------------
/**
 * @class ConvertableToCopyExprTag
 * 
 * This is a tag for all types that can be converted to a @c CopyExprTag so
 * that it can be used to build conditions. This tag is usefull if you have
 * a type which has:
 * 
 * 1) an interface/handler which can be used in the protest script
 * 2) a internal representation which is then referenced by the handle
 *    an hides the internal to the protest user
 * 3) a class which can be used in the expression itself.
 * 
 * @c SamplePort is for example such a class: 
 * 
 * 1) @c SamplePort is used as handle for the protest user to build the
 *    protest scripts
 * 2) @c SamplePortInternal holds data and function which should not be
 *    accessed by the protest script.
 * 3) and @c SamplePortExpr which is used to represent a @c SamplePort
 *    inside of an expression
 */
struct ConvertableToCopyExprTag : ExprTag
{
};

// ---------------------------------------------------------------------------
/**
 * @class Copy
 */
template <typename T>
class Copy : public CopyExprTag
{
public:
  template <typename Expr>
  friend class ExprCondition;

  template <typename Opr, typename L, typename R>
  friend class BinaryExpression;

  template <typename Opr, typename O>
  friend class UnaryExpression;

  using Type = T;

  explicit Copy(T value);

  Copy(const Copy&) = default;

  Copy(Copy&&) noexcept = delete;

  Copy&
  operator=(const Copy&) = delete;

  Copy&
  operator=(Copy&&) noexcept = delete;

  ~Copy() = default;

  T
  getValue();

private:
  void
  conditionEnable(RunnerRaw* runner, Condition* condition);

  void
  conditionDisable();

  T mValue;
};

// ---------------------------------------------------------------------------
template <typename T>
Copy<T>::Copy(T value) : mValue(value)
{
}

template <typename T>
T
Copy<T>::getValue()
{
  return mValue;
}

// ---------------------------------------------------------------------------
template <typename T>
void
Copy<T>::conditionEnable(RunnerRaw* runner, Condition* condition)
{
}

template <typename T>
void
Copy<T>::conditionDisable()
{
}

// ---------------------------------------------------------------------------
/**
 * @class BinaryResult
 */
template <typename Lhs, typename Rhs, typename Operation>
struct BinaryResult
{
};

#define BINARY_OPERATOR_INFO(oprcls, opr)                                      \
  template <typename Lhs, typename Rhs>                                        \
  struct BinaryResult<Lhs, Rhs, oprcls>                                        \
  {                                                                            \
    using LhsType = typename Lhs::Type;                                        \
    using RhsType = typename Rhs::Type;                                        \
    using Type =                                                               \
        decltype(std::declval<LhsType>() opr std::declval<RhsType>());         \
                                                                               \
    static Type                                                                \
    apply(LhsType lhs, RhsType rhs);                                           \
  };                                                                           \
                                                                               \
  template <typename Lhs, typename Rhs>                                        \
  typename BinaryResult<Lhs, Rhs, oprcls>::Type                                \
  BinaryResult<Lhs, Rhs, oprcls>::apply(LhsType lhs, RhsType rhs)              \
  {                                                                            \
    return lhs opr rhs;                                                        \
  }

BINARY_OPERATOR_INFO(Plus, +);
BINARY_OPERATOR_INFO(Minus, -);
BINARY_OPERATOR_INFO(Mul, *);
BINARY_OPERATOR_INFO(Div, /);
BINARY_OPERATOR_INFO(Equals, ==);
BINARY_OPERATOR_INFO(NotEquals, !=);
BINARY_OPERATOR_INFO(Greater, >);
BINARY_OPERATOR_INFO(GreaterOrEquals, >=);
BINARY_OPERATOR_INFO(Less, <);
BINARY_OPERATOR_INFO(LessOrEquals, <=);
BINARY_OPERATOR_INFO(And, &&);
BINARY_OPERATOR_INFO(Or, ||);

// ---------------------------------------------------------------------------
/**
 * @class BinaryExpression
 */
template <typename Operation, typename Lhs, typename Rhs>
class BinaryExpression : public CopyExprTag
{
public:
  template <typename Expr>
  friend class ExprCondition;

  template <typename Opr, typename L, typename R>
  friend class BinaryExpression;

  template <typename Opr, typename O>
  friend class UnaryExpression;

  using Type = typename BinaryResult<Lhs, Rhs, Operation>::Type;

  explicit BinaryExpression(Lhs lhs, Rhs rhs);

  BinaryExpression(const BinaryExpression&) = default;

  BinaryExpression(BinaryExpression&&) noexcept = delete;

  BinaryExpression&
  operator=(const BinaryExpression&) = delete;

  BinaryExpression&
  operator=(BinaryExpression&&) noexcept = delete;

  ~BinaryExpression() = default;

// ---------------------------------------------------------------------------
  Type
  getValue();

  operator Type();

private:
  void
  conditionEnable(RunnerRaw* runner, Condition* condition);

  void
  conditionDisable();

  Lhs mLhs;
  Rhs mRhs;
};

// ---------------------------------------------------------------------------
template <typename Operation, typename Lhs, typename Rhs>
BinaryExpression<Operation, Lhs, Rhs>::BinaryExpression(Lhs lhs, Rhs rhs) :
  mLhs(lhs),
  mRhs(rhs)
{
}

template <typename Operation, typename Lhs, typename Rhs>
typename BinaryExpression<Operation, Lhs, Rhs>::Type
BinaryExpression<Operation, Lhs, Rhs>::getValue()
{
  return BinaryResult<Lhs, Rhs, Operation>::apply(mLhs.getValue(),
                                                  mRhs.getValue());
}

template <typename Operation, typename Lhs, typename Rhs>
BinaryExpression<Operation, Lhs, Rhs>::operator Type()
{
  return getValue();
}

// ---------------------------------------------------------------------------
template <typename Operation, typename Lhs, typename Rhs>
void
BinaryExpression<Operation, Lhs, Rhs>::conditionEnable(RunnerRaw* runner,
                                                       Condition* condition)
{
  mLhs.conditionEnable(runner, condition);
  mRhs.conditionEnable(runner, condition);
}

template <typename Operation, typename Lhs, typename Rhs>
void
BinaryExpression<Operation, Lhs, Rhs>::conditionDisable()
{
  mLhs.conditionDisable();
  mRhs.conditionDisable();
}

// ---------------------------------------------------------------------------
#define BINARY_OPERATOR(oprcls, opr)                                           \
  template <typename Lhs, typename Rhs>                                        \
  std::enable_if_t<std::is_base_of_v<CopyExprTag, Lhs> &&                      \
                       std::is_fundamental_v<Rhs>,                             \
                   BinaryExpression<oprcls, Lhs, Copy<Rhs>>>                   \
  operator opr(Lhs lhs, Rhs rhs)                                               \
  {                                                                            \
    return BinaryExpression<oprcls, Lhs, Copy<Rhs>>(lhs, Copy(rhs));           \
  }                                                                            \
                                                                               \
  template <typename Lhs, typename Rhs>                                        \
  std::enable_if_t<std::is_base_of_v<CopyExprTag, Lhs> &&                      \
                       std::is_base_of_v<ConvertableToCopyExprTag, Rhs>,       \
                   BinaryExpression<oprcls, Lhs, typename Rhs::Type>>          \
  operator opr(Lhs lhs, Rhs rhs)                                               \
  {                                                                            \
    return BinaryExpression<oprcls, Lhs, typename Rhs::Type>(                  \
        lhs,                                                                   \
        decltype(rhs)::Converter::toCopyExpr(rhs));                            \
  }                                                                            \
                                                                               \
  template <typename Lhs, typename Rhs>                                        \
  std::enable_if_t<std::is_base_of_v<ConvertableToCopyExprTag, Lhs> &&         \
                       std::is_fundamental_v<Rhs>,                             \
                   BinaryExpression<oprcls, typename Lhs::Type, Copy<Rhs>>>    \
  operator opr(Lhs lhs, Rhs rhs)                                               \
  {                                                                            \
    return BinaryExpression<oprcls, typename Lhs::Type, Copy<Rhs>>(            \
        decltype(lhs)::Converter::toCopyExpr(lhs),                             \
        Copy(rhs));                                                            \
  }                                                                            \
                                                                               \
  template <typename Lhs, typename Rhs>                                        \
  std::enable_if_t<std::is_fundamental_v<Lhs> &&                               \
                       std::is_base_of_v<CopyExprTag, Rhs>,                    \
                   BinaryExpression<oprcls, Copy<Lhs>, Rhs>>                   \
  operator opr(Lhs lhs, Rhs rhs)                                               \
  {                                                                            \
    return BinaryExpression<oprcls, Copy<Lhs>, Rhs>(Copy(lhs), rhs);           \
  }                                                                            \
                                                                               \
  template <typename Lhs, typename Rhs>                                        \
  std::enable_if_t<std::is_base_of_v<ConvertableToCopyExprTag, Lhs> &&         \
                       std::is_base_of_v<CopyExprTag, Rhs>,                    \
                   BinaryExpression<oprcls, typename Lhs::Type, Rhs>>          \
  operator opr(Lhs lhs, Rhs rhs)                                               \
  {                                                                            \
    return BinaryExpression<oprcls, typename Lhs::Type, Rhs>(                  \
        decltype(lhs)::Converter::toCopyExpr(lhs),                             \
        rhs);                                                                  \
  }                                                                            \
                                                                               \
  template <typename Lhs, typename Rhs>                                        \
  std::enable_if_t<std::is_fundamental_v<Lhs> &&                               \
                       std::is_base_of_v<ConvertableToCopyExprTag, Rhs>,       \
                   BinaryExpression<oprcls, Copy<Lhs>, typename Rhs::Type>>    \
  operator opr(Lhs lhs, Rhs rhs)                                               \
  {                                                                            \
    return BinaryExpression<oprcls, Copy<Lhs>, typename Rhs::Type>(            \
        Copy(lhs),                                                             \
        decltype(rhs)::Converter::toCopyExpr(rhs));                            \
  }                                                                            \
                                                                               \
  template <typename Lhs, typename Rhs>                                        \
  std::enable_if_t<std::is_base_of_v<CopyExprTag, Lhs> &&                      \
                       std::is_base_of_v<CopyExprTag, Rhs>,                    \
                   BinaryExpression<oprcls, Lhs, Rhs>>                         \
  operator opr(Lhs lhs, Rhs rhs)                                               \
  {                                                                            \
    return BinaryExpression<oprcls, Lhs, Rhs>(lhs, rhs);                       \
  }                                                                            \
                                                                               \
  template <typename Lhs, typename Rhs>                                        \
  std::enable_if_t<                                                            \
      std::is_base_of_v<ConvertableToCopyExprTag, Lhs> &&                      \
          std::is_base_of_v<ConvertableToCopyExprTag, Rhs>,                    \
      BinaryExpression<oprcls, typename Lhs::Type, typename Rhs::Type>>        \
  operator opr(Lhs lhs, Rhs rhs)                                               \
  {                                                                            \
    return BinaryExpression<oprcls, typename Lhs::Type, typename Rhs::Type>(   \
        decltype(lhs)::Converter::toCopyExpr(lhs),                             \
        decltype(rhs)::Converter::toCopyExpr(rhs));                            \
  }

BINARY_OPERATOR(Plus, +);
BINARY_OPERATOR(Minus, -);
BINARY_OPERATOR(Mul, *);
BINARY_OPERATOR(Div, /);
BINARY_OPERATOR(Equals, ==);
BINARY_OPERATOR(NotEquals, !=);
BINARY_OPERATOR(Greater, >);
BINARY_OPERATOR(GreaterOrEquals, >=);
BINARY_OPERATOR(Less, <);
BINARY_OPERATOR(LessOrEquals, <=);
BINARY_OPERATOR(And, &&);
BINARY_OPERATOR(Or, ||);

// ---------------------------------------------------------------------------
/**
 * @class UnaryResult
 */
template <typename Operand, typename Operation>
struct UnaryResult
{
};

#define UNARY_OPERATOR_INFO(oprcls, opr)                                       \
  template <typename Operand>                                                  \
  struct UnaryResult<Operand, oprcls>                                          \
  {                                                                            \
    using OperandType = typename Operand::Type;                                \
    using Type = decltype(opr std::declval<OperandType>());                    \
                                                                               \
    static Type                                                                \
    apply(typename Operand::Type operand);                                     \
  };                                                                           \
                                                                               \
  template <typename Operand>                                                  \
  typename UnaryResult<Operand, oprcls>::Type                                  \
  UnaryResult<Operand, oprcls>::apply(typename Operand::Type operand)          \
  {                                                                            \
    return opr operand;                                                        \
  }

UNARY_OPERATOR_INFO(Not, !);

// ---------------------------------------------------------------------------
/**
 * @class UnaryExpression
 */
template <typename Operation, typename Operand>
class UnaryExpression : public CopyExprTag
{
public:
  template <typename Expr>
  friend class ExprCondition;

  template <typename Opr, typename L, typename R>
  friend class BinaryExpression;

  template <typename Opr, typename O>
  friend class UnaryExpression;

  using Type = typename UnaryResult<Operand, Operation>::Type;

  explicit UnaryExpression(Operand operand);

  UnaryExpression(const UnaryExpression&) = default;

  UnaryExpression(UnaryExpression&&) noexcept = delete;

  UnaryExpression&
  operator=(const UnaryExpression&) = delete;

  UnaryExpression&
  operator=(UnaryExpression&&) noexcept = delete;

  ~UnaryExpression() = default;

// ---------------------------------------------------------------------------
  Type
  getValue();

  operator Type();

private:
  void
  conditionEnable(RunnerRaw* runner, Condition* condition);

  void
  conditionDisable();

  Operand mOperand;
};

// ---------------------------------------------------------------------------
template <typename Operation, typename Operand>
UnaryExpression<Operation, Operand>::UnaryExpression(Operand operand) :
  mOperand(operand)
{
}

template <typename Operation, typename Operand>
typename UnaryExpression<Operation, Operand>::Type
UnaryExpression<Operation, Operand>::getValue()
{
  return UnaryResult<Operand, Operation>::apply(mOperand.getValue());
}

template <typename Operation, typename Operand>
UnaryExpression<Operation, Operand>::operator Type()
{
  return getValue();
}

// ---------------------------------------------------------------------------
template <typename Operation, typename Operand>
void
UnaryExpression<Operation, Operand>::conditionEnable(RunnerRaw* runner,
                                                     Condition* condition)
{
  mOperand.conditionEnable(runner, condition);
}

template <typename Operation, typename Operand>
void
UnaryExpression<Operation, Operand>::conditionDisable()
{
  mOperand.conditionDisable();
}

// ---------------------------------------------------------------------------
// the enable_if_t avoids clashes with other implementations of operator! when
// 'using namespace protest::core' is used.
#define UNARY_OPERATOR(oprcls, opr)                                            \
  template <typename Operand>                                                  \
  std::enable_if_t<std::is_base_of_v<ExprTag, Operand>, UnaryExpression<oprcls, Operand>> operator opr(Operand operand) \
  {                                                                            \
    return UnaryExpression<oprcls, Operand>(operand);                          \
  }

UNARY_OPERATOR(Not, !);

} // namespace core

} // namespace protest
