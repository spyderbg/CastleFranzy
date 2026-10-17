@echo off

rem **************************************************************
rem 		Project specific paths
rem **************************************************************

set DATA_BUILD_FLOW=Strings;Fonts;Sprites;Scenes;Sounds;Effects

rem **************************************************************
rem 		Create output directories
rem **************************************************************
	
if not exist %HEADERS_OUT_PATH% (
	mkdir %HEADERS_OUT_PATH%
)
if not exist %OUTPUT_PATH% (
	mkdir %OUTPUT_PATH%
)

