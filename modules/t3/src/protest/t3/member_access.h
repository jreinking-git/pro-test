/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2023 Janosch Reinking
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to 
 * deal in the Software without restriction, including without limitation the 
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#pragma once

#include <functional>
#include <iostream>

#include <cstddef>
#include <cstdint>

namespace protest
{

// ---------------------------------------------------------------------------
/**
 * @brief getMemberFunction
 * 
 * This function can be used to access a private (or public) member function
 * of a given object. The function has no implementation. It will be
 * implemented by the protest-compiler. If there is no function with the
 * given signature the compilation of the pt script will fail since
 * the @c getMemberFunction is not implemented for the given signature.
 * If a function with the signature exists, but it has a diffrent name an
 * assertion will be raised.
 * 
 * @tparam F
 *  The signature of the function
 * 
 * @tparam T
 *  The type of the class which functions should be accessed
 * 
 * @param obj
 *  The object to access
 * 
 * @param name
 *  The name of the method
 * 
 * @return The function with the given name
 */
template <typename F, typename T>
std::function<F>
getMemberFunction(T& obj, const char* name);

/**
 * @brief getMemberFunction
 * 
 * Same as @c getMemberFunction from above but for static function.
 * The default argument is used so that T is present in the argument list
 * to use full template specialiation.
 * 
 * @tparam T
 *  The class to access
 * 
 * @tparam F
 *  The signature of the function to access
 * 
 * @param name
 *  The name of the function to access
 * 
 * @return The function
 */
template <typename T, typename F>
std::function<F>
getMemberFunction(const char* name, T* = nullptr);

// ---------------------------------------------------------------------------
/**
 * @brief getMemberAttr
 * 
 * This function can be used to access member variables of a given class/
 * object. If no member variable exists for the given type a compile error
 * occure on compilation. If a member exists for the given type but the name
 * does not match, a assertion is raised.
 * 
 * @tparam R
 *  The type of the member variable
 * 
 * @tparam T
 *  The type of the class to access
 * 
 * @param obj
 *  The object to access
 * 
 * @param name
 *  The name of the member to access
 * 
 * @return A reference to the member
 */

template <typename R, typename T>
R&
getMemberAttr(T& obj, const char* name);

/**
 * @brief getMemberAttr
 * 
 * Same as @c getMemberAttr but for static variables.
 * the default argument is used so that T is present in the argument list
 * to use full template specialiation.
 * 
 * @tparam T
 *  The type of the class to access
 * 
 * @tparam R
 *  The type of the member variable
 * 
 * @param obj
 *  The object to access
 * 
 * @param name
 *  The name of the member to access
 * 
 * @return A reference to the member
 */
template <typename T, typename R>
R&
getMemberAttr(const char* name, T* = nullptr);

// ---------------------------------------------------------------------------
// do not use it directly. will be implemented by protest-compiler.
template <typename T>
const void*
getMemberAttrRaw(T& obj, const char* name, size_t size);

/**
 * @brief getMemberAttrRaw
 * 
 * This function allows to access members which types are private. I.e.: the
 * member and it's type are in the private section of the outer class:
 * 
 * class ClsA
 * {
 * private:
 *   struct Type { uint8_t value; };
 *   Type mValue;
 * };
 * 
 * Since the type is private it cannot be used with @c getMemberAttr. This
 * function has a void-pointer as it return value. Therefore it can return
 * every type which then can be casted into a type which has the same memory
 * layout as the origin type:
 * 
 * struct MyType { uint8_t value; };
 * auto* ptr = getMemberAttrRaw<MyType>(obj, "mValue");
 * 
 * This is not type-safe but atleast the size of the both types are checked.
 * If the member with the given name was not found or if the types have
 * a diffrent size an assertion is raised.
 * 
 * @tparam R
 *  Return value type
 * 
 * @tparam T
 *  Type of the object which member will be accessed
 * 
 * @param obj
 *  The obj to access
 * 
 * @param name
 *  The name of the member to access
 * 
 * @return The member
 */
template <typename R, typename T>
const R*
getMemberAttrRaw(T& obj, const char* name)
{
  return reinterpret_cast<const R*>(getMemberAttrRaw(obj, name, sizeof(R)));
}

// ---------------------------------------------------------------------------
template <typename Func>
struct AsVoidFunction;

template <typename Ret, typename... Args>
struct AsVoidFunction<Ret(Args...)>
{
  using Function = void(Args...);
};

// ---------------------------------------------------------------------------
void*
getStaticVariableRaw(const char* name, size_t index);

/**
 * @brief getStaticVariable
 * 
 * @tparam R
 *  the type of the variable
 * 
 * @param name
 *  the fully qualified name of the variable
 * 
 * @param index
 *  if there are multiple matches (e.g.: there is for example more then one
 *  definition of a variable for the given name) the index will be used to
 *  identify the variable to return.
 * 
 * @return pointer to the variable
 */
template <typename R>
R*
getStaticVariable(const char* name, size_t index = 0, const char* nm = "")
{
  std::cout << "nm: " << nm << std::endl;
  void* ptr = getStaticVariableRaw(nm, index);
  return (R*) ptr;
}

template <typename R, typename F, typename FF = typename AsVoidFunction<F>::Function>
R*
getStaticVariable(const char* function, const char* name, size_t index = 0, const char* nm = 0)
{
  std::cout << "nm: " << nm << std::endl;
  void* ptr = getStaticVariableRaw(nm, index);
  return (R*) ptr;
}

// ---------------------------------------------------------------------------
void*
getStaticFunctionRaw(const char* name, size_t index);

/**
 * @brief getStaticFunction
 * 
 * @tparam F
 *  the signature
 * 
 * @tparam FF
 *  it is easier to get the gcc mangled name from clang when having a void
 *  function. FF should not be used.
 * 
 * @param name
 *  the fully qualified name of the function
 * 
 * @param index
 *  if there are multiple matches (e.g.: there is for example more then one
 *  function with the given name) the index will be used to identify the
 *  function to return.
 * 
 * @return std::function to function
 */
template <typename F, typename FF = typename AsVoidFunction<F>::Function>
std::function<F>
getStaticFunction(const char* name, size_t index = 0, const char* nm = "")
{
  auto* var = getStaticFunctionRaw(nm, index);
  return std::function<F>(*((F*) var));
}

} // namespace protest
