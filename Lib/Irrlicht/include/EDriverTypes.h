//-*-c++-*-
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __E_DRIVER_TYPES_H_INCLUDED__
#define __E_DRIVER_TYPES_H_INCLUDED__

namespace irr
{
namespace video
{

//! An enum for all types of drivers the Irrlicht Engine supports.
enum E_DRIVER_TYPE
{
	//! Null driver, useful for applications to run the engine without
	//! visualisation. The null device is able to load textures, but does not
	//! render and display any graphics.
	EDT_NULL = 0,

	//!
	EDT_OPENGL_FAMILY = 1 << 3,

	//! OpenGL device, available on most platforms.  Performs hardware
	//! accelerated rendering of 3D and 2D primitives.
	EDT_OPENGL = (unsigned int)(EDT_OPENGL_FAMILY | 1),

	//! OpenGL ES1 device, available on some embedded devices.  Performs hardware
	//! accelerated rendering of 3D and 2D primitives.
	EDT_OPENGLES1 = (unsigned int)(EDT_OPENGL_FAMILY | 2),

	//! OpenGL ES2 device, available on some embedded device.  Performs hardware
	//! accelerated rendering of 3D and 2D primitives.
	EDT_OPENGLES2 = (unsigned int)(EDT_OPENGL_FAMILY | 3),

	//! PS3 device, only available on PS3 platform.  Performs hardware
	//! accelerated rendering of 3D and 2D primitives using OpenGL ES 1.0 and Cg
	EDT_PS3 = (unsigned int)(EDT_OPENGL_FAMILY | 4),

	//! Direct3D8 device, only available on Win32 platforms.  Performs hardware
	//! accelerated rendering of 3D and 2D primitives.
	EDT_DIRECT3D8 = 2 << 3,

	//! Direct3D 9 device, only available on Win32 platforms.  Performs hardware
	//! accelerated rendering of 3D and 2D primitives.
	EDT_DIRECT3D9 = 3 << 3,

	//! Wii device, only available on Wii platform.  Performs hardware
	//! accelerated rendering of 3D and 2D primitives.
	EDT_WII = 4 << 3,

	// masks -------------------------------------------------------------------

	//!
	EDT_FAMILY_MASK = ~7
};

//Antialiasing setting
enum E_FULLSCREEN_ANTIALIASING_SETTING
{
	EAA_NONE = 0,
	EAA_2X,
		EAA_4X,
        EAA_4XR,
};

//Buffering mode setting
enum E_BUFFERING_MODE
{
	EBM_DOUBLE = 0,
	EBM_TRIPLE,
};

} // end namespace video
} // end namespace irr


#endif

