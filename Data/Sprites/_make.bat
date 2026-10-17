@echo off

rem ////////////////////////////////////////
rem 		Init pack
rem ///////////////////////////////////////

set PACK_NAME=Sprites

call %~dp0\..\..\config.bat
call %~dp0\..\..\setenv.bat

rem ///////////////////////////////////////

set SPRCMD_NAME=%PACK_NAME%.sprcmd
set LOG=%CD%\make%PACK_NAME%.log

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

if not exist "bin" (
	mkdir bin
)
if exist "bin\*.h" (
	del /s /q bin\*.h
)

rem Create 'sprcmd' file for AuroraGT
echo InitBSpriteFFT( ^"GP_export.fft^", WRITE_LOG ) > %SPRCMD_NAME%

rem Remove all bkp files
echo.
echo Delete all bakcup files generated from AuroraGT
echo.
del /Q /S .\*.bak
echo.

rem  Go through all sprite directories
for /D %%d in (*.) do (
	if not "%%d"=="bin" (
	
		if not exist bin\%%d (
			mkdir bin\%%d
		)
		
		echo.																			>> %SPRCMD_NAME%				
		echo.																			>> %SPRCMD_NAME%				
		echo //---------------------------------------------------						>> %SPRCMD_NAME%						
		echo // Export %%d																>> %SPRCMD_NAME%				
		echo //---------------------------------------------------						>> %SPRCMD_NAME%						
		echo.																			>> %SPRCMD_NAME%				
				
		rem Generate sprite command file for current dir
		for %%f in (%%d\*.sprite) do (
			echo Load(^"%%f^"^) 														>> %SPRCMD_NAME%
rem			echo ExportModulesID(^"bin\%%d_m.h^", ^"%%d_M_%%~nf_^", APPEND^|TOUPPER^)	>> %SPRCMD_NAME%
			echo ExportFramesID(^"bin\%%d.h^", ^"%%d_F_%%~nf_^", APPEND^|TOUPPER^)	>> %SPRCMD_NAME%
			echo ExportAnimsID(^"bin\%%d.h^", ^"%%d_A_%%~nf_^", APPEND^|TOUPPER^)		>> %SPRCMD_NAME%			
rem			echo ExportBSpriteEx(^"bin\%%d_%%~nf.bsprite^",GLOBAL, I256, _8888^)							>> %SPRCMD_NAME%	
			echo ExportBSpriteFFT(^"bin\%%d\%%~nf.bsprite^"^)							>> %SPRCMD_NAME%
		)

		rem Copy all .pvr files to 'bin' dir
		for %%f in (%%d\*.pvr) do (
			copy /Y %%f bin\%%d > NUL
		)
    
		rem Copy .tga files if no .pvr is present 
		for %%f in (%%d\*.tga) do (
			if not exist %%~pnf.pvr (
				copy /Y %%f bin\%%d > NUL
			)
		)
	)
)

%PROJECT_TOOLS_PATH%\AuroraGT\AuroraGT.exe %SPRCMD_NAME%

rem Copy generated files
for %%f in (bin\*.h) do (
	if not exist %HEADERS_OUT_PATH%\%%~nf.h (
		move /Y %%f %HEADERS_OUT_PATH%
	) else (		
		FC %%f %HEADERS_OUT_PATH%\%%~nf.h | FIND "FC: no dif" > nul 
		if ERRORLEVEL 1 ( 
			move /Y %%f %HEADERS_OUT_PATH%
		) else (
			del /Q /S %%f > nul
		)
	)
)

rem Copy data to temp directory
rem if not exist %TEMP_PATH%\%PACK_NAME% (
rem 	mkdir %TEMP_PATH%\%PACK_NAME%
rem )
rem xcopy /S /Y bin %TEMP_PATH%\%PACK_NAME%
rem rmdir /S /Q bin


rem Build bar file or not
if not "%1"=="no_bar" (
	echo.
	echo **************************************************************
	echo    Achiving %PACK_NAME%
	echo **************************************************************
	
	rem del /Q %OUTPUT_PATH%\%PACK_NAME%.bar
	rem %JAVA_PATH%\bin\java -jar %PROJECT_TOOLS_PATH%\ResBuilder.jar %PACK_NAME% %TEMP_PATH% %OUTPUT_PATH%
	
	rem copy /Y %OUTPUT_PATH%\%PACK_NAME%.bar %OUTPUT_PATH_WII%\%PACK_NAME%.bar
	
	if not exist %OUTPUT_PATH%\%PACK_NAME% (
		mkdir %OUTPUT_PATH%\%PACK_NAME%
	)
	xcopy /S /Y bin %OUTPUT_PATH%\%PACK_NAME%
	rmdir bin	

	if ERRORLEVEL 1 ( 
		echo.
		echo ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR
		echo.
		pause
		goto END
	)

	for %%f in (%OUTPUT_PATH%\*.h) do (
		if not exist %HEADERS_OUT_PATH%\%%~nf.h (
			move /Y %%f %HEADERS_OUT_PATH%
		) else (		
			FC %%f %HEADERS_OUT_PATH%\%%~nf.h | FIND "FC: no dif" > nul 
			if ERRORLEVEL 1 ( 
				move /Y %%f %HEADERS_OUT_PATH%
			) else (
				del /Q /S %%f > nul 
			)
		)
	)	

)

:END

