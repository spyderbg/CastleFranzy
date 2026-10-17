@echo off

rem ////////////////////////////////////////
rem 		Init pack
rem ///////////////////////////////////////

set PACK_NAME=Sounds

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

if exist %OUTPUT_PATH% (
	del /q %OUTPUT_PATH%\*.pack
)
