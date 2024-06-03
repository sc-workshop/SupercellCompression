@echo off

pushd %~dp0..

cmake -G "Visual Studio 17 2022" -B .

popd

pause
