
#include <my_class.h>

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

// ---------------------------------------------------------------------------
class MyClass3
{
private:
  uint8_t mMember1 = 48;
};

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
      auto sec = section("private method");

      MyClass obj;

      /** @info(format)
       * @seperator
       * Get the private method @c func and call it.
       * @seperator
       */
      {
        auto func = getMemberFunction<void()>(obj, "func");
        func();
        assertThat(true, IsTrue());
      }

      /** @info(format)
       * @seperator
       * Get the private method @c func and call it. It has a return value and
       * a parameter.
       * @seperator
       */
      {
        auto func = getMemberFunction<int(int)>(obj, "func");
        auto value = func(32);
        assertThat(value, Eq(44));
      }

      /** @info(format)
       * @seperator
       * Get the private method @c other and call it. It has the same signature
       * as @c func.
       * @seperator
       */
      {
        auto func = getMemberFunction<int(int)>(obj, "other");
        auto value = func(32);
        assertThat(value, Eq(45));
      }

      /** @info(format)
       * @seperator
       * Get the private method @c other and call it. @c other is a overloaded
       * method. @c getMemberFunction should return the function which takes
       * an double as first parameter.
       * @seperator
       */
      {
        auto func = getMemberFunction<int(double)>(obj, "other");
        auto value = func(32.0);
        assertThat(value, Eq(46));
      }

      /** @info(format)
       * @seperator
       * Get private static function @c value and call it.
       * @seperator
       */
      {
        auto func = getMemberFunction<MyClass, uint32_t()>("value");
        auto value = func();
        assertThat(value, Eq(40));
      }
    }

    {
      auto sec = section("private data member");

      MyClass obj;

      {
        /** @info(format)
         * @seperator
         * Get private member @c mMember1. 
         * @seperator
         */
        auto& value = getMemberAttr<uint8_t>(obj, "mMember1");
        assertThat(value, Eq(41));
      }

      {
        /** @info(format)
         * @seperator
         * Get private member @c mMember2 which has the same type as
         * @c mMember1.
         * @seperator
         */
        auto& value = getMemberAttr<uint8_t>(obj, "mMember2");
        assertThat(value, Eq(42));
      }

      {
        /** @info(format)
         * @seperator
         * Get private member @c mMember3 which has a diffrent type then
         * @c mMember1 and @c mMember2.
         * @seperator
         */
        auto& value = getMemberAttr<uint16_t>(obj, "mMember3");
        assertThat(value, Eq(43));
      }

      {
        /** @info(format)
         * @seperator
         * Get private member @c mMember4 which is a reference to 
         * @c mMember1.
         * @seperator
         */
        auto& value = getMemberAttr<uint8_t>(obj, "mMember4");
        assertThat(value, Eq(41));
      }

      {
        /** @info(format)
         * @seperator
         * Get private static const data @c mMember5.
         * @seperator
         */
        auto& value = getMemberAttr<const uint8_t>(obj, "mMember5");
        assertThat(value, Eq(44));
      }

      {
        /** @info(format)
         * @seperator
         * Get private data @c mMember6.
         * @seperator
         */
        auto& value = getMemberAttr<MyStruct>(obj, "mMember6");
        assertThat(value.mValue, Eq(45));
      }

      {
        /** @info(format)
         * @seperator
         * Get private data @c mMember7.
         * @seperator
         */
        auto& value = getMemberAttr<MyClass::MyStruct2>(obj, "mMember7");
        assertThat(value.mValue, Eq(46));
      }

      {
        /** @info(format)
         * @seperator
         * Get private data @c mMode.
         * @seperator
         */
        auto& value = getMemberAttr<MyClass::Mode>(obj, "mMode");
        assertThat(value, Eq(MyClass::Mode::ground));
      }

      {
        /** @info(format)
         * @seperator
         * Get private static data @c myvalue.
         * @seperator
         */
        auto& value = getMemberAttr<MyClass, uint32_t>("myvalue");
        assertThat(value, Eq(42));
      }
    }

    {
      auto sec = section("class in namespace");

      test::MyClass obj;

      {
        /** @info(format)
         * @seperator
         * Get private static data @c myvalue.
         * @seperator
         */
        auto& value = getMemberAttr<test::MyClass, uint32_t>("myvalue");
        assertThat(value, Eq(42));
      }

     /** @info(format)
       * @seperator
       * Get private static function @c value and call it.
       * @seperator
       */
      {
        auto func = getMemberFunction<test::MyClass, uint32_t()>("value");
        auto value = func();
        assertThat(value, Eq(40));
      }

      {
        /** @info(format)
         * @seperator
         * Get private member @c mMember1. 
         * @seperator
         */
        auto& value = getMemberAttr<uint8_t>(obj, "mMember1");
        assertThat(value, Eq(41));
      }

      /** @info(format)
       * @seperator
       * Get the private method @c func and call it. It has a return value and
       * a parameter.
       * @seperator
       */
      {
        auto func = getMemberFunction<int(int)>(obj, "func");
        auto value = func(32);
        assertThat(value, Eq(44));
      }
    }

    {
      auto sec = section("class in two namespaces");

      test::test2::MyClass obj;

      {
        /** @info(format)
         * @seperator
         * Get private static data @c myvalue.
         * @seperator
         */
        auto& value = getMemberAttr<test::test2::MyClass, uint32_t>("myvalue");
        assertThat(value, Eq(42));
      }

     /** @info(format)
       * @seperator
       * Get private static function @c value and call it.
       * @seperator
       */
      {
        auto func = getMemberFunction<test::test2::MyClass, uint32_t()>("value");
        auto value = func();
        assertThat(value, Eq(40));
      }

      {
        /** @info(format)
         * @seperator
         * Get private member @c mMember1. 
         * @seperator
         */
        auto& value = getMemberAttr<uint8_t>(obj, "mMember1");
        assertThat(value, Eq(41));
      }

      /** @info(format)
       * @seperator
       * Get the private method @c func and call it. It has a return value and
       * a parameter.
       * @seperator
       */
      {
        auto func = getMemberFunction<int(int)>(obj, "func");
        auto value = func(32);
        assertThat(value, Eq(44));
      }
    }

    {
      auto sec = section("definition in pt script");

      MyClass3 obj;

      {
        /** @info(format)
         * @seperator
         * Get private member @c mMember1 of class which is defined in this
         * pt script.
         * @seperator
         */
        auto& value = getMemberAttr<uint8_t>(obj, "mMember1");
        assertThat(value, Eq(48));
      }
    }

    {
      auto sec = section("private type");

      MyClass obj;

      {
        struct MyType
        {
          uint8_t mValue1;
          uint16_t mValue2;
        };
        
        /** @info(format)
         * @seperator
         * The member @c mMember8 has a private type. This type cannot be used
         * in the pt script without getting a invalid access error from the
         * IDE. Therefore a @c MyType is used. This struct has the same layout
         * as the private type of the member.
         * @seperator
         */
        auto* value = const_cast<MyType*>(getMemberAttrRaw<MyType>(obj, "mMember8"));
        assertThat(value->mValue1, Eq(49));
        assertThat(value->mValue2, Eq(50));
      }
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