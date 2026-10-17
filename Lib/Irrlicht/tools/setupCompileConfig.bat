set target="%~dp0..\include\IrrCompileGameConfig.h"
set default="%~dp0..\include\IrrCompileDefaultConfig.h"
set source="%1"

IF %source%=="" GOTO :Default

IF NOT %source%=="" GOTO :Specified

:Specified

set fileA=%source%
set fileB=%target%

FOR %%A IN (%fileA%) DO SET FILE_A_DATE=%%~tA
FOR %%B IN (%fileB%) DO SET FILE_B_DATE=%%~tB

IF NOT "%FILE_A_DATE%"=="%FILE_B_DATE%" copy %fileA% %fileB% 

GOTO :END

:Default

IF NOT EXIST %target% copy %default% %target% 

:END