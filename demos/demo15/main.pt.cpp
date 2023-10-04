#include <protest/api.h>
#include <protest/doc.h>
#include <protest/rtos.h>

#include <cxxabi.h>

using namespace protest;

// ---------------------------------------------------------------------------
/**
 * @test    demo-15
 * @author  Janosch Reinking
 * @version 1.0.0
 */
Context context;

// ---------------------------------------------------------------------------
static uint32_t my_value = 42;

void
function()
{
  static uint32_t variable = 43;
}

static int
staticFunction()
{
  static uint32_t variable = 43;
  return variable;
}

namespace demo
{

static uint32_t my_value = 44;

void
function()
{
  static uint32_t variable = 45;
}

} // namespace demo

void
function(int a)
{
  static uint32_t variable = 46;
}

void
function(std::string)
{
  static uint32_t variable = 47;
}

void
other()
{
  {
    static uint32_t variable = 48;
  }
  {
    static uint32_t variable = 49;
  }
}

void
another(int)
{
  static uint32_t variable = 50;
}

void
another(double)
{
  static uint32_t variable = 51;
}

// ---------------------------------------------------------------------------
static bool staticFunction1WasCalled = false;

static void staticFunction1()
{
  staticFunction1WasCalled = true;
}

static bool staticFunction2WasCalled = false;

static int staticFunction2()
{
  staticFunction2WasCalled = true;
  return 0;
}

static bool staticFunction3WasCalled = false;

static int staticFunction3(int, std::string)
{
  staticFunction3WasCalled = true;
  return 0;
}

static bool staticFunction4WasCalled = false;

static std::string staticFunction4(int, std::string)
{
  staticFunction4WasCalled = true;
  return "staticFunction4";
}

static bool staticFunction5WasCalled = false;

static std::string staticFunction5(std::string, std::string)
{
  staticFunction5WasCalled = true;
  return "staticFunction5";
}

static bool staticFunction6WasCalled = false;

static std::string staticFunction6(protest::Runner&, protest::Runner&)
{
  staticFunction6WasCalled = true;
  return "staticFunction6";
}

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
      /** @info(format)
       * @seperator
       * Get the static variable @c my_value.
       * @seperator
       */
      auto* ptr = protest::getStaticVariable<uint32_t>("my_value");
      assertThat(*ptr, Eq(42));
    }

    {
      /** @info(format)
       * @seperator
       * Get the static variable @c variable of function @c function().
       * @seperator
       */
      auto* ptr = protest::getStaticVariable<uint32_t, void()>("function", "variable");
      assertThat(*ptr, Eq(43));
    }

    {
      /** @info(format)
       * @seperator
       * Get the static variable @c variable of static function @c staticFunction().
       * @seperator
       */
      auto* ptr = protest::getStaticVariable<uint32_t, int()>("staticFunction", "variable");
      assertThat(*ptr, Eq(43));
    }

    {
      /** @info(format)
       * @seperator
       * Get the static variable @c my_value of namespace @c demo.
       * @seperator
       */
      // TODO
      // auto* ptr = protest::getStaticVariable<uint32_t>("demo::my_value");
      // assertThat(*ptr, Eq(44));
    }

    {
      /** @info(format)
       * @seperator
       * Get the static variable @c variable of function @c function() of
       * namespace @c demo.
       * @seperator
       */
      // TODO (jreinking)
      // auto* ptr =
      //     protest::getStaticVariable<uint32_t>("demo::function()::variable");
      // assertThat(*ptr, Eq(45));
    }

    {
      /** @info(format)
       * @seperator
       * Get the static variable @c variable of function @c function(int).
       * @seperator
       */
      auto* ptr =
          protest::getStaticVariable<uint32_t, void(int)>("function", "variable");
      assertThat(*ptr, Eq(46));
    }

    {
      /** @info(format)
       * @seperator
       * Get the static variable @c variable of function
       * @c function(std::string). 
       * @seperator
       */
      // clang-format: off
      auto* ptr = protest::getStaticVariable<uint32_t, void(std::string)>("function", "variable");
      // clang-format: on
      
      assertThat(*ptr, Eq(47));
    }

    {
      /** @info(format)
       * @seperator
       * Get the static variable @c variable of function @c other(). Since
       * the name exists twice an index has to be provided.
       * @seperator
       */
      auto* ptr1 = protest::getStaticVariable<uint32_t, void()>("other", "variable", 0);
      auto* ptr2 = protest::getStaticVariable<uint32_t, void()>("other", "variable", 1);

      assertThat(*ptr1, Eq(48));
      assertThat(*ptr2, Eq(49));
    }

    {
      /** @info(format)
       * @seperator
       * Get the static variable @c variable of function @c another(). The
       * signature identifies the function exactly.
       * @seperator
       */
      auto* ptr1 = protest::getStaticVariable<uint32_t, void(int)>("another", "variable");
      auto* ptr2 = protest::getStaticVariable<uint32_t, void(double)>("another", "variable");

      assertThat(*ptr1, Eq(50));
      assertThat(*ptr2, Eq(51));
    }

    {
      /** @info(format)
       * @seperator
       * Get the static function @c staticFunction*(). Tested with diffrent
       * signatures to test name mangling.
       * @seperator
       */

      {
        auto func = protest::getStaticFunction<void()>("staticFunction1");
        func();
        assertThat(staticFunction1WasCalled, IsTrue());
      }

      {
        auto func = protest::getStaticFunction<int()>("staticFunction2");
        auto ret = func();
        assertThat(staticFunction2WasCalled, IsTrue());
        assertThat(ret, Eq(0));
      }

      {
        auto func = protest::getStaticFunction<int(int, std::string)>("staticFunction3");
        auto ret = func(1, "");
        assertThat(staticFunction3WasCalled, IsTrue());
        assertThat(ret, Eq(0));
      }

      {
        auto func = protest::getStaticFunction<std::string(int, std::string)>("staticFunction4");
        auto ret = func(1, "");
        assertThat(staticFunction4WasCalled, IsTrue());
        assertThat(ret, Eq(std::string("staticFunction4")));
      }

      {
        auto func = protest::getStaticFunction<std::string(std::string, std::string)>("staticFunction5");
        auto ret = func("", "");
        assertThat(staticFunction5WasCalled, IsTrue());
        assertThat(ret, Eq(std::string("staticFunction5")));
      }

      {
        auto func = protest::getStaticFunction<std::string(protest::Runner&, protest::Runner&)>("staticFunction6");
        auto ret = func(*this, *this);
        assertThat(staticFunction6WasCalled, IsTrue());
        assertThat(ret, Eq(std::string("staticFunction6")));
      }

      // TODO (jreinking) test with namespaces
    }
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
  context.initialize(argc, argv);
  MyRunner runner(context);
  return context.run();
}
