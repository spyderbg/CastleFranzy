@echo off
@echo Performing Custom Build Step
D:/usr/local/cell\host-win32\Cg\bin\sce-cgc -quiet -profile sce_vp_rsx -o "vpLIGHTMAP.vpo.tmp" "vpLIGHTMAP.cg"
D:/usr/local/cell\host-win32\Cg\bin\cgnv2elf "vpLIGHTMAP.vpo.tmp" "vpLIGHTMAP.vpo"
del "d:\vc\arena\trunk\src\irrlicht\source\Irrlicht\PS3\shaders\vpLIGHTMAP.vpo.tmp"
