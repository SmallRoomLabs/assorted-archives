@echo off
del /Q src\*.*
mkdir src\8008
copy /Y ..\emulator\start.cpp src
copy /Y ..\emulator\sys_processor.* src
copy /Y ..\emulator\sys_debug_system.h src
copy /Y ..\emulator\hardware.* src
copy /Y ..\emulator\__image.h src
copy /Y ..\emulator\8008\*.* src\8008
copy /Y ..\miscellany\ST7920\src\ST7920*.* src


platformio run