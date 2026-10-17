@echo off

rem ////////////////////////////////////////
rem 		Init pack
rem ///////////////////////////////////////

set PACK_NAME=Strings

call %~dp0\..\..\config.bat
call %~dp0\..\..\setenv.bat

rem ///////////////////////////////////////


echo.
echo **************************************************************
echo    Cleaning %PACK_NAME%
echo **************************************************************

if exist bin (
	rmdir /S /Q bin
)

if exist %TEMP_PATH%\%PACK_NAME% (
	rmdir /S /Q %TEMP_PATH%\%PACK_NAME%
)

if exist %OUTPUT_PATH%\%PACK_NAME% (
	rd /s /q %OUTPUT_PATH%\%PACK_NAME%
)
