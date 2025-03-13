#!/bin/bash

set -x

cmake -B build-release -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=$VCPKG_CMAKE_FILE -DCMAKE_EXPORT_COMPILE_COMMANDS=1
ln -s build-release/compile_commands.json .
