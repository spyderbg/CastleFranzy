@echo off

:copy_for_max8
reg query HKEY_LOCAL_MACHINE\SOFTWARE\Autodesk\3dsMax\8.0>nul
if not %ERRORLEVEL%==0 goto :end_copy_max8

reg query HKEY_LOCAL_MACHINE\SOFTWARE\Autodesk\3dsMax\8.0 /v Installdir | find "Installdir">max8_dir.txt
if not %ERRORLEVEL%==0 goto :end_copy_max8

set /p MAX8_DIR=<max8_dir.txt

set MAX8_DIR=%MAX8_DIR:~22,-1%

set cmd_copy=copy "bin\Release 8\IrrColladaMax.dle" "%MAX8_DIR%\plugins"
@echo %cmd_copy%
%cmd_copy%>nul

set cmd_copy=copy "External\DirectX\Bin\D3DX9_37.dll" "%MAX8_DIR%"
@echo %cmd_copy%
%cmd_copy%>nul

set cmd_copy=copy "..\BatchExporter\batchexporter.ms" "%MAX8_DIR%\Scripts"
@echo %cmd_copy%
%cmd_copy%>nul

set cmd_copy=copy "..\AnimationSelector\animationselector.ms" "%MAX8_DIR%\Scripts"
@echo %cmd_copy%
%cmd_copy%>nul

set cmd_copy=copy "..\AnimationSelector\animselector.mcr" "%MAX8_DIR%\Scripts"
@echo %cmd_copy%
%cmd_copy%>nul

set cmd_copy=copy "..\UserPropertiesEditor\UserPropertiesEditor.ms" "%MAX8_DIR%\Scripts"
@echo %cmd_copy%
%cmd_copy%>nul

if not %ERRORLEVEL%==0 goto :end_copy_max8

echo copied plugin to max8

:end_copy_max8


reg query HKEY_LOCAL_MACHINE\SOFTWARE\Autodesk\3dsMax\9.0\MAX-1:409\AdLM>nul
if not %ERRORLEVEL%==0 goto :end_copy_max9

reg query HKEY_LOCAL_MACHINE\SOFTWARE\Autodesk\3dsMax\9.0\MAX-1:409\AdLM /v InfoPath | find "InfoPath">max9_dir.txt
if not %ERRORLEVEL%==0 goto :end_copy_max9

set /p MAX9_DIR=<max9_dir.txt

set MAX9_DIR=%MAX9_DIR:~20,-1%

set cmd_copy=copy "bin\Release 9\IrrColladaMax.dle" "%MAX9_DIR%\plugins"
@echo %cmd_copy%
%cmd_copy%>nul

set cmd_copy=copy "External\DirectX\Bin\D3DX9_37.dll" "%MAX9_DIR%"
@echo %cmd_copy%
%cmd_copy%>nul

set cmd_copy=copy "..\BatchExporter\batchexporter.ms" "%MAX9_DIR%\Scripts"
@echo %cmd_copy%
%cmd_copy%>nul

set cmd_copy=copy "..\AnimationSelector\animationselector.ms" "%MAX9_DIR%\Scripts"
@echo %cmd_copy%
%cmd_copy%>nul

set cmd_copy=copy "..\AnimationSelector\animselector.mcr" "%MAX9_DIR%\Scripts"
@echo %cmd_copy%
%cmd_copy%>nul

set cmd_copy=copy "..\UserPropertiesEditor\UserPropertiesEditor.ms" "%MAX9_DIR%\Scripts"
@echo %cmd_copy%
%cmd_copy%>nul

if not %ERRORLEVEL%==0 goto :error

echo copied plugin to max9
goto :end_copy_max9

:error

echo fail
pause

:end_copy_max9

