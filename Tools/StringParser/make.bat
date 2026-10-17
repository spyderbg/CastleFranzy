@echo off
cls
call .\..\..\setenv.bat

SET CLASS_NAME=StringParser
SET JAR_NAME=StringParser

SET CURR_DIR=%CD%
SET TOOL_DIR=%PROJECT_PATH%\Tools\%JAR_NAME%
SET SRC_DIR=%TOOL_DIR%\src
SET BIN_DIR=%TOOL_DIR%\bin
SET CLASSES_DIR=%TOOL_DIR%\classes
SET LIB_DIR=%TOOL_DIR%\lib


echo *** Make Manifest file
echo Main-Class: %CLASS_NAME%> %TOOL_DIR%\MANIFEST.MF

echo *** Compile source code
IF NOT EXIST %CLASSES_DIR% ( MD %CLASSES_DIR% )

rem %JAVAC% -classpath .;log4j-1.2.14.jar -d classes *.java jxl\*.java jxl\biff\*.java jxl\biff\drawing\*.java jxl\biff\formula\*.java jxl\format\*.java jxl\read\biff\*.java jxl\write\*.java jxl\write\biff\*.java common\*.java common\log\*.java
%JAVA_PATH%\bin\javac -classpath %LIB_DIR%\log4j-1.2.14.jar -d %CLASSES_DIR% %SRC_DIR%\*.java %SRC_DIR%\jxl\*.java %SRC_DIR%\jxl\biff\*.java %SRC_DIR%\jxl\biff\drawing\*.java %SRC_DIR%\jxl\biff\formula\*.java %SRC_DIR%\jxl\format\*.java %SRC_DIR%\jxl\read\biff\*.java %SRC_DIR%\jxl\write\*.java %SRC_DIR%\jxl\write\biff\*.java %SRC_DIR%\common\*.java %SRC_DIR%\common\log\*.java

rem echo *** Package JAR file
if not exist %BIN_DIR% ( MD %BIN_DIR% )

rem Make jar file
cd %CLASSES_DIR%
%JAVA_PATH%\bin\jar -cvfm %BIN_DIR%\%JAR_NAME%.jar %TOOL_DIR%\MANIFEST.MF *.* > NUL
cd %CURR_DIR%

del /q %TOOL_DIR%\MANIFEST.MF > NUL
rd /q /s %CLASSES_DIR% > NUL

rem Copy jar file to proper place
copy /Y %BIN_DIR%\%JAR_NAME%.jar %TOOL_DIR%\..\

pause
