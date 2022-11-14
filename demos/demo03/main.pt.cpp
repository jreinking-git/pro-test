#include <protest/api.h>
#include <protest/doc.h>

using namespace protest;
using namespace protest::time;

// ---------------------------------------------------------------------------
/**
 * @test    demo-03
 * @author  Janosch Reinking
 * @version 1.0.0
 */
Context context;

// ---------------------------------------------------------------------------
/**
 * @class MyRunner
 */
class MyRunner : public Runner
{
public:
  MyRunner(Context& context) : Runner(context, "main")
  {
  }

  void
  process()
  {
    {
      auto sec = section("stopwatch basic");

      /** @info
       * A stopwatch can be used to measure time diff
       */

      /** @info
       * @seperator
       * The stopwatch should measure 1000 ms
       * @seperator
       */
      auto sw = createStopwatch();
      wait(1000_ms);
      assertThat(sw.lap() == 1000_ms);

      /** @info
       * @seperator
       * Stopwatch is not active. Therefore the measure time will not change.
       * @seperator
       */
      sw.stop();
      wait(1000_ms);
      assertThat(sw.lap() == 1000_ms);

      /** @info
       * @seperator
       * A reseted stopwatch should yield 0 ms
       * @seperator
       */
      sw.reset();
      assertThat(sw.lap() == 0_ms);
      wait(1000_ms);
      assertThat(sw.lap() == 0_ms);
      sw.start();
      wait(1000_ms);
      assertThat(sw.lap() == 1000_ms);
    }

    {
      auto sec = section("stopwatch in condition");

      /** @info
       * A stopwatch can also be used in conditions
       */

      /** @info
       * @seperator
       * It is also possible to use a stopwatch in a wait call.
       * @seperator
       */
      auto sw = createStopwatch();
      sw.reset();
      wait(sw >= 1000_ms);
      assertThat(sw.lap() == 1000_ms);
    }
  }

private:
};

int
main(int argc, char const* argv[])
{
  context.initialize();
  MyRunner runner(context);
  return context.run();
}
