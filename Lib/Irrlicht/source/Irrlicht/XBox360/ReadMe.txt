========================================================================
    STATIC LIBRARY : XBox360 Project Overview
========================================================================
[overall]

Although Xbox 360's graphic system comes from the DirectX, but it is not DirectX on PC.

from XDK said

The Xbox 360TM features an implementation of Microsoft? DirectX? 9.0 that has been optimized for the Xbox 360 hardware. The DirectX 9.0 application programming interface (API) for Xbox 360 is similar to the DirectX 9.0 API for Microsoft Windows? : both APIs contain many of the same interfaces, methods, and functions. However, the Xbox 360 version does not support every DirectX 9.0 feature that the Windows version supports. The Xbox 360 version also has some features that the Windows version lacks

So, Xbox 360 Driver maintains seperatedly. with _IRR_XBOX_PLATFORM_ Macro.

Note: 
---++++ AntiAliasing
Xbox 360 Dirver will ignore the SIrrlichtCreationParameters::AntiAlias, the driver alway using antialiasing respecting the XDK Certification rules.

User can set the SIrrlichtCreationParameters::AntiAliasSetting as video::EAA_2X or video::EAA_4X the decide which multisampled antialiasing type you prefered.

Because the TCR, if you set AntiAliasSetting  as video::EAA_NONE, the driver will auto change the setting to video::EAA_2X to meet rules.

---++++ Screen Size
Due to antialiasing just was implemented by some XDK recommended screend size. so, currently, the Xbox 360 Driver only support the below screen size and antialiasing mode:

   * 640*480 (EAA_4X)
   * 1280*720 (EAA_2X,EAA_4X)
   * 1920*1080 (EAA_2X,EAA_4X)

If you try to initlize irrlicht device with other screen size, the creating will failed.


[NOTICE]
Xbox 360 porting's main MACRO in project is _IRR_XBOX_PLATFORM_

Currently, this porting only works 
without:
_IRR_USE_RIGHT_HAND_CONVENTION_

[DONE]
+add freetype porting.  [porint done] [testing is TODO]
+add XBOX 360 porting. 
+Mark all TODO or Need review place in porting codeing with "//TODO:XBOX 360" and "//HACK:XBOX360"
+Make HelloWorld work well on XBOX360 

* review the HelloWorld Example codes for XBOX360 and commit.
* refine the materials
+ Add 2x&4x MSAA(multisample antialiasing) with EDRAM&Tiling technique 
+ implement Texture0 matrix and Texutre stage 0 in shader
* fix the draw2DImage in XBOX360 driver

[TODO]
 * fix the HelloWorld model rendering bug(new bug)
 * Porting more Materials with sequence by "//TODO:XBOX 360 Material: priority: " mark
 * Fix the bug when create MipMap Texture manually. mark as "//HACK:XBOX 360 Texture" or "//TODO:XBOX 360 Texture"
 * Test the freetype porting.
 * Render Target in EDRAM&Memory 
 * implement the joy stick on XBOX360
 * implement Texture 1,2,3's matrix and Texutre stage 1,2,3 in shader
 * Make the irrlicht lib build for Release work
 * Make the irrlicht build (debug&& release) as DLL, for XBOX360


