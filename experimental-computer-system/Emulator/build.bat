@echo off
cd ..\Processor
call build

cd ..\Emulator
mingw32-make

