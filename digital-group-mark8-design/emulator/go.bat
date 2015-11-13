@echo off
\mingw\bin\asw -L test.asm
if errorlevel 1 goto norun
\mingw\bin\p2bin -r 0-2047 -l 0 test.p
python bintoc.py
del test.p
dgm8.exe test.bin
:norun