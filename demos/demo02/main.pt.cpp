#include <protest/api.h>
#include <protest/doc.h>

using namespace protest;
using namespace protest::time;

// ---------------------------------------------------------------------------
/**
 * @test    demo-02
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
      auto sec = section("timer basic");

      /** @info
       * This should create a timer which expires after 1000 ms.
       * @seperator
       */
      {
        int i = 0;
        // clang-format off
        auto timer = createTimer([&]() {
          i++;
        });
        // clang-format on

        timer.start(1000_ms);
        wait(1000_ms);

        assertThat(i == 1);
      }

      /** @info
       * @seperator
       * The timer should immediatly started and expires after 1000 ms.
       * @seperator
       */
      {
        int i = 0;

        // clang-format off
        createTimer([&]() {
          i++;
        }, 1000_ms);
        // clang-format on

        wait(1000_ms);

        assertThat(i == 1);
      }

      /** @info
       * @seperator
       * The method @c stop can be used to stop a running timer.
       * @seperator
       */
      {
        int i = 0;
        auto timer = createTimer(
            [&]()
            {
              i++;
            },
            1000_ms);

        wait(500_ms);
        timer.stop();
        wait(500_ms);

        assertThat(i == 0);
      }

      /** @info
       * @seperator
       * @c reset can be used to start the timer from zero again. This method can be
       * called on running but also on stopped timers. In both cases the timer starts
       * from zero.
       * @seperator
       */
      {
        int i = 0;
        // clang-format off
        auto timer = createTimer([&]() {
          i++;
        }, 1000_ms);
        // clang-format on

        wait(500_ms);
        timer.reset();
        wait(500_ms);
        timer.stop();
        timer.reset();
        wait(500_ms);

        assertThat(i == 0);

        wait(500_ms);

        assertThat(i == 1);
      }

      /** @info
       * @seperator
       * Recursive timers should work
       * @seperator
       */
      {
        // clang-format off
        int i = 0;
        createTimer([&] {
          createTimer([&]() {
            i++;
          }, 1000_ms);
        }, 1000_ms);

        wait(2000_ms);

        assertThat(i == 1);
        // clang-format on
      }

      /** @info
       * @seperator
       * By adding a parameter to the callback it is possible to access and restart the 
       * timer from the callback itself.
       * @seperator
       */
      {
        int i = 0;
        createTimer(
            [&](auto& timer)
            {
              i++;
              if (i < 7)
              {
                logger() << i;
                timer.start(1000_ms);
              }
            },
            1000_ms);

        wait(10000_ms);

        assertThat(i == 7);
      }

      /** @info
       * @seperator
       * A timer can be started or stopped via the methods @c stop and @c start
       * @seperator
       */
      {
        // clang-format off
        int i = 0;
        auto stopAndGo = createTimer([&]() {
          i++;
        }, 1000_ms);

        wait(500_ms);
        stopAndGo.stop();
        wait(500_ms);
        stopAndGo.start();
        assertThat(i == 0);
        wait(500_ms);

        assertThat(i == 1);
        // clang-format on
      }

      /** @info
       * @seperator
       * Other protest api function can also be called inside the callback. Since the
       * compiler will evaluate the expr from inner to outer the @c assertThat will be
       * handled before @c createTimer.
       * @seperator
       */
      {
        // clang-format off

        createTimer([&]() {
          assertThat(true);
        }, 1000_ms);
        wait(1000_ms);

        // clang-format on
      }
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
