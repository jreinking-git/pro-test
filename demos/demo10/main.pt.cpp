#include <protest/api.h>
#include <protest/doc.h>

using namespace protest;
using namespace protest::time;

// ---------------------------------------------------------------------------
/**
 * @test    demo-10
 * @author  Janosch Reinking
 * @version 1.0.0
 */
Context context;

struct Result
{
  int mValue;
};

Signal<Result> structSignal(context);

// ---------------------------------------------------------------------------
template <typename T>
void
func(T param)
{
  // param is bound to 'dependent type'
  // -> can not directly be resolved by looking at tha AST since the type of
  // param is not known unless the function gets actually called. Needs extra
  // effort from the protest/protest-compiler.
  assertThat(param == 42);

  // no 'dependent types' involved
  // -> can be directly resolved as a call to protest::assertThat
  assertThat(true);
}

class AClass
{
public:
  template <typename T>
  void
  func(T param)
  {
    // param is bound to 'dependent type'
    // -> can not directly be resolved by looking at tha AST since the type of
    // param is not known unless the function gets actually called. Needs extra
    // effort from the protest/protest-compiler.
    assertThat(param == 42);

    // no 'dependent types' involved
    // -> can be directly resolved as a call to protest::assertThat
    assertThat(true);
  }

  void
  func()
  {
    // no 'dependent types' involved
    // -> can be directly resolved as a call to protest::assertThat
    assertThat(true);
  }
};

template <typename T>
class BClass
{
public:
  void
  func(T param)
  {
    // param is bound to 'dependent type'
    // -> can not directly be resolved by looking at tha AST since the type of
    // param is not known unless the function gets actually called. Needs extra
    // effort from the protest/protest-compiler.
    assertThat(param == 42);

    // no 'dependent types' involved
    // -> can be directly resolved as a call to protest::assertThat
    assertThat(true);
  }

  void
  func()
  {
    // no 'dependent types' involved
    // -> can be directly resolved as a call to protest::assertThat
    assertThat(true);
  }
};

template <typename T>
class CClass
{
public:
  CClass(T func) : mFunc(func)
  {
  }

  void
  func()
  {
    mFunc(42);
  }

  template <typename U>
  void
  func(U value)
  {
    mFunc(value);
  }

  T mFunc;
};

class MyRunner : public Runner
{
public:
  MyRunner(Context& context) : Runner(context, "main")
  {
  }

  void
  initialize()
  {
    mResultPort = createPort(structSignal);
  }

  void
  process()
  {
    int anyValue = 42;

    {
      auto sec = section("template");

      func(anyValue);

      AClass clzA;
      clzA.func(anyValue);
      clzA.func();

      BClass<int> clzB;
      clzB.func(anyValue);
      clzB.func();
    }

    {
      auto sec = section("lambda / generic lambda");

      // classic lambda
      auto fun1 = [&]()
      {
        // no 'dependent types' involved
        // -> can be directly resolved as a call to protest::assertThat
        assertThat(true);
      };

      fun1();

      // classic lambda with param
      auto fun2 = [&](int param)
      {
        // no 'dependent types' involved
        // -> can be directly resolved as a call to protest::assertThat
        assertThat(param == anyValue);
      };

      fun2(anyValue);

      // generic lambda with param
      auto fun3 = [&](auto param)
      {
        // param is bound to 'dependent type'
        // -> can not directly be resolved by looking at tha AST since the type
        // of param is not known unless the function gets actually called.
        // Needs extra effort from the protest/protest-compiler.
        assertThat(param == anyValue);
      };

      fun3(anyValue);
      {
        // clang-format off
        CClass clzC([&](int param)
        {
          // no 'dependent types' involved
          // -> can be directly resolved as a call to protest::assertThat
          assertThat(param == anyValue);
        });
        // clang-format on

        clzC.func();
        clzC.func<int>(anyValue);
        clzC.func<int&>(anyValue);
      }
      {
        // clang-format off
        CClass clzC([&](auto param)
        {
          // param is bound to 'dependent type'
          // -> can not directly be resolved by looking at tha AST since the type
          // of param is not known unless the function gets actually called.
          // Needs extra effort from the protest/protest-compiler.
          assertThat(param == anyValue);
        });
        // clang-format on

        clzC.func();
        clzC.func<int>(anyValue);
        clzC.func<int&>(anyValue);
      }

      // clang-format off
      mResultPort.addListener([]()
      {
        // no 'dependent types' involved
        // -> can be directly resolved as a call to protest::assertThat
        assertThat(true);
      });

      mResultPort.addListener([](Result param)
      {
        // no 'dependent types' involved
        // -> can be directly resolved as a call to protest::assertThat
        assertThat(param.mValue == 42);
      });

      mResultPort.addListener([&](auto param)
      {
        // param is bound to 'dependent type'
        // -> can not directly be resolved by looking at tha AST since the type
        // of param is not known unless the function gets actually called.
        // Needs extra effort from the protest/protest-compiler.
        assertThat(param.mValue == 42);
      });
      // clang-format on

      Result res = {anyValue};
      structSignal.push(res);
    }
  }

  void
  finalize()
  {
  }

private:
  SamplePort<Result> mResultPort;
};

int
main(int argc, char const* argv[])
{
  context.initialize(argc, argv);
  MyRunner runner(context);
  return context.run();
}
