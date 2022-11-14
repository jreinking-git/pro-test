#include "reliable_broadcast.h"

#include <protest/rtos/log.h>

#include <iostream>

using namespace protest::rtos;
using namespace protest::time;

// ---------------------------------------------------------------------------
/**
 * @class CommunicationImpl
 */
class CommunicationImpl : public ReliableBroadcast::Communication
{
public:
  explicit CommunicationImpl() = default;

  CommunicationImpl(const CommunicationImpl& other) = delete;

  CommunicationImpl(CommunicationImpl&& other) = delete;

  CommunicationImpl&
  operator=(const CommunicationImpl& other) = delete;

  CommunicationImpl&
  operator=(CommunicationImpl&& other) = delete;

  ~CommunicationImpl() = default;

// ---------------------------------------------------------------------------
  void
  broadcast(Message& message)
  {
    PT_WARN("Not implemented!");
  }

  void
  deliverResult(uint8_t result)
  {
    PT_WARN("Not implemented!");
    mGotResult = true;
  }

  bool
  gotResult()
  {
    return mGotResult;
  }

private:
  bool mGotResult = false;
};

// ---------------------------------------------------------------------------
int
main(int argc, char const* argv[])
{
  CommunicationImpl impl;
  ReliableBroadcast module(impl, "thd1");
  module.start();
  module.acquire();
  module.propose(42);
  while (!impl.gotResult())
  {
    module.release();
    Thread::sleep(Millisecond(10));
    module.acquire();
  }
  return 0;
}
