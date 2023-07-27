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

#include "protest/core/job.h"
#include "protest/coro/coroutine.h"
#include "protest/utils/heap.h"
#include "protest/log/logger.h"
#include "protest/core/condition.h"
#include "protest/core/context.h"

#include <array>

namespace protest
{

class Section;

namespace core
{

class Condition;

// ---------------------------------------------------------------------------
/**
 * @class RunnerRaw
 */
class RunnerRaw : public coro::Coroutine, public Condition::Listener
{
public:
  static constexpr size_t numberOfPerRunnerData = 10;

  friend class List<RunnerRaw>;

// ---------------------------------------------------------------------------
  /**
   * @class Userdata
   */
  class Userdata
  {
  public:
    friend class RunnerRaw;

    explicit Userdata(size_t id);

    Userdata(const Userdata& other) = delete;

    Userdata(Userdata&& other) = delete;

    Userdata&
    operator=(const Userdata& other) = delete;

    Userdata&
    operator=(Userdata&& other) = delete;

    virtual ~Userdata() = default;

  private:
    size_t mId;
  };

// ---------------------------------------------------------------------------
  explicit RunnerRaw(const char* name);

  explicit RunnerRaw(core::Context& context, const char* name);

  RunnerRaw(const RunnerRaw&) = delete;

  RunnerRaw(RunnerRaw&&) noexcept = delete;

  RunnerRaw&
  operator=(const RunnerRaw&) = delete;

  RunnerRaw&
  operator=(RunnerRaw&&) noexcept = delete;

  virtual ~RunnerRaw();

// ---------------------------------------------------------------------------
  void
  internalInitialize();

  virtual void
  initialize();

  virtual void
  process();

  void
  internalFinalize();

  virtual void
  finalize();

// ---------------------------------------------------------------------------
  void
  add(Job& job);

  void
  remove(Job& job);

// ---------------------------------------------------------------------------
  void
  coroRun() override;

// ---------------------------------------------------------------------------
  bool
  waitInternal(time::Duration timeout, Condition* condition);

  bool
  waitInternal(time::Duration timeout);

  void
  wakeup();

// ---------------------------------------------------------------------------
  void
  startSection(Section* section);

  void
  endSection();

// ---------------------------------------------------------------------------
  const char*
  getName();

  log::Logger&
  getLogger();

// ---------------------------------------------------------------------------
  core::Context&
  getContext();

// ---------------------------------------------------------------------------
  /**
   * @brief getUserdataAs
   * 
   * Userdata can be used to store `per-runner` data. It can be accessed with
   * this method.
   * 
   * @tparam T
   *  the type of the user data
   * 
   * @return
   *  pointer to the user data
   */
  template <typename T>
  T*
  getUserdataAs();

  /**
   * @brief setUserdata
   * 
   * Userdata can be used to store `per-runner` data. It can be set with
   * this method.
   * 
   * @tparam T
   *  the type of the user data
   * 
   * @param userdata 
   *  pointer to the user data
   */
  template <typename T>
  void
  setUserdata(T* userdata);

private:
  static constexpr size_t seperatorLength = 79;

  void
  notify() override;

  // TODO (jreinking) magic number
  Heap<Job, 100> mPriorityQueue;
  Condition* mCondition;
  const char* mName;
  bool mWakeUpEvent;
  log::Logger mLogger;
  RunnerRaw* mNext;

  uint32_t mTestSteps;
  Section* mCurrentTestStepName;
  std::array<Userdata*, numberOfPerRunnerData> mUserdata;
};

// ---------------------------------------------------------------------------
template <typename T>
T*
RunnerRaw::getUserdataAs()
{
  assert(T::id < numberOfPerRunnerData);
  return static_cast<T*>(mUserdata.at(T::id));
}

template <typename T>
void
RunnerRaw::setUserdata(T* userdata)
{
  mUserdata.at(T::id) = userdata;
}

} // namespace core

} // namespace protest
