#include "any_class.h"

#include <protest/api.h>
#include <protest/doc.h>

#include <memory>
#include <vector>
#include <set>

using namespace protest;
using namespace protest::time;

// ---------------------------------------------------------------------------
/**
 * @test    demo-07
 * @author  Janosch Reinking
 * @version 1.0.0
 */
Context context;

// ---------------------------------------------------------------------------
enum class TestEnum
{
  value1,
  value2
};

// ---------------------------------------------------------------------------
namespace test
{

enum class TestEnum
{
  test1,
  test2
};

}

// ---------------------------------------------------------------------------
namespace test
{

struct Parent
{
  enum class TestEnum
  {
    test1,
    test2
  };
};

} // namespace test

// ---------------------------------------------------------------------------
struct EmptyStruct
{
};

// ---------------------------------------------------------------------------
enum class TestStructEnum
{
  mode1,
  mode2
};

struct TestStruct
{
  uint8_t value1;
  uint16_t value2;
  TestStructEnum mode;
};

// ---------------------------------------------------------------------------
enum class CustomPrinterEnum
{
  value1,
  value2
};

namespace protest
{

namespace log
{

protest::log::UniversalStream&
toStringCustom(protest::log::UniversalStream& out,
               const CustomPrinterEnum& value)
{
  if (value == CustomPrinterEnum::value1)
  {
    out.getPlain() << "{ custom value1 }";
  }
  else if (value == CustomPrinterEnum::value2)
  {
    out.getPlain() << "{ custom value2 }";
  }
  return out;
}

} // namespace log

} // namespace protest

struct CustomPrinterStruct
{
  CustomPrinterEnum value;
};

// ---------------------------------------------------------------------------
struct NestedStruct
{

  uint8_t mValue;
};

class EnclosingStruct
{
public:
  EnclosingStruct(uint8_t p1, uint64_t p2, uint8_t p3) :
    mValue1(p1),
    mValue2(p2)
  {
    mValue3.mValue = p3;
  }

private:
  uint8_t mValue1;
  uint64_t mValue2;
  NestedStruct mValue3;
};

// ---------------------------------------------------------------------------
struct PtrStruct
{
  uint8_t* value;
};

// ---------------------------------------------------------------------------
struct Pointee
{
  uint8_t value;
};

struct PtrStruct2
{
  Pointee* ptr;
};

// ---------------------------------------------------------------------------
struct StructDepth1
{
  uint8_t value;
};

struct StructDepth2
{
  StructDepth1 value;
};

struct StructDepth3
{
  StructDepth2 value;
};

// ---------------------------------------------------------------------------
struct RecursionStruct
{
  uint8_t value;
  RecursionStruct* self;
};

// ---------------------------------------------------------------------------
struct SetStruct
{
  std::set<int> values;
};

// ---------------------------------------------------------------------------
struct Dummy2
{
  uint8_t value;
};

struct SharedPtrStruct
{
  std::shared_ptr<Dummy2> value;
};

namespace protest
{

namespace log
{

template <typename T>
protest::log::UniversalStream&
toStringCustom(protest::log::UniversalStream& stream,
               const std::shared_ptr<T>& value)
{
  if (value)
  {
    stream.getPlain() << "shared_ptr to ";
    // ignore shared pointer in depth calculation
    stream.decrementDepth();
    stream << (const T&) *value.get();
  }
  else
  {
    stream.getPlain() << "{ nullptr }";
  }
  return stream;
}

} // namespace log

} // namespace protest

// ---------------------------------------------------------------------------
struct Dummy3
{
  uint8_t value;
};

struct VectorStruct
{
  std::vector<Dummy3> value;
};

