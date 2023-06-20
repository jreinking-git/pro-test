#!/bin/bash

DEVMODE=0
ACCEPT_ALL=0
TEST=""
while [[ $# -gt 0 ]]; do
  case $1 in
    # do not remove build folders.
    # This is useful for developing to avoid that the project will always
    # rebuild. This script must be run once so that the build folder exists
    # when executing with -d/--dev-mode
    -d|--dev-mode)
      DEVMODE=1
      # shift # past argument
      shift # past value
      ;;
    # this will copy the main.log of the last run to the expected.log. Any
    # further execution will then compare the expected.log with the actual
    # result (main.log).
    -a|--accept-all)
      ACCEPT_ALL=1
      # shift # past argument
      shift # past value
      ;;
    # Execute a single test
    -t|--test)
      TEST="$2"
      shift # past argument
      shift # past value
      ;;
    -*|--*)
      echo "Unknown option $1"
      exit 1
      ;;
    *)
      POSITIONAL_ARGS+=("$1") # save positional arg
      shift # past argument
      ;;
  esac
done

if [[ -z "$TEST" ]]
then
  list=("demo01"
        "demo02"
        "demo03"
        "demo04"
        "demo05"
        "demo06"
        "demo07"
        "demo08"
        "demo09"
        "demo10"
        "demo11"
        "demo12"
        "demo13"
        "reliable_broadcast"
        "tutorial01"
        "tutorial02")
else
  list=("$TEST")
fi

if [ $ACCEPT_ALL -ne 0 ]; then
  for i in "${list[@]}"
  do
    mv ./$i/build/main.log ./$i/expected.log
  done
  echo "All accepted"
  exit 0
fi

if [ $DEVMODE -ne 1 ]; then
  rm -rf ./build
  mkdir ./build
  cmake cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX:PATH=./install -B./build -S./..
  if [ $? -ne 0 ]; then
    echo "Error"
    exit 1
  fi

  make -s -C ./build
  if [ $? -ne 0 ]; then
    echo "Error"
    exit 1
  fi

  make -s -C ./build install
  if [ $? -ne 0 ]; then
    echo "Error"
    exit 1
  fi
fi

returnVal=0
for i in "${list[@]}"
do
  if (( $DEVMODE ==  1 )); then
    echo "BUILD $i"
    mkdir -p ./$i/build
    echo "cmake -DCMAKE_BUILD_TYPE=Debug -DDEVELOPMENT_BUILD=1 -DCMAKE_PREFIX_PATH="`pwd`/build/install" -B./$i/build -S./$i"
    cmake -DCMAKE_BUILD_TYPE=Debug -DDEVELOPMENT_BUILD=1 -DCMAKE_PREFIX_PATH="`pwd`/build/install" -B./$i/build -S./$i
    if [ $? -ne 0 ]; then
      echo "Error"
      exit 1
    fi
    make -s -C ./$i/build
    ./$i/build/run_test > ./$i/build/main.log
    # ignore @date and object@<addr> since it will always change
    diff <(grep -Ev 'pt.cpp:|@0x|@date|object|        0x|But is: 0x|Actual: 0x|        @|But is: @|Actual: @' ./$i/expected.log) <(grep -Ev 'pt.cpp:|@0x|@date|object|        0x|But is: 0x|Actual: 0x|        @|But is: @|Actual: @' ./$i/build/main.log)
    if [ $? -eq 0 ]; then
      echo "PASS $i"
    else
      echo "FAIL $i"
      returnVal=1
    fi
  else
    echo "BUILD $i"
    rm -rf ./$i/build
    mkdir ./$i/build
    echo "cmake -DCMAKE_PREFIX_PATH="`pwd`/build/install" -B./$i/build -S./$i"
    cmake -DCMAKE_PREFIX_PATH="`pwd`/build/install" -B./$i/build -S./$i
    if [ $? -ne 0 ]; then
      echo "Error"
      exit 1
    fi
    make -s -C ./$i/build
    ./$i/build/run_test > ./$i/build/main.log
    # ignore @date and object@<addr> since it will always change
    diff <(grep -Ev '@0x|@date|object|        0x|But is: 0x|Actual: 0x|        @|But is: @|Actual: @' ./$i/expected.log) <(grep -Ev '@0x|@date|object|        0x|But is: 0x|Actual: 0x|        @|But is: @|Actual: @' ./$i/build/main.log)
    if [ $? -eq 0 ]; then
      echo "PASS $i"
    else
      echo "FAIL $i"
      returnVal=1
    fi
  fi
done

echo "=========================="
if [ "$returnVal" == "1" ]; then
  echo "TOTAL: FAIL"
else
  echo "TOTAL: PASS"
fi

exit $returnVal