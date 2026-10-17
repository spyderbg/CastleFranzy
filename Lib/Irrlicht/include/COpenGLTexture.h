// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_OPEN_GL_TEXTURE_H_INCLUDED__
#define __C_OPEN_GL_TEXTURE_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "CCommonGLTexture.h"

namespace irr
{
namespace video
{

class COpenGLDriver;
//! OpenGL texture.
class COpenGLTexture : public CCommonGLTexture
{
public:

	//! constructor
	COpenGLTexture(IImage* surface, const char* name, COpenGLDriver* driver=0);
	//! FrameBufferObject constructor
	COpenGLTexture(const core::dimension2d<s32>& size, const char* name, COpenGLDriver* driver=0, bool useStencil=false, bool colorTexture=true, bool depthTexture=false);

#ifdef _IRR_ENABLE_NATIVE_TEXTURE_FORMAT_
	//! constructor for "native/PVR" formats
	COpenGLTexture(io::IReadFile* file, const char* name, COpenGLDriver* driver=0);
#endif

	//! destructor
	virtual ~COpenGLTexture();

	//! returns driver type of texture (=the driver, that created it)
	virtual E_DRIVER_TYPE getDriverType() const;

	//! returns color format of texture
	virtual ECOLOR_FORMAT getColorFormat() const;
};

} // end namespace video
} // end namespace irr

#endif
#endif // _IRR_COMPILE_WITH_OPENGL_

