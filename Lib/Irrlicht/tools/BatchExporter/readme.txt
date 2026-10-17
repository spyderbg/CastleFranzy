Coupling with IrrColladaMax
---------------------------

	Always make sure to get the latest IrrColladaMax.dle before getting batchexporter.ms.  batchexporter.ms requires functionalities provided by the plugin to work properly.
	
To test
-------

- Copy the latest IrrColladaMax.dle to your Max plugins directory
- in example/batchexporter.bat, modify the absolute path to your Max directory 
- modify the paths in example/batchexporter.cfg
- run example/batchexporter.bat

Export options
--------------

You can use the "Display Batch Export Option String" in the ColladaMax export dialog to print out the the batch export option string the corresponds to the currently selected options.

- LibrariesToStrip

The librariesToStrip option allows the user to specify exactly which library he wants to strip from the exported file.  Each library has an associated flag value :

	STRIP_LIB_ANIMS            = 0x00000001,
	STRIP_LIB_ANIMCLIPS        = 0x00000002,
	STRIP_LIB_CAMERAS          = 0x00000004,
	STRIP_LIB_CONTROLLERS      = 0x00000008,
	STRIP_LIB_GEOMETRIES       = 0x00000010,
	STRIP_LIB_LIGHTS           = 0x00000020,
	STRIP_LIB_NODES            = 0x00000040,
	STRIP_LIB_VISUALSCENES     = 0x00000080,

	// COLLADA FX
	STRIP_LIB_EFFECTS          = 0x00000100,
	STRIP_LIB_IMAGES           = 0x00000200,
	STRIP_LIB_MATERIALS        = 0x00000400,

	// COLLADA PHYSICS
	STRIP_LIB_FORCEFIELDS      = 0x00001000,
	STRIP_LIB_PHYSICSMATERIALS = 0x00002000,
	STRIP_LIB_PHYSICSSCENES    = 0x00004000,
	STRIP_LIB_PHYSICSMODELS    = 0x00008000,

	STRIP_LIB_EXTRALIBS        = 0x00010000,
	STRIP_LIB_SCENE            = 0x00020000,

	STRIP_LIB_ALL              = 0xffffffff

These flag values are defined in ColladaMax\FCollada\FCDocument\FCDocument.h.

- animClipOptions

The animation clip(s) you want to export :

	UserClips	
		All the user defined animation clips.  
		Falls back to AUTO_SceneAnims_Range if no user defined animationc clips are present.
		
	AUTO_AnimClips_Range 
		One clip corresponding to the range covered by all user defined animation clips.  
		Falls back to AUTO_SceneAnims_Range if no user defined animationc clips are present.
	
	AUTO_SceneAnims_Range 
		One clip corresponding to the range covered by all the animation keys in the exported scene.
		Falls back to AUTO_Current_Range if no animation keys are present in the scene.
		
	AUTO_Current_Range
		One clip corresponding to Max's current display range ( not very useful for batch export ).
		Always defined, no fallback mechanism.
		
	AUTO_Total_Range
		One clip corresponding to the range covered by the three auto clips defined above ( not very useful for batch export ).
		Always defined, no fallback mechanism.

The default value for this option is UserClips.
Option value is case insensitive.

- cubicInterpolation

The interpolation method used for cubic animation curves :

	2 : Bezier interpolation
	3 : Hermite interpolation

See ColladaMax/FCollada/FUtils/FUDaeEnum.h ==> FUDaeInterpolation::Interpolation enum for most up to date values.  Only supported cubic interpolation enumeration values are valid for this option, namely BEZIER ( 2 ) and HERMITE ( 3 ).

- geometryPositionFormat
  geometryNormalFormat
  geometryTextureFormat
  geometryColorFormat

These options are format suggestions for binary geometry channel data which the Collada Converter will attempt to honor.  In some cases the Collada Converter will not be able to convert the geometry channel data to the suggested format and will force a specific format for a channel.

Here are the currently supported values for specific channels ( strings, case insensitive ) :

	- geometryPositionFormat :
	
		SHORT
		FLOAT
		
	- geometryNormalFormat :
	
		BYTE
		SHORT
		FLOAT
		
	- geometryTextureFormat :
	
		SHORT
		FLOAT
		
	- geometryColorFormat :
	
		UNSIGNED_BYTE
