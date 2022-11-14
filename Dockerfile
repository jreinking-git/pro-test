FROM ubuntu:22.10

RUN apt update
RUN apt -y install clang-14
RUN apt -y install libclang-14-dev
RUN apt -y install cmake
RUN apt -y install g++
RUN apt -y install git
RUN apt -y install clang-format
RUN apt -y install gawk

WORKDIR /
RUN git clone https://github.com/wolfcw/libfaketime.git
WORKDIR /libfaketime/src
RUN make install
WORKDIR /home/root
RUN echo "export LD_PRELOAD=/usr/local/lib/faketime/libfaketime.so.1 && export FAKETIME_NO_CACHE=1 && export FAKETIME=\"2022-01-01 12:00:00\"" >> /etc/bash.bashrc
