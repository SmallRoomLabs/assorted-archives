@echo off
cd ..\Processor
call build

cd ..\Emulator
python bintoh.py
mingw32-make
