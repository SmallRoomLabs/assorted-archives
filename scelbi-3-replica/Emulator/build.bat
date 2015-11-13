@echo off
cd ..\Processor
call build

cd ..\Emulator
mingw32-make


copy /Y drivers.* ..\ScelbiDuino\src
copy /Y sys_processor.* ..\ScelbiDuino\src
copy /Y sys_debug_system.h ..\ScelbiDuino\src