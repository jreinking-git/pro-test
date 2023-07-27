#include <protest/api.h>
#include <protest/doc.h>

using namespace protest;
using namespace protest::time;

// ---------------------------------------------------------------------------
/**
 * @test    demo-01
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
      auto sec = section("basic assertion");

      /** @info
       * This assertion should not fail.
       */
      assertThat(true);

      /** @info
       * @seperator
       * This assertion should also not fail. Furthermore it should enter the if
       * statment.
       * @seperator
       */
      if (assertThat(true))
      {
        info() << "I am here!\n";
      }
      else
      {
        assert(false);
      }

      /** @info
       * @seperator
       * This assertion should not fail. But it should also print a message next to it.
       * @seperator
       */
      assertThat(true) << "Hello, World";

      /** @info
       * @seperator
       * This assertion should not fail, enter the if statment and print a message to
       * the log.
       * @seperator
       */
      if (assertThat(true) << "Hello, World")
      {
        info() << "I am here!\n";
      }
      else
      {
        assert(false);
      }

      /** @info
       * @seperator
       * This assertion should fail.
       * @seperator
       */
      assertThat(false);

      /** @info
       * @seperator
       * This assertion should fail, should not enter the if statment and print a
       * message to the log.
       * @seperator
       */
      if (assertThat(false) << "Hello, World")
      {
        assert(false);
      }
      else
      {
      }
    }

    {
      auto sec = section("formatting and escaping for assertion");
      /** @info
       * This assertion contains quotes. This should be handled properly by the
       * protest-compiler.
       * @seperator
       */
      assertThat(std::string("true") == "true");

      /** @info
       * @seperator
       * This assertion spreads over multiple lines. This should be handled
       * properly by the protest-compiler.
       * @seperator
       */
      // clang-format off
      assertThat(std::string("true") ==
                "true");
      // clang-format on

      /** @info
       * @seperator
       * This assertion has an inner assertion. The protest compiler should handle this
       * case properly. I.e.: it should process the inner @c assertThat first then the
       * outer.
       * @seperator
       */
      // clang-format off
      assertThat([]() {
        assertThat(true);
        return true;
      }());
      // clang-format on

      /** @info
       * @seperator
       * Not executed @c assertThat should be reported
       * @seperator
       */
      if (false)
      {
        assertThat(false);
      }
    }

    {
      auto sec = section("basic checks");
      /** @info
       * This check should not fail. Which means that this check will not print anything
       * to the log.
       */
      checkThat(true);

      /** @info
       * @seperator
       * This check should also not fail. Furthermore it should enter the if
       * statment.
       * @seperator
       */
      if (checkThat(true))
      {
        info() << "I am here!\n";
      }
      else
      {
        assert(false);
      }

      /** @info
       * @seperator
       * This check should not fail. It should also not print a log message.
       * @seperator
       */
      checkThat(true) << "Hello, World";

      /** @info
       * @seperator
       * This check should not fail, enter the if statment. But a message should not
       * appear in the debug log.
       * @seperator
       */
      if (checkThat(true) << "Hello, World")
      {
        info() << "I am here!\n";
      }
      else
      {
        assert(false);
      }

      /** @info
       * @seperator
       * This check should fail.
       * @seperator
       */
      checkThat(false);

      /** @info
       * @seperator
       * This check should fail, should not enter the if statment and print a
       * message to the log.
       * @seperator
       */
      if (checkThat(false) << "Hello, World")
      {
        assert(false);
      }
      else
      {
      }
    }

    {
      auto sec = section("formatting and escaping for checks");

      /** @info
       * This check contains quotes. This should be handled properly by the
       * protest-compiler.
       */
      checkThat(std::string("true") == "false");

      /** @info
       * @seperator
       * This check spreads over multiple lines. This should be handled
       * properly by the protest-compiler.
       * @seperator
       */
      // clang-format off
      checkThat(std::string("true") ==
                "false");
      // clang-format on

      /** @info
       * @seperator
       * This check has an inner check. The protest compiler should handle this
       * case properly. I.e.: it should process the inner @c checkThat first the the
       * outer.
       * @seperator
       */
      // clang-format off
      checkThat([]() {
        checkThat(false);
        return false;
      }());
      // clang-format on
    }
  }

  void
  func()
  {
    assertThat(true);
  }

private:
};

int
main(int argc, char const* argv[])
{
  context.initialize(argc, argv);
  MyRunner runner(context);
  return context.run();
}
