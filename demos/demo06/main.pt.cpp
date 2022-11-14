#include <protest/api.h>
#include <protest/doc.h>

using namespace protest;
using namespace protest::time;

// ---------------------------------------------------------------------------
/**
 * @test    demo-06
 * @author  Janosch Reinking
 * @version 1.0.0
 */
Context context;

// ---------------------------------------------------------------------------
Signal<int> mSignal1(context);
Signal<int> mSignal2(context);

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
  initialize()
  {
    mSamplePort = createPort(mSignal1);
    mQueuePort = createPort(mSignal2);
  }

  void
  process()
  {
    {
      auto sec = section("condition basic");

      /** @info
       * Most of the protest objects can be used to form expressions. These expressions
       * can be stored as @c Conditions. Condition are used for example in invariants
       * or calls to @c wait. A condition reflects the structure of the expression in
       * its type and stores also runtime information of the expression. Therefore, it
       * knows about the object used to build the @c Condition. This allows protest to
       * reflect changes in the condition which can be used to re-evaluate it.
       */

      {
        /**
         * The following statments will create a condition which includes the @c mSamplePort
         * at run-time but also reflects the type of the expression.
         */
        auto condition = (mSamplePort == 5);

        /** @info
         * @seperator
         * The condition should reflect the change of the port.
         * @seperator
         */
        mSignal1.push(5);
        assertThat(condition);

        mSignal1.push(6);
        assertThat(!condition);

        /** @info
         * @seperator
         * The runner should wait until the condition becomes true.
         * @seperator
         */
        // clang-format off
        createTimer([]() {
          mSignal1.push(5);
        }, 1000_ms);
        // clang-format on

        wait(condition);
        assertThat(condition);
      }

      /** @info
       * @seperator
       * It is also possible to use primitive types if there are wrapped into a
       * @c Value.
       * @seperator
       */
      {
        auto value = Value(42);
        auto condition = (value == 5);
        assert(!condition);

        // clang-format off
        createTimer([&]() {
          value = 5;
        }, 1000_ms);
        // clang-format on

        wait(condition);
        assertThat(condition);
      }

      /** @info
       * @seperator
       * @c Value should be implicit convertable to the underlying type.
       * @seperator
       */

      auto value = Value(5);
      // lhs is primitive -> expression has type bool
      assertThat(4 + value == 9);
      // lhs is a protest::Value -> expression has type condition
      assertThat(value + 4 == 9);

      /** @info
       * @seperator
       * Stopwatches can be used to bring time into the condition
       * @seperator
       */
      auto sw = createStopwatch();
      wait(sw >= 1000_ms);
      assertThat(sw.lap() == 1000_ms);

      /** @info
       * @seperator
       * Condition are also used to build invariants
       * @seperator
       */
      {
        auto value = Value(5);
        auto condition = (value == 5);
        auto inv = createInvariant(sw < 10000_ms && condition);
        assertThat(inv.holds());
      }
    }
  }

private:
  SamplePort<int> mSamplePort;
  QueuePort<int> mQueuePort;
};

int
main(int argc, char const* argv[])
{
  context.initialize();
  MyRunner runner(context);
  return context.run();
}
