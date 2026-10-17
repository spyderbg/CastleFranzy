#include "stdafx.h"
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"

#if defined(_IRR_COMPILE_WITH_OPENGL_) || defined(_IRR_COMPILE_WITH_OPENGL_ES_) || defined(_IRR_COMPILE_WITH_PS3_)

#include "irrTypes.h"
#include "irrString.h"
#include "irros.h"
#include "CImage.h"
#include "CCommonGLTexture.h"
#include "CCommonGLDriver.h"
#include "CCompressedImage.h"
#include "CColorConverter.h"
#include "IReadFile.h"
#include "irrProcessBufferHeap.h"

#if (defined(_IRR_WINDOWS_API_) || defined(_IRR_LINUX_PLATFORM_)) && defined(_IRR_ENABLE_NATIVE_TEXTURE_FORMAT_)
#include "PVRT/PVRTDecompress.h"
#endif

namespace irr
{
namespace video
{

namespace
{

static const GLenum FilterMap[] =
{
	GL_NEAREST,                // ETFT_NEAREST
	GL_LINEAR,                 // ETFT_LINEAR
	GL_NEAREST_MIPMAP_NEAREST, // ETFT_NEAREST_MIPMAP_NEAREST
	GL_LINEAR_MIPMAP_NEAREST,  // ETFT_LINEAR_MIPMAP_NEAREST
	GL_NEAREST_MIPMAP_LINEAR,  // ETFT_NEAREST_MIPMAP_LINEAR
	GL_LINEAR_MIPMAP_LINEAR    // ETFT_LINEAR_MIPMAP_LINEAR
};

static const GLenum WrapModeMap[] =
{
	GL_REPEAT,          // ETC_REPEAT
#ifdef _IRR_COMPILE_WITH_OPENGL_ES_
	GL_CLAMP_TO_EDGE,   // ETC_CLAMP, not supported
	GL_CLAMP_TO_EDGE,   // ETC_CLAMP_TO_EDGE
	GL_CLAMP_TO_EDGE,   // ETC_CLAMP_TO_BORDER, not supported
	#ifdef GL_OES_texture_mirrored_repeat
	GL_MIRRORED_REPEAT_OES, // ETC_MIRROR
	#else
	GL_REPEAT,          // ETC_MIRROR, not supported
	#endif
#else
	GL_CLAMP,           // ETC_CLAMP
	GL_CLAMP_TO_EDGE,   // ETC_CLAMP_TO_EDGE
	GL_CLAMP_TO_BORDER, // ETC_CLAMP_TO_BORDER
	GL_MIRRORED_REPEAT, // ETC_MIRROR
#endif
};


#ifdef _IRR_OPENGL_FORCE_COMMIT_TO_VRAM_
void
forceCommitToVRAM()
{
    u32 clientStateChange = 0;

	//! OpenGL drivers should always keep this active
	_IRR_DEBUG_BREAK_IF(!glIsEnabled(GL_VERTEX_ARRAY));

    if (glIsEnabled(GL_NORMAL_ARRAY))
    {
        clientStateChange |= EVA_NORMAL;
        glDisableClientState(GL_NORMAL_ARRAY);
    }

    if (glIsEnabled(GL_COLOR_ARRAY))
    {
        clientStateChange |= EVA_COLOR0;
        glDisableClientState(GL_COLOR_ARRAY);
    }

    GLint activeTexture;
    glGetIntegerv(GL_CLIENT_ACTIVE_TEXTURE, &activeTexture);
    GLint maxUnits;
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &maxUnits);

    for (GLint i = 1; i < maxUnits; ++i)
    {
        glClientActiveTexture(GL_TEXTURE0 + i);
        if (glIsEnabled(GL_TEXTURE_COORD_ARRAY))
        {
            clientStateChange |= EVA_TEXCOORD0 << i;
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }
    }
    glClientActiveTexture(GL_TEXTURE0);
    if (!glIsEnabled(GL_TEXTURE_COORD_ARRAY))
    {
        clientStateChange |= EVA_TEXCOORD0;
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }

	GLint arrayBufferBinding;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &arrayBufferBinding);
	if (arrayBufferBinding != 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	GLint elementArrayBufferBinding;
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &elementArrayBufferBinding);
	if (elementArrayBufferBinding != 0)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

    GLint vSize, vStride, vType;
    glGetIntegerv(GL_VERTEX_ARRAY_SIZE, &vSize);
    glGetIntegerv(GL_VERTEX_ARRAY_STRIDE, &vStride);
    glGetIntegerv(GL_VERTEX_ARRAY_TYPE, &vType);
    GLvoid* vPtr;
    glGetPointerv(GL_VERTEX_ARRAY_POINTER, &vPtr);

    GLint tSize, tStride, tType;
    glGetIntegerv(GL_TEXTURE_COORD_ARRAY_SIZE, &tSize);
    glGetIntegerv(GL_TEXTURE_COORD_ARRAY_STRIDE, &tStride);
    glGetIntegerv(GL_TEXTURE_COORD_ARRAY_TYPE, &tType);
    GLvoid* tPtr;
    glGetPointerv(GL_TEXTURE_COORD_ARRAY_POINTER, &tPtr);

    GLboolean texDisabled = !glIsEnabled(GL_TEXTURE_2D);
    if (texDisabled)
    {
        glEnable(GL_TEXTURE_2D);
    }

    GLboolean blendDisabled = !glIsEnabled(GL_BLEND);
    GLint blendSrc, blendDst;
    glGetIntegerv(GL_BLEND_SRC, &blendSrc);
    glGetIntegerv(GL_BLEND_DST, &blendDst);

    if (blendDisabled)
    {
        glEnable(GL_BLEND);
    }
    glBlendFunc(GL_ZERO, GL_ONE);

    GLshort dummy[] = { 0 , 0 };
    glVertexPointer(2, GL_SHORT, 0, &dummy[0]) ;
    glTexCoordPointer(2, GL_SHORT, 0, &dummy[0]) ;
    glDrawArrays(GL_POINTS, 0, 1);

	if (arrayBufferBinding != 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, arrayBufferBinding);
	}

	if (elementArrayBufferBinding != 0)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBufferBinding);
	}

    glVertexPointer(vSize, vType, vStride, vPtr);
    glTexCoordPointer(tSize, tType, tStride, tPtr);

    if (texDisabled)
    {
        glDisable(GL_TEXTURE_2D);
    }
    if (blendDisabled)
    {
        glDisable(GL_BLEND);
    }
    if (blendSrc != GL_ZERO || blendDst != GL_ONE)
    {
        glBlendFunc(blendSrc, blendDst);
    }

    if (clientStateChange & EVA_NORMAL)
    {
        glEnableClientState(GL_NORMAL_ARRAY);
    }
    if (clientStateChange & EVA_COLOR0)
    {
        glEnableClientState(GL_COLOR_ARRAY);

    }

    GLint lastActiveTexture = GL_TEXTURE0;
    for (GLint i = 1; i < maxUnits; ++i)
    {
        if (clientStateChange & (EVA_TEXCOORD0 << i))
        {
            lastActiveTexture = GL_TEXTURE0 + i;
            glClientActiveTexture(lastActiveTexture);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        }
    }
    if (clientStateChange & EVA_TEXCOORD0)
    {
        lastActiveTexture = GL_TEXTURE0;
        glClientActiveTexture(GL_TEXTURE0);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    if (lastActiveTexture != activeTexture)
    {
        glClientActiveTexture(activeTexture);
    }
}
#endif

