#define PROTEST_SOURCE_FILE main

#include <protest/api.h>
#include <protest/doc.h>
#include <protest/rtos.h>

using namespace protest;
using namespace protest::time;
using namespace protest::meta;

// ---------------------------------------------------------------------------

namespace protest
{

/**
 * @class ExecutionTimer
 */
class ExecutionTimer
{
public:
  explicit ExecutionTimer(Duration duration, bool min, CallContext& context) :
    mCallContext(context),
    mDuration(duration),
    mMin(min)
  {
    auto* runner = Context::getCurrentContext()->getCurrentVirtual();
    mStartAt = runner->now();
  }

  ExecutionTimer(const ExecutionTimer& other) = delete;
  
  ExecutionTimer(ExecutionTimer&& other) = delete;

  ExecutionTimer& operator=(const ExecutionTimer& other) = delete;

  ExecutionTimer& operator=(ExecutionTimer&& other) = delete;

  ~ExecutionTimer()
  {
    auto* runner = Context::getCurrentContext()->getCurrentVirtual();
    auto& logger = runner->getLogger();
    auto diff = (runner->now() - mStartAt);
    if (mMin && diff < mDuration)
    {
      logger.startLog("FAIL",
                      runner->getName(),
                      mCallContext.getUnit().getFileName(),
                      mCallContext.getLine(),
                      runner->now())
        << "Min execution time not reached!\n"
        << "Expected min: "
        << mDuration.milliseconds()
        << " ms\n"
        << "     But got: "
        << diff.milliseconds()
        << " ms";
    }
    else if (!mMin && diff > mDuration)
    {
      logger.startLog("FAIL",
                      runner->getName(),
                      mCallContext.getUnit().getFileName(),
                      mCallContext.getLine(),
                      runner->now())
        << "Max execution time exceeded!\n"
        << "Expected max: "
        << mDuration.milliseconds()
        << " ms\n"
        << "     But got: "
        << diff.milliseconds()
        << " ms";
    }
  }

private:
  CallContext& mCallContext;
  Duration mDuration;
  TimePoint mStartAt;
  bool mMin;
};

// ---------------------------------------------------------------------------
ExecutionTimer minExecutionTime(Duration min, CallContext& context)
{
  return ExecutionTimer(min, true, context);
}

ExecutionTimer maxExecutionTime(Duration max, CallContext& context)
{
  return ExecutionTimer(max, false, context);
}

int protectedBy(protest::rtos::Mutex& mutex, CallContext& context)
{
  if (!mutex.isAcquired())
  {
    auto* runner = Context::getCurrentContext()->getCurrentVirtual();
    auto& logger = runner->getLogger();
    logger.startLog("FAIL",
                runner->getName(),
                context.getUnit().getFileName(),
                context.getLine(),
                runner->now())
        << "Function is not protected!";
  }
  else
  {

  }
  return 0;
}

}; // namespace protest

// ---------------------------------------------------------------------------
/**
 * @test    demo-13
 * @author  Janosch Reinking
 * @version 1.0.0
 */
Context context;

class MyRunner : public Runner
{
public:
  MyRunner(Context& context) : Runner(context, "main")
  {
  }

  [[
    pt::minExecutionTime(100_ms),
    pt::maxExecutionTime(1000_ms),
  ]]
  void
  process()
  {
    func1();

    mMutex.acquire();
    func1();
    mMutex.release();
  }

  [[ pt::protectedBy(mMutex) ]]
  void
  func1()
  {

  }

  void
  finalize()
  {
  }

private:
  protest::rtos::Mutex mMutex;
};

int
main(int argc, char const* argv[])
{
  context.initialize(argc, argv);
  MyRunner runner(context);
  return context.run();
}

