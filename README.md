# PRO-TEST

## Motivation

Pro-Test is a test framework for concurrent, distributed, and embedded systems designed for TDD (Test-Driven-Development). Usually, C++ test frameworks use macros and template magic to retriev meta-information. Pro-Test instead uses a custom clang/LLVM frontend tool (called **protest-compiler**) which preprocesses test scripts. Doing so the test script can be enhanced by adding meta information, manipulating the source code, and source code generation. The test script is also written in C++. Since running tests in a concurrent, distributed or embedded environment is often not sufficient and time-consuming (or not even possible) Pro-Test provides a simulated environment. This environment uses coroutines to simulate concurrent processes and simulated time. This allows developers to run many tests and still getting fast and reproducible test results which is crucial for TDD.

## Summary

Pro-Test provides ...

 - an operating system abstraction layer to write testable code which can be executed in the test but also in the production environment
 - a test environment with simulated time which allows fast and reproducible test results
 - an API to create tests for concurrent systems
 - a test log that is suitable for concurrent systems

Pro-Test uses ...
 
 - CMake
 - C++17
 - python 3
 - clang-14/LLVM-14

## Example

```c
#include <protest/api.h>

using namespace protest::time;
using namespace protest;

// ---------------------------------------------------------------------------
/**
 * @test    Tutorial-02
 * @author  jreinking
 * @version 0.0.1
 */
protest::Context context;

// ---------------------------------------------------------------------------
/**
 * @class TestRunner
 */
class TestRunner : public protest::Runner
{
public:
  TestRunner(protest::Context& context) : protest::Runner(context, "main")
  {
  }

  void
  process() override
  {
    auto sw = createStopwatch();
    sw.stop();

    checkThat(sw.lap() == 1_ms)
      << "Stopwatch lap: " << sw.lap().milliseconds() << "ms";


    auto value = Value<int>(0);
    auto timer = createTimer([&]() {
      value = 42;
    }, 1000_ms);

    auto inv = createInvariant((!(sw < 1000_ms) || value == 0) &&
                               (!(sw > 1000_ms) || value == 42));

    sw.start();
    bool success = wait(value == 42, 2000_ms);

    assertThat(success)
      << "Should not run into a timeout!";

    assertThat(sw.lap() == 1000_ms) 
      << "Timer should run after 1000_ms";

    assertThat(value == 42)
      << "Timer should set value to 42";

    wait(1000_ms);

    value = 1
  }
};

int
main(int argc, char const* argv[])
{
  context.initialize(argc, argv);
  TestRunner runner(context);
  return context.run();
}

```

```log
                      *******************************************************************************
                      * @test           Tutorial-02
                      * @author         jreinking
                      * @version        0.0.1
                      * @date           01-01-2022 12-00-00
                      *******************************************************************************
INFO             main -------------------------------------------------------------------------------
                      Start runner 'main'
                      -------------------------------------------------------------------------------
WARN 0000000000  main /home/root/pro-test/demos/tutorial02/main.pt.cpp:31
                      WARN: the condition evaluates to false:
                      'sw.lap() == 1_ms'
                      Stopwatch lap: 0ms
WAIT 0000000000  main /home/root/pro-test/demos/tutorial02/main.pt.cpp:43
                      Wait for condition:
                      'value == 42'
EXPR 0000001000  main /home/root/pro-test/demos/tutorial02/main.pt.cpp:35
                      Timer 'timer' expired after:
                      1000 ms
RESM 0000001000  main /home/root/pro-test/demos/tutorial02/main.pt.cpp:43
                      Condition is fulfilled:
                      'value == 42'
PASS 0000001000  main /home/root/pro-test/demos/tutorial02/main.pt.cpp:45
                      PASS: the condition evaluates to true:
                      'success'
                      Should not run into a timeout!
PASS 0000001000  main /home/root/pro-test/demos/tutorial02/main.pt.cpp:48
                      PASS: the condition evaluates to true:
                      'sw.lap() == 1000_ms'
                      Timer should run after 1000_ms
PASS 0000001000  main /home/root/pro-test/demos/tutorial02/main.pt.cpp:51
                      PASS: the condition evaluates to true:
                      'value == 42'
                      Timer should set value to 42
WAIT 0000001000  main /home/root/pro-test/demos/tutorial02/main.pt.cpp:54
                      Wait for 1000 ms
WAIT 0000002000  main /home/root/pro-test/demos/tutorial02/main.pt.cpp:54
                      Timeout of 1000 ms elapsed!
INV  0000002000  main /home/root/pro-test/demos/tutorial02/main.pt.cpp:39
                      Invariant does not hold:
                      '(!(sw < 1000_ms) || value == 0) && (!(sw > 1000_ms) || value == 42)'
INFO             main -------------------------------------------------------------------------------
                      Exit of 'main'
                      -------------------------------------------------------------------------------
                      *******************************************************************************
                      * NUMBER OF VIOLATED INVARIANTS: 1
                      * /home/root/pro-test/demos/tutorial02/main.pt.cpp:39
                      *******************************************************************************
                      * NUMBER OF WARNINGS: 1
                      * /home/root/pro-test/demos/tutorial02/main.pt.cpp:31
                      *******************************************************************************
                      * FAILURES:                0                                               PASS
                      * WARNINGS:                1                                               WARN
                      * INV VIOLATION:           1                                               FAIL
                      *
                      * TOTAL:                                                                   FAIL
                      *******************************************************************************
```

### Installation

You can install clang/LLVM for your system if available. Otherwise, you can build it from source:

```shell
> git clone https://github.com/llvm/llvm-project.git
> git checkout llvmorg-14.0.0
> cd llvm-project
> mkdir build
> cd build
> cmake -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX:PATH=/opt/clang  \
        ../llvm
> make
> sudo make install
```

Now pro-test should be ready to compile:

```shell
> cd <path-to-protest>
> mkdir build
> cd build
> cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="/opt/pro-test"
> make
> sudo make install
```
To verify the installation you can run one of the demo projects that can be found under ```pro-test/demos/```. Enter a demo project and run the following commands:

```shell
> mkdir build
> cd build
> cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="/opt/pro-test"
> make
> ./run_test
```

### Docker

The fastest and easiest way to get started is by using docker. Enter the following commands in your terminal:

```shell
> cd <path-to-pro-test>
> docker build . -t pro-test
> docker run -v `pwd`:/home/root/pro-test  -it --entrypoint bash demo
```

Then within the docker container run the following commands to build and install the Pro-Test project and run all demos:

```shell
> cd /home/root/pro-test/demos/
> ./build_and_run_demos.sh
```