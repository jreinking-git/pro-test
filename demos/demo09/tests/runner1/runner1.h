#include <protest/api.h>
#include <protest/doc.h>

#pragma once

// ---------------------------------------------------------------------------
/**
 * @class Runner1
 */
class Runner1 : public protest::Runner
{
public:
  Runner1(protest::Context& context);

  void
  process();

private:
};
