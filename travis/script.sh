#!/usr/bin/env bash
set -e

pushd build
make -j2

cd shared
./EmbMessengerSharedTest

cd ../host
./EmbMessengerHostTest

cd ../device
./EmbMessengerDeviceTest

popd

cd shared
cppcheck src/*.cpp include/EmbMessenger/*.hpp

cd ../host
cppcheck src/*.cpp include/EmbMessenger/*.hpp

cd ../device
cppcheck src/*.cpp include/EmbMessenger/*.hpp
