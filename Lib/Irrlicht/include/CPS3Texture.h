// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_PS3_TEXTURE_H_INCLUDED__
#define __C_PS3_TEXTURE_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_PS3_
#include "CCommonGLTexture.h"

namespace irr
{
namespace video
{

class CPS3Driver;
//! OpenGL texture.
class CPS3Texture : public CCommonGLTexture
{
public:
    CPS3Texture(const core::dimension2d<s32>& size, int fmt, const char* name, CPS3Driver* driver);
	//! constructor
	CPS3Texture(IImage* surface, const char* name, CPS3Driver* driver=0);
	//! FrameBufferObject constructor
	CPS3Texture(const core::dimension2d<s32>& size, const char* name, CPS3Driver* driver=0, bool useStencil=false, bool colorTexture=true, bool depthTexture=true);

	//! destructor
	virtual ~CPS3Texture();

	//! returns driver type of texture (=the driver, that created it)
	virtual E_DRIVER_TYPE getDriverType() const;

	//! returns color format of texture
	virtual ECOLOR_FORMAT getColorFormat() const;
};


} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_PS3_
#endif

