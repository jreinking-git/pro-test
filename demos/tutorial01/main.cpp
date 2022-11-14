#include "simple_thread.h"

int
main(int argc, char const* argv[])
{
  SimpleThread thd;
  thd.start();
  thd.waitForThread();
  return 0;
}
