@echo off
cd ..\Processor
call build

cd ..\Emulator
python bintoh.py
mingw32-make
copy /Y mark8.exe ..\software