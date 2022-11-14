#include <protest/api.h>

using namespace protest::time;
using namespace protest;

// ---------------------------------------------------------------------------
/**
 * @test    Tutorial-02
 * @author  jreinking
 * @version 0.0.1
 */
protest::Context context;

// ---------------------------------------------------------------------------
/**
 * @class TestRunner
 */
class TestRunner : public protest::Runner
{
public:
  TestRunner(protest::Context& context) : protest::Runner(context, "main")
  {
  }

  void
  process() override
  {
    auto sw = createStopwatch();
    sw.stop();

    checkThat(sw.lap() == 1_ms)
        << "Stopwatch lap: " << sw.lap().milliseconds() << "ms";

    auto value = Value<int>(0);
    auto timer = createTimer(
        [&]()
        {
          value = 42;
        },
        1000_ms);

    auto inv = createInvariant((!(sw < 1000_ms) || value == 0) &&
                               (!(sw > 1000_ms) || value == 42));

    sw.start();
    bool success = wait(value == 42, 2000_ms);

    assertThat(success) << "Should not run into a timeout!";

    assertThat(sw.lap() == 1000_ms) << "Timer should run after 1000_ms";

    assertThat(value == 42) << "Timer should set value to 42";

    wait(1000_ms);

    value = 1;
  }

private:
};

int
main(int argc, char const* argv[])
{
  context.initialize();
  TestRunner runner(context);
  return context.run();
}
