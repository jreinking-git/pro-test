#include <protest/api.h>
#include <protest/doc.h>

using namespace protest;
using namespace protest::time;
using namespace protest::matcher;

// ---------------------------------------------------------------------------
/**
 * @test    demo-12
 * @author  Janosch Reinking
 * @version 1.0.0
 */
Context context;

struct MyStruct
{
  bool
  operator==(const MyStruct& other)
  {
    return a == other.a;
  }

  int a;
};

enum class Asdf
{
  test1,
  test2
};

class MyRunner : public Runner
{
public:
  MyRunner(Context& context) : Runner(context, "main")
  {
  }

  void
  process()
  {
    assertThat(42, Eq(13));

    MyStruct a;
    a.a = 34;
    MyStruct b;
    b.a = 42;

    assertThat(a, Eq(b));

    assertThat(Asdf::test1, Eq(Asdf::test2));
    assertThat(std::string("Hello, world!"), Eq(std::string("Hello, World!")));
    assertThat(std::string("Hello, World!"), Eq(std::string("Hello, World!")));
    assertThat("Hello, World!", Eq(std::string("Hello, World!")));

    assertThat(42, Le(42));
    assertThat(42, Lt(42));
    assertThat(44, Gt(42));

    assertThat(44, Eq(44));
    assertThat(44, Not(Eq(44)));
    assertThat(44, Not(Not(Eq(44))));
  }

  void
  finalize()
  {
  }

private:
};

int
main(int argc, char const* argv[])
{
  context.initialize();
  MyRunner runner(context);
  return context.run();
}
