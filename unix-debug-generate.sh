#!/bin/bash

set -x

# -DCMAKE_EXPORT_COMPILE_COMMANDS=1 - to create compile_commands.json for clangd

cmake -B build-debug -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=$VCPKG_CMAKE_FILE -DCMAKE_EXPORT_COMPILE_COMMANDS=1
ln -s build-debug/compile_commands.json .
