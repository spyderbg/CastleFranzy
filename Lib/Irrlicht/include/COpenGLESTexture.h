// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_OPEN_GL_TEXTURE_H_INCLUDED__
#define __C_OPEN_GL_TEXTURE_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_OPENGL_ES_

#include "CCommonGLTexture.h"

namespace irr
{
namespace video
{

class COpenGLESDriver;
//! OpenGL texture.
class COpenGLESTexture : public CCommonGLTexture
{
public:

	//! constructor
	COpenGLESTexture(IImage* surface, const char* name, COpenGLESDriver* driver=0);

#ifdef _IRR_ENABLE_NATIVE_TEXTURE_FORMAT_
	//! constructor for "native" formats
	COpenGLESTexture(io::IReadFile* file, const char* name, COpenGLESDriver* driver=0);
#endif

	//! FrameBufferObject constructor
	COpenGLESTexture(const core::dimension2d<s32>& size, const char* name, COpenGLESDriver* driver=0, bool useStencil=false);

	//! destructor
	virtual ~COpenGLESTexture();

	//! returns driver type of texture (=the driver, that created it)
	virtual E_DRIVER_TYPE getDriverType() const;

	//! returns color format of texture
	virtual ECOLOR_FORMAT getColorFormat() const;
};

} // end namespace video
} // end namespace irr

#endif
#endif // _IRR_COMPILE_WITH_OPENGL_ES_

