@echo off

call %~dp0\..\..\..\config.bat
call %~dp0\..\..\..\setenv.bat

rem ///////////////////////////////////////

set PVR_TOOL=%PROJECT_PATH%\Tools\PVRTexTool\PVRTexTool.exe

for %%f in (*.tga) do (

  %PVR_TOOL% -i%%f -q1 -m -nt -fOGLPVRTC4 -pvrtciterations8 -p -o%%~nf -yflip0
  
)