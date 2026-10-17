
#include "stdafx.h"

// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "COpenGLESDriver.h"

#ifdef _IRR_COMPILE_WITH_OPENGL_ES_

#include "irrTypes.h"
#include "COpenGLESTexture.h"
#include "COpenGLESDriver.h"
#include "irros.h"
#include "CImage.h"
#include "CColorConverter.h"

#include "irrString.h"
#include "irrProcessBufferHeap.h"

namespace irr
{
namespace video
{

//! constructor for usual textures
COpenGLESTexture::COpenGLESTexture(IImage* origImage, const char* name, COpenGLESDriver* driver)
	: CCommonGLTexture(origImage, name, driver)
{
	#ifdef _DEBUG
	setDebugName("COpenGLESTexture");
	#endif
}

#ifdef _IRR_ENABLE_NATIVE_TEXTURE_FORMAT_
//! constructor for "native" formats
COpenGLESTexture::COpenGLESTexture(io::IReadFile* file, const char* name, COpenGLESDriver* driver)
	: CCommonGLTexture(file, name, driver)
{
	#ifdef _DEBUG
	setDebugName("COpenGLESTexture");
	#endif
}
#endif

//! RTT ColorFrameBuffer constructor
COpenGLESTexture::COpenGLESTexture(const core::dimension2d<s32>& size,
								   const char* name,
								   COpenGLESDriver* driver,
								   bool useStencil)
	: CCommonGLTexture( size, name, driver, useStencil)
{
	#ifdef _DEBUG
	setDebugName("COpenGLESTexture_FBO");
	#endif
}


//! destructor
COpenGLESTexture::~COpenGLESTexture()
{
}

E_DRIVER_TYPE
COpenGLESTexture::getDriverType() const
{
	return EDT_OPENGLES1;
}

//! returns color format of texture
ECOLOR_FORMAT
COpenGLESTexture::getColorFormat() const
{
	switch (InternalFormat)
	{
		case GL_RGBA:
			if (PixelType == GL_UNSIGNED_SHORT_5_5_5_1)
				return ECF_A1R5G5B5;
			else if(PixelType == GL_UNSIGNED_BYTE)
				return ECF_A8R8G8B8;
			else
				return ECF_UNKNOWN;		
			break;
		case GL_RGB:
			if (PixelType == GL_UNSIGNED_SHORT_5_6_5)
				return ECF_R5G6B5;
			else if(PixelType == GL_UNSIGNED_BYTE)
				return ECF_R8G8B8;
			else
				return ECF_UNKNOWN;		
			break;
		case GL_ALPHA:
			return ECF_A8;
			break;
		default:
			return ECF_UNKNOWN;		
			break;
	}
	
//	if (Image)
//		return Image->getColorFormat();
//	else
//		return ECF_UNKNOWN;
}
} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_OPENGL_ES_
