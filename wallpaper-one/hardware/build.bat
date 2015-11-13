@echo off
copy /Y "..\arduino testing\7920 library\src\lcd7920.*" src
copy /Y "..\emulator\hardware.*" src
copy /Y "..\emulator\sys_processor.*" src
copy /Y "..\emulator\sys_debug_system.h" src
copy /Y "..\emulator\__*.h" src
platformio run -t clean
platformio run

