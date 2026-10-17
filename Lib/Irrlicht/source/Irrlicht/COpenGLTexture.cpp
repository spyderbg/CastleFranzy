
#include "stdafx.h"

// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "irrTypes.h"
#include "COpenGLTexture.h"
#include "COpenGLDriver.h"
#include "irros.h"
#include "CImage.h"
#include "CCompressedImage.h"
#include "CColorConverter.h"

#include "irrString.h"
#include "irrProcessBufferHeap.h"

namespace irr
{
namespace video
{

//! constructor for usual textures
COpenGLTexture::COpenGLTexture(IImage* origImage, const char* name, COpenGLDriver* driver)
	: CCommonGLTexture(origImage, name, driver)
{
	#ifdef _DEBUG
	setDebugName("COpenGLTexture");
	#endif
}

//! RTT ColorFrameBuffer constructor
COpenGLTexture::COpenGLTexture(const core::dimension2d<s32>& size,
								const char* name,
								COpenGLDriver* driver,
								bool useStencil,
								bool colorTexture, 
								bool depthTexture)
	: CCommonGLTexture( size, name, driver, useStencil, colorTexture, depthTexture )
{
	#ifdef _DEBUG
	setDebugName("COpenGLTexture_FBO");
	#endif
}


#ifdef _IRR_ENABLE_NATIVE_TEXTURE_FORMAT_
//! constructor for "native" formats
COpenGLTexture::COpenGLTexture(io::IReadFile* file, const char* name, COpenGLDriver* driver)
	: CCommonGLTexture(file, name, driver)
{
	#ifdef _DEBUG
	setDebugName("COpenGLTexture_Native");
	#endif
}
#endif

//! destructor
COpenGLTexture::~COpenGLTexture()
{
}

//! returns driver type of texture (=the driver, that created it)
E_DRIVER_TYPE
COpenGLTexture::getDriverType() const
{
	return EDT_OPENGL;
}

//! returns color format of texture
ECOLOR_FORMAT COpenGLTexture::getColorFormat() const
{
	if (Image)
		return Image->getColorFormat();
	else
		return ECF_A8R8G8B8;
}


} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_OPENGL_
