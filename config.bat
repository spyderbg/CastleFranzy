@echo off 
rem ///////////////////////////////////
rem 	SET PROJECT CONFIGURATION
rem //////////////////////////////////

if not defined FORTRESS_DEFENDER_CONFIG (
	set FORTRESS_DEFENDER_CONFIG="SET"
) else goto END


rem ---------------------------------------------
rem              BUILD LANGUAGE
rem ---------------------------------------------
set ALL_LANGUAGES=EN;DE;FR;IT;SP
set VERSION_LANGUAGE=ML


rem ---------------------------------------------
rem              BUILD VERSION
rem ---------------------------------------------
rem Digits number: 1
set VERSION_MAJOR=1
set VERSION_MINOR=1
set VERSION_BUILD=0
set BUILD_VERSION=%VERSION_MAJOR%%VERSION_MINOR%%VERSION_BUILD%

echo Version set to %VERSION_MAJOR%.%VERSION_MINOR%.%VERSION_BUILD%
echo.

:END