@echo off
rem
del /Q src\*.*

copy /Y ..\emulator\start.cpp src
copy /Y ..\emulator\sys_processor.* src
copy /Y ..\emulator\sys_debug_system.h src
copy /Y ..\emulator\hardware.cpp src
copy /Y ..\emulator\__8008*.h src
copy /Y ..\emulator\__image.h src

copy /Y ..\miscellany\font-3x5\__smallfont.h src
copy /Y ..\miscellany\ST7920-4PixelText\src\ST7920*.* src

platformio run