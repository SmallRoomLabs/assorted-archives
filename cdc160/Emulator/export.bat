@echo off
mkdir \temp\framework
del /Q \temp\framework\*.*
copy *.? \temp\framework
del /Q \temp\framework\sys*.*
del /Q \temp\framework\__*.*
del /Q \temp\framework\makefile

