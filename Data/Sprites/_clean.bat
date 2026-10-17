@echo off

rem ////////////////////////////////////////
rem 		Init pack
rem ///////////////////////////////////////

set PACK_NAME=Sprites

call %~dp0\..\..\config.bat
call %~dp0\..\..\setenv.bat

rem ///////////////////////////////////////

set SPRCMD_NAME=%PACK_NAME%.sprcmd
set LOG=%CD%\make%PACK_NAME%.log

echo.
echo **************************************************************
echo    Cleaning %PACK_NAME%
echo **************************************************************

if exist %SPRCMD_NAME% (
	del /s /q %SPRCMD_NAME%
)
if exist *.log (
	del /s /q %SPRCMD_NAME%.log
)
if exist bin (	
	rmdir /S /Q bin
)

if exist %OUTPUT_PATH%\%PACK_NAME% (
	rd /s /q %OUTPUT_PATH%\%PACK_NAME%
)

