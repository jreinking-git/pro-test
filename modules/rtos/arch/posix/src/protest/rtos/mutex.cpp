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

#include "protest/rtos/mutex.h"
#include "protest/utils/debug.h"

#include <cstdio>
#include <cerrno>
#include <ctime>

using namespace protest::rtos;

// ---------------------------------------------------------------------------
static constexpr int64_t nanoToSeconds = 1000000000U;

// ---------------------------------------------------------------------------
Mutex::Mutex() : mUserdata(nullptr)
{
  // NOLINTNEXTLINE
  mUserdata = new pthread_mutex_t();
  auto* userdata = reinterpret_cast<pthread_mutex_t*>(mUserdata);
  if ((pthread_mutex_init(userdata, nullptr)) != 0)
  {
    PROTEST_ASSERT(false);
  }
  else
  {
  }
}

Mutex::~Mutex()
{
  auto* userdata = reinterpret_cast<pthread_mutex_t*>(mUserdata);
  pthread_mutex_destroy(userdata);
  // NOLINTNEXTLINE
  delete userdata;
  mUserdata = nullptr;
}

bool
Mutex::acquire(time::Duration timeout)
{
  auto* userdata = reinterpret_cast<pthread_mutex_t*>(mUserdata);
  bool gotTimeout = false;
  if (timeout == time::Duration::infinity())
  {
    pthread_mutex_lock(userdata);
    gotTimeout = false;
  }
  else
  {
    struct timespec now
    {
      0
    };
    int ret = clock_gettime(CLOCK_MONOTONIC, &now);
    PROTEST_ASSERT(ret == 0);
    int64_t nanoseconds = timeout.nanoseconds();
    struct timespec time
    {
      0
    };
    time.tv_nsec = now.tv_nsec + nanoseconds;
    time.tv_sec = now.tv_sec + (time.tv_nsec / nanoToSeconds);
    time.tv_nsec = time.tv_nsec % nanoToSeconds;
    ret = pthread_mutex_timedlock(userdata, &time);
    if (ret != 0)
    {
      if (ret == ETIMEDOUT)
      {
        gotTimeout = true;
      }
      else
      {
        PROTEST_ASSERT(false);
      }
    }
    else
    {
      gotTimeout = false;
    }
  }
  return gotTimeout;
}

void
Mutex::release()
{
  auto* userdata = reinterpret_cast<pthread_mutex_t*>(mUserdata);
  pthread_mutex_unlock(userdata);
}
