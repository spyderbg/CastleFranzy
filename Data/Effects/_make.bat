@echo off

rem ////////////////////////////////////////
rem 		Init pack
rem ///////////////////////////////////////

set PACK_NAME=Effects

call %~dp0\..\..\config.bat
call %~dp0\..\..\setenv.bat

rem ///////////////////////////////////////

set LOG=%CD%\make%PACK_NAME%.log
set TEXTURE_FORMATS=tga;pvr;
set PVR_TOOL=%PROJECT_PATH%\Tools\PVRTexTool\PVRTexTool.exe

rem Clean data
if "%1"=="clean" (
	call _clean.bat
	goto END
)
if "%1"=="" (
	call _clean.bat
)

rem Build data
echo.
echo **************************************************************
echo    Make %PACK_NAME%
echo **************************************************************

mkdir %OUTPUT_PATH%\%PACK_NAME%

copy *.pvr %OUTPUT_PATH%\%PACK_NAME%
copy effects.xml %OUTPUT_PATH%\%PACK_NAME%

:END

