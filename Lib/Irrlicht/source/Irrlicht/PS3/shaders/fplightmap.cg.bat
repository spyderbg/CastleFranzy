@echo off
@echo Performing Custom Build Step
D:/usr/local/cell\host-win32\Cg\bin\sce-cgc -quiet -profile sce_fp_rsx -o "fpLIGHTMAP.fpo.tmp" "fpLIGHTMAP.cg"
D:/usr/local/cell\host-win32\Cg\bin\cgnv2elf "fpLIGHTMAP.fpo.tmp" "fpLIGHTMAP.fpo"
del "fpLIGHTMAP.fpo.tmp"