namespace protest
{

namespace log
{

template <typename T>
protest::log::UniversalStream&
toStringCustom(protest::log::UniversalStream& stream,
               const std::vector<T>& value)
{
  stream.getPlain() << "vector [";
  stream.incrementIndent();
  bool first = true;
  for (const auto& elem : value)
  {
    if (!first)
    {
      stream.getPlain() << ",";
    }
    stream.getPlain() << "\n";
    stream.printIndent();
    stream << elem;
    first = false;
  }
  stream.decrementIndent();
  stream.getPlain() << "\n";
  stream.printIndent();
  stream.getPlain() << "]";
  return stream;
}

template <>
protest::log::UniversalStream&
toStringCustom(protest::log::UniversalStream& stream,
               const std::vector<int>& value)
{
  if (value.size() < 5)
  {
    stream.getPlain() << "vector [";
    bool first = true;
    for (const auto& elem : value)
    {
      if (!first)
      {
        stream.getPlain() << ", ";
      }
      stream.getPlain() << elem;
      first = false;
    }
    stream.getPlain() << "]";
  }
  else
  {
    stream.getPlain() << "vector [";
    stream.incrementIndent();
    bool first = true;
    for (const auto& elem : value)
    {
      if (!first)
      {
        stream.getPlain() << ",";
      }
      stream.getPlain() << "\n";
      stream.printIndent();
      stream.getPlain() << elem;
      first = false;
    }
    stream.decrementIndent();
    stream.getPlain() << "\n";
    stream.printIndent();
    stream.getPlain() << "]";
  }
  return stream;
}

} // namespace log

} // namespace protest

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
  initialize()
  {
  }

  void
  process()
  {
    auto* self = context.getCurrentVirtual();
    std::stringstream format;
    protest::log::UniversalStream universalStream(format);

    {
      auto sec = section("unsigned integer types");

      /** @info(format)
       * This test will print all unsigned integer which are available. The
       * formatting is '{ <value as integer>, <value as hex> }'
       * @seperator
       */
      {
        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        uint8_t value = 0;
        universalStream << value;
        info() << format.str() << "\n";
        assertThat(format.str() == "{ 0, 0x00 }");
      }
      {
        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        uint16_t value = 1;
        universalStream << value;
        info() << format.str() << "\n";
        assertThat(format.str() == "{ 1, 0x0001 }");
      }
      {
        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        uint32_t value = 2;
        universalStream << value;
        info() << format.str() << "\n";
        assertThat(format.str() == "{ 2, 0x00000002 }");
      }
      {
        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        uint64_t value = 3;
        universalStream << value;
        info() << format.str() << "\n";
        assertThat(format.str() == "{ 3, 0x0000000000000003 }");
      }
    }

    {
      auto sec = section("signed integer types");

      /** @info(format)
       * This test will print all signed integer which are available. The
       * formatting is '{ <value as integer>, <value as hex> }'
       * @seperator
       */
      {
        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        int8_t value = 0;
        universalStream << value;
        info() << format.str() << "\n";
        assertThat(format.str() == "{ 0, 0x00 }");
      }
      {
        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        int16_t value = -1;
        universalStream << value;
        info() << format.str() << "\n";
        assertThat(format.str() == "{ -1, 0xffff }");
      }
      {
        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        int32_t value = -2;
        universalStream << value;
        info() << format.str() << "\n";
        assertThat(format.str() == "{ -2, 0xfffffffe }");
      }
      {
        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        int64_t value = -3;
        universalStream << value;
        info() << format.str() << "\n";
        assertThat(format.str() == "{ -3, 0xfffffffffffffffd }");
      }
    }

    {
      auto sec = section("floating point types");

      /** @info(format)
       * This test will print all floating points types which are available.
       * The formatting is '{ <value as integer>, <value as hex> }'
       * @seperator
       */
      {
        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        float value = 43.3f;
        universalStream << value;
        info() << format.str() << "\n";
        assertThat(format.str() == "{ 43.3f }");
      }
      {
        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        double value = -1.32;
        universalStream << value;
        info() << format.str() << "\n";
        assertThat(format.str() == "{ -1.32f }");
      }
    }

    {
      auto sec = section("pointer to primitive types");

      /** @info(format)
       * Pointers should be derefered by using one of the template
       * specialisation of the operator<<(). A pointers value should
       * be printed or { nullptr } if the pointer is a nullptr.
       */

      /** @info
       * @seperator
       * Print a nullptr
       * @seperator
       */
      {
        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        double* ptr = nullptr;
        universalStream << ptr;
        info() << format.str() << "\n";
        assertThat(format.str() == "{ nullptr }");
      }

      /** @info
       * @seperator
       * Print a pointer to float
       * @seperator
       */
      {
        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        double value = -1.32;
        double* ptr = &value;
        universalStream << ptr;
        info() << format.str() << "\n";
        assertThat(format.str() == "{ -1.32f }");
      }

      /** @info
       * @seperator
       * Print a pointer to uint8_t
       * @seperator
       */
      {
        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        uint8_t value = 42;
        uint8_t* ptr = &value;
        universalStream << ptr;
        info() << format.str() << "\n";
        assertThat(format.str() == "{ 42, 0x2a }");
      }
    }

    {
      auto sec = section("enum types");

      /** @info(format)
       * This test will print enums which has a generated printer function
       * created by the protest-compiler. A enum value should be printed as
       * string with it's scope (namespace, class, etc), the value as decimal
       * and as hex.
       */

      /** @info(info)
       * @seperator
       * Print a simple enum which does not have any namespace or life inside
       * a class.
       * @seperator
       */
      {
        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        TestEnum value = TestEnum::value1;
        universalStream << value;
        info() << format.str() << "\n";
        assertThat(format.str() == "{ TestEnum::value1, 0, 0x00000000 }");
      }

      /** @info
       * @seperator
       * Print a enum which lives in a namespace.
       * @seperator
       */
      {
        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        test::TestEnum value = test::TestEnum::test2;
        universalStream << value;
        info() << format.str() << "\n";
        assertThat(format.str() == "{ test::TestEnum::test2, 1, 0x00000001 }");
      }

      /** @info
       * @seperator
       * Print a enum which lives in a class which lives in a namespace.
       * @seperator
       */
      // TODO (jreinking) I guess this should also work, protest-compiler
      // needs a adjustment.
      warn() << "Does not work since it tries to forward a inner enum";
      // {
      //   std::stringstream format;
      //   protest::log::UniversalStream universalStream(format);
      //   test::Parent::TestEnum value = test::Parent::TestEnum::test1;
      //   universalStream << value;
      //   info() << format.str() << "\n";
      //   assertThat(format.str() == "{ test::TestEnum::test2, 1, 0x00000001 }");
      // }

      /** @info(format)
       * @seperator
       * Print a enum which lives in a class namespace. The class itself will
       * be include via #include. Therefore the type is know at the start of
       * this file. This case is easier for the protest-compiler to generate a
       * printer function.
       * @seperator
       */
      {
        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        auto value = AnyClass::InnerEnum::mode1;
        universalStream << value;
        info() << format.str() << "\n";
        assertThat(format.str() ==
                   "{ AnyClass::InnerEnum::mode1, 0, 0x00000000 }");
      }

      /** @info(info)
       * @seperator
       * Print a enum which lives in a class namespace. The class itself will
       * be include via #include. Therefore the type is know at the start of
       * this file. This case is easier for the protest-compiler to generate a
       * printer function. This class also lives in a namespace.
       * @seperator
       */
      {
        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        auto value = test::AnyClass::InnerEnum::status1;
        universalStream << value;
        info() << format.str() << "\n";
        assertThat(format.str() ==
                   "{ test::AnyClass::InnerEnum::status1, 0, 0x00000000 }");
      }
    }

    {
      auto sec = section("record types");

      /** @info(format)
       * Records types are classes or structs. The test will demonstrate the
       * printer function generation capabilities of protest.
       */

      /** @info
       * @seperator
       * Print an empty struct. This is a edge case.
       * @seperator
       */
      {
        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        EmptyStruct value;
        universalStream << value;
        info() << format.str() << "\n";
        assertThat(format.str() == "struct EmptyStruct {\n}");
      }

      /** @info
       * @seperator
       * Print struct which has some primitive fields and an enum.
       * @seperator
       */
      {
        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        TestStruct value;
        value.mode = TestStructEnum::mode1;
        value.value1 = 42;
        value.value2 = 32;
        universalStream << value;
        info() << format.str() << "\n";
        auto expected =
            std::string("struct TestStruct {\n"
                        "  value1 : { 42, 0x2a }\n"
                        "  value2 : { 32, 0x0020 }\n"
                        "  mode   : { TestStructEnum::mode1, 0, 0x00000000 }\n"
                        "}");
        assertThat(format.str() == expected);
      }

      /** @info
       * @seperator
       * Print struct which has an enum with a custom printer function.
       * @seperator
       */
      {
        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        CustomPrinterStruct value;
        value.value = CustomPrinterEnum::value2;
        universalStream << value;
        info() << format.str() << "\n";
        auto expected = std::string("struct CustomPrinterStruct {\n"
                                    "  value : { custom value2 }\n"
                                    "}");
        assertThat(format.str() == expected);
      }

      /** @info(format)
       * @seperator
       * Print class which has an nested class. Furtheremore the class has
       * private attributes. 
       * @seperator
       */
      {
        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        EnclosingStruct value(42, 32, 1);
        universalStream << value;
        info() << format.str() << "\n";
        auto expected = std::string("class EnclosingStruct {\n"
                                    "  mValue1 : { 42, 0x2a }\n"
                                    "  mValue2 : { 32, 0x0000000000000020 }\n"
                                    "  mValue3 : struct NestedStruct {\n"
                                    "    mValue : { 1, 0x01 }\n"
                                    "  }\n"
                                    "}");
        assertThat(format.str() == expected);
      }

      /** @info(format)
       * @seperator
       * Print a struct which has a ptr to a primitive type. Should be
       * derefered by using one of the template specialisation for pointers.
       * @seperator
       */
      {
        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        uint8_t a = 32;
        PtrStruct value;
        value.value = &a;
        universalStream << value;
        info() << format.str() << "\n";
        auto expected = std::string("struct PtrStruct {\n"
                                    "  value : { 32, 0x20 }\n"
                                    "}");
        assertThat(format.str() == expected);
      }

      /** @info(format)
       * @seperator
       * Print a struct which has a ptr to a other struct. The
       * protest-compiler should be able to see the usage in the AST. A printer
       * function should be generated.
       * @seperator
       */
      {
        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        PtrStruct2 value;
        Pointee pointee;
        pointee.value = 0x20;
        value.ptr = &pointee;
        universalStream << value;
        info() << format.str() << "\n";
        auto expected = std::string("struct PtrStruct2 {\n"
                                    "  ptr : struct Pointee {\n"
                                    "    value : { 32, 0x20 }\n"
                                    "  }\n"
                                    "}");
        assertThat(format.str() == expected);
      }

      /** @info(format)
       * @seperator
       * Print a struct which has a depth of 3. Protest should only print 2 of
       * them since the max depth is currently 2. The third should be printed
       * as object@<addr>.
       * @seperator
       */
      {
        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        StructDepth3 value;
        value.value.value.value = 32;
        universalStream << value;
        info() << format.str() << "\n";
        assertThat(format.str().find("object@") != std::string::npos);
        assertThat(format.str().find("StructDepth1") == std::string::npos);
      }

      /** @info
       * @seperator
       * Print a struct which has a depth of 2.
       * @seperator
       */
      {
        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        StructDepth3 value;
        value.value.value.value = 32;
        universalStream << value.value;
        info() << format.str() << "\n";
        auto expected = std::string("struct StructDepth2 {\n"
                                    "  value : struct StructDepth1 {\n"
                                    "    value : { 32, 0x20 }\n"
                                    "  }\n"
                                    "}");
        assertThat(format.str() == expected);
      }

      /** @info(format)
       * @seperator
       * Print a struct which has a pointer to itself. The recursion should
       * stop on depth 2.
       * @seperator
       */
      {
        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        RecursionStruct value;
        value.value = 8;
        value.self = &value;
        universalStream << value;
        info() << format.str() << "\n";
        assertThat(format.str().find("object@") != std::string::npos);
        assertThat(format.str().find("self  : struct RecursionStruct") !=
                   std::string::npos);
      }

      /** @info(format)
       * @seperator
       * Print a struct which has a std::set as member. Should be printed as
       * object@<addr> since there is no custom printer function for std::set
       * yet.
       * @seperator
       */
      {
        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        SetStruct value;
        universalStream << value;
        info() << format.str() << "\n";
        assertThat(format.str().find("object@") != std::string::npos);
      }

      /** @info(format)
       * @seperator
       * Print a struct which has a std::shared_ptr as member. Whereas there is
       * a custom printer function for std::shared_ptr. since the protest-
       * compiler cannot find the operator<<() with Dummy2 the printer function
       * generation must be triggered explicit by using
       * enforcePrinterGeneration
       * @seperator
       */
      {
        // enforce generation of printer function for Dummy2 since there is no
        // call to operator<<() for the type Dummy2 when protest-compiler runs.
        // Using this function adds Dummy2 to the AST (without using it at
        // runtime).
        enforcePrinterGeneration<Dummy2>();

        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        SharedPtrStruct value;
        value.value = std::make_shared<Dummy2>();
        universalStream << value;
        info() << format.str() << "\n";
        auto expected = std::string("struct SharedPtrStruct {\n"
                                    "  value : shared_ptr to struct Dummy2 {\n"
                                    "    value : { 0, 0x00 }\n"
                                    "  }\n"
                                    "}");
        assertThat(format.str() == expected);
      }

      /** @info(format)
       * @seperator
       * Since there is a template function for std::vector<T> protest should
       * use it. I.e.: it should print a nicely formatted list.
       * @seperator
       */
      {
        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        std::vector<Dummy3> vec;
        Dummy3 d;
        vec.push_back(d);
        vec.push_back(d);
        vec.push_back(d);
        universalStream << vec;
        info() << format.str() << "\n";

        auto expected = std::string("vector [\n"
                                    "  struct Dummy3 {\n"
                                    "    value : { 0, 0x00 }\n"
                                    "  },\n"
                                    "  struct Dummy3 {\n"
                                    "    value : { 0, 0x00 }\n"
                                    "  },\n"
                                    "  struct Dummy3 {\n"
                                    "    value : { 0, 0x00 }\n"
                                    "  }\n"
                                    "]");
        assertThat(format.str() == expected);
      }

      /** @info(format)
       * @seperator
       * Since there is a specialization for std::vector<int> of the
       * printer function, protest should prever it. I.e all elements should
       * be printed as unformated integers.
       * @seperator
       */
      {
        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        std::vector<int> vec;
        vec.push_back(1);
        vec.push_back(2);
        vec.push_back(3);
        universalStream << vec;
        info() << format.str() << "\n";

        auto expected = std::string("vector [1, 2, 3]");
        assertThat(format.str() == expected);
      }

      /** @info(format)
       * @seperator
       * Since this vector has more than 5 elements it should print a element
       * per line.
       * @seperator
       */
      {
        std::stringstream format;
        protest::log::UniversalStream universalStream(format);
        std::vector<int> vec;
        vec.push_back(0);
        vec.push_back(1);
        vec.push_back(2);
        vec.push_back(3);
        vec.push_back(4);
        vec.push_back(5);
        vec.push_back(6);
        vec.push_back(7);
        vec.push_back(8);
        vec.push_back(9);
        universalStream << vec;
        info() << format.str() << "\n";

        auto expected = std::string("vector [\n"
                                    "  0,\n"
                                    "  1,\n"
                                    "  2,\n"
                                    "  3,\n"
                                    "  4,\n"
                                    "  5,\n"
                                    "  6,\n"
                                    "  7,\n"
                                    "  8,\n"
                                    "  9\n"
                                    "]");
        assertThat(format.str() == expected);
      }
    }
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
