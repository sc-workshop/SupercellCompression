@echo off

set ABI=armeabi-v7a
IF NOT "%~3"=="" set ABI=%3

cd %NDK_CMAKE%/bin/

cmake^
    -DCMAKE_TOOLCHAIN_FILE="%NDK%/build/cmake/android.toolchain.cmake"^
    -DCMAKE_MAKE_PROGRAM="%NDK_CMAKE%/bin/ninja"^
    -DANDROID_ABI=%ABI%^
    -DCMAKE_ANDROID_ARCH_ABI=%ABI%^
    -DCMAKE_SYSTEM_NAME=Android^
    -DCMAKE_SYSTEM_VERSION=23^
    -DANDROID_PLATFORM=android-23^
    -DANDROID_STL=c++_static^
    -DCMAKE_BUILD_TYPE=%2^
    -S%1^
    -B%1/android_build^
    -GNinja

cd %1/build-android

call "%NDK_CMAKE%/bin/ninja"

pause
