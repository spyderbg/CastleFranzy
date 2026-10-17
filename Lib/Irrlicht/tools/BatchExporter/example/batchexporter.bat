@echo off
setlocal

set MAXBIN="C:\Program Files\Autodesk\3ds Max 9\3dsmax.exe"
set BATCHEXPORTER_SCRIPT=%~dp0%..\batchexporter.ms
set BATCHEXPORTER_CFG=%~dp0%batchexporter.cfg

REM Prefixing cfg file to prevent Max from attempting to open the file
set ARG_TRICK=_

REM This command line must be <= 260 characters otherwise Max9 crashes
%MAXBIN% -U MAXScript %BATCHEXPORTER_SCRIPT% %ARG_TRICK%%BATCHEXPORTER_CFG%