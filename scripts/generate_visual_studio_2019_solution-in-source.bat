@echo off

pushd %~dp0..

cmake -G "Visual Studio 16 2019" -B .

popd

pause