#ifdef _IRR_ENABLE_NATIVE_TEXTURE_FORMAT_

GLuint
loadPVRTexture(io::IReadFile* file,
			   core::dimension2d<s32>& dims,
			   GLint& internalFormat,
			   GLenum& pixelFormat,
			   GLenum& pixelType,
			   u32& mipmaps,
			   CCommonGLDriver* driver,
			   u32 prefixSize = 0)
{
	// most of the code here is inspired by PVRTextureLoader sample from Apple

	// TODO: check endianness of host vs format (which is little endian)
	//_IRR_DEBUG_BREAK_IF(isBigEndian());

	u32 fileBufferSize = file->getSize() - prefixSize;
	u8* fileBuffer = irrnew u8 [fileBufferSize];
	{
		u32 readSize = file->read(fileBuffer, fileBufferSize);
		if (readSize != fileBufferSize)
		{
			delete[] fileBuffer;
			return 0;
		}
	}
	u8* fileBufferEnd = fileBuffer + fileBufferSize;
	u8* buffer = fileBuffer; 
	u8* pvrtDestBuffer = NULL; 

	struct PVRHeader
	{
		u32 HeaderLength;
		u32 Height;
		u32 Width;
		u32 NumMipmaps;
		u32 Flags;
		u32 DataLength;
		u32 BitsPerPixel;
		u32 BitmaskRed;
		u32 BitmaskGreen;
		u32 BitmaskBlue;
		u32 BitmaskAlpha;
		char PVRTag[4];
		u32 NumSurfs;
	}* header = reinterpret_cast<PVRHeader*>(buffer);

	if (fileBufferSize < sizeof(PVRHeader)
		|| header->HeaderLength != sizeof(PVRHeader)
		|| strncmp(header->PVRTag, "PVR!", 4) != 0
		|| header->DataLength != fileBufferSize - sizeof(PVRHeader))
	{
		return 0;
	}
	buffer += sizeof(PVRHeader);

	mipmaps = header->NumMipmaps + 1;

	u32 format = header->Flags & 0xff;

	u32 blockSizeX = 1;
	u32 blockSizeY = 1;
	u32 minBlocks = 1;
	pixelType = GL_UNSIGNED_BYTE;
	bool isCompressed = false;

	// Format values taken from PVRTexLib
	switch (format)
	{
		case 0x10 : // OGL_RGBA_4444
		{
			internalFormat = pixelFormat = GL_RGBA;
			pixelType = GL_UNSIGNED_SHORT_4_4_4_4;
		}
		break;

		case 0x01 : // OGL_ARGB_1555
		{
			// TODO: needs validation (doesn't seems like this will work)
			internalFormat = GL_RGBA;
			pixelFormat = GL_BGRA;
			pixelType = GL_UNSIGNED_SHORT_5_5_5_1;
		}
		break;

		case 0x11 : // OGL_RGBA_5551
		{
			internalFormat = pixelFormat = GL_RGBA;
			pixelType = GL_UNSIGNED_SHORT_5_5_5_1;
		}
		break;

		case 0x12 : // OGL_RGBA_8888
		{
			internalFormat = pixelFormat = GL_RGBA;
		}
		break;
		
		case 0x13 : // OGL_RGB_565
		{
			internalFormat = pixelFormat = GL_RGB;
			pixelType = GL_UNSIGNED_SHORT_5_6_5;
		}
		break;

		case 0x15 : // OGL_RGB_888
		{
			internalFormat = pixelFormat = GL_RGB;
		}
		break;

		case 0x16 : // OGL_I_8
		{
			internalFormat = pixelFormat = GL_LUMINANCE;
		}
		break;

		case 0x17 : // OGL_AI_88
		{
			internalFormat = pixelFormat = GL_LUMINANCE_ALPHA;
		}
		break;

#if defined(_IRR_WINDOWS_API_) || defined(_IRR_LINUX_PLATFORM_)
		// Decompress PVRT format in OpenGL RGB/RGBA texture (windows)
		case 0x18 : // OGL_PVRTC2
		{
			internalFormat = pixelFormat = GL_RGBA;

			pvrtDestBuffer = irrnew u8[header->Width * header->Height * 4]; 
			PVRTCDecompress( buffer, 1, header->Width, header->Height, pvrtDestBuffer);
			buffer = pvrtDestBuffer;
		}
		break;

		case 0x19 : // OGL_PVRTC4
		{
			internalFormat = pixelFormat = GL_RGBA;

			pvrtDestBuffer = irrnew u8[header->Width * header->Height * 4]; 
			PVRTCDecompress( buffer, 0, header->Width, header->Height, pvrtDestBuffer);
			buffer = pvrtDestBuffer;
		}
		break;

#elif defined(GL_IMG_texture_compression_pvrtc)
		// Below cases: 0x8000 is the bit mask for presence of alpha (from
		// PVRTexTool documentation)

		case 0x18 : // OGL_PVRTC2
		{
			internalFormat = pixelFormat
				= ((header->Flags & 0x8000)
				   ? GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG
				   : GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG);
			blockSizeX = 8;
			blockSizeY = 4;
			minBlocks = 2;
			isCompressed = true;
		}
		break;

		case 0x19 : // OGL_PVRTC4
		{
			internalFormat = pixelFormat
				= ((header->Flags & 0x8000)
				   ? GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG
				   : GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG);
			blockSizeX = blockSizeY = 4;
			minBlocks = 2;
			isCompressed = true;
		}
		break;
#endif

		default :
		{
			return 0;
		}
		break;
	}

	GLuint textureName = 0;
	glGenTextures(1, &textureName);
	if (textureName == 0)
	{
		return 0;
	}
	glBindTexture(GL_TEXTURE_2D, textureName);
	if (driver->testGLError())
	{
		glDeleteTextures(1, &textureName);
		return 0;
	}

	GLint oldUnpackAlignment;
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &oldUnpackAlignment);
	if (oldUnpackAlignment != 1)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}

	bool error = false;
	GLsizei width = dims.Width = header->Width;
	GLsizei height = dims.Height = header->Height;
	for (u32 m = 0; m <= header->NumMipmaps; ++m)
	{
		u32 dataSize = (core::max_(width / blockSizeX, minBlocks)
						* core::max_(height / blockSizeY, minBlocks)
						* blockSizeX
						* blockSizeY
						* header->BitsPerPixel
						/ 8);

#if defined(_IRR_WINDOWS_API_) || defined(_IRR_LINUX_PLATFORM_)
		if(buffer + dataSize > fileBufferEnd && format != 0x18 && format != 0x19 )
		{
			error = true;
			break;
		}
#else
		if (buffer + dataSize > fileBufferEnd)
		{
			error = true;
			break;
		}
#endif
		
		if (isCompressed)
		{
			driver->compressedTexImage2D(GL_TEXTURE_2D,
										 (GLint)m,
										 internalFormat,
										 width,
										 height,
										 0,
										 dataSize,
										 buffer);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D,
						 (GLint)m,
						 internalFormat,
						 width,
						 height,
						 0,
						 pixelFormat, // not sure about this one
						 pixelType,
						 buffer);
		}

		if (driver->testGLError())
		{
			if (isCompressed && width != height)
			{
				os::Printer::log("glCompressedTexImage2D failed possibly "
								 "because the texture is not square",
								 ELL_ERROR);
			}
			error = true;
			break;
		}

		buffer += dataSize;
		width = core::max_(width >> 1, 1);
		height = core::max_(height >> 1, 1);
	}

	// reset unpack alignment
	if (oldUnpackAlignment != 1)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, oldUnpackAlignment);
	}

	if (error)
	{
		dims.Width = dims.Height = 0;
		glDeleteTextures(1, &textureName);
		glBindTexture(GL_TEXTURE_2D, 0);
		textureName = 0;
	}

	delete[] fileBuffer;

	if(pvrtDestBuffer)
		delete[] pvrtDestBuffer;

	return textureName;
}
#endif

}

