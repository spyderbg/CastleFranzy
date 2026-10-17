@echo off
title make res for GL Live
call config.bat

rem make strings
call build_strings.bat

rem make gfx
call build_gfx.bat

PAUSE
