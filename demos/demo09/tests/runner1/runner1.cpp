#include "runner1.h"

#include "module_a.h"
#include "module_b.h"

#include <protest/api.h>
#include <protest/doc.h>

using namespace protest;
using namespace protest::time;

Runner1::Runner1(Context& context) : Runner(context, "tsk1")
{
}

void
Runner1::process()
{
  funcA();
  funcB();
}
