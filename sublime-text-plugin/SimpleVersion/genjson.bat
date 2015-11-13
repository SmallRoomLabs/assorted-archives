@echo off

rem Creates the source JSON from a Python script.

python simplegen.py >AppGameKit2.JSON-tmLanguage

echo Now build AppGameKit2.JSON-tmLanguage by building it in Sublime Text.
echo then run build.bat to make the plugin.