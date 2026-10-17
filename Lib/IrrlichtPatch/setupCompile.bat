set source=%~dp0
set target=%~dp0..\Irrlicht

copy %source%\include\*.h %target%\include\

copy %source%\source\Irrlicht\*.cpp %target%\source\Irrlicht\
copy %source%\source\Irrlicht\*.vcproj %target%\source\Irrlicht\

rem Collada converter tool
copy %source%\tools\ColladaConverter\*.cpp %target%\tools\ColladaConverter\src\
copy %source%\tools\ColladaConverter\*.h %target%\tools\ColladaConverter\src\
copy %source%\tools\ColladaConverter\*.exe %target%\tools\ColladaConverter\bin\Release\

pause