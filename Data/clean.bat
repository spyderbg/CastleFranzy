@echo off

if not defined FORTRESS_DEFENDER_CONFIG (
	call %~dp0\..\setenv.bat
)
call %PROJECT_PATH%\config.bat
call %PROJECT_DATA_PATH%\make_prepare.bat

echo.
echo **************************************************************
echo    Cleaning Data 
echo **************************************************************               

rem Proceesing all clean files in each subdirectory
for %%d in (%DATA_BUILD_FLOW%) do (
   cd %%d
   echo.
   call _clean.bat
   cd.. 
)  


:END
