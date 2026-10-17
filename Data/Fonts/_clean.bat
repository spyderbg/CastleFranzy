@echo off

rem ////////////////////////////////////////
rem 		Init pack
rem ///////////////////////////////////////

set PACK_NAME=Fonts

call %~dp0\..\..\config.bat
call %~dp0\..\..\setenv.bat

rem ///////////////////////////////////////

set SPRCMD_NAME=%PACK_NAME%.sprcmd
set LOG=%CD%\make%PACK_NAME%.log

echo.
echo **************************************************************
echo    Cleaning %PACK_NAME%
echo **************************************************************

if exist *.log (
	del /s /q *.log
)

if exist bin (	
	rmdir /S /Q bin
)

if exist %PROJECT_DATA_PATH%\Sprites\%PACK_NAME% (
	rmdir /S /Q %PROJECT_DATA_PATH%\Sprites\%PACK_NAME%
)

if exist %OUTPUT_PATH%\%PACK_NAME% (
	rd /s /q %OUTPUT_PATH%\%PACK_NAME%
)
