@echo off
python process.py
copy /Y __font8x8.h ..\..\emulator
copy /Y __font8x8.h ..\ST7920Driver\src
