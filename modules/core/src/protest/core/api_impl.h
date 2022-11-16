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

namespace protest
{

template <typename T>
void
enforcePrinterGeneration()
{
  auto* context = protest::core::Context::getCurrentContext();
  auto* runner = context->getCurrentVirtual();
  // just trigger the protest-compiler to generate a printer for the type.
  // but do not acutally use the universal stream here
  if (false)
  {
    void* ptr = nullptr;
    runner->getLogger().getNullStream()
        << (const T&) *reinterpret_cast<const T*>(ptr);
  }
  else
  {
  }
}

template <typename Callback>
core::Timer<Callback>
createTimer(Callback callback,
            protest::meta::CallContext& callContext =
                protest::meta::CallContext::defaultContext())
{
  auto context = core::Context::getCurrentContext();
  auto runner = context->getCurrentVirtual();
  return core::Timer<Callback>(runner, callback, callContext);
}

template <typename Callback>
core::Timer<Callback>
createTimer(Callback callback,
            time::Duration timeout,
            protest::meta::CallContext& callContext =
                protest::meta::CallContext::defaultContext())
{
  auto context = core::Context::getCurrentContext();
  auto runner = context->getCurrentVirtual();
  return core::Timer<Callback>(runner, callback, timeout, callContext);
}

// ---------------------------------------------------------------------------
template <typename T>
core::AnyPortCreator<T>
createPort(protest::core::Signal<T>& signal)
{
  auto context = core::Context::getCurrentContext();
  auto runner = context->getCurrentVirtual();
  // TODO remove name?
  return core::AnyPortCreator<T>(signal, runner, "");
}

// ---------------------------------------------------------------------------
template <typename Expr>
core::Invariant
createInvariant(Expr expr, protest::meta::Invariant& callContext)
{
  auto context = core::Context::getCurrentContext();
  auto runner = context->getCurrentVirtual();
  core::Invariant invariant(runner, expr, &callContext);
  invariant.start();
  return invariant;
}

// ---------------------------------------------------------------------------
template <typename T>
std::enable_if_t<!std::is_base_of_v<time::Duration, std::remove_const_t<T>>,
                 void>
wait(T condition, protest::meta::CallContext& callContext)
{
  auto context = core::Context::getCurrentContext();
  auto runner = context->getCurrentVirtual();

  core::ExprCondition<T>* expr =
      new core::ExprCondition<T>(runner, condition, runner);
  {
    auto stream =
        runner->getLogger().startLog("WAIT",
                                     runner->getName(),
                                     callContext.getUnit().getFileName(),
                                     callContext.getLine(),
                                     runner->now());
    stream.operator std::ostream&()
        << "Wait for condition:\n'" << callContext.getArg(0) << "'";
  }

  expr->enable();
  bool gotTimeout =
      runner->RunnerRaw::waitInternal(protest::time::Duration::infinity(),
                                      expr);
  assert(!gotTimeout);
  expr->disable();
  {
    auto stream =
        runner->getLogger().startLog("RESM",
                                     runner->getName(),
                                     callContext.getUnit().getFileName(),
                                     callContext.getLine(),
                                     runner->now());
    stream.operator std::ostream&()
        << "Condition is fulfilled:\n'" << callContext.getArg(0) << "'\n";
  }
  delete expr;
}

template <typename T>
std::enable_if_t<!std::is_base_of_v<time::Duration, std::remove_const_t<T>>,
                 bool>
wait(T condition,
     time::Duration timeout,
     protest::meta::CallContext& callContext)
{
  auto context = core::Context::getCurrentContext();
  auto runner = context->getCurrentVirtual();

  core::ExprCondition<T>* expr =
      new core::ExprCondition<T>(runner, condition, runner);

  {
    auto stream =
        runner->getLogger().startLog("WAIT",
                                     runner->getName(),
                                     callContext.getUnit().getFileName(),
                                     callContext.getLine(),
                                     runner->now());
    stream.operator std::ostream&()
        << "Wait for condition:\n'" << callContext.getArg(0) << "'\n";
  }

  expr->enable();
  bool gotTimeout = runner->RunnerRaw::waitInternal(timeout, expr);
  expr->disable();

  if (!gotTimeout)
  {
    auto stream =
        runner->getLogger().startLog("RESM",
                                     runner->getName(),
                                     callContext.getUnit().getFileName(),
                                     callContext.getLine(),
                                     runner->now());
    stream.operator std::ostream&()
        << "Condition is fulfilled:\n'" << callContext.getArg(0) << "'\n";
  }
  else
  {
    auto stream =
        runner->getLogger().startLog("RESM",
                                     runner->getName(),
                                     callContext.getUnit().getFileName(),
                                     callContext.getLine(),
                                     runner->now());
    stream.operator std::ostream&()
        << "Condition is not fulfilled (timeout after "
        << std::to_string(timeout.milliseconds()) << " ms):\n'"
        << callContext.getArg(0) << "'\n";
  }
  delete expr;
  return !gotTimeout;
}

} // namespace protest