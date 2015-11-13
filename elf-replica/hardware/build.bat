@echo off
copy /Y ..\emulator\sys_processor.* src
copy /Y ..\emulator\sys_debug_system.h src
copy /Y ..\emulator\hardware.* src
copy /Y ..\emulator\__1802*.* src
copy /Y ..\miscellany\test_SSD1306_64x32\src\SSD* src
platformio run