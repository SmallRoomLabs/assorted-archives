@echo off
\mingw\bin\asw -L test.asm 
if errorlevel 1 goto exit
\mingw\bin\p2bin -r 0-2047 test.p
del test.p
scrumpi3
:exit
