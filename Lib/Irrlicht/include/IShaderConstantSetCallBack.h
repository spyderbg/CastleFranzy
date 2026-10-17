// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_SHADER_CONSTANT_SET_CALLBACT_H_INCLUDED__
#define __I_SHADER_CONSTANT_SET_CALLBACT_H_INCLUDED__

#include "IReferenceCounted.h"

namespace irr
{
namespace video
{
	class IMaterialRendererServices;

//! Interface making it possible to set constants for gpu programs every frame.
/** Implement this interface in an own class and pass a pointer to it to one of
the methods in IGPUProgrammingServices when creating a shader. The
onSetConstants method will be called every frame now. */
class IShaderConstantSetCallBack : public virtual IReferenceCounted
{
public:

	//! Called to let the callBack know the used material (optional method)
	/**
	 \code
	class MyCallBack : public IShaderConstantSetCallBack
	{
		const video::SMaterial *UsedMaterial;

		onSetMaterial(const video::SMaterial& material)
		{
			UsedMaterial=&material;
		}

		onSetConstants(IMaterialRendererServices* services, s32 userData)
		{
			services->setVertexShaderConstant("myColor", reinterpret_cast<f32*>(&UsedMaterial->color), 4);
		}
	}
	\endcode
	*/
	virtual void onSetMaterial(const SMaterial& material) { }

	//! Called by the engine when the vertex and/or pixel shader constants for an material renderer should be set.
	/**
	Implement the IShaderConstantSetCallBack in an own class and implement your own
	onSetConstants method using the given IMaterialRendererServices interface.
	Pass a pointer to this class to one of the methods in IGPUProgrammingServices
	when creating a shader. The onSetConstants method will now be called every time
	before geometry is being drawn using your shader material. A sample implementation
	would look like this:
	\code
	virtual void onSetConstants(video::IMaterialRendererServices* services, s32 userData)
	{
		video::IVideoDriver* driver = services->getVideoDriver();

		// set clip matrix at register 4
		core::matrix4 worldViewProj(driver->getTransform(video::ETS_PROJECTION));
		worldViewProj *= driver->getTransform(video::ETS_VIEW);
		worldViewProj *= driver->getTransform(video::ETS_WORLD);
		services->setVertexShaderConstant(&worldViewProj.M[0], 4, 4);
		// for high level shading languages, this would be another solution:
		//services->setVertexShaderConstant("mWorldViewProj", worldViewProj.M, 16);

		// set some light color at register 9
		video::SColorf col(0.0f,1.0f,1.0f,0.0f);
		services->setVertexShaderConstant(reinterpret_cast<const f32*>(&col), 9, 1);
		// for high level shading languages, this would be another solution:
		//services->setVertexShaderConstant("myColor", reinterpret_cast<f32*>(&col), 4);
	}
	\endcode
	\param services: Pointer to an interface providing methods to set the constants for the shader.
	\param userData: Userdata int which can be specified when creating the shader.
	*/
	virtual void onSetConstants(IMaterialRendererServices* services, s32 userData) = 0;

	//! Called to let the callBack unset the material (optional method)
	virtual void onUnsetMaterial() { }

};


} // end namespace video
} // end namespace irr

#endif

