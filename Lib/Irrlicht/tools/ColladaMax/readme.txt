--------------------------------------------------------------------------------
1) How to use
--------------------------------------------------------------------------------

	- Copy the ColladaMax plugin to your max plugins directory ( see .\bin )
	- Copy .\External\DirectX\Bin\D3DX9_37.dll to your max directory ( or install the latest DirectX runtime )
	
--------------------------------------------------------------------------------
2) Export Options
--------------------------------------------------------------------------------

Standard Options
----------------

- Bake Matrices : The transforms of each scene node and their animations will be exported as matrices. This is used mostly for interoperability, e.g. with another DCC tool. Note that Bipeds automatically have their matrices baked

- Relative Paths : Whether the absolute or relative file path will be written to the COLLADA document. Mostly impacts texture and shader filenames.

Path Options
------------

- Remove Paths : Removes paths from XRefs.  Overrides Relative Paths option.

- Replace Paths : Replaces paths from XRefs with the provided path.  '\' or '/' are accepted path separators.  All '\' are converted to '/' upon export.  Leading and trailing white spaces are trimmed.  If no path separator is present at the end of the path, '/' is automatically appended.  White spaces in paths are converted to %20 upon export.  Overrides Relative Paths option.

Geometry
--------

- Normals : Whether to export the normals of meshes.

- Triangulate : Whether to export the tessellation of a mesh as triangles or N-sided polygons. In the COLLADA document, this will generate either the <triangles> or the <polylist> element. Since 3ds Max doesn’t support holes, the 3ds Max exporter will never write the <polygons> element. It is recommended to keep this option on, as N-sided polygon export is still experimental.

- TriangleStrips : Exports the tessalation of a mesh as triangle strips.  Use in conjunction with the Triangulate option.

- XRefs : Whether externally referenced meshes will be exported as external references. If this option is checked off, they will simply be dropped.

- Tangents/Binormals : Exports the texture tangents and binormals for all given map channels of meshes, as well as their indices within the mesh tessellation.

- Deindex Vertex Data : Rearranges vertices indexes so that each Vertices will reference the corresponding Position, Normal, Texcoord with the same index number. The size of source for position, normal, and texcoord might increase.

Animation
---------

- Enable Export : Whether to export animations or not.

- Sample animation : Enables time-sampling the animated parameters, within the Start and End time interval. One sample is taken per time unit. If this option is disabled, ColladaMax will attempt to export the animated parameters as key-frames. Animated bipeds or objects involving IK are automatically sampled, since COLLADA doesn’t have direct support for these features yet.  You should not modify the sampling range unless you know what you are doing.

- Create Clip(s) : Creates the specified animation clips.  You need to manually select the clips you want to export in the dialog box.  All animations that are not referenced by an animation clip will be dropped if this option is enabled.  Some animation clips are automatically created for you :

	AUTO_SceneAnims_Range ==> The range of the animation keys in the scene
	AUTO_Current_Range ==> The currently displayed animation range in Max
	AUTO_AnimClips_Range ==> The range covered by all the user created animation clips
	AUTO_Total_Range ==> The range covered by the other three automatically created animation clips

- Remove Unreferenced Animation Keys : If enabled ( and if Create Clip(s) is enabled as well ), all animation keys that not referenced by any animation clips will be removed.

Skin
----

- Remove Null Weights : Removes all null weights from skin data.

- Remove Duplicate Skin Weights : Removes all duplicate skin weights from skin data.

Target Coordinate System
------------------------

- Y Up, Z Up : Transforms scene data according to the specified coordinate system ( Z Up does nothing since Max's up axis is Z ).

Libraries to Export
-------------------

- Deselected libraries in this section will be removed from the exported file.

- Available libraries

	- COLLADA Core :
	
		- Geometries :      <library_geometries>
		- Controllers :     <library_controllers>
		- Lights :          <library_lights>
		- Cameras :         <library_cameras>
		- Animations :      <library_animations>
		- Animation Clips : <library_animation_clips>
		- Nodes :           <library_nodes>
		- Visual Scenes :   <library_visual_scenes>
		- Scene :           <scene>
		- Extra Libs :      <extra>
		
	- COLLADA FX :
	
		- Effects :      <library_effects>
		- Images :       <library_images>
		- Materials :    <library_materials>
		- Force Fields : <library_force_fields>
		
	- COLLADA Physics :
	
		- Materials : <library_physics_materials>
		- Models :    <library_physics_models>
		- Scenes :    <library_visual_scenes>
	
	
	

See www.feelingsoftware.com/content/view/65/79/lang,en/index.php for other info.
	
--------------------------------------------------------------------------------
3) User Defined Animation Clips
--------------------------------------------------------------------------------

	- If you intend to export user defined animation clips, copy animationselector.ms to your max Scripts directory ( see .\Scripts )
	- To launch the animation selector, select an object in your scene and run animationselector.ms.  Note that you can define a shortcut for that action in Customize/Customize User Interface.  Look for the "Animation Selector" action.
	- To create a clip, modify the current animation range, enter a name for your clip and press Enter. 
	- When exporting your scene, enable CreateClip and select the clips you want to export.  Note that for now only the clips of the last object on which animation selector was run will be present in the export dialog.  Note that animationselector.ms needs to be run at least once for the user defined animation clips to appear in the export dialog.  These limitations will be dealt with in the coming weeks.
		

--------------------------------------------------------------------------------
4) How to build
--------------------------------------------------------------------------------

Dependencies
------------

	- 3dsMax
	- MaxSDK installed in your 3dsMax directory
		==> MaxSDK installer note: Choose your max directory as the installation directory, not <maxdir>\maxsdk
	- If building using VC 2008 Express, you also need to install the Microsoft Platform SDK 
	- VersionManager tool ( see post-build events in .vcproj )
	
Environment Variables
---------------------

	- MAX_PATH<version> must point to your 3dsmax directory, where <version> is either 7, 8, 9 or 2008 depending on the version for which you are building the .dle
	
Solutions
---------

	- VC 2005             ==> ColladaMax/ColladaMax.sln
	- VC 2008 Express     ==> ColladaMax/ColladaMax2008.sln
	
	==> If building using VC 2008 Express, add <MSPlatformSDK>\include\mfc to your Visual Studio include directories
	
	==> IMPORTANT : VC 2008 Express solution is no longer maintained. 
	
Build configurations
--------------------

	- Maintained configurations :
		- Debug 9
		- Release 9
	
	- Max8 binaries are simply copied from the Max9 ones in post-build.  There are currently no differences between the Max9 and Max8 plugin versions.  I would need to build with the Max8 SDK or do some slight modifications to the Max8 build configuration to provide a true Max8 plugin.  However, the Max9 plugin has not caused any issues so far when used with Max8.	
Notes
-----

	- Upon a successful build 
		==> the .dle is copied to your max plugins directory, overwriting the existing Collada Max exporter.
		==> the DirectX dll is copied to your max directory
	- Resource editing is not allowed in VC Express.  I recommend using VC 2005 if you need to modify resources ( i.e. GUI widgets ).
	
Version Numbers
---------------

	- After each build, the version number of the plugin is incremented.  This means that building the release version and then the debug version of the lib will yield libraries with two different version numbers.