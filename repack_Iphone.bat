@echo off

rem /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

call %~dp0\config.bat
call %~dp0\setenv.bat

rem /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

set BUILD_PATH=Projects\IPhone\build\

set APP_NAME=CastleFrenzy

set BUILD_1=Distribution-iphoneos
set BUILD_2=Release Gameloft-iphoneos
set BUILD_3=Release Ludigames-iphoneos

set ARCHIVE_1=%APP_NAME%_Apple_iPhone_iPodTouch_ML_MS_TS_Online_Gameloft_Distribution_v%BUILD_VERSION%
set ARCHIVE_2=%APP_NAME%_Apple_iPhone_iPodTouch_ML_MS_TS_Online_Gameloft_Final_v%BUILD_VERSION%
set ARCHIVE_3=%APP_NAME%_Apple_iPhone_iPodTouch_ML_MS_TS_Online_Ludigames_Final_v%BUILD_VERSION%

if exist %PROJECT_PATH%%ARCHIVE_1% del /Q %PROJECT_PATH%%ARCHIVE_1%
if exist %PROJECT_PATH%%ARCHIVE_2% del /Q %PROJECT_PATH%%ARCHIVE_2%
if exist %PROJECT_PATH%%ARCHIVE_3% del /Q %PROJECT_PATH%%ARCHIVE_3%

echo.
echo ///////////////////////////////////////
echo	Gather Iphone Builds
echo ///////////////////////////////////////
echo.

copy "%PROJECT_PATH%%BUILD_PATH%%BUILD_1%\%APP_NAME%.zip" %PROJECT_PATH%%ARCHIVE_1%.zip
copy "%PROJECT_PATH%%BUILD_PATH%%BUILD_2%\%APP_NAME%.zip" %PROJECT_PATH%%ARCHIVE_2%.zip
copy "%PROJECT_PATH%%BUILD_PATH%%BUILD_3%\%APP_NAME%.zip" %PROJECT_PATH%%ARCHIVE_3%.zip

python "%PROJECT_TOOLS_PATH%\zip_directory.py" "%PROJECT_PATH%%ARCHIVE_1%.dSYM.zip" "%PROJECT_PATH%%BUILD_PATH%%BUILD_1%\%APP_NAME%.app.dSYM" --include-root
if errorlevel 1 exit /b 1
python "%PROJECT_TOOLS_PATH%\zip_directory.py" "%PROJECT_PATH%%ARCHIVE_2%.dSYM.zip" "%PROJECT_PATH%%BUILD_PATH%%BUILD_2%\%APP_NAME%.app.dSYM" --include-root
if errorlevel 1 exit /b 1
python "%PROJECT_TOOLS_PATH%\zip_directory.py" "%PROJECT_PATH%%ARCHIVE_3%.dSYM.zip" "%PROJECT_PATH%%BUILD_PATH%%BUILD_3%\%APP_NAME%.app.dSYM" --include-root
if errorlevel 1 exit /b 1

echo.
echo ///////////////////////////////////////
echo	Done
echo ///////////////////////////////////////
echo.

pause
