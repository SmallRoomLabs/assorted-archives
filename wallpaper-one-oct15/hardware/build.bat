@echo off
rem
del /Q src\*.*

copy /Y ..\emulator\start.cpp src
copy /Y ..\emulator\sys_processor.* src
copy /Y ..\emulator\sys_debug_system.h src
copy /Y ..\emulator\hardware.cpp src
copy /Y ..\emulator\__scmp*.h src
copy /Y ..\emulator\__image.h src
copy /Y ..\miscellany\ST7920Driver\src\ST7920*.* src
copy /Y ..\miscellany\font\__font8x8.h src

platformio run