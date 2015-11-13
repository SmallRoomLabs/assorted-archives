@echo off
cd ..\processor
call build
cd ..\miscellany\font-7x9
call build
cd ..\..\emulator
mingw32-make
