#include "simple_thread.h"

#include <protest/api.h>
#include <protest/doc.h>

using namespace protest;
using namespace protest::time;

// ---------------------------------------------------------------------------
/**
 * @test    tutorial-01
 * @version 1.0.0
 */
Context context;

// ---------------------------------------------------------------------------
/**
 * @class TestRunner
 */
class TestRunner : public Runner
{
public:
  TestRunner(Context& context) : Runner(context, "main")
  {
  }

  void
  initialize() override
  {
    mThd.start();
  }

  void
  process() override
  {
    auto sw = createStopwatch();
    mThd.waitForThread();
    assertThat(sw.lap() == 1000_ms);
  }

private:
  SimpleThread mThd;
};

int
main(int argc, char const* argv[])
{
  context.initialize(argc, argv);
  TestRunner runner(context);
  return context.run();
}
