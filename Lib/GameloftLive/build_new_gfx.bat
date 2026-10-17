@echo off
echo **************************************
echo *** build NEW GFX
echo **************************************

call config.bat

SET RES_DIR=%PRJ_DIR%\resource\newgfx
SET DATA_DIR=%RES_DIR%\temp

md %DATA_DIR%

pushd %RES_DIR%
rem %TEXCONV% -d %RES_DIR% -bmp

%IMG_CONVERT% -s2n -bmp -tga %RES_DIR% %RES_DIR%
for %%i in (*.tga) do (
	%TEXCONV% .\%%i -32
)

%PYTHON25% %SPRITE2CONSTANTS% sprite_constants.h
move /y sprite_constants.h %PRJ_DIR%\src\NEW_STYLE_Game\
%AURORA_GT% export.sprcmd

move *.gtx %DATA_DIR%\
move *.bsprite %DATA_DIR%\

md %DATA_DIR%\font
move %DATA_DIR%\font*.* %DATA_DIR%\font
%respack% -s %DATA_DIR%\font -d GLL_font.pak -zlib
rem copy /y GLL_font.pak %PRJ_DIR%\platform\Win32\GLLogin\demo\data\
copy /y GLL_font.pak %PRJ_DIR%\platform\Win32\GLLive\demo\data\
move /y GLL_font.pak %PRJ_DIR%\platform\iPhone\data\gll_font.pak
rd /s/q %DATA_DIR%\font

%respack% -s %DATA_DIR% -d GLL_res.pak -zlib
rem for win32
rem copy /y GLL_res.pak %PRJ_DIR%\platform\Win32\GLLogin\demo\data\
copy /y GLL_res.pak %PRJ_DIR%\platform\Win32\GLLive\demo\data\
rem for mac
copy /y GLL_res.pak %PRJ_DIR%\platform\iPhone\data\gll_res.pak
rem /y copy others\oconf.bar %PRJ_DIR%\platform\iPhone\data\

del GLL_res.pak
rd /s /q %DATA_DIR%
popd
