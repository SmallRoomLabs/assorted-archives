@echo off
cd ..\micro
call build

cd ..\emulator
mingw32-make
