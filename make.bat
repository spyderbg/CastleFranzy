@echo off
@echo MAKE STARTS AT %DATE% %TIME% > %~dp0BUILD_TIME.LOG

set ACTION_DONE=
set SETENV_ERROR=

rem call %~dp0\config.bat
call %~dp0\setenv.bat

if not "%SETENV_ERROR%"=="" (     
     rem Aborting execution
     goto batch_error
)

if "%1"=="data_build" (
	 cls	 

     cd %PROJECT_DATA_PATH%
	 
     call make_data.bat 
	 
     set ACTION_DONE=data   
)

if "%1"=="data_clean" (
	 cls
	 
     cd %PROJECT_DATA_PATH%
	 
     call clean.bat
	 
     set ACTION_DONE=clean
)

if "%1"=="data_rebuild" (
	 cls
	 
     cd %PROJECT_DATA_PATH%
	 
     call rebuild_data.bat
	 
     set ACTION_DONE=clean
)

if "%ACTION_DONE%"=="" (
     echo Usage:                
     echo "make.bat clean"   - Cleans the previously compiled headers           
     echo "make.bat data"    - Builds the data files        
     set ACTION_DONE=Usage info
)

cd %PROJECT_PATH%

echo.
echo.
echo "%ACTION_DONE%" done  at %time% on %date%
goto end_batch
:batch_error
echo error, terminating...
echo.
echo %SETENV_ERROR%
echo.
:end_batch
@echo MAKE ENDS AT %DATE% %TIME% >> %~dp0BUILD_TIME.LOG
