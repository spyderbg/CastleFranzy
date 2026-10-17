@echo off

cls 

call subst /D P:		> NUL
call subst P: %~dp0.	> NUL

echo Map drive:
subst