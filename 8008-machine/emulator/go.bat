@echo off
\mingw\bin\asw -L test.asm
if errorlevel 1 goto norun
\mingw\bin\p2bin -r 0-8191 -l 0 test.p
del test.p
mike4.exe test.bin
:norun