#include <protest/api.h>
#include <protest/doc.h>

using namespace protest;
using namespace protest::time;

// ---------------------------------------------------------------------------
/**
 * @test    demo-05
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
    auto& tm = context.getTestManager();

    {
      auto sec = section("invariant basic");

      /** @info
       * A invariant describes a condition which must always met. When ever a part of
       * the condition changed, the invariant will be re-evaluated.
       */

      /** @info
       * @seperator
       * Since the value of mSamplePort is smaller then 5 the invariants holds.
       * @seperator
       */
      auto inv1 = createInvariant(mSamplePort < 5);
      assertThat(inv1.holds());

      /** @info
       * @seperator
       * By pushing the value 6 to mSignal1 the invariant does not hold any more. The
       * violation will be reported once.
       * @seperator
       */
      mSignal1.push(5);
      assertThat(!inv1.holds());
      assertThat(tm.getNumberOfFailedInvariants() == 1);
      inv1.stop();

      /** @info
       * @seperator
       * The method @c stop can be used to disable an invariant. It can be reactivated
       * by using @c start.
       * @seperator
       */
      {
        auto inv2 = createInvariant(mSamplePort < 100);
        inv2.stop();

        mSignal1.push(100);
        assertThat(inv2.holds());
        mSignal1.push(1);
        inv2.start();

        assertThat(inv2.holds());
        assertThat(tm.getNumberOfFailedInvariants() == 1);
      }

      /** @info
       * @seperator
       * An invariant that does not hold on creating should also report an
       * violation.
       * @seperator
       */
      mSignal1.push(100);
      auto inv3 = createInvariant(mSamplePort != 100);
      inv3.stop();

      assertThat(!inv3.holds());
      logger() << tm.getNumberOfFailedInvariants();
      assertThat(tm.getNumberOfFailedInvariants() == 2);

      /** @info
       * @seperator
       * A invariant is active as long as the handle exists. The following code
       * should not report an vialation of the invariant.
       * @seperator
       */
      {
        auto inv4 = createInvariant(mSamplePort != 1000);
      }
      mSignal1.push(1000);

      /** @info
       * @seperator
       * A invariant which was not created by the test can be retrieved using the
       * @c TestManager.
       * @seperator
       */
      if (false)
      {
        auto inv5 = createInvariant(mSamplePort == 0);
      }
      assertThat(tm.getNumberOfNotExecutedInvariants() == 1);
    }
  }

private:
  SamplePort<int> mSamplePort;
  QueuePort<int> mQueuePort;
};

int
main(int argc, char const* argv[])
{
  context.initialize(argc, argv);
  MyRunner runner(context);
  return context.run();
}
