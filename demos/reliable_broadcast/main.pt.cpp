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

#include "reliable_broadcast.h"

#include <protest/api.h>
#include <protest/doc.h>

using namespace protest;
using namespace protest::time;

// ---------------------------------------------------------------------------
/**
 * @test    Reliable Broadcast Test
 * @author  Janosch Reinking
 * @version 1.0.0
 */
Context context;

// ---------------------------------------------------------------------------
Signal<Message> channel(context);

Signal<std::tuple<uint8_t, uint8_t>> deliver(context);

// ---------------------------------------------------------------------------
/**
 * @class MyRunner
 */
class MyRunner : public Runner, public ReliableBroadcast::Communication
{
public:
  MyRunner(Context& context,
           const char* name,
           const char* thdName,
           uint8_t id,
           bool isReceiver,
           bool isFaulty) :
    Runner(context, name),
    mId(id),
    mIsReceiver(isReceiver),
    mIsFaulty(isFaulty),
    mModule(*this, thdName),
    mRessourcesBusy(0)
  {
    assert(!isFaulty || isReceiver);
  }

  void
  initialize() override
  {
    mStopwatch = createStopwatch();
    mStopwatch.stop();

    mMessagePort = createPort(channel);
    mDeliverPort = createPort(deliver);

    // clang-format off
    mMessagePort.addFilter([&](auto& value) {
      return value.mTo == mId;
    });

    mMessagePort.addListener([&](auto& value) {
      mModule.acquire();
      mModule.handle(value);
      mModule.release();
    });

    mDeliverPort.addListener([&](auto& value) {
      if (std::get<0>(value) == mId)
      {
        assertThat(mRessourcesBusy == 0 || mIsFaulty);
        uint8_t v = std::get<1>(value);
        assertThat(v == 42u || mIsFaulty);
      }
    });
    // clang-format on
  }

  void
  process() override
  {
    mModule.start();
    static constexpr size_t numberOfSequences = 5;
    auto inv = createInvariant((mStopwatch <= maxExecutionTime) || mIsFaulty);

    for (int i = 0; i < numberOfSequences; i++)
    {
      if (mIsFaulty && coinFlip(0.1))
      {
        // faulty note can be to early or to late
        wait(random(maxExecutionTime * 4));
      }
      else
      {
        wait(maxExecutionTime * 2);
      }

      /// let all runners be synchronized within @c maxTimeError
      wait(random(maxTimeError));

      uint8_t result = 0;
      mStopwatch.start();

      mModule.acquire();
      if (mIsReceiver)
      {
        mModule.receive();
      }
      else
      {
        mModule.propose(42U);
      }
      mModule.release();

      /**
       * wait for the first 3 (don't give the faulty note the change to break
       * time mesaurement by delaying it's result)
       */
      wait(mDeliverPort.size() == numberOfNodes - 1);

      /**
       * wait for the last result. Even a faulty node will deliver a result.
       */
      wait(mDeliverPort.size() == numberOfNodes);

      mDeliverPort.clear();
    }
  }

  void
  broadcast(Message& message) override
  {
    // clang-format off
    message.setFrom(mId);
    mRessourcesBusy += numberOfNodes;

    // a faulty node can have a invalid value
    if (mIsFaulty && coinFlip(0.1))
    {
      info() << "Fault injection: use invalid value!";
      message.setValue(rand() % 100);
    }

    // a faulty node can have a invalid sequence number
    if (mIsFaulty && coinFlip(0.1))
    {
      info() << "Fault injection: use invalid sequence!";
      message.setSequence(message.getSequence() + ((rand() % 10) - 5));
    }
    
    // a faulty node can have a invalid type
    if (mIsFaulty && coinFlip(0.1))
    {
      info() << "Fault injection: use invalid message type!";
      message.setType((message.getType() == Type::initial) ? Type::echo : Type::initial);
    }

    for (uint8_t id = 0; id < numberOfNodes; id++)
    {
      if (mIsFaulty && coinFlip(0.1))
      {
        // a faulty node can send it's message to early or to late
        info() << "Fault injection: send message later or earlier!";
        message.setTo(id);
        auto delay = createTimer([=]() {
          channel.push(message);
        }, random(Duration::zero(), maxExecutionTime * 2));
      }
      else
      {
        // a faulty node can omitted a single messages
        if (!mIsFaulty || coinFlip(0.9))
        {
          message.setTo(id);
          auto delay = createTimer([=]() {
            mRessourcesBusy--;
            channel.push(message);
          }, random(minSendTime, maxSendTime));
        }
        else
        {
          info() << "Fault injection: omit a message!";
        }
      }
    }

    // clang-format on
  }

  void
  deliverResult(uint8_t result) override
  {
    auto tpl = std::tuple<uint8_t, uint8_t>(mId, result);
    mStopwatch.stop();
    mStopwatch.reset();
    deliver.push(tpl);
  }

private:
  uint8_t mId;
  bool mIsReceiver;
  bool mIsFaulty;
  ReliableBroadcast mModule;

  SamplePort<Message> mMessagePort;
  QueuePort<std::tuple<uint8_t, uint8_t>> mDeliverPort;
  size_t mRessourcesBusy;
  Stopwatch mStopwatch;
};

int
main(int argc, char const* argv[])
{
  context.initialize(argc, argv);
  MyRunner runner0(context, "tsk0", "thd0", 0, true, true);
  MyRunner runner1(context, "tsk1", "thd1", 1, true, false);
  MyRunner runner2(context, "tsk2", "thd2", 2, true, false);
  MyRunner runner3(context, "tsk3", "thd3", 3, false, false);
  return context.run();
}
