#include <protest/api.h>
#include <protest/doc.h>

using namespace protest;
using namespace protest::time;

// ---------------------------------------------------------------------------
/**
 * @test    demo-04
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
    /**
     * This will attache a port to a signal. This should always be done in the
     * initializing phase.
     */
    mSamplePort = createPort(mSignal1);
    mQueuePort = createPort(mSignal2);
  }

  void
  process()
  {
    {
      auto sec = section("sample port");

      /** @info
       * Object of type Signal can be used for the communcation between diffrent Runner.
       * A Runner can attach a SamplePort or QueuePort to the Signal in order to listen
       * on it. The Signal itself can be used to send values. 
       */

      /** @info
       * @seperator
       * A SamplePort will keep the last value received via the connected signal. The
       * sample port can be cast to the type of the received value.
       * @seperator
       */
      mSignal1.push(4);
      int value = mSamplePort;

      assertThat(static_cast<int>(mSamplePort) == 4);
      assertThat(value == 4);
      assertThat(mSamplePort == 4);

      /** @info
       * @seperator
       * Listener can also be attached to a port in order to receive the messages.
       * The callback has a parameter which can be omitted if the received value is
       * not relevant.
       * @seperator
       */
      int called = 0;
      value = 0;

      // clang-format off
      auto listener1 = mSamplePort.addListener([&](auto& v) {
        value = v;
        called++;
      });

      auto listener2 = mSamplePort.addListener([&]() {
        called++;
      });
      // clang-format on

      mSignal1.push(4);
      assertThat(called == 2);
      assertThat(value == 4);

      /** @info
       * @seperator
       * Listener can also be removed
       * @seperator
       */
      called = 0;
      mSamplePort.removeListener(listener1);
      mSamplePort.removeListener(listener2);

      mSignal1.push(4);
      assertThat(called == 0);

      // clang-format off
      /** @info
       * @seperator
       * A Runner can also wait until a SamplePort has a given value.
       * @seperator
       */
      int expectedValue = 5;
      auto sw = createStopwatch();
      auto timeout = 1000_ms;
      createTimer([&]() {
        mSignal1.push(expectedValue);
      }, timeout);

      // clang-format on

      wait(mSamplePort == expectedValue);

      assertThat(sw.lap() == timeout);
      assertThat(mSamplePort == expectedValue);

      /** @info
       * @seperator
       * Filter can be used to ignore certain values.
       * @seperator
       */
      // clang-format off
      int filteredValue = 42;
      auto filter = mSamplePort.addFilter([&](auto& value) {
        return value != filteredValue;
      });

      createTimer([&]() {
        mSignal1.push(filteredValue);
      }, 500_ms);
      // clang-format on

      bool success = wait(mSamplePort == filteredValue, 1000_ms);
      assertThat(!success);

      /** @info
       * @seperator
       * Remove the filter again
       * @seperator
       */
      mSamplePort.removeFilter(filter);

      // clang-format off
      createTimer([&]() {
        mSignal1.push(42);
      }, 500_ms);
      // clang-format on

      success = wait(mSamplePort == 42, 1000_ms);
      assertThat(success);

      /** @info
       * @seperator
       * With setPrinterFunction() the user can use there own printer function for the
       * port. The printer function is only used, when there is at least a listener
       * which triggers a log message.
       * @seperator
       */
      // clang-format off
      auto printer = [](std::ostream& logger, auto& value) {
        logger << "This is my value:\n" << value << "\n";
      };
      mSamplePort.setPrinterFunction(printer);

      auto listener3 = mSamplePort.addListener([]() {});
      // clang-format on
      mSignal1.push(32);
      mSamplePort.removeListener(listener3);
    }

    {
      auto sec = section("queue port");

      /** @info
       * In contrast to the SamplePort a QueuePort stores the received values in a Queue.
       * @seperator
       */

      mSignal2.push(1);
      mSignal2.push(2);

      assertThat(mQueuePort.size() == 2);
      auto v1 = mQueuePort.pop();
      auto v2 = mQueuePort.pop();
      assertThat(v1 == 1);
      assertThat(v2 == 2);

      /** @info
       * @seperator
       * Analogously to the Sample port listener can be attached to the port.
       * @seperator
       */

      int called = 0;
      int value = 0;

      // clang-format off
      auto listener1 = mQueuePort.addListener([&](auto& v) {
        value = v;
        called++;
      });

      auto listener2 = mQueuePort.addListener([&]() {
        called++;
      });
      // clang-format on

      mSignal2.push(4);
      assertThat(called == 2);
      assertThat(value = 4);
      mQueuePort.pop();

      /** @info
       * @seperator
       * Listener can also be removed
       * @seperator
       */
      called = 0;

      mQueuePort.removeListener(listener1);
      mQueuePort.removeListener(listener2);

      mSignal2.push(4);
      assertThat(called == 0);
      mQueuePort.pop();

      /** @info
       * @seperator
       * One can also add filter to ignore certain values
       * @seperator
       */
      // clang-format off
      int filteredValue = 42;
      auto filter = mQueuePort.addFilter([&](auto& value) {
        return value != filteredValue;
      });
      // clang-format on

      mSignal2.push(filteredValue);
      assertThat(!mQueuePort.isAvailable());

      /** @info
       * @seperator
       * Remove the filter again
       * @seperator
       */
      mQueuePort.removeFilter(filter);

      mSignal2.push(filteredValue);

      auto v4 = mQueuePort.pop();
      assertThat(v4 == filteredValue);

      /** @info
       * @seperator
       * With setPrinterFunction() the user can use there own printer function for the
       * port. The printer function is only used, when there is at least a listener
       * which triggers a log message.
       * @seperator
       */
      // clang-format off
      auto printer = [](std::ostream& logger, auto& value) {
        logger << "This is my value:\n" << value;
      };
      mQueuePort.setPrinterFunction(printer);

      auto listener3 = mQueuePort.addListener([]() {});
      // clang-format on
      mSignal2.push(32);

      mQueuePort.removeListener(listener3);
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
