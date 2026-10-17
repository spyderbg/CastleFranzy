REM This file used to make document for GLXPlayer£¨java£© API
REM For configuration, please view 
@echo OFF
echo Start document generation for java
rem remove old doc.
del /f /q j2me_doc\*.*
..\tools\doxygen_1.5.8\bin\doxygen doxygen_java_config

echo Generation completed.
pause
