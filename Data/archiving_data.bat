@echo off

call %~dp0\..\config.bat
call %~dp0\..\setenv.bat

echo.
echo **************************************************************
echo    Archiving Data
echo **************************************************************

pushd %OUTPUT_PATH%

  set ARCHIVE_NAME="resources.pack"
  
  if exist %ARCHIVE_NAME% (
    del /Q %ARCHIVE_NAME%
  )

  %SEVEN_ZIP% a %ARCHIVE_NAME% -tzip -x!*.* -scsWIN -mx=5
  %SEVEN_ZIP% d %ARCHIVE_NAME% .svn -r -tzip -scsWIN
  
popd

:END