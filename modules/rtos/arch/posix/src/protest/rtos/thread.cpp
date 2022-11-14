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

#include "protest/rtos/thread.h"
#include "protest/utils/debug.h"

#include <cstdio>
#include <cerrno>
#include <cstring>
#include <unistd.h>

using namespace protest::rtos;

// ---------------------------------------------------------------------------
struct Userdata
{
  static constexpr size_t numberOfLocalStorages = 1U;

  pthread_t mThread;
  pthread_attr_t mAttr;
  uint8_t mPriority;
  // void* mLocalStorage[numberOfLocalStorages];
};

// ---------------------------------------------------------------------------
static void*
wrapper(void* attr)
{
  auto* thread = reinterpret_cast<Thread*>(attr);
  thread->run();
  return nullptr;
}

// ---------------------------------------------------------------------------
void
Thread::sleep(time::Duration duration)
{
  usleep(duration.microseconds());
}

// ---------------------------------------------------------------------------
Thread::Thread(const char* name) : Thread(0, name)
{
}

Thread::Thread(uint8_t priority, const char* name) : Thread(priority, 0, name)
{
}

Thread::Thread(uint8_t /* priority */,
               size_t stackSize,
               const char* /* name */) :
  mUserdata(nullptr)
{
  // NOLINTNEXTLINE
  mUserdata = reinterpret_cast<void*>(new Userdata());
  memset(mUserdata, 0, sizeof(Userdata));
  auto* userdata = reinterpret_cast<Userdata*>(mUserdata);

  // for (size_t i = 0; i < Userdata::numberOfLocalStorages; i++)
  // {
  //   userdata->mLocalStorage[i] = nullptr;
  // }

  int retValue = pthread_attr_init(&userdata->mAttr);
  if (retValue != 0)
  {
    perror("pthread_attr_init");
    PROTEST_ASSERT(false);
  }
  else
  {
  }

  if (stackSize > 0)
  {
    // retValue = pthread_attr_setstacksize(&userdata->mAttr, stackSize);
    // if (retValue != 0)
    // {
    //   perror("pthread_attr_setstacksize");
    //   PROTEST_ASSERT(false);
    // }
    // else
    // {
    // }
  }
  else
  {
  }
}

Thread::~Thread()
{
  auto* userdata = reinterpret_cast<Userdata*>(mUserdata);
  int ret = pthread_detach(userdata->mThread);
  if (ret != 0)
  {
    perror("pthread_detach");
  }
  // NOLINTNEXTLINE
  delete userdata;
  mUserdata = nullptr;
}

// ---------------------------------------------------------------------------
void
Thread::start()
{
  auto* userdata = reinterpret_cast<Userdata*>(mUserdata);
  int retValue =
      pthread_create(&userdata->mThread, &userdata->mAttr, &wrapper, this);
  if (retValue != 0)
  {
    perror("pthread_create");
    PROTEST_ASSERT(false);
  }
  else
  {
  }
}