#if defined(GL_EXT_framebuffer_object) || defined(GL_OES_framebuffer_object) || defined(_IRR_COMPILE_WITH_PS3_)
// helper function for render to texture
static bool checkFBOStatus(CCommonGLDriver* Driver);
#endif

// This backup wrapper is necessary because CCommonGLTexture constructors modify the OpenGL states (texture bind) without using telling driver.
class CCommonGLTextureStateBackup
{
public:
	CCommonGLTextureStateBackup(CCommonGLDriver* driver) :
		m_driver(driver),
		m_activeTexture(0),
		m_currentTexture(NULL)
	{
		m_activeTexture = m_driver->getActiveTexture() - GL_TEXTURE0;
		m_currentTexture = (irr::video::ITexture*)m_driver->getCurrentTexture(m_activeTexture);
	}

	~CCommonGLTextureStateBackup()
	{
		// Set texture to NULL to force change
		m_driver->setTexture(m_activeTexture, NULL);
		m_driver->setTexture(m_activeTexture, m_currentTexture);
	}

private:
	CCommonGLDriver* m_driver;
	u32 m_activeTexture;
	ITexture* m_currentTexture;
};

//! constructor for usual textures
CCommonGLTexture::CCommonGLTexture(IImage* origImage, const char* name, CCommonGLDriver* driver)
	: ITexture(name)
	, Driver(driver)
	, Image(0)
	, TextureName(0)
	, InternalFormat(GL_RGBA)
	, PixelFormat(GL_BGRA)
	, PixelType(GL_UNSIGNED_BYTE)
	, ColorFrameBuffer(0)
	, DepthRenderBuffer(0)
	, StencilRenderBuffer(0)
	, AutomaticMipmapUpdate(false)
	, UseStencil(false)
	, ReadOnlyLock(false)
	, DepthTextureName(0)
{
	// backup now and restore when exits
	CCommonGLTextureStateBackup backup(driver);

	#ifdef _DEBUG
	setDebugName("CCommonGLTexture");
	#endif
	
#ifdef GL_EXT_texture_filter_anisotropic
	setAnisotropy(1.0f),
#endif

	getImageData(origImage);

	if (Driver->getOption(EVDO_CREATE_TEXTURE_MIPMAPS))
	{
		setFlag(ETF_HAS_MIPMAPS);
	}
	if (Image)
	{
#ifdef  _IRR_MAYBE_RETURN_THREAD_FLUSH
        _IRR_MAYBE_RETURN_THREAD_FLUSH(this);
#endif
		glGenTextures(1, &TextureName);
		copyTexture();
		Image->drop();
		Image = 0;
	}
}
#ifdef _IRR_COMPILE_WITH_THREAD_FLUSH
bool CCommonGLTexture::flushVRM()
{
    if (Image)
    {
        glGenTextures(1, &TextureName);
        copyTexture();
        Image->drop();
        Image = 0;
        return true;
    }
    return false;
}
#endif

//! constructor for minimal textures used to initialize the default values
//! when making a new constructor for any derivated class (protected constructor)
CCommonGLTexture::CCommonGLTexture(const char* name, CCommonGLDriver* driver)
	: ITexture(name)
	, Driver(driver)
	, Image(0)
	, TextureName(0)
	, InternalFormat(GL_RGBA)
	, PixelFormat(GL_BGRA)
	, PixelType(GL_UNSIGNED_BYTE)
	, ColorFrameBuffer(0)
	, DepthRenderBuffer(0)
	, StencilRenderBuffer(0)
	, AutomaticMipmapUpdate(false)
	, UseStencil(false)
	, ReadOnlyLock(false)
	, DepthTextureName(0)
{
	#ifdef _DEBUG
	setDebugName("CCommonGLTexture");
	#endif
}

//! RTT ColorFrameBuffer constructor
CCommonGLTexture::CCommonGLTexture(const core::dimension2d<s32>& size,
								   const char* name,
								   CCommonGLDriver* driver,
								   bool useStencil,
								   bool colorTexture, 
								   bool depthTexture)
	: ITexture(name)
	, ImageSize(size)
	, Driver(driver)
	, Image(0)
	, TextureName(0)
	, InternalFormat(GL_RGBA)
	, PixelFormat(GL_RGBA)
	, PixelType(GL_UNSIGNED_BYTE)
	, ColorFrameBuffer(0)
	, DepthRenderBuffer(0)
	, StencilRenderBuffer(0)
	, AutomaticMipmapUpdate(false)
	, UseStencil(useStencil)
	, ReadOnlyLock(false)
	, DepthTextureName(0)
{
	// backup now and restore when exits
	CCommonGLTextureStateBackup backup(driver);

	#ifdef _DEBUG
	setDebugName("CCommonGLTexture_FBO");
	#endif

	setFlag(ETF_IS_RENDER_TARGET);

#if !defined(_IRR_COMPILE_WITH_PS3_)
	if (useStencil)
	{
		glGenTextures(1, &DepthRenderBuffer);
		glBindTexture(GL_TEXTURE_2D, DepthRenderBuffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		Driver->testGlErrorParanoid();
	#if defined(GL_EXT_packed_depth_stencil) || defined(GL_OES_packed_depth_stencil)
		if (Driver->queryOpenGLFeature(IRR_EXT_packed_depth_stencil) 
			|| Driver->queryOpenGLFeature(IRR_OES_packed_depth_stencil))
		{
			// generate packed depth stencil texture
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_STENCIL, ImageSize.Width,
				ImageSize.Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);
			StencilRenderBuffer = DepthRenderBuffer; // stencil is packed with depth
			Driver->testGlErrorParanoid();
		}
		else // generate separate stencil and depth textures
	#endif
		{
	#ifdef _IRR_COMPILE_WITH_OPENGL_ES_
			// generate depth texture
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24_OES, ImageSize.Width,
				ImageSize.Height, 0, GL_DEPTH_COMPONENT16_OES, GL_UNSIGNED_BYTE, 0);
			Driver->testGlErrorParanoid();

	#else
			// generate depth texture
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, ImageSize.Width,
				ImageSize.Height, 0, GL_DEPTH_COMPONENT24, GL_UNSIGNED_BYTE, 0);
			Driver->testGlErrorParanoid();

			// we 're in trouble! the code below does not complete
			// the FBO currently...  stencil buffer is only
			// supported with EXT_packed_depth_stencil extension
			// (above)

			// generate stencil texture
			glGenTextures(1, &StencilRenderBuffer);
			glBindTexture(GL_TEXTURE_2D, StencilRenderBuffer);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_STENCIL_INDEX, ImageSize.Width,
			ImageSize.Height, 0, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, 0);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			Driver->testGlErrorParanoid();
	#endif
		}
	}
	#ifdef _IRR_COMPILE_WITH_OPENGL_ES_
	else
	{
		// generate depth buffer
		Driver->genRenderbuffers(1, &DepthRenderBuffer);
		Driver->bindRenderbuffer(GL_RENDERBUFFER, DepthRenderBuffer);
		Driver->renderbufferStorage(GL_RENDERBUFFER,
				GL_DEPTH_COMPONENT16_OES, ImageSize.Width,
				ImageSize.Height);
		Driver->testGlErrorParanoid();
	}
	#endif
