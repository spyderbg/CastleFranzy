@echo off
echo **************************************
echo *** build strings
echo **************************************

call config.bat

SET STRING_DIR=%PRJ_DIR%\resource\text
pushd %STRING_DIR%

call export_strings.bat
rem for pictures
del *_pic.strings

rem English (main)...
set  lang=EN,DE,FR,SP,IT,JP
for %%I in (%lang%) do (
	echo %%I "%%I_main.strings" > StringManager.ini
	%STRINGMANAGER% StringManager.ini 	 
)

del StringManager.ini
del *.java
copy /y EN.h %PRJ_DIR%\src\System\String_Indices.h

md temp
move *.off temp\
move *.bin temp\
%respack% -s temp -d GLL_str.pak -zlib
rd /s/q temp
rem for win32
rem copy GLL_str.pak %PRJ_DIR%\platform\Win32\GLLogin\demo\data
copy GLL_str.pak %PRJ_DIR%\platform\Win32\GLLive\demo\data
rem for mac
copy GLL_str.pak %PRJ_DIR%\platform\iPhone\data\gll_str.pak

popd