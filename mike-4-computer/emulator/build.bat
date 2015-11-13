@echo off
cd ..\Processor
call build

cd ..\Emulator
python torom.py
mingw32-make
