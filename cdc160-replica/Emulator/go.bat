@echo off
python ..\osap\osap.py test.asm
if errorlevel 1 goto exit
cdc160.exe
:exit
