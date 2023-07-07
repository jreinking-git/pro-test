
#include <my_class.h>
#include <functional>

template <typename F, typename T>
std::function<F>
getMemberFunction(T& obj, const char* name);

// the default argument is used so that T is present in the argument list
// to use full template specialiation.
template <typename T, typename F>
std::function<F>
getMemberFunction(const char* name, T* = nullptr);

template <typename R, typename T>
R&
getMemberAttr(T& obj, const char* name);

// the default argument is used so that T is present in the argument list
// to use full template specialiation.
template <typename T, typename R>
R&
getMemberAttr(const char* name, T* = nullptr);

#include <protest/api.h>
#include <protest/doc.h>
#include <protest/rtos.h>

using namespace protest;

// ---------------------------------------------------------------------------
/**
 * @test    demo-14
 * @author  Janosch Reinking
 * @version 1.0.0
 */
Context context;

class MyRunner : public Runner
{
public:
  MyRunner(Context& context) : Runner(context, "main")
  {
  }
 
  void 
  process()
  {
    MyClass obj;

    auto func1 = getMemberFunction<int(int)>(obj, "func");
    info() << func1(32);

    auto func2 = getMemberFunction<int(int)>(obj, "other");
    info() << func2(32);

    auto func3 = getMemberFunction<int(double)>(obj, "other");
    info() << func3(32.0);

    auto func4 = getMemberFunction<MyClass, uint32_t()>("value");
    info() << func4();

    auto& value1 = getMemberAttr<uint8_t>(obj, "mMember1");
    info() << (int) value1;

    auto& value2 = getMemberAttr<uint8_t>(obj, "mMember2");
    info() << (int) value2;

    auto& value3 = getMemberAttr<uint16_t>(obj, "mMember3");
    info() << (int) value3;
  
    auto& value4 = getMemberAttr<MyClass, uint32_t>("myvalue");
    info() << (int) value4;
  }

  void
  finalize()
  {
  }
 
private:
  protest::rtos::Mutex mMutex;
};

int
main(int argc, char const* argv[])
{
  context.initialize();
  MyRunner runner(context);
  return context.run();
}