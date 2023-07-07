#define PROTEST_SOURCE_FILE main

#include <protest/api.h>
#include <protest/doc.h>

using namespace protest;
using namespace protest::time;
using namespace protest::matcher;
using namespace protest::mock;

// ---------------------------------------------------------------------------
/**
 * @class MyInterface
 */
class MyInterface
{
public:
  explicit MyInterface() = default;

  MyInterface(const MyInterface& other) = delete;

  MyInterface(MyInterface&& other) = delete;

  MyInterface&
  operator=(const MyInterface& other) = delete;

  MyInterface&
  operator=(MyInterface&& other) = delete;

  ~MyInterface() = default;

// ---------------------------------------------------------------------------
  virtual uint8_t
  setValue(uint8_t value) = 0;

  virtual void
  doSomething() = 0;

  virtual void
  doSomething(uint8_t param) = 0;

private:
};

#include "protest/mock.h"

// ---------------------------------------------------------------------------
/**
 * @test    demo-12
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

  void
  checkUnexpectedFuncCalls(size_t diff)
  {
    auto& mgr = context.getTestManager();
    if ((mgr.getNumberOfUnexpectedFunctionCalls() - mNumberOfUnexpectedFunctionCalls) != diff)
    {
      // std::cout << "have: " << mgr.getNumberOfUnexpectedFunctionCalls() << std::endl;
      // std::cout << "expected: " << mNumberOfUnexpectedFunctionCalls + diff << std::endl;
      assert(false);
    }
    mNumberOfUnexpectedFunctionCalls += diff;
  }

  void
  checkOversaturatedFuncCalls(size_t diff)
  {
    auto& mgr = context.getTestManager();
    if ((mgr.getNumberOfOversaturatedFunctionCalls() - mNumberOfOversaturatedCalls) != diff)
    {
      // std::cout << "have: " << mgr.getNumberOfOversaturatedFunctionCalls() << std::endl;
      // std::cout << "expected: " << mNumberOfOversaturatedCalls + diff << std::endl;
      assert(false);
    }
    mNumberOfOversaturatedCalls += diff;
  }

  void
  checkUnmetPrerequisites(size_t diff)
  {
    auto& mgr = context.getTestManager();
    if ((mgr.getNumberOfUnmetPrerequisties() - mNumberUnmetPrerequisitesCalls) != diff)
    {
      // std::cout << "have: " << mgr.getNumberOfUnmetPrerequisties() << std::endl;
      // std::cout << "expected: " << mNumberUnmetPrerequisitesCalls + diff << std::endl;
      assert(false);
    }
    mNumberUnmetPrerequisitesCalls += diff;
  }

  void
  checkMissingFuncCalls(size_t diff)
  {
    auto& mgr = context.getTestManager();
    // std::cout << mgr.getNumberOfMissingFunctionCalls() << std::endl;
    // std::cout << mNumberOfMissingFunctionCalls << std::endl;
    assert((mgr.getNumberOfMissingFunctionCalls() - mNumberOfMissingFunctionCalls) == diff);
    mNumberOfMissingFunctionCalls += diff;
  }

  void
  process()
  {
    // clang-format off

    /** @info(format)
     * If there is no expectation for a given call, pro-test will print a
     * 'Unexpected call' message. If there are expections but non of them
     * matches the given parameter pro-test will also print an
     * 'Unexpected call' message.
     */
    {
      auto sec = section("unexpected call");

      /** @info(format)
       * @seperator
       * No expectation. A call to 'void doSomething(void)' should yield in a
       * 'Unexpected call' message.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;
        mock.doSomething();
      }

      checkUnexpectedFuncCalls(1);

      /** @info(format)
       * @seperator
       * There is an expectation. But it requires a 42 as parameter. Therfore
       * a call to 'void doSomething(23)' should yield in a 'Unexpected call'
       * message.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.doSomething(Eq(42)));

        mock.doSomething(23);
      }

      checkUnexpectedFuncCalls(1);
      checkMissingFuncCalls(1);

      /** @info(format)
       * @seperator
       * No expectation. Two calls to 'void doSomething(void)' should yield in
       * two 'Unexpected call' message.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        mock.doSomething(32);
        mock.doSomething(33);

      }
      
      checkUnexpectedFuncCalls(2);
    }

    /** @info(format)
     * @seperator
     * The cardinality 'exactly' checks if the call count match excactly the
     * expectation. In this section the tests match the expectation. Therefore
     * only the recognized function mock calls should be printed. But there
     * should'nt be a error message. The parameter and the return value of
     * each function mock call should be printed.
     * @seperator
     */
    {
      auto sec = section("cardinality - exactly - satisfied");

      /** @info(format)
       * @seperator
       * Expect one call to doSomething(void)
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.doSomething());

        mock.doSomething();
      }

      /** @info(format)
       * @seperator
       * Expect one call to doSomething(42)
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.doSomething(Eq(42)));
        mock.doSomething(42);
      }

      /** @info(format)
       * @seperator
       * Expect two calls to @c doSomething(42)
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.doSomething(Eq(42)))
          .times(exactly(2));
      
        mock.doSomething(42);
        mock.doSomething(42);
      }

      /** @info(format)
       * @seperator
       * Expect 3 calls to @c doSomething(42)
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.doSomething(Eq(42)))
          .times(exactly(3));
       
        mock.doSomething(42);
        mock.doSomething(42);
        mock.doSomething(42);
      }

      /** @info(format)
       * @seperator
       * Expect 3 calls to @c setValue(42). The default value should be returned
       * since there was no action specified.
       * uint8_t setValue(uint8_t)
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.setValue(Eq(42)))
          .times(exactly(3));
       
        auto value1 = mock.setValue(42);
        auto value2 = mock.setValue(42);
        auto value3 = mock.setValue(42);

        assert(value1 == 0);
        assert(value2 == 0);
        assert(value3 == 0);
      }
    }

    /** @info(format)
     * If the cardinality 'exactly' is used but at the destruction of the
     * mocker there are still missing calls, pro-test will print a 
     * 'Missing call' message, which explains why the cardinality was not
     * met.
     */
    {
      auto sec = section("cardinality - exactly - missing call");

      /** @info(format)
       * @seperator
       * Expected one call to @c doSomething() but it is missing.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.doSomething());
      }

      checkMissingFuncCalls(1);

      /** @info(format)
       * @seperator
       * Expected one call to @c doSomething(42) but it is missing.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.doSomething(Eq(42)));
      }

      checkMissingFuncCalls(1);

      /** @info(format)
       * @seperator
       * Expected two calls to @c doSomething(42) but both are missing.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.doSomething(Eq(42)))
          .times(exactly(2));
      }

      checkMissingFuncCalls(1);

      /** @info(format)
       * @seperator
       * Expected two calls to @c doSomething(42) but one is missing.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.doSomething(Eq(42)))
          .times(exactly(2));

        mock.doSomething(42);
      }

      checkMissingFuncCalls(1);

      /** @info(format)
       * @seperator
       * Expected 3 calls to @c doSomething(42) but all of them are missing.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.doSomething(Eq(42)))
          .times(exactly(3));
      }

      checkMissingFuncCalls(1);

      /** @info(format)
       * @seperator
       * Expected 3 calls to @c setValue(42) but two of them are missing.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.setValue(Eq(42)))
          .times(exactly(3));
       
        mock.setValue(42);
      }

      checkMissingFuncCalls(1);

      /** @info(format)
       * @seperator
       * Expected 3 calls to @c setValue(42) but one of them is missing.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.setValue(Eq(42)))
          .times(exactly(3));
       
        mock.setValue(42);
        mock.setValue(42);
      }

      checkMissingFuncCalls(1);
    }

    /** @info(format)
     * @seperator
     * When the parameter of a mock function call matches with one of the
     * expectation, but the expectation is already saturated a
     * 'Unexpected call' message will be printed. This message contains
     * an explanation why the cardinality is not met.
     * @seperator
     */
    {
      auto sec = section("cardinality - exactly - oversaturated");

      /** @info(format)
       * @seperator
       * Expect a call to @c doSomething() but there are two.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.doSomething());
        
        mock.doSomething();
        mock.doSomething();
      }

      checkOversaturatedFuncCalls(1);

      /** @info(format)
       * @seperator
       * Expect a call to @c doSomething(42) but there are two.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.doSomething(Eq(42)));
        
        mock.doSomething(42);
        mock.doSomething(42);
      }

      checkOversaturatedFuncCalls(1);

      /** @info(format)
       * @seperator
       * Expect 3 calls to @c setValue(42) but there are 4. The first 3 calls
       * should return 3. The 4 call should yield in a 'Unexpected call'
       * message. In this case the default value should be returned. Which
       * is 0 in this case.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.setValue(Eq(42)))
          .times(exactly(3))
          .willRepeatedly(Return(3));
       
        auto value0 = mock.setValue(42);
        auto value1 = mock.setValue(42);
        auto value2 = mock.setValue(42);
        auto value3 = mock.setValue(42);

        assert(value0 == 3);
        assert(value1 == 3);
        assert(value2 == 3);
        assert(value3 == 0);
      }

      checkOversaturatedFuncCalls(1);
    }

    /** @info(format)
     * @seperator
     * 'atMost' will accept all call counters that are equal or less to the
     * expected value. The recognized mock function calls should be printed.
     * But there should'nt be an error message.
     * @seperator
     */
    {
      auto sec = section("cardinality - at most - satisfied");

      /** @info(format)
       * @seperator
       * Expected at most one call to @c doSomething() . Actually zero calls
       * performed.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.doSomething())
          .times(atMost(1));
      }

      /** @info(format)
       * @seperator
       * Expected at most one call to @c doSomething() . Actually one call
       * performed.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.doSomething())
          .times(atMost(1));

        mock.doSomething();
      }

      /** @info(format)
       * @seperator
       * Expected at most one call to @c doSomething(42). Actually zero calls
       * performed.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.doSomething(Eq(42)))
          .times(atMost(1));
      }

      /** @info(format)
       * @seperator
       * Expected at most one call to @c doSomething(42) . Actually one call
       * performed.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.doSomething(Eq(42)))
          .times(atMost(1));

        mock.doSomething(42);
      }

      /** @info(format)
       * @seperator
       * Expected at most 3 calls to @c setValue(42) . Actually one call
       * performed.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.setValue(Eq(42)))
          .times(atMost(3));
        
        mock.setValue(42);
      }

      /** @info(format)
       * @seperator
       * Expected at most 3 calls to @c setValue(42) . Actually two calls
       * performed.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.setValue(Eq(42)))
          .times(atMost(3));
        
        mock.setValue(42);
        mock.setValue(42);
      }

      /** @info(format)
       * @seperator
       * Expected at most 3 calls to @c setValue(42) . Actually 3 calls
       * are performed. There should return the default value.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.setValue(Eq(42)))
          .times(atMost(3));
        
        auto value1 = mock.setValue(42);
        auto value2 = mock.setValue(42);
        auto value3 = mock.setValue(42);
      
        assert(value1 == 0);
        assert(value2 == 0);
        assert(value3 == 0);
      }
    }

    /** @info(format)
     * @seperator
     * When the parameter of a mock function call matches with one of the
     * expectation, but the expectation is already saturated a
     * 'Unexpected call' message will be printed. This message contains
     * an explanation why the cardinality is not met.
     * @seperator
     */
    {
      auto sec = section("cardinality - at most - oversaturated");

      /** @info(format)
       * @seperator
       * Expected at most one call to 'doSomething()'. Actually two calls are
       * performed.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.doSomething())
          .times(atMost(1));

        mock.doSomething();
        mock.doSomething();
      }

      checkOversaturatedFuncCalls(1);

      /** @info(format)
       * @seperator
       * Expected at most 3 calls to @c setValue(42) . Actually 4 calls are
       * performed.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.setValue(Eq(42)))
          .times(atMost(3));
        
        mock.setValue(42);
        mock.setValue(42);
        mock.setValue(42);
        mock.setValue(42);
      }

      checkOversaturatedFuncCalls(1);
    }

    /** @info(format)
     * @seperator
     * When the minimum set with 'atLeast' is reached no failure message
     * should be reported.
     * @seperator
     */
    {
      auto sec = section("cardinality - at least - satisfied");

      /** @info(format)
       * @seperator
       * Expect at least 0 calls to @c doSomething() which should be always
       * fulfilled.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.doSomething())
          .times(atLeast(0));
      }

      /** @info(format)
       * @seperator
       * Expect at least 0 calls to @c doSomething() which should be always
       * fulfilled. Actually call @c doSomething() once.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.doSomething())
          .times(atLeast(0));

        mock.doSomething();
      }

      /** @info(format)
       * @seperator
       * Expected at least two calls to @c setValue(42). Call it actually
       * twice.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.setValue(Eq(42)))
          .times(atLeast(2));
        
        mock.setValue(42);
        mock.setValue(42);
      }
    }

    /** @info(format)
     * @seperator
     * When the minimum set with 'atLeast' is not reached a 'missing call'
     * should be reported.
     * @seperator
     */
    {
      auto sec = section("cardinality - at least - missing call");

      /** @info(format)
       * @seperator
       * Expected at least two calls to @c setValue(42). Call it actually
       * once.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.setValue(Eq(42)))
          .times(atLeast(2));
        
        mock.setValue(42);
      }

      checkMissingFuncCalls(1);
    }

    /** @info(format)
     * @seperator
     * When the minimum and maximum are fulfilled in 'between' should not
     * report an failure message.
     * @seperator
     */
    {
      auto sec = section("cardinality - in between - satisfied");

      /** @info(format)
       * @seperator
       * Expect at calls between 0 and 1. Should be fulfilled with 0 calls.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.doSomething())
          .times(between(0, 1));
      }

      /** @info(format)
       * @seperator
       * Expect calls between 0 and 1. Should be fulfilled with 1 calls.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.doSomething())
          .times(between(0, 1));
      
        mock.doSomething();
      }

      /** @info(format)
       * @seperator
       * Expect calls between 1 and 2. Should be fulfilled with 1 calls.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.doSomething())
          .times(between(1, 2));

        mock.doSomething();
      }

      /** @info(format)
       * @seperator
       * Expect calls between 1 and 2. Should be fulfilled with 2 calls.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.doSomething())
          .times(between(1, 2));

        mock.doSomething();
        mock.doSomething();
      }
    }

    /** @info(format)
     * @seperator
     * When the minimum is not reached when set with 'between' a 'missing call'
     * should be reported
     * @seperator
     */
    {
      auto sec = section("cardinality - in between - missing call");

      /** @info(format)
       * @seperator
       * Expect calls between 1 and 2. Should report a missing call when there
       * was no call to the mock function.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.doSomething())
          .times(between(1, 2));
      }

      checkMissingFuncCalls(1);
    }

    /** @info(format)
     * @seperator
     * When the minimum is not reached when set with 'between' a 'missing call'
     * should be reported
     * @seperator
     */
    {
      auto sec = section("cardinality - between - oversaturated");

      /** @info(format)
       * @seperator
       * Expect calls between 1 and 2. Should report a missing call when there
       * was no call to the mock function.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.doSomething())
          .times(between(1, 2));

        mock.doSomething();
        mock.doSomething();
        mock.doSomething();
      }

      checkOversaturatedFuncCalls(1);
    }

    /** @info(format)
     * @seperator
     * Expectation can be overwritten. The latter a expecation is added the
     * higher the priority.
     * @seperator
     */
    {
      auto sec = section("cardinality - expectation - override");

      /** @info(format)
       * @seperator
       * The first expectation will be overwritten by the second since the
       * matcher are overlapping. So if there is a call to @c doSomething()
       * with @c param greater then 50 the second expecation will be choosen.
       * If @c param is greater then 42 but less or equals then 50 the first
       * expectation will be choosen. In this test no failure message
       * should be reported since both expectations are fulfilled.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.doSomething(Gt(42)));

        // overide the previouse expectation
        expectCall(mocker.doSomething(Gt(50)))
          .times(exactly(2));

        mock.doSomething(51); // second expectation
        mock.doSomething(51); // second expectation
        mock.doSomething(43); // first expectation
      }

      /** @info(format)
       * @seperator
       * The first expectation will be overwritten by the second since the
       * matcher are overlapping. So if there is a call to @c doSomething()
       * with @c param greater then 50 the second expecation will be choosen.
       * If @c param is greater then 42 but less or equals then 50 the first
       * expectation will be choosen. In this test a failure message should
       * be reported since the second expectation oversaturates.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.doSomething(Gt(42)))
          .times(any());

        // overide the previouse expectation
        expectCall(mocker.doSomething(Gt(50)))
          .times(exactly(2));

        mock.doSomething(51); // second expectation
        mock.doSomething(51); // second expectation
        mock.doSomething(43); // first expectation
        mock.doSomething(51); // second expectation is oversaturated
      }

      checkOversaturatedFuncCalls(1);
    }

    /** @info(format)
     * @seperator
     * A expectation can retire when 'retireOnSaturation()' is used. If this is
     * the case when the expectation saturates it will be ignored for furthere
     * calls to the mock function.
     * @seperator
     */
    {
      auto sec = section("cardinality - between - retires on saturation");

      /** @info(format)
       * @seperator
       * Calling @c doSomething(42) will match both expectation. The latter will
       * be used until its saturates. This is the case after 2 calls. The third
       * call will then be matched with the first expectation. No failure
       * message should be reported.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        // fallback when the second expectation retires
        expectCall(mocker.doSomething(Gt(40)));

        // overide the previouse expectation
        expectCall(mocker.doSomething(Eq(42)))
          .times(between(1, 2))
          .retireOnSaturation();

        mock.doSomething(42); // second expectation
        mock.doSomething(42); // retire second expectation
        mock.doSomething(42); // first expectation
      }

      /** @info(format)
       * @seperator
       * Calling @c doSomething(42) will match both expectation. The latter will
       * be used until its saturates. This is the case after 2 calls. The third
       * call will then be matched with the first expectation. The fourth call
       * will oversaturate the first expectation which should yield in a
       * 'unexpected call' report.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        // fallback when the second expectation retires
        expectCall(mocker.doSomething(Gt(40)));

        // overide the previouse expectation
        expectCall(mocker.doSomething(Eq(42)))
          .times(between(1, 2))
          .retireOnSaturation();

        mock.doSomething(42); // second expectation
        mock.doSomething(42); // retire second expectation
        mock.doSomething(42); // first expectation
        mock.doSomething(42); // first expectation oversaturates -> failure
      }

      checkOversaturatedFuncCalls(1);
    }

    /** @info(format)
     * @seperator
     * A @c InSequnce object can be used to expect that expectation will occur
     * in a specific order
     * @seperator
     */
    {
      auto sec = section("in sequence");

      /** @info(format)
       * @seperator
       * A call to 'doSomething(40)' will match the first and the last
       * expectation. Pro-test should use the first since the prerequisites
       * of the thrid expectation are not met yet. The test should not report
       * an violation of the expectations.
       * @seperator
       */
      {
        InSequence seq;

        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.doSomething(Eq(40)));
        expectCall(mocker.doSomething(Eq(41)));
        expectCall(mocker.doSomething(Eq(40)));

        mock.doSomething(40);
        mock.doSomething(41);
        mock.doSomething(40);
      }

      /** @info(format)
       * @seperator
       * A call to @c doSomething(40) will match the first and the last
       * expectation. But since the later will override the first and it has no
       * prerequisites itself it will be matched against the function call.
       * The test should report an missing call to the #1 and an unmet
       * prerequisites of #2.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        {
          InSequence seq;
          expectCall(mocker.doSomething(Eq(40))); // #1
          expectCall(mocker.doSomething(Eq(41))); // #2
        }
        expectCall(mocker.doSomething(Eq(40))) // #3
          .retireOnSaturation();

        mock.doSomething(40); // match #3
        mock.doSomething(41); // match #2 -> unmet prerequisites
        mock.doSomething(40); // match #1
      }

      checkUnmetPrerequisites(1);

      /** @info(format)
       * @seperator
       * When calling @c doSomething(41) #1 will be matched since #3 has unmet
       * prerequisites. when calling @c doSomething(40) #4 will be matched
       * since it has no prerequisites and the highest priority. Calling
       * @c doSomething(41) again will lead to a unexpected function call since
       * #3 has unmet prerequisites. @c doSomething(40) will then match #2
       * since #4 is retired. @c doSomething(41) will then match #3 since it
       * has no unmet prerequisites and the highest priority.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        {
          InSequence seq;
          expectCall(mocker.doSomething(Eq(41))); // #1
          expectCall(mocker.doSomething(Eq(40))); // #2
          expectCall(mocker.doSomething(Eq(41))); // #3
        }
        expectCall(mocker.doSomething(Eq(40))) // #4
          .retireOnSaturation();

        mock.doSomething(41); // match #1
        mock.doSomething(40); // match #4
        mock.doSomething(41); // unexpected function call
        mock.doSomething(40); // match #2
        mock.doSomething(41); // match #3
      }

      checkOversaturatedFuncCalls(1);

      /** @info(format)
       * @seperator
       * Expectation rely on expectation of other mock objects. In this test
       * case all prerequisites are met and therefore there will be no
       * corresponding error reports.
       * @seperator
       */
      {
        auto mocker1 = createMock<MyInterface>();
        MyInterface& mock1 = mocker1;

        auto mocker2 = createMock<MyInterface>();
        MyInterface& mock2 = mocker2;

        {
          InSequence seq;
          expectCall(mocker1.doSomething(Eq(41))); // #1
          expectCall(mocker2.doSomething(Eq(40))); // #2
          expectCall(mocker1.doSomething(Eq(41))); // #3
          expectCall(mocker2.doSomething(Eq(40))); // #4
        }

        mock1.doSomething(41); // match #1
        mock2.doSomething(40); // match #2
        mock1.doSomething(41); // match #3
        mock2.doSomething(40); // match #4
      }

      /** @info(format)
       * @seperator
       * Expectation rely on expectation of other mock objects. In this test
       * not all prerequisites are met and therefore there will be a
       * corresponding unmet prerequisite report.
       * @seperator
       */
      {
        auto mocker1 = createMock<MyInterface>();
        MyInterface& mock1 = mocker1;

        auto mocker2 = createMock<MyInterface>();
        MyInterface& mock2 = mocker2;

        {
          InSequence seq;
          expectCall(mocker1.doSomething(Eq(41))); // #1
          expectCall(mocker2.doSomething(Eq(40)))
            .retireOnSaturation(); // #2
          expectCall(mocker1.doSomething(Eq(41))); // #3
          expectCall(mocker2.doSomething(Eq(40))); // #4
        }

        mock1.doSomething(41); // match #1
        mock2.doSomething(40); // match #2
        mock2.doSomething(41); // unexpected call
        mock2.doSomething(40); // #4 unmet prerequisite
        // missing function call to #3
      }

      checkUnexpectedFuncCalls(1);
      checkUnmetPrerequisites(1);

      /** @info(format)
       * @seperator
       * Expectation can be in a sequence even if the corresponding mock object
       * does not exists any more. In this test case all prerequisites are met.
       * @seperator
       */
      {
        InSequence seq;
        {
          auto mocker1 = createMock<MyInterface>();
          MyInterface& mock1 = mocker1;

          expectCall(mocker1.doSomething(Eq(41))); // #1
          expectCall(mocker1.doSomething(Eq(41))); // #2
        
          mock1.doSomething(41); // match #1
          mock1.doSomething(41); // match #2
        }

        {
          auto mocker2 = createMock<MyInterface>();
          MyInterface& mock2 = mocker2;

          expectCall(mocker2.doSomething(Eq(40))); // #3
          expectCall(mocker2.doSomething(Eq(40))); // #4

          mock2.doSomething(40); // match #3
          mock2.doSomething(40); // match #4
        }
      }

      /** @info(format)
       * @seperator
       * Expectation can be in a sequence even if the corresponding mock object
       * does not exists any more. In this test case not all prerequisites are
       * met and therefore there should be unmet precondition report.
       * @seperator
       */
      {
        InSequence seq;
        {
          auto mocker1 = createMock<MyInterface>();
          MyInterface& mock1 = mocker1;

          expectCall(mocker1.doSomething(Eq(41))); // #1
          expectCall(mocker1.doSomething(Eq(41))); // #2
        
          mock1.doSomething(41); // match #1
          // mock1.doSomething(41);
        }

        {
          auto mocker2 = createMock<MyInterface>();
          MyInterface& mock2 = mocker2;

          expectCall(mocker2.doSomething(Eq(40))); // #3 -> unmet precondition
          expectCall(mocker2.doSomething(Eq(40))); // #4

          mock2.doSomething(40); // match #3
          mock2.doSomething(40); // match #4
        }
      }

      checkUnmetPrerequisites(1);
    }

    /** @info(format)
     * @seperator
     * A @c after can be used to expect that expectation will occur
     * in a specific order.
     * @seperator
     */
    {
      auto sec = section("after");

      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        auto exp = expectCall(mocker.doSomething(Eq(40)));
        expectCall(mocker.doSomething(Eq(41)))
          .after(exp);

        mock.doSomething(40);
        mock.doSomething(41);
      }

      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        auto exp = expectCall(mocker.doSomething(Eq(40)));
        expectCall(mocker.doSomething(Eq(41)))
          .after(exp);

        mock.doSomething(41); // # 2 -> unmet precondition
        mock.doSomething(40); // # 1
      }
    }

    checkUnmetPrerequisites(1);

    /** @info(format)
     * @seperator
     * A @c Sequence object can be used to expect that expectation will occur
     * in a specific order.
     * @seperator
     */
    {
      auto sec = section("sequence");

      {
        Sequence seq;

        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.doSomething(Eq(40)))
          .inSequence(seq);
        expectCall(mocker.doSomething(Eq(41)))
          .inSequence(seq);

        mock.doSomething(40); // #1
        mock.doSomething(41); // #2
      }

      {
        Sequence seq;

        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.doSomething(Eq(40)))
          .inSequence(seq);
        expectCall(mocker.doSomething(Eq(41)))
          .inSequence(seq);

        mock.doSomething(41); // #2 unmet precondition
        mock.doSomething(40); // #1
      }

      checkUnmetPrerequisites(1);
    }

    {
      auto sec = section("action");

      /** @info(format)
       * @seperator
       * Lambda functions can be implicit converted into a action.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        expectCall(mocker.setValue(Eq(40)))
          .willOnce([](auto a) { return a + 1; });

        auto ret = mock.setValue(40);

        assert(ret == 41);
      }
    }

    {
      auto sec = section("when");

      /** @info(format)
       * @seperator
       * When the when-clause is fulfilled and the arguments match the
       * expectation is a match for the given mock function call. The
       * mock function call can be seen in the log.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        auto value = Value(0);
        expectCall(mocker.setValue(Eq(40)))
          .willRepeatedly(Return(42))
          .when(value == 5); // #1

        value = 5;
        assert(mock.setValue(40) == 42); // match #1
      }

      /** @info(format)
       * @seperator
       * When the when-clause is not fulfilled, it will not be matched even if
       * the arguments match. In this case an unexpected function call will be
       * reported. Furthere more a reason given, why the best candidate does
       * not match the function call (I.e.: the not fulfilled when-clause).
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        auto value = Value(0);
        expectCall(mocker.setValue(Eq(40)))
          .willRepeatedly(Return(42))
          .when(value == 5); // #1

        mock.setValue(40); // report unexpected function call
      }

      checkOversaturatedFuncCalls(1);

      /** @info(format)
       * @seperator
       * When the when-clause is not fulfilled and the argument dose not match,
       * no reason will be given since there is no candidate which is close to
       * match. Only a unexpected function call will be reported.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        auto value = Value(0);
        expectCall(mocker.setValue(Eq(40)))
          .willRepeatedly(Return(42))
          .when(value == 5); // #1

        mock.setValue(41); // report unexpected function call
      }

      checkUnexpectedFuncCalls(1);

      /** @info(format)
       * @seperator
       * When the when-clause is not fulfilled, it will not be matched even if
       * the arguments match. In this case an unexpected function call will be
       * reported. Furthere more a reason given, why the best candidate does
       * not match the function call (I.e.: the not fulfilled when-clause). In
       * this specific case there are two candidates but the last one will be
       * reported as best candidate due the higher priority (the later the
       * higher the priority).
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        auto value = Value(0);

        expectCall(mocker.setValue(Eq(40)))
          .willRepeatedly(Return(42))
          .when(value == 5); // #1
        
        expectCall(mocker.setValue(Eq(40)))
          .willRepeatedly(Return(42))
          .when(value == 6); // #2

        // TODO (jreinking) bug, should use #2 not #1 for diagnostics

        mock.setValue(40); // report unexpected function call with #2
      }

      checkOversaturatedFuncCalls(1);
      checkMissingFuncCalls(2);

      /** @info(format)
       * @seperator
       * Normally, later expectations have a higher priority than earlier ones.
       * But this mechanism is deactivated in this case by the when clause.
       * Therefore #1 will matched first and then #2.
       * @seperator
       */
      {
        auto mocker = createMock<MyInterface>();
        MyInterface& mock = mocker;

        auto value = Value(0);
        expectCall(mocker.setValue(Eq(40)))
          .willRepeatedly(Return(42))
          .when(value == 5); // #1

        expectCall(mocker.setValue(Eq(40)))
          .willRepeatedly(Return(43))
          .when(value == 6); // #2

        value = 5;
        assertThat(mock.setValue(40), Eq(42)); // #1

        value = 6;
        assertThat(mock.setValue(40), Eq(43)); // #2
      }
    }
    // clang-format on
  }

  void
  finalize()
  {
  }

private:
  size_t mNumberOfUnexpectedFunctionCalls = 0;
  size_t mNumberOfMissingFunctionCalls = 0;
  size_t mNumberOfOversaturatedCalls = 0;
  size_t mNumberUnmetPrerequisitesCalls = 0;
};

int
main(int argc, char const* argv[])
{
  context.initialize();
  MyRunner runner(context);
  return context.run();
}