#endif

#if defined(GL_EXT_framebuffer_object) || defined(_IRR_COMPILE_WITH_OPENGL_ES_) || defined(_IRR_USE_PS3_DEVICE_)
	// generate frame buffer
	Driver->genFramebuffers(1, &ColorFrameBuffer);
	Driver->bindFramebuffer(GL_FRAMEBUFFER, ColorFrameBuffer);
	Driver->testGlErrorParanoid();

#ifndef _IRR_COMPILE_WITH_OPENGL_ES_
	if(colorTexture)
#endif
	{
		setWrapS(ETC_CLAMP);
		setWrapT(ETC_CLAMP);

		// generate color texture
		glGenTextures(1, &TextureName);
		glBindTexture(GL_TEXTURE_2D, TextureName);
		Driver->testGlErrorParanoid();

		glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, ImageSize.Width,
			ImageSize.Height, 0, PixelFormat, PixelType, 0);
		setMinFilter(ETFT_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		Driver->testGlErrorParanoid();

		// attach color texture to frame buffer
		Driver->framebufferTexture2D(GL_FRAMEBUFFER,
									 GL_COLOR_ATTACHMENT0,
									 GL_TEXTURE_2D,
									 TextureName,
									 0);
		Driver->testGlErrorParanoid();
	}

	if (useStencil)
	{
		// attach stencil texture to stencil buffer
		Driver->framebufferTexture2D(GL_FRAMEBUFFER,
									 GL_STENCIL_ATTACHMENT,
									 GL_TEXTURE_2D,
									 StencilRenderBuffer,
									 0);
		Driver->testGlErrorParanoid();

		//attach depth texture to depth buffer
		Driver->framebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D,
			DepthRenderBuffer,
			0
		);
		Driver->testGlErrorParanoid();
	}
	else
	{
#ifndef _IRR_COMPILE_WITH_OPENGL_ES_
		if (depthTexture)
		{
			// create a depth texture
			glGenTextures(1, &DepthTextureName);
			glBindTexture(GL_TEXTURE_2D, DepthTextureName);
			Driver->testGlErrorParanoid();
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, ImageSize.Width,
				ImageSize.Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
			Driver->testGlErrorParanoid();

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);
			glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE);		
			Driver->testGlErrorParanoid();

			// attach depth texture to frame buffer
			Driver->framebufferTexture2D(GL_FRAMEBUFFER,
										 GL_DEPTH_ATTACHMENT,
										 GL_TEXTURE_2D,
										 DepthTextureName,
										 0);
			Driver->testGlErrorParanoid();
#if !defined(_IRR_USE_PS3_DEVICE_)
			if (!colorTexture)
			{
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);
				Driver->testGlErrorParanoid();
			}
#endif
		}
		else
		{
			// generate depth render buffer
			Driver->genRenderbuffers(1, &DepthRenderBuffer);
			Driver->testGlErrorParanoid();
			Driver->bindRenderbuffer(GL_RENDERBUFFER, DepthRenderBuffer);
			Driver->testGlErrorParanoid();
			Driver->renderbufferStorage(GL_RENDERBUFFER,
					GL_DEPTH_COMPONENT24, ImageSize.Width,
					ImageSize.Height);
			Driver->testGlErrorParanoid();
			// attach depth renderbuffer to depth buffer
			Driver->framebufferRenderbuffer(GL_FRAMEBUFFER,
											GL_DEPTH_ATTACHMENT,
											GL_RENDERBUFFER,
											DepthRenderBuffer);
			Driver->testGlErrorParanoid();
		}
#else
		// attach depth renderbuffer to depth buffer
		Driver->framebufferRenderbuffer(GL_FRAMEBUFFER,
										GL_DEPTH_ATTACHMENT,
										GL_RENDERBUFFER,
										DepthRenderBuffer);
		Driver->testGlErrorParanoid();
#endif
	}

	//glGetError();

	// check the status
	if (!checkFBOStatus(Driver))
	{
		printf("FBO=%u, Color=%u, Depth=%u, Stencil=%u\n",
			ColorFrameBuffer, TextureName, DepthRenderBuffer, StencilRenderBuffer);
		if (ColorFrameBuffer)
			Driver->deleteFramebuffers(1, &ColorFrameBuffer);
		if (DepthRenderBuffer)
		{
			if (useStencil)
				glDeleteTextures(1, &DepthRenderBuffer);
			else
				Driver->deleteRenderbuffers(1, &DepthRenderBuffer);
		}
		if (StencilRenderBuffer && StencilRenderBuffer != DepthRenderBuffer)
			glDeleteTextures(1, &StencilRenderBuffer);
		ColorFrameBuffer = 0;
		DepthRenderBuffer = 0;
		StencilRenderBuffer = 0;
	}
	Driver->bindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
}


#ifdef _IRR_ENABLE_NATIVE_TEXTURE_FORMAT_
//! constructor for "native" formats
CCommonGLTexture::CCommonGLTexture(io::IReadFile* file, const char* name, CCommonGLDriver* driver)
	:  ITexture(name)
	, Driver(driver)
	, Image(0)
	, TextureName(0)
	, InternalFormat(GL_RGBA)
	, PixelFormat(GL_BGRA)
	, PixelType(GL_UNSIGNED_BYTE)
	, ColorFrameBuffer(0)
	, DepthRenderBuffer(0)
	, StencilRenderBuffer(0)
	, AutomaticMipmapUpdate(false)
	, UseStencil(false)
	, ReadOnlyLock(false)
	, DepthTextureName(0)
{
	// backup now and restore when exits
	CCommonGLTextureStateBackup backup(driver);

	#ifdef _DEBUG
	setDebugName("COpenGLESTexture");
	#endif
		
	setMinFilter(ETFT_NEAREST); 
	setMagFilter(ETFT_NEAREST);
#ifdef GL_EXT_texture_filter_anisotropic
	setAnisotropy(1.0f),
#endif
	setWrap(ETC_REPEAT);

	if (Driver->getOption(ETCF_CREATE_MIP_MAPS))
	{
		setFlag(ETF_HAS_MIPMAPS);
	}

	if (file)
	{
		//char base_header[8];
		//s32 sizeRead = file->read(&base_header[0], 8);
		//if (sizeRead == 8 && strncmp(base_header, "BTEX", 4) == 0)
		//{
		//	if (strncmp(&base_header[4], "pvr", 4) == 0)
		//	{
				u32 mipmaps = 0;
				TextureName = loadPVRTexture(file,
											 ImageSize,
											 InternalFormat,
											 PixelFormat,
											 PixelType,
											 mipmaps,
											 driver,
											 0);
				if(mipmaps <= 1 && hasMipMaps())
				{
					unsetFlag(ETF_HAS_MIPMAPS);
				}
		//	}
			// else if (strncmp(&base_header[4], "more formats"))
			// Add more formats that we might want to support
		//}

		// post-condition here should be that TextureName is bound is it was
		// successfully loaded

		if (isValid())
		{
			setMagFilter(ETFT_LINEAR);
			if (hasMipMaps()) 
			{
				// enable bilinear mipmap filter
				setMinFilter(ETFT_LINEAR_MIPMAP_NEAREST);
			}
			else
			{
				// enable bilinear filter without mipmaps
				setMinFilter(ETFT_LINEAR);
			}

#ifdef _IRR_OPENGL_FORCE_COMMIT_TO_VRAM_
			updateParameters();
			forceCommitToVRAM();
#endif
		}
	}
}
#endif


//! destructor
CCommonGLTexture::~CCommonGLTexture()
{
	if (ColorFrameBuffer)
		Driver->deleteFramebuffers(1, &ColorFrameBuffer);
	if (DepthRenderBuffer && UseStencil)
		glDeleteTextures(1, &DepthRenderBuffer);
	else if (DepthRenderBuffer)
		Driver->deleteRenderbuffers(1, &DepthRenderBuffer);
	if (StencilRenderBuffer && StencilRenderBuffer != DepthRenderBuffer)
		glDeleteTextures(1, &StencilRenderBuffer);
	Driver->testGlErrorParanoid();

	glDeleteTextures(1, &TextureName);
	Driver->testGlErrorParanoid();

	if (DepthTextureName)
		glDeleteTextures(1, &DepthTextureName);
	Driver->testGlErrorParanoid();

	if (Image)
		Image->drop();
}

//! Process the color format and set the InternalFormat, PixelFormat, PixelType, compressed variables.
//! This method can't be virtual and re-implemented in the childs as it's called in this class's 
//! constructor (when the vtable is not yet built...)
void
CCommonGLTexture::processColorFormat(ECOLOR_FORMAT format, 
									 GLint& InternalFormat, 
									 GLenum& PixelFormat, 
									 GLenum& PixelType, 
									 bool& compressed)
{
	switch (format)
	{
#if defined(_IRR_COMPILE_WITH_OPENGL_ES_)
	case ECF_A1R5G5B5:
		// TODO: needs validation (doesn't seems like this works)
		InternalFormat=GL_RGBA;
		PixelFormat=GL_BGRA;
		PixelType=GL_UNSIGNED_SHORT_5_5_5_1;
		break;
	case ECF_R5G6B5:
		InternalFormat=GL_RGB;
		PixelFormat=GL_RGB;
		PixelType=GL_UNSIGNED_SHORT_5_6_5;
		break;
	case ECF_R8G8B8:
		InternalFormat=GL_RGB;
		PixelFormat=GL_RGB; //GL_BGR;
		PixelType=GL_UNSIGNED_BYTE;
		break;
	case ECF_A8R8G8B8:
		#ifdef _IRR_USE_WINDOWS_DEVICE_
			InternalFormat= GL_BGRA;
			PixelFormat   = GL_BGRA;
		#else
			InternalFormat= GL_RGBA;
			PixelFormat   = GL_BGRA;
		#endif
		PixelType=GL_UNSIGNED_BYTE;
		break;
#else
	case ECF_A1R5G5B5:
		InternalFormat=GL_RGBA;
		PixelFormat=GL_BGRA;
		PixelType=GL_UNSIGNED_SHORT_1_5_5_5_REV;
		break;
	case ECF_R5G6B5:
		InternalFormat=GL_RGB;
		PixelFormat=GL_BGR;
		PixelType=GL_UNSIGNED_SHORT_5_6_5_REV;
		break;
#if defined(_IRR_COMPILE_WITH_PS3_)
	case ECF_R8G8B8:
		InternalFormat=GL_RGBA;
		PixelFormat=GL_RGB;
		PixelType=GL_UNSIGNED_BYTE;				
		break;
	case ECF_A8R8G8B8:
		//InternalFormat=GL_ARGB_SCE;
		//PixelFormat=GL_ARGB_SCE;
		//PixelType=GL_UNSIGNED_BYTE;
		//break;
    case ECF_R8G8B8A8:
        InternalFormat=GL_RGBA;
        PixelFormat=GL_RGBA;
        PixelType=GL_UNSIGNED_INT_8_8_8_8;
        break;
#else
	case ECF_R8G8B8:
		InternalFormat=GL_RGB;
		PixelFormat=GL_BGR;
		PixelType=GL_UNSIGNED_BYTE;
		break;
	case ECF_A8R8G8B8:
		InternalFormat=GL_RGBA;
		PixelFormat=GL_BGRA;
		if (Driver->getVersion() > 101)
			PixelType=GL_UNSIGNED_INT_8_8_8_8_REV;
		break;
#endif
	case ECF_COMPRESSED_RGB_S3TC_DXT1:
		compressed = true;
		InternalFormat= GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
		PixelFormat=GL_UNSIGNED_BYTE;
		break;
	case ECF_COMPRESSED_RGBA_S3TC_DXT1:
		compressed = true;
		InternalFormat= GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		PixelFormat=GL_UNSIGNED_BYTE;
		break;
	case ECF_COMPRESSED_RGBA_S3TC_DXT3:
		compressed = true;
		InternalFormat= GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		PixelFormat=GL_UNSIGNED_BYTE;
		break;
	case ECF_COMPRESSED_RGBA_S3TC_DXT5:
		compressed = true;
		InternalFormat= GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		PixelFormat=GL_UNSIGNED_BYTE;
		break;
#endif
	case ECF_A8:
		InternalFormat=GL_ALPHA;
		PixelFormat=GL_ALPHA;
		break;
	default:
		os::Printer::log("Unsupported texture format", ELL_ERROR);
		break;
	}
}


ECOLOR_FORMAT
CCommonGLTexture::getBestColorFormat(ECOLOR_FORMAT format)
{
#if defined(_IRR_COMPILE_WITH_PS3_)
    if(format == ECF_R8G8B8A8) return ECF_R8G8B8A8;
    else if(format == ECF_R8G8B8) return ECF_R8G8B8;
    else if(format == ECF_A1R5G5B5) return ECF_R8G8B8A8;
    else 
    {
        os::Printer::log("Error: ECOLOR_FORMAT", ELL_ERROR);
        return ECF_R8G8B8A8;//ECF_A8R8G8B8;
    }
#else
	
	ECOLOR_FORMAT destFormat = ECF_A8R8G8B8;
	switch (format)
	{
		case ECF_A1R5G5B5:
			if (!Driver->getOption(EVDO_CREATE_TEXTURE_ALWAYS_32_BIT))
				destFormat = ECF_A1R5G5B5;
		break;
		case ECF_R5G6B5:
			if (!Driver->getOption(EVDO_CREATE_TEXTURE_ALWAYS_32_BIT))
				destFormat = ECF_A1R5G5B5;
		break;
		case ECF_A8R8G8B8:
			if (Driver->getOption(EVDO_CREATE_TEXTURE_ALWAYS_16_BIT
								  | EVDO_CREATE_TEXTURE_OPTIMIZED_FOR_SPEED))
				destFormat = ECF_A1R5G5B5;
		break;
		case ECF_R8G8B8:
			if (Driver->getOption(EVDO_CREATE_TEXTURE_ALWAYS_16_BIT
								  | EVDO_CREATE_TEXTURE_OPTIMIZED_FOR_SPEED))
				destFormat = ECF_A1R5G5B5;
		break;
		case ECF_A8:
			destFormat = ECF_A8;
		break;
	}
	if (Driver->getOption(EVDO_CREATE_TEXTURE_NO_ALPHA))
	{
		switch (destFormat)
		{
			case ECF_A1R5G5B5:
				destFormat = ECF_R5G6B5;
			break;
			case ECF_A8R8G8B8:
				destFormat = ECF_R8G8B8;
			break;
			default:
			break;
		}
	}
	return destFormat;
#endif
}

void
CCommonGLTexture::getImageData(IImage* image)
{
	if (!image)
	{
		os::Printer::log("No image for OpenGL texture.", ELL_ERROR);
		return;
	}

	ImageSize = image->getDimension();

	if ( !ImageSize.Width || !ImageSize.Height)
	{
		os::Printer::log("Invalid size of image for OpenGL Texture.", ELL_ERROR);
		return;
	}

	core::dimension2d<s32> nImageSize = ImageSize;

#ifndef _IRR_COMPILE_WITH_OPENGL_ES_
	if (image->getColorFormat() >= ECF_COMPRESSED_RGB_S3TC_DXT1 &&
		image->getColorFormat() <= ECF_COMPRESSED_RGBA_S3TC_DXT5)
	{
		//Image = irrnew CCompressedImage(image);
		Image = image;
		Image->grab();
		return;
	}
#endif

/*
	if (Driver->queryFeature(EVDF_TEXTURE_NPOT))
		nImageSize=ImageSize;
	else
*/
	if (!Driver->queryFeature(EVDF_TEXTURE_NPOT))
	{
		nImageSize.Width = getTextureSizeFromSurfaceSize(ImageSize.Width);
		nImageSize.Height = getTextureSizeFromSurfaceSize(ImageSize.Height);
	}

#ifndef _IRR_COMPILE_WITH_OPENGL_ES_
	const core::dimension2d<s32>& maxTextureSize = Driver->getMaxTextureSize();

	if (maxTextureSize.Width > 0 && maxTextureSize.Height > 0)
	{
		if (nImageSize.Width > maxTextureSize.Width)
		{
			nImageSize.Width = maxTextureSize.Width;
			float height = ((float)ImageSize.Height / (float)ImageSize.Width) * (float)nImageSize.Width;
			nImageSize.Height = static_cast<s32>(height);
		}
		if (nImageSize.Height > maxTextureSize.Height)
		{
			nImageSize.Height = maxTextureSize.Height;
			float width = ((float)ImageSize.Width / (float)ImageSize.Width) * (float)nImageSize.Width;
			nImageSize.Width = static_cast<s32>(width);
		}
	}
#endif

	ECOLOR_FORMAT destFormat = getBestColorFormat(image->getColorFormat());
	if (ImageSize==nImageSize)
    {
        ECOLOR_FORMAT sourceFormat = image->getColorFormat();
        if(destFormat == sourceFormat)
        {
            Image = image;
            Image->grab();
        }
        else
        {
            Image = irrnew CImage(destFormat, image);
        }
    }
	else
	{
		Image = irrnew CImage(destFormat, nImageSize);
		// scale texture
		image->copyToScaling(Image);
	}
}


//! copies the the texture into an open gl texture.
void
CCommonGLTexture::copyTexture(bool newTexture)
{
	glBindTexture(GL_TEXTURE_2D, TextureName);
	Driver->testGlErrorParanoid();

	if (!Image)
	{
		os::Printer::log("No image for OpenGL texture to upload", ELL_ERROR);
		return;
	}

	bool compressed = false;

	processColorFormat(Image->getColorFormat(), InternalFormat, PixelFormat, PixelType, compressed);

//#ifndef _IRR_COMPILE_WITH_OPENGL_ES_
	if (compressed)
	{
		CCompressedImage* compressedImage = static_cast<CCompressedImage*>(Image);
		
		u32 mipMapCount = compressedImage->getMipMapCount();
		if (mipMapCount == 0)
		{
			mipMapCount = 1;
			unsetFlag(ETF_HAS_MIPMAPS);
		}

		if (mipMapCount > 1)
		{
			// enable bilinear mipmap filter
			setMinFilter(ETFT_LINEAR_MIPMAP_NEAREST);
		}
		else
		{
			// enable bilinear filter without mipmaps
			setMinFilter(ETFT_LINEAR);
		}
		setMagFilter(ETFT_LINEAR);
		Driver->testGlErrorParanoid();

		void* source = Image->lock();

		u32 width = compressedImage->getDimension().Width;
		u32 height = compressedImage->getDimension().Height;
		u32 offset = 0;

		for( u32 i = 0; i < mipMapCount; ++i )
        {
            if (width  == 0)
			{
				width  = 1;
			}
            if (height == 0)
			{
				height = 1;
			}

            u32 size = ((width + 3) / 4) * ((height + 3) / 4) * compressedImage->getBytesPerBlock();

            Driver->compressedTexImage2D(GL_TEXTURE_2D,
										 i,
										 InternalFormat,
										 width,
										 height,
										 0,
										 size,
										 (u8 *)source + offset);
			Driver->testGlErrorParanoid();

            offset += size;

            // Half the image size for the next mip-map level...
            width  = (width  / 2);
            height = (height / 2);
        }

		Image->unlock();
		Driver->testGlErrorDebug();

		return;
	}
//#endif

	if (newTexture)
	{
#ifndef DISABLE_MIPMAPPING
#if !defined(_IRR_USE_PS3_DEVICE_)
		if (hasMipMaps() && Driver->queryFeature(EVDF_MIP_MAP_AUTO_UPDATE))
		{
			// automatically generate and update mipmaps
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
			AutomaticMipmapUpdate = true;
			setMaxLOD( f32(core::log2i(core::max_(Image->getDimension().Width,
										          Image->getDimension().Height))));
			Driver->testGlErrorParanoid();
		}
		else
#endif
		{
			AutomaticMipmapUpdate = false;
			regenerateMipMapLevels();
		}
		if (hasMipMaps()) // might have changed in regenerateMipMapLevels
		{
			// enable bilinear mipmap filter
			setMinFilter(ETFT_LINEAR_MIPMAP_NEAREST);
			setMagFilter(ETFT_LINEAR);
		}
		else
#else
		unsetFlag(ETF_HAS_MIPMAPS);
		os::Printer::log("Did not create OpenGL texture mip maps.", ELL_INFORMATION);
#endif
		{
			// enable bilinear filter without mipmaps
			setMinFilter(ETFT_LINEAR);
			setMagFilter(ETFT_LINEAR);
		}
	}

	void* source = Image->lock();
	Driver->testGlErrorParanoid();
	
	
#if defined(_IRR_COMPILE_WITH_OPENGL_ES_) && defined(_IRR_WINDOWS_API_)
	if(1)
#else
	if(newTexture)
#endif
	{
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 InternalFormat,
					 Image->getDimension().Width,
					 Image->getDimension().Height,
					 0,
					 PixelFormat,
					 PixelType,
					 source);
		if (Driver->testGlErrorParanoid())
			os::Printer::log("Could not glTexImage2D", ELL_ERROR);
	}
	else
	{
		glTexSubImage2D(GL_TEXTURE_2D,
						0,
						0,
						0,
						Image->getDimension().Width,
						Image->getDimension().Height,
						PixelFormat,
						PixelType,
						source);
		if (Driver->testGlErrorParanoid())
			os::Printer::log("Could not glTexSubImage2D", ELL_ERROR);
	}
#ifdef _IRR_OPENGL_FORCE_COMMIT_TO_VRAM_
	forceCommitToVRAM();
#endif
	
	Image->unlock();

	if (Driver->testGlErrorDebug())
		os::Printer::log("Could not unlock", ELL_ERROR);
}


//! returns the size of a texture which would be the optimal size for rendering it
s32
CCommonGLTexture::getTextureSizeFromSurfaceSize(s32 size) const
{
	s32 ts = 0x01;
	while(ts < size)
		ts <<= 1;

	return ts;
}


//! lock function
void*
CCommonGLTexture::lock(bool readOnly)
{
	ReadOnlyLock |= readOnly;

	if (!Image)
	{
		if(PixelFormat == GL_ALPHA)
			Image = irrnew CImage(ECF_A8, ImageSize);
		else
		{
			Image = irrnew CImage(ECF_A8R8G8B8, ImageSize);
		}
	}

	if (isRenderTarget())
	{
		u8* pPixels = static_cast<u8*>(Image->lock());
		if (!pPixels)
		{
			return 0;
		}

#if !defined(_IRR_USE_PS3_DEVICE_)
		// we need to keep the correct texture bound...
		GLint tmpTexture;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &tmpTexture);
#endif

		glBindTexture(GL_TEXTURE_2D, TextureName);

		// allows to read pixels in top-to-bottom order
#ifdef GL_MESA_pack_invert
		if (Driver->queryOpenGLFeature(IRR_MESA_pack_invert))
			glPixelStorei(GL_PACK_INVERT_MESA, GL_TRUE);
#endif

#if !defined(_IRR_COMPILE_WITH_OPENGL_ES_) && !defined(_IRR_USE_PS3_DEVICE_)
		glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pPixels);
#endif

#ifdef GL_MESA_pack_invert
		if (Driver->queryOpenGLFeature(IRR_MESA_pack_invert))
			glPixelStorei(GL_PACK_INVERT_MESA, GL_FALSE);
		else
#endif
		{
			// opengl images are vertically flipped, so we have to fix that here.
			const s32 pitch=Image->getPitch();
			u8* p2 = pPixels + (ImageSize.Height - 1) * pitch;
			SScopedProcessBuffer tmpBuffer(pitch);
			for (s32 i=0; i < ImageSize.Height; i += 2)
			{
				memcpy(tmpBuffer.get(), pPixels, pitch);
				memcpy(pPixels, p2, pitch);
				memcpy(p2, tmpBuffer.get(), pitch);
				pPixels += pitch;
				p2 -= pitch;
			}
		}
		Image->unlock();

#if !defined(_IRR_USE_PS3_DEVICE_)
		//reset old bound texture
		glBindTexture(GL_TEXTURE_2D, tmpTexture);
#endif

	}
	return Image->lock();
}

//! unlock function
void
CCommonGLTexture::unlock()
{
	Image->unlock();
	if (!ReadOnlyLock)
		copyTexture(false);
	ReadOnlyLock = false;
}

//! Returns size of the original image.
const core::dimension2d<s32>&
CCommonGLTexture::getOriginalSize() const
{
	return ImageSize;
}

//! Returns size of the texture.
const core::dimension2d<s32>&
CCommonGLTexture::getSize() const
{
	if (Image)
		return Image->getDimension();
	else
		return ImageSize;
}

E_DRIVER_TYPE
CCommonGLTexture::getDriverType() const
{
	return EDT_OPENGL_FAMILY;
}

//! returns pitch of texture (in bytes)
u32
CCommonGLTexture::getPitch() const
{
	if (Image)
		return Image->getPitch();
	else
		return 0;
}


//! return open gl texture name
GLuint
CCommonGLTexture::getOpenGLTextureName() const
{
	return TextureName;
}

//! return open gl texture name
GLuint
CCommonGLTexture::getOpenGLDepthTextureName() const
{
	return DepthTextureName;
}

//! Regenerates the mip map levels of the texture. Useful after locking and
//! modifying the texture
void
CCommonGLTexture::regenerateMipMapLevels()
{
	if (AutomaticMipmapUpdate || !hasMipMaps())
		return;
	if ((Image->getDimension().Width==1) && (Image->getDimension().Height==1))
		return;

#if defined(_IRR_COMPILE_WITH_PS3_)
	glGenerateMipmapOES(GL_TEXTURE_2D);
	Driver->testGlErrorParanoid();
#else
	// Manually create mipmaps
	u32 width = Image->getDimension().Width;
	u32 height = Image->getDimension().Height;
	u32 i = 0;
	// below is generally too large to use a process buffer 
	u8* target = irrnew u8[Image->getImageDataSizeInBytes()];
	do
	{
		if (width>1)
			width>>=1;
		if (height>1)
			height>>=1;
		++i;
		Image->copyToScaling(target, width, height, Image->getColorFormat());
		glTexImage2D(GL_TEXTURE_2D,
					 i,
					 InternalFormat,
					 width,
					 height,
					 0,
					 PixelFormat,
					 PixelType,
					 target);
		Driver->testGlErrorParanoid();
	}
	while (width != 1 || height != 1);
#ifndef _IRR_COMPILE_WITH_OPENGL_ES_
	setMaxLOD(f32(i));
#endif
	delete [] target;
#endif
#ifdef _IRR_OPENGL_FORCE_COMMIT_TO_VRAM_
	updateParameters();
	forceCommitToVRAM();
#endif
	Driver->testGlErrorParanoid();
	Image->unlock();
}

