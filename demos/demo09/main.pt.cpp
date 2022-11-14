
#include "runner1.h"
#include "runner2.h"
#include "runner3.h"

// ---------------------------------------------------------------------------
/**
 * @test    demo-09
 * @author  Janosch Reinking
 * @version 1.0.0
 */
protest::Context context;

#ifndef FLAG_A
static_assert(false);
#endif

#ifndef FLAG_B
static_assert(false);
#endif

#ifndef COMPILED_WITH_MODULE_B
static_assert(false);
#endif

int
main(int argc, char const* argv[])
{
  context.initialize();
  Runner1 runner1(context);
  Runner2 runner2(context);
  Runner3 runner3(context);
  return context.run();
}
