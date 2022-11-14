#include <protest/api.h>
#include <protest/doc.h>

using namespace protest;
using namespace protest::time;

// ---------------------------------------------------------------------------
/**
 * @test    demo-08
 * @author  Janosch Reinking
 * @version 1.0.0
 */
Context context;

// clang-format off
class MyRunner : public Runner
{
public:
  MyRunner(Context& context) : Runner(context, "main")
  {
  }

  void
  initialize()
  {
  }

  void
  process()
  {
    {
      auto sec = section("basic print block comment");

      /** @info
       * Should be printed
       */

      /** @info 
       * Should not be printed
       */

      /**
       * @info
       * Should not be printed
       */
    }

    {
      auto sec = section("auto format");

      /** @info(format)
       * This is a looooooooooong sentence. It should be splitted into multiple lines. Whereas each line should not have more then 80 chars.
       */

      /** @info(format)
       * These
       * words
       * should
       * be
       * printed
       * as
       * a
       * single
       * line
       */
    }

    {
      auto sec = section("seperator");

      /** @info(format)
       * @seperator
       * This is a looooooooooong sentence. It should be splitted into multiple lines. Whereas each line should not have more then 80 chars.
       */

      /** @info(format)
       * @seperator
       * These
       * words
       * should
       * be
       * printed
       * as
       * a
       * single
       * line
       */
    }
  }

  void
  finalize()
  {
  }

private:
};
// clang-format on

int
main(int argc, char const* argv[])
{
  context.initialize();
  MyRunner runner(context);
  return context.run();
}
