@echo off
del /q *.strings 2> NUL

%PYTHON25% %XLS2STRINGS% -xlssheets2xml strings.xls LANG
rem pause