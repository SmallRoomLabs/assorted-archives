@echo off
..\bin\asw -L monitor.asm
..\bin\p2bin monitor.p -r 0-255 -l 255
