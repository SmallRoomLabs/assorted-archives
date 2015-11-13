@echo off
\mingw\bin\asw -L test.asm
if errorlevel 1 goto norun
\mingw\bin\p2bin -r 8192-16383 -l 0 test.p
del test.p
c8008.exe test.bin
:norun