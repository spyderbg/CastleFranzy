@echo off


call %~dp0\..\config.bat
call %~dp0\..\setenv.bat


rem Set version
echo #pragma once > %HEADERS_OUT_PATH%\version.h
echo const wchar_t g_castle_frenzy_version_str[] = L"%VERSION_MAJOR%.%VERSION_MINOR%.%VERSION_BUILD%"; >> %HEADERS_OUT_PATH%\version.h


echo.
echo **************************************************************
echo    Building Data
echo **************************************************************

rem Proceesing all make files in each subdirectory
for %%d in (%DATA_BUILD_FLOW%) do (
   cd %%d
   call _make.bat no_pause
   cd.. 
)

rem Archiving Data
call archiving_data.bat

rem Done
echo.
echo **************************************************************
echo     Done
echo **************************************************************

:END