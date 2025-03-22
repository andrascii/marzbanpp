#!/bin/bash

set -x

cmake -B build-relwithdeb -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_TOOLCHAIN_FILE=$VCPKG_CMAKE_FILE -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DUSE_CLANG=OFF
ln -s build-relwithdeb/compile_commands.json .
