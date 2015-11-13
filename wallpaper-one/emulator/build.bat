@echo off
cd ..\processor
call build
cd ..\emulator
python testtoc.py
mingw32-make

