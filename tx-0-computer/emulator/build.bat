@echo off
cd ..\microcode
call build
cd ..\emulator
mingw32-make
