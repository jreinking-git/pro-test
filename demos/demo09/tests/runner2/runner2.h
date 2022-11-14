#include <protest/api.h>
#include <protest/doc.h>

#pragma once

// ---------------------------------------------------------------------------
/**
 * @class Runner2
 */
class Runner2 : public protest::Runner
{
public:
  Runner2(protest::Context& context);

  void
  process();

private:
};
