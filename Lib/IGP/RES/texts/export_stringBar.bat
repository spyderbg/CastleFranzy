@echo off
set DIR_TOOLS=%cd%\tools\
set RESCOMPILER="%DIR_TOOLS%\ResCompiler.exe"
%RESCOMPILER% strings.lst	>>NUL
xcopy /Q /Y		strings.bar    ..\..\outputRes\
copy strings.h		..\..\sources\classesToAdd\string_headers\