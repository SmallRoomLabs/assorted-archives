@echo off
rem
rem "Currently it runs at about 1/7 of the speed of the real Apple 1."
rem
del /Q src\*.*

copy /Y ..\emulator\start.cpp src
copy /Y ..\emulator\sys_processor.* src
copy /Y ..\emulator\sys_debug_system.h src
copy /Y ..\emulator\driver_common.h src
copy /Y ..\emulator\driver_terminal.h src
copy /Y ..\emulator\6502\*.* src\6502
copy /Y ..\emulator\code\*.* src\code
copy /Y ..\miscellany\3x5-font\__smallfont.h src

rem "Any driver using the generic terminal code (e.g. ST7920) needs this"
copy /Y ..\emulator\drivers\driver_terminal.cpp src

rem "Various drivers"
rem copy /Y ..\emulator\driver_arduino_serial_in.cpp src
rem copy /Y ..\emulator\drivers\driver_arduino_serial_out.cpp src

copy /Y ..\emulator\drivers\driver_arduino_ps2_in.cpp src
copy /Y ..\emulator\drivers\driver_terminal_st7920.cpp src

rem "Required for ST7920"
copy /Y ..\miscellany\ST7920\src\ST7920*.* src


platformio run