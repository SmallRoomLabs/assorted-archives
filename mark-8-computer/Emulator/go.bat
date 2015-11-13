@echo off
\mingw\bin\asw -L testcode.asm
if errorlevel 1 goto norun
\mingw\bin\p2bin -r 0-1023 -l 0 testcode.p
del testcode.p
mark8.exe testcode.bin
:norun