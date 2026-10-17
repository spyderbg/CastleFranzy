// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_COMMON_GL_TEXTURE_H_INCLUDED__
#define __C_COMMON_GL_TEXTURE_H_INCLUDED__

#include "IrrCompileConfig.h"
#if defined(_IRR_COMPILE_WITH_OPENGL_) || defined(_IRR_COMPILE_WITH_OPENGL_ES_) || defined(_IRR_COMPILE_WITH_PS3_)

#include "CCommonGLHeaders.h"
#include "ITexture.h"
#include "IImage.h"

namespace irr
{
namespace io
{
	class IReadFile;
} // end namespace io
namespace video
{

class CCommonGLDriver;
//! OpenGL texture.
class CCommonGLTexture : public ITexture
{
protected:
	//! Constructor for child initialisation when creating a specialized constructor
	CCommonGLTexture(const char* name, CCommonGLDriver* driver);

public:

	//! constructor
	CCommonGLTexture(IImage* surface, const char* name, CCommonGLDriver* driver=0);

	//! FrameBufferObject constructor
	CCommonGLTexture(const core::dimension2d<s32>& size, const char* name, CCommonGLDriver* driver=0, bool useStencil=false, bool colorTexture=true, bool depthTexture=false);

#ifdef _IRR_ENABLE_NATIVE_TEXTURE_FORMAT_
	//! constructor for "native/PVR" formats
	CCommonGLTexture(io::IReadFile* file, const char* name, CCommonGLDriver* driver=0);
#endif

	//! destructor
	virtual ~CCommonGLTexture();

	//! lock function
	virtual void* lock(bool readOnly = false);

	//! unlock function
	virtual void unlock();

	//! Returns original size of the texture (image).
	virtual const core::dimension2d<s32>& getOriginalSize() const;

	//! Returns size of the texture.
	virtual const core::dimension2d<s32>& getSize() const;

	//! returns driver type of texture (=the driver, that created it)
	virtual E_DRIVER_TYPE getDriverType() const;

	//! returns color format of texture
	virtual ECOLOR_FORMAT getColorFormat() const = 0;

	//! returns pitch of texture (in bytes)
	virtual u32 getPitch() const;

	//! return open gl texture name
	GLuint getOpenGLTextureName() const;

	//! return open gl texture name
	GLuint getOpenGLDepthTextureName() const;

	//! Regenerates the mip map levels of the texture. Useful after
	//! locking and modifying the texture
	virtual void regenerateMipMapLevels();

	//! is it a FrameBufferObject?
	bool isFrameBufferObject() const;

	//! Bind RenderTargetTexture
	void bindRTT();

    void copyPixelsFromColorBuffer(const irr::core::rect<irr::s32>& bounder);
	//! Unbind RenderTargetTexture
	void unbindRTT();

	//! sets whether this texture is intended to be used as a render target.
	void setIsRenderTarget(bool isTarget);

	void bind() const;
	void updateParameters() const;

	//! Returns whether the texture's content is valid.
	bool isValid() const;

protected:
	#ifdef _IRR_COMPILE_WITH_THREAD_FLUSH
    virtual bool flushVRM();
    #endif
	//! OpenGL implementation dependant code to process color format
	void processColorFormat( ECOLOR_FORMAT format, GLint& InternalFormat, GLenum& PixelFormat, GLenum& PixelType, bool& compressed);

	//! get the desired color format based on texture creation flags and the input format.
	ECOLOR_FORMAT getBestColorFormat(ECOLOR_FORMAT format);

	//! convert the image into an internal image with better properties for this driver.
	void getImageData(IImage* image);

	//! copies the the texture into an open gl texture.
	//! \param: newTexture is true if method is called from a newly created texture for the first time. Otherwise call with false to improve memory handling.
	void copyTexture(bool newTexture=true);

	//! returns the size of a texture which would be optimal for rendering
	inline s32 getTextureSizeFromSurfaceSize(s32 size) const;

	core::dimension2d<s32> ImageSize;
	CCommonGLDriver* Driver;
	IImage* Image;

	GLuint TextureName;
	GLuint DepthTextureName;
	GLint InternalFormat;
	GLenum PixelFormat;
	GLenum PixelType;

	GLuint ColorFrameBuffer; // for FBO path
	GLuint DepthRenderBuffer; // for FBO path
	GLuint StencilRenderBuffer; // for FBO path

	bool AutomaticMipmapUpdate;
	bool UseStencil;
	bool ReadOnlyLock;
};

inline void CCommonGLTexture::bind() const
{
	glBindTexture(GL_TEXTURE_2D, TextureName);
	if (isDirty())
	{
		updateParameters();
	}
}

} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_OPENGL_ || _IRR_COMPILE_WITH_OPENGL_ES_
#endif // __C_COMMON_GL_TEXTURE_H_INCLUDED__