bool
CCommonGLTexture::isFrameBufferObject() const
{
    return ColorFrameBuffer != 0;
}

void
CCommonGLTexture::setIsRenderTarget(bool isTarget)
{
	if (isTarget)
	{
		setFlag(ETF_IS_RENDER_TARGET);
	}
	else
	{
		unsetFlag(ETF_IS_RENDER_TARGET);
	}
}

void
CCommonGLTexture::updateParameters() const
{
	Driver->testGlErrorParanoid();
#if !defined(_IRR_COMPILE_WITH_PS3_)
	_IRR_DEBUG_BREAK_IF(Driver->getTextureBinding() != TextureName);
#endif
	if (getFlag(ETF_MIN_FILTER_DIRTY))
	{
		glTexParameteri(GL_TEXTURE_2D,
						GL_TEXTURE_MIN_FILTER,
						FilterMap[getMinFilter()]);
	}
	if (getFlag(ETF_MAG_FILTER_DIRTY))
	{
		glTexParameteri(GL_TEXTURE_2D,
						GL_TEXTURE_MAG_FILTER,
						FilterMap[getMagFilter()]);
	}
	if (getFlag(ETF_WRAP_S_DIRTY))
	{
		glTexParameteri(GL_TEXTURE_2D, 
						GL_TEXTURE_WRAP_S, 
						WrapModeMap[getWrapS()]);
	}
	if (getFlag(ETF_WRAP_T_DIRTY))
	{
		glTexParameteri(GL_TEXTURE_2D, 
						GL_TEXTURE_WRAP_T, 
						WrapModeMap[getWrapT()]);
	}
#ifdef GL_EXT_texture_filter_anisotropic
	if (getFlag(ETF_ANISOTROPY_DIRTY) && Driver->queryOpenGLFeature(IRR_EXT_texture_filter_anisotropic))
	{
		glTexParameterf(GL_TEXTURE_2D,
						GL_TEXTURE_MAX_ANISOTROPY_EXT,
						core::min_(static_cast<f32>(Driver->getMaxAnisotropy()),
								   getAnisotropy()));
	}
#endif
	
#ifdef _IRR_COMPILE_WITH_OPENGL_ES_
	#ifdef GL_EXT_texture_lod_bias
	if (getFlag(ETF_LOD_BIAS_DIRTY))
	{
		glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, getLODBias());
	}
	#endif
#else
	if (getFlag(ETF_LOD_BIAS_DIRTY))
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, getLODBias());
	}
	if (getFlag(ETF_MIN_LOD_DIRTY))
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, getMinLOD());
	}
	if (getFlag(ETF_MAX_LOD_DIRTY))
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, getMaxLOD());
	}
#endif

	Driver->testGlErrorDebug();
	clearDirtyFlags();
}

//! Returns whether the texture's content is valid.
bool
CCommonGLTexture::isValid() const
{
	return TextureName != 0;
}

//! Bind Render Target Texture
void
CCommonGLTexture::bindRTT()
{
	glViewport(0, 0, getSize().Width, getSize().Height);
	if (isFrameBufferObject())
	{
#if defined(GL_EXT_framebuffer_object) || defined(GL_OES_framebuffer_object) || !defined(_IRR_COMPILE_WITH_PS3_)
		if (ColorFrameBuffer != 0)
			Driver->bindFramebuffer(GL_FRAMEBUFFER, ColorFrameBuffer);
#endif
	}
}
void CCommonGLTexture::copyPixelsFromColorBuffer(const irr::core::rect<irr::s32>& bounder)
{
    glBindTexture(GL_TEXTURE_2D, getOpenGLTextureName());
    glCopyTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, bounder.UpperLeftCorner.X,bounder.UpperLeftCorner.Y,
        bounder.getWidth(),bounder.getHeight(),0);
}

//! Unbind Render Target Texture
void
CCommonGLTexture::unbindRTT()
{
	if (isFrameBufferObject())
	{
#if defined(GL_EXT_framebuffer_object) || defined(GL_OES_framebuffer_object) || defined(_IRR_COMPILE_WITH_PS3_)
		if (ColorFrameBuffer != 0)
		{
		//glBindTexture(GL_TEXTURE_2D, getOpenGLDepthTextureName());
		// Copy Our ViewPort To The Texture
		//glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, getSize().Width, getSize().Height);

		Driver->bindFramebuffer(GL_FRAMEBUFFER, 0);
		}
#endif
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, getOpenGLTextureName());
		// Copy Our ViewPort To The Texture
		
#ifdef _IRR_COMPILE_WITH_OPENGL_ES_
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0,  getSize().Width, getSize().Height, 0);
#else
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, getSize().Width, getSize().Height);
#endif
	}
}

#if defined(GL_EXT_framebuffer_object) || defined(GL_OES_framebuffer_object) || defined(_IRR_COMPILE_WITH_PS3_)
bool
checkFBOStatus(CCommonGLDriver* Driver)
{
	GLenum status = Driver->checkFramebufferStatus(GL_FRAMEBUFFER);

	switch (status)
	{
		//Our FBO is perfect, return true
		case GL_FRAMEBUFFER_COMPLETE:
			return true;

#if !defined(_IRR_COMPILE_WITH_OPENGL_ES_) && !defined(_IRR_COMPILE_WITH_PS3_)
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
			os::Printer::log("FBO has invalid read buffer", ELL_ERROR);
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
			os::Printer::log("FBO has invalid draw buffer", ELL_ERROR);
			break;
#endif

		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			os::Printer::log("FBO has one or several incomplete image attachments", ELL_ERROR);
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS:
			os::Printer::log("FBO has one or several image attachments with different internal formats", ELL_ERROR);
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
			os::Printer::log("FBO has one or several image attachments with different dimensions", ELL_ERROR);
			break;

// not part of fbo_object anymore, but won't harm as it is just a return value
#ifdef GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT
		case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT:
			os::Printer::log("FBO has a duplicate image attachment", ELL_ERROR);
			break;
#endif

		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			os::Printer::log("FBO missing an image attachment", ELL_ERROR);
			break;

		case GL_FRAMEBUFFER_UNSUPPORTED:
			os::Printer::log("FBO format unsupported", ELL_ERROR);
			break;

		default:
			break;
	}
	os::Printer::log("FBO error", ELL_ERROR);
	return false;
}
#endif

} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_OPENGL_ || _IRR_COMPILE_WITH_OPENGL_ES_
