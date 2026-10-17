//-*-c++-*-
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __E_MATERIAL_FLAGS_H_INCLUDED__
#define __E_MATERIAL_FLAGS_H_INCLUDED__

namespace irr
{
namespace video
{

//! Material flags
enum E_MATERIAL_FLAG
{
	//! Flat or Gouraud shading? Default: true
	EMF_GOURAUD_SHADING = 1,

	//! Will this material be lighted? Default: true
	EMF_LIGHTING = EMF_GOURAUD_SHADING << 1,

	//!
	EMF_PER_VERTEX_MATERIAL_COLOR = EMF_LIGHTING << 1,

	//! Normalizes normals.You can enable this if you need to scale a dynamic
	//! lighted model. Usually, its normals will get scaled too then and it will
	//! get darker. If you enable the EMF_NORMALIZE_NORMALS flag, the normals
	//! will be normalized again, and the model will look as bright as it
	//! should.
	EMF_NORMALIZE_NORMALS = EMF_PER_VERTEX_MATERIAL_COLOR << 1,

	//! Is fog enabled? Default: false
	EMF_FOG_ENABLE = EMF_NORMALIZE_NORMALS << 1,

 	//! This is not a flag, but a value indicating the greatest bit mask used
 	//! for critical flags.
	// See comment below.
	EMF_LAST_CRITICAL_FLAG = EMF_FOG_ENABLE,

	//! Bit mask representing flags that are more important when sorting.
	// See comment below.
	EMF_CRITICAL_MASK = (unsigned int)((EMF_LAST_CRITICAL_FLAG << 1) - 1),

	//--------------------------------------------------------------------------
	// Above this line are flags that eventually should be part of material
	// renderers because they affect the configuration of the rendering pipeline
	// (and on modern GPUs, including PS3 and XBox360, they can cause costly
	// shader changes).
	//
	// Under the line below are flags related to raster operations that are
	// cheaper to change. Eventually, blending should fall here.
	//--------------------------------------------------------------------------

	//! Draw as wireframe or filled triangles? Default: false
	EMF_WIREFRAME = EMF_LAST_CRITICAL_FLAG << 1,

	//! Draw as point cloud or filled triangles? Default: false
	EMF_POINTCLOUD = EMF_WIREFRAME << 1,

	//! Is the ZBuffer enabled? Default: true
	EMF_ZBUFFER = EMF_POINTCLOUD << 1,

	//! May be written to the zbuffer or is it readonly. Default: true
	//! This flag is ignored, if the material type is a transparent type.
	EMF_ZWRITE_ENABLE = EMF_ZBUFFER << 1,

	//! Is backface culling enabled? Default: true
	EMF_BACK_FACE_CULLING = EMF_ZWRITE_ENABLE << 1,

	//! Is frontface culling enabled? Default: false
	EMF_FRONT_FACE_CULLING = EMF_BACK_FACE_CULLING << 1,

	//! does the material use the vertices alpha
	EMF_2D_USE_VERTEX_ALPHA = EMF_FRONT_FACE_CULLING << 1,

	//! does the material use the texture's alpha channel
	EMF_2D_USE_TEXTURE_ALPHA = EMF_2D_USE_VERTEX_ALPHA << 1,

 	//! This is not a flag, but a value indicating the greatest bit mask used
 	//! for all flags.
 	EMF_LAST_MATERIAL_FLAG = EMF_2D_USE_TEXTURE_ALPHA
};

} // end namespace video
} // end namespace irr

#endif // __E_MATERIAL_FLAGS_H_INCLUDED__

