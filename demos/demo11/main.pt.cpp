#include <protest/api.h>
#include <protest/doc.h>

using namespace protest;
using namespace protest::time;
using namespace protest::matcher;

// ---------------------------------------------------------------------------
/**
 * @test    demo-11
 * @author  Janosch Reinking
 * @version 1.0.0
 */
Context context;

struct TestStruct
{
  bool
  operator==(const TestStruct& other)
  {
    return a == other.a;
  }

  bool
  operator!=(const TestStruct& other)
  {
    return a != other.a;
  }

  bool
  operator>(const TestStruct& other)
  {
    return a > other.a;
  }

  bool
  operator<(const TestStruct& other)
  {
    return a < other.a;
  }

  bool
  operator>=(const TestStruct& other)
  {
    return a >= other.a;
  }

  bool
  operator<=(const TestStruct& other)
  {
    return a <= other.a;
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
    {
      auto sec = section("binary comparison - primitive types");

      bool ret =
          assertThat(static_cast<uint8_t>(42), Eq(static_cast<uint8_t>(42)));
      assert(ret);
      ret = assertThat(static_cast<uint8_t>(42), Eq(static_cast<uint8_t>(43)));
      assert(!ret);
      ret = assertThat(static_cast<uint8_t>(42), Eq(static_cast<float>(42)));
      assert(ret);
      ret = assertThat(static_cast<uint8_t>(42), Eq(static_cast<float>(43)));
      assert(!ret);
      ret = assertThat(static_cast<uint8_t>(42), Eq(static_cast<int8_t>(42)));
      assert(ret);
      ret = assertThat(static_cast<uint8_t>(42), Eq(static_cast<int8_t>(43)));
      assert(!ret);

      int a = 42;
      ret = assertThat(a, Eq(42));

      ret = assertThat(42, Eq(42));
      assert(ret);
      ret = assertThat(42, Eq(43));
      assert(!ret);

      ret = assertThat(42, Ne(42));
      assert(!ret);
      ret = assertThat(42, Ne(43));
      assert(ret);

      ret = assertThat(42, Gt(41));
      assert(ret);
      ret = assertThat(42, Gt(42));
      assert(!ret);

      ret = assertThat(42, Lt(43));
      assert(ret);
      ret = assertThat(42, Lt(42));
      assert(!ret);

      ret = assertThat(42, Le(43));
      assert(ret);
      ret = assertThat(42, Le(42));
      assert(ret);
      ret = assertThat(42, Le(41));
      assert(!ret);

      ret = assertThat(42, Ge(41));
      assert(ret);
      ret = assertThat(42, Ge(42));
      assert(ret);
      ret = assertThat(42, Ge(43));
      assert(!ret);
    }

    {
      auto sec = section("binary comparison - record types");

      {
        TestStruct a;
        a.a = 42;
        TestStruct b;
        b.a = 42;
        bool ret = assertThat(a, Eq(b));
        assert(ret);
      }

      {
        TestStruct a;
        a.a = 42;
        TestStruct b;
        b.a = 43;
        bool ret = assertThat(a, Eq(b));
        assert(!ret);
      }

      {
        TestStruct a;
        a.a = 42;
        TestStruct b;
        b.a = 42;
        bool ret = assertThat(a, Ne(b));
        assert(!ret);
      }

      {
        TestStruct a;
        a.a = 42;
        TestStruct b;
        b.a = 43;
        bool ret = assertThat(a, Ne(b));
        assert(ret);
      }

      {
        TestStruct a;
        a.a = 42;
        TestStruct b;
        b.a = 41;
        bool ret = assertThat(a, Gt(b));
        assert(ret);
      }

      {
        TestStruct a;
        a.a = 42;
        TestStruct b;
        b.a = 42;
        bool ret = assertThat(a, Gt(b));
        assert(!ret);
      }

      {
        TestStruct a;
        a.a = 42;
        TestStruct b;
        b.a = 43;
        bool ret = assertThat(a, Lt(b));
        assert(ret);
      }

      {
        TestStruct a;
        a.a = 42;
        TestStruct b;
        b.a = 42;
        bool ret = assertThat(a, Lt(b));
        assert(!ret);
      }

      {
        TestStruct a;
        a.a = 42;
        TestStruct b;
        b.a = 43;
        bool ret = assertThat(a, Le(b));
        assert(ret);
      }

      {
        TestStruct a;
        a.a = 42;
        TestStruct b;
        b.a = 42;
        bool ret = assertThat(a, Le(b));
        assert(ret);
      }

      {
        TestStruct a;
        a.a = 42;
        TestStruct b;
        b.a = 41;
        bool ret = assertThat(a, Le(b));
        assert(!ret);
      }

      {
        TestStruct a;
        a.a = 42;
        TestStruct b;
        b.a = 41;
        bool ret = assertThat(a, Ge(b));
        assert(ret);
      }

      {
        TestStruct a;
        a.a = 42;
        TestStruct b;
        b.a = 42;
        bool ret = assertThat(a, Ge(b));
        assert(ret);
      }

      {
        TestStruct a;
        a.a = 42;
        TestStruct b;
        b.a = 43;
        bool ret = assertThat(a, Ge(b));
        assert(!ret);
      }
    }

    {
      auto sec = section("binary comparison - pointer");

      {
        int a = 42;
        int b = 42;
        int* ptr = &a;
        bool ret = assertThat(ptr, Eq(&a));
        assert(ret);
        ret = assertThat(ptr, Eq(&b));
        assert(!ret);
      }

      {
        int a = 42;
        int b = 42;
        int* aptr = &a;
        int* bptr = &b;
        int** adptr = &aptr;
        int** bdptr = &bptr;

        bool ret = assertThat(adptr, Eq(&aptr));
        assert(ret);
        ret = assertThat(adptr, Eq(bdptr));
        assert(!ret);
      }
    }

    {
      auto sec = section("pointer null check");
      {
        int a = 42;
        int* aptr = &a;

        bool ret = assertThat(aptr, NotNull());
        assert(ret);
        ret = assertThat(nullptr, NotNull());
        assert(!ret);
        ret = assertThat(nullptr, Not(NotNull()));
        assert(ret);
        ret = assertThat(nullptr, IsNull());
        assert(ret);
        ret = assertThat(aptr, IsNull());
        assert(!ret);
      }
    }

    {
      auto sec = section("reference check");

      int a = 34;
      int b = 42;
      bool ret = assertThat(a, Ref(a));
      assert(ret);
      ret = assertThat(a, Ref(b));
      assert(!ret);
      ret = assertThat(a, Not(Ref(a)));
      assert(!ret);
      ret = assertThat(a, Not(Ref(b)));
      assert(ret);
      ret = assertThat(a, Not(Not(Ref(b))));
      assert(!ret);
    }

    {
      auto sec = section("boolean check");

      bool ret = false;
      ret = assertThat(true, IsTrue());
      assert(ret);
      ret = assertThat(false, IsTrue());
      assert(!ret);
      ret = assertThat(true, Not(IsTrue()));
      assert(!ret);
      ret = assertThat(false, Not(IsTrue()));
      assert(ret);
      ret = assertThat(true, IsFalse());
      assert(!ret);
      ret = assertThat(false, IsFalse());
      assert(ret);
      ret = assertThat(true, Not(IsFalse()));
      assert(ret);
      ret = assertThat(false, Not(IsFalse()));
      assert(!ret);
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
  context.initialize();
  MyRunner runner(context);
  return context.run();
}
