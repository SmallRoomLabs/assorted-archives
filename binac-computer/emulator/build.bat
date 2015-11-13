@echo off
cd ..\arithmetic
call build
cd ..\emulator
mingw32-make
