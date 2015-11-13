@echo off
\mingw\bin\asw -L test.asm
if errorlevel 1 goto norun
\mingw\bin\p2bin -r 0-1023 -l 0 test.p
del test.p
mark8.exe test.bin
:norun