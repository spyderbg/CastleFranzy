@echo off
if not exist "%JDK_HOME%" (
	set JDK_HOME=%JDK_PATH%
)

if not exist "%JDK_HOME%" (
	set JDK_HOME=%JAVA_HOME%
)

if not exist "%JDK_HOME%" (
	echo Java path not found. Var JAVA_HOME or JDK_PATH must be setted.
)


set DIR_TOOLS=%cd%\tools\
set RESCOMPILER="%DIR_TOOLS%\ResCompiler.exe"
set STRINGSEXPORTER=%JDK_HOME%\bin\java -jar "%DIR_TOOLS%\JStringManagerXLS\JStringExporterXLS.jar"
set STRINGMANAGER="%DIR_TOOLS%\UTF16StringManager.exe"

rem Exporting XLS file

set XLS_STRINGS_FILE=IGP_strings.xls
%STRINGSEXPORTER% -unicode -xls %XLS_STRINGS_FILE% -row_start 4 -col_id 2 -col_strings 4  -nobreak -strings EN
%STRINGSEXPORTER% -unicode -xls %XLS_STRINGS_FILE% -row_start 4 -col_id 2 -col_strings 5  -nobreak -strings DE
%STRINGSEXPORTER% -unicode -xls %XLS_STRINGS_FILE% -row_start 4 -col_id 2 -col_strings 6  -nobreak -strings FR
%STRINGSEXPORTER% -unicode -xls %XLS_STRINGS_FILE% -row_start 4 -col_id 2 -col_strings 7  -nobreak -strings IT
%STRINGSEXPORTER% -unicode -xls %XLS_STRINGS_FILE% -row_start 4 -col_id 2 -col_strings 8  -nobreak -strings SP
%STRINGSEXPORTER% -unicode -xls %XLS_STRINGS_FILE% -row_start 4 -col_id 2 -col_strings 9  -nobreak -strings JP
%STRINGSEXPORTER% -unicode -xls %XLS_STRINGS_FILE% -row_start 4 -col_id 2 -col_strings 10 -nobreak -strings KR
REM %STRINGSEXPORTER% -unicode -xls %XLS_STRINGS_FILE% -row_start 4 -col_id 2 -col_strings 10 -nobreak -strings AR

for %%L IN (EN, DE, FR, IT, SP, JP, KR) DO (

	echo %%L "%%L.strings"
	%STRINGMANAGER% StringManager_%%L.ini

	copy *.h		..\
 	move *.bin		bin>>NUL
 	move *.off		bin>>NUL
 	move *.h		bin>>NUL
 	del *.java

 	xcopy /Q /Y		bin\%%L.h    	..\..\sources\classesToAdd\string_headers\
)
rem %RESCOMPILER% strings.lst	>>NUL
rem xcopy /Q /Y		strings.bar    ..\..\outputRes\
rem copy strings.h		..\..\sources\classesToAdd\string_headers\
