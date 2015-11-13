@echo off
copy /Y ..\emulator\sys_processor.* src
copy /Y ..\emulator\sys_debug_system.h src
copy /Y ..\emulator\hardware.* src
copy /Y ..\emulator\drivers.* src
copy /Y ..\emulator\_*.* src
platformio run
