#include "stdafx.h"
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_PS3_

#include "irrTypes.h"
#include "CPS3Texture.h"
#include "CPS3Driver.h"
#include "irros.h"
#include "CImage.h"
#include "CCompressedImage.h"
#include "CColorConverter.h"

#include "irrString.h"


namespace irr
{
namespace video
{

// helper function for render to texture
static bool checkFBOStatus(CPS3Driver* Driver);

//! constructor for usual textures
CPS3Texture::CPS3Texture(IImage* origImage, const char* name, CPS3Driver* driver)
	: CCommonGLTexture(origImage, name, driver)
{
	#ifdef _DEBUG
	setDebugName("CPS3Texture");
	#endif
    //MAYBE_RETURN_DELY_FLUSH(this);
}
CPS3Texture::CPS3Texture(const core::dimension2d<s32>& size, int fmt, const char* name, CPS3Driver* driver)
   :CCommonGLTexture(name, driver)
{
    ImageSize = size;
    InternalFormat = fmt;
    glGenTextures(1, &TextureName);
}
/*
void CPS3Texture::flush()
{
    if (Image)
    {
        glGenTextures(1, &TextureName);
        glBindTexture(GL_TEXTURE_2D, TextureName);
        copyTexture();
        Image->drop();
        Image = 0;
    }
}*/
//! RTT ColorFrameBuffer constructor
CPS3Texture::CPS3Texture(const core::dimension2d<s32>& size,
						 const char* name,
						 CPS3Driver* driver,
						 bool useStencil,
						 bool colorTexture, 
						 bool depthTexture)
	: CCommonGLTexture(size, name, driver, useStencil, colorTexture, depthTexture)
{
	#ifdef _DEBUG
	setDebugName("CPS3Texture_FBO");
	#endif
/*
	// generate frame buffer
	glGenFramebuffersOES(1, &ColorFrameBuffer);
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, ColorFrameBuffer);

	if (colorTexture)
	{
		// generate color texture
		glGenTextures(1, &TextureName);
		glBindTexture(GL_TEXTURE_2D, TextureName);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_ALLOCATION_HINT_SCE, GL_TEXTURE_TILED_GPU_SCE);
		glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, ImageSize.Width,
			ImageSize.Height, 0, PixelFormat, PixelType, 0);

		// attach color texture to frame buffer
		glFramebufferTexture2DOES(GL_FRAMEBUFFER_OES,
							GL_COLOR_ATTACHMENT0_EXT,
							GL_TEXTURE_2D,
							TextureName,
							0);
	}

	if (depthTexture)
	{
		// generate color texture
		glGenTextures(1, &DepthTextureName);
		glBindTexture(GL_TEXTURE_2D, DepthTextureName);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB,GL_LEQUAL);
		glTexParameteri(GL_TEXTURE_2D,GL_DEPTH_TEXTURE_MODE_ARB,GL_INTENSITY);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_MODE_ARB,GL_COMPARE_R_TO_TEXTURE_ARB);

		if (useStencil)
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_ALLOCATION_HINT_SCE,GL_TEXTURE_TILED_GPU_SCE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, ImageSize.Width,
				ImageSize.Height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT_24_8_SCE, 0);
			
			StencilRenderBuffer = DepthRenderBuffer = DepthTextureName;

			// attach stencil texture to stencil buffer
			glFramebufferTexture2DOES(
				GL_FRAMEBUFFER_OES,
				GL_STENCIL_ATTACHMENT_OES,
				GL_TEXTURE_2D,
				StencilRenderBuffer,
				0);

			glFramebufferTexture2DOES(
				GL_FRAMEBUFFER_OES,
				GL_DEPTH_ATTACHMENT_OES,
				GL_TEXTURE_2D,
				DepthRenderBuffer,
				0);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_ALLOCATION_HINT_SCE,GL_TEXTURE_TILED_GPU_SCE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, ImageSize.Width,
				ImageSize.Height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, 0);

			glFramebufferTexture2DOES(
				GL_FRAMEBUFFER_OES,
				GL_DEPTH_ATTACHMENT_OES,
				GL_TEXTURE_2D,
				DepthTextureName,
				0);
		}
	}
	else
	{
		// generate depth render buffer
		glGenFramebuffersOES(1, &DepthRenderBuffer);
		glBindRenderbufferOES(GL_RENDERBUFFER_OES, DepthRenderBuffer);
		glRenderbufferStorageOES(GL_RENDERBUFFER_OES,
				GL_DEPTH_COMPONENT16, ImageSize.Width,
				ImageSize.Height);

		glBindFramebufferOES(GL_FRAMEBUFFER_OES, ColorFrameBuffer);
		// attach depth renderbuffer to depth buffer
		glFramebufferRenderbufferOES(
						GL_FRAMEBUFFER_OES,
						GL_DEPTH_ATTACHMENT_OES,
						GL_RENDERBUFFER_OES,
						DepthRenderBuffer);
	}
	
	glGetError();
	
	// check the status
	if (!checkFBOStatus(Driver))
	{
		//printf("FBO=%u, Color=%u, Depth=%u, Stencil=%u\n",
		//	ColorFrameBuffer, TextureName, DepthRenderBuffer, StencilRenderBuffer);
		if (ColorFrameBuffer)
			glDeleteFramebuffersOES(1, &ColorFrameBuffer);
		if (DepthRenderBuffer)
		{
			if (useStencil)
				glDeleteTextures(1, &DepthRenderBuffer);
			else
				glDeleteRenderbuffersOES(1, &DepthRenderBuffer);
		}
		if (StencilRenderBuffer && StencilRenderBuffer != DepthRenderBuffer)
			glDeleteTextures(1, &StencilRenderBuffer);
		ColorFrameBuffer = 0;
		DepthRenderBuffer = 0;
		StencilRenderBuffer = 0;
	}

	glBindFramebufferOES(GL_FRAMEBUFFER_OES, 0);
	*/
}


//! destructor
CPS3Texture::~CPS3Texture()
{

}

E_DRIVER_TYPE
CPS3Texture::getDriverType() const
{
	return EDT_PS3;
}

//! returns color format of texture
ECOLOR_FORMAT
CPS3Texture::getColorFormat() const
{
	if (Image)
		return Image->getColorFormat();
	else
		return ECF_A8R8G8B8;
}

} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_PS3_
