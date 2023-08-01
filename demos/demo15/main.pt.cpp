#include <protest/api.h>
#include <protest/doc.h>
#include <protest/rtos.h>

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

void function()
{
  static uint32_t variable = 43;
}

static int staticFunction()
{
  static uint32_t variable = 43;
  return variable;
}

namespace demo
{

static uint32_t my_value = 44;

void function()
{
  static uint32_t variable = 45;
}

} // demo

void function(int a)
{
  static uint32_t variable = 46;
}

void function(std::string)
{
  static uint32_t variable = 47;
}

void other()
{
  {
    static uint32_t variable = 48;
  }
  {
    static uint32_t variable = 49;
  }
}

void another(int)
{
  static uint32_t variable = 50;
}

void another(double)
{
  static uint32_t variable = 51;
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
      auto* ptr = protest::getStaticVariable<uint32_t>("function()::variable");
      assertThat(*ptr, Eq(43));
    }

    {
      /** @info(format)
       * @seperator
       * Get the static variable @c variable of static function @c function().
       * @seperator
       */
      auto* ptr = protest::getStaticVariable<uint32_t>("staticFunction()::variable");
      assertThat(*ptr, Eq(43));
    }

    {
      /** @info(format)
       * @seperator
       * Get the static variable @c my_value of namespace @c demo.
       * @seperator
       */
      auto* ptr = protest::getStaticVariable<uint32_t>("demo::my_value");
      assertThat(*ptr, Eq(44));
    }

    {
      /** @info(format)
       * @seperator
       * Get the static variable @c variable of function @c function() of
       * namespace @c demo.
       * @seperator
       */
      auto* ptr = protest::getStaticVariable<uint32_t>("demo::function()::variable");
      assertThat(*ptr, Eq(45));
    }

    {
      /** @info(format)
       * @seperator
       * Get the static variable @c variable of function @c function(int).
       * @seperator
       */
      auto* ptr = protest::getStaticVariable<uint32_t>("function(int)::variable");
      assertThat(*ptr, Eq(46));
    }

    {
      /** @info(format)
       * @seperator
       * Get the static variable @c variable of function
       * @c function(std::string). 
       * @seperator
       */
      auto* ptr = protest::getStaticVariable<uint32_t>("function(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >)::variable");
      assertThat(*ptr, Eq(47));
    }

    {
      /** @info(format)
       * @seperator
       * Get the static variable @c variable of function
       * @c function(std::string) using regular expression.
       * @seperator
       */
      auto* ptr = protest::getStaticVariable<uint32_t>("function(std::.*string.*)::variable");
      assertThat(*ptr, Eq(47));
    }

    {
      /** @info(format)
       * @seperator
       * Get the static variable @c variable of function @c other(). Since
       * the name exists twice an index has to be provided.
       * @seperator
       */
      auto* ptr1 = protest::getStaticVariable<uint32_t>("other()::variable", 0);
      auto* ptr2 = protest::getStaticVariable<uint32_t>("other()::variable", 1);

      assertThat(*ptr1, Eq(48));
      assertThat(*ptr2, Eq(49));
    }

    {
      /** @info(format)
       * @seperator
       * Get the static variable @c variable of function @c other(). Since
       * the name exists twice an index has to be provided.
       * @seperator
       */
      // TODO (jreinking) is the order random? sort the list
      auto* ptr1 = protest::getStaticVariable<uint32_t>("another(.*)::variable", 1);
      auto* ptr2 = protest::getStaticVariable<uint32_t>("another(.*)::variable", 0);

      assertThat(*ptr1, Eq(50));
      assertThat(*ptr2, Eq(51));
    }

    {
      /** @info(format)
       * @seperator
       * Get the static function @c staticFunction()
       * @seperator
       */
      auto func = protest::getStaticFunction<int()>("staticFunction()");
      auto ret = func();

      assertThat(ret, Eq(43));
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