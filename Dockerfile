FROM ubuntu:22.10

RUN apt update
RUN apt -y install clang-14
RUN apt -y install libclang-14-dev
RUN apt -y install cmake
RUN apt -y install g++
RUN apt -y install git
RUN apt -y install clang-format
RUN apt -y install clang-tidy
RUN apt -y install gawk
RUN apt -y install lcov

RUN git clone https://github.com/nlohmann/json.git/ && cd json && git checkout 4c6cde72e533158e044252718c013a48bcff346c
RUN cd json && mkdir build && cd build && cmake .. && make && make install

RUN git clone https://github.com/pantor/inja.git && cd inja && git checkout d462b9f82909cdca3d08cec6c2c9c2a4d78fbd6a
RUN sed -i '/.*option(INJA_USE_EMBEDDED_JSON.*/c\option(INJA_USE_EMBEDDED_JSON "Use the shipped json header if not available on the system" OFF)' ./inja/CMakeLists.txt
RUN sed -i '/.*option(INJA_INSTALL.*/c\option(INJA_INSTALL "Generate install targets for inja" ON)' ./inja/CMakeLists.txt
RUN sed -i '/.*option(INJA_EXPORT.*/c\option(INJA_EXPORT "Export the current build tree to the package registry" ON)' ./inja/CMakeLists.txt
RUN sed -i '/.*option(BUILD_TESTING.*/c\option(BUILD_TESTING "Build unit tests" OFF)' ./inja/CMakeLists.txt
RUN sed -i '/.*option(INJA_BUILD_TESTS.*/c\option(INJA_BUILD_TESTS "Build unit tests when BUILD_TESTING is enabled." OFF)' ./inja/CMakeLists.txt
RUN sed -i '/.*option(BUILD_BENCHMARK.*/c\option(BUILD_BENCHMARK "Build benchmark" OFF)' ./inja/CMakeLists.txt
RUN sed -i '/.*option(COVERALLS.*/c\option(COVERALLS "Generate coveralls data" OFF)' ./inja/CMakeLists.txt

run cd inja && mkdir build && cd build && cmake .. && make && make install

WORKDIR /
RUN git clone https://github.com/wolfcw/libfaketime.git
WORKDIR /libfaketime/src
RUN make install
WORKDIR /home/root
RUN echo "export LD_PRELOAD=/usr/local/lib/faketime/libfaketime.so.1 && export FAKETIME_NO_CACHE=1 && export FAKETIME=\"2022-01-01 12:00:00\"" >> /etc/bash.bashrc
