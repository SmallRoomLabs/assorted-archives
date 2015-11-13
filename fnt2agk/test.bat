@echo off
rem
rem		This is a test for font conversion in my system, should work if you have the whole repository
rem

rem convert retrofont.png/.fnt in src directory to an AGK font demo
python fnt2agk.py src\retrofont demo

rem I copy it somewhere to test it.
copy demo*.* \AppGameKit\HandsOnAgk\Font\media