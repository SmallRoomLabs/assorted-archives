@echo off
\mingw\bin\asw -L test.asm 
if errorlevel 1 goto exit
\mingw\bin\p2bin -r 0-1023 test.p
del test.p
wp1
:exit
