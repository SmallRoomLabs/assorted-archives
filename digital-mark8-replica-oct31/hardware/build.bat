@echo off
rem
del /Q src\*.*
mkdir src\8008

copy /Y ..\emulator\start.cpp src
copy /Y ..\emulator\sys_processor.* src
copy /Y ..\emulator\sys_debug_system.h src
copy /Y ..\emulator\hardware.cpp src
copy /Y ..\emulator\drivers.h src
copy /Y ..\emulator\8008\__8008*.h src\8008
copy /Y ..\emulator\__image.h src

copy /Y ..\emulator\drivers\xd_keyboard_arduino.cpp src
copy /Y ..\emulator\drivers\xd_display_arduino_st7920.cpp src
copy /Y ..\emulator\drivers\xd_tape_dummy.cpp src

copy /Y ..\miscellany\font-3x5\__smallfont.h src
copy /Y ..\miscellany\ST7920-4PixelText\src\ST7920*.* src

platformio run