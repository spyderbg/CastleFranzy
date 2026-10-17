#include "stdafx.h"
#include "IrrCompileConfig.h"
#ifdef _IRR_USE_WII_DEVICE_

#include "CWiiTexture.h"
#include "WiiTextureConv.h"
#include "IReadFile.h"
#include "irrMath.h"
//#include "irros.h"

#include <revolution.h>

namespace irr
{
namespace video
{

namespace
{

const static GXTexFilter FilterMap[] =
{
	GX_NEAR,          // ETFT_NEAREST
	GX_LINEAR,        // ETFT_LINEAR
	GX_NEAR_MIP_NEAR, // ETFT_NEAREST_MIPMAP_NEAREST
	GX_LIN_MIP_NEAR,  // ETFT_LINEAR_MIPMAP_NEAREST
	GX_NEAR_MIP_LIN,  // ETFT_NEAREST_MIPMAP_LINEAR
	GX_LIN_MIP_LIN,   // ETFT_LINEAR_MIPMAP_LINEAR
};

const static GXTexWrapMode WrapModeMap[] =
{
	GX_REPEAT, // ETC_REPEAT
	GX_CLAMP,  // ETC_CLAMP
	GX_CLAMP,  // ETC_CLAMP_TO_EDGE
	GX_CLAMP,  // ETC_CLAMP_TO_BORDER
	GX_MIRROR, // ETC_MIRROR
};

}

CWiiTexture::CWiiTexture(const char* name, 
						 int w, 
						 int h, 
						 GXTexFmt fmt, 
						 void* data)
	: ITexture(name)
	, TexData(reinterpret_cast<u8*>(data))
	, TexSize(0)
	, File(0)
{
	unsetFlag(ETFE_OWNS_TEX_DATA);

	#ifdef _DEBUG
	setDebugName("CWiiTexture");
	#endif

	Dim.set(w, h);
	DimOriginal.set(w, h);
	
	int bpp = 4;
	switch (fmt)
	{
		case GX_TF_RGB565:	bpp = 16; break;
		case GX_TF_RGB5A3:	bpp = 16; break;
		case GX_TF_IA8:		bpp = 8; break;
	}
	
	TexSize = (w * h * bpp) / 8;

	GXTexWrapMode wrapS = GX_REPEAT;
	GXTexWrapMode wrapT = GX_REPEAT;
	if (!core::isPowerOfTwo(w))
	{
		wrapS = GX_CLAMP;
		setWrapS(ETC_CLAMP);
	}
	if (!core::isPowerOfTwo(h))
	{
		wrapT = GX_CLAMP;
		setWrapT(ETC_CLAMP);
	}

	DCFlushRange(data, TexSize);
	GXInitTexObj(&TexObj, data, w, h, fmt, wrapS, wrapT, GX_FALSE);
}

//! constructor
CWiiTexture::CWiiTexture(IImage* image, 
						 const char* name, 
						 CWiiDriver* driver)
	: ITexture(name)
	, TexData(0)
	, TexSize(0)
	, File(0)
{
	#ifdef _DEBUG
	setDebugName("CWiiTexture");
	#endif

	if (image)
	{
		DimOriginal.set(image->getDimension().Width, image->getDimension().Height);

		s32 width = image->getDimension().Width;//256;//8;//512;//
		s32 height = image->getDimension().Height;//256;//8;//512;//

		// TODO: limit to 1024x1024, scale to powers of two if it's a mipmap?

		TexSize =  ((width + 3) & ~3) * ((height + 3) & ~3) * 4 ;
		TexData = irrnew u8[TexSize];
		setFlag(ETFE_OWNS_TEX_DATA);
		image->copyToScaling(TexData, width, height); 

#if 0
		// test: for color pattern
		memset(TexData, 0x00, width * height * 4);
		GXColor* c = (GXColor*)TexData;
		int texCount = width * height ;	
		const GXColor col1 = { 0xFF, 0xFF, 0xFF, 0xFF };	
		const GXColor col2 = { 0xFF, 0x00, 0x00, 0xFF };
		const GXColor col3 = { 0x00, 0xFF, 0x00, 0xFF };	
		const GXColor col4 = { 0x00, 0x00, 0x00, 0xFF };
		for (int line = 0 ; line < height ; ++line)
		{
			for (int column = 0 ; column < width ; ++column)
			{
				const GXColor col = ( line < (height/2) ) ?
					( ( column < (width/2) ) ? col1 : col2 ) :
					( ( column < (width/2) ) ? col3 : col4 ) ;
				*(c++) = col;
			}
		}
#endif

#if 0 // make all textures semi-transparent
		u8* p = TexData;
#    ifndef __BIG_ENDIAN__
		p += 3;
#    endif
		int texelCount = width * height ;
		while ( texelCount-- )
		{
			*p = 0x80;
			p += 4;
		}
#endif

		Dim.set(width, height);
		
		GXTexFmt format = GX_TF_RGBA8; // TODO: temp implementation

		// Convert to RGBA8 format
		ConvertRGBAtoRGBA8(TexData, width, height);

		int iPaddedSize = ((width + 3) & ~3) * ((height + 3) & ~3) * 4;
		DCFlushRange((void*)TexData, iPaddedSize);

		GXTexWrapMode wrapS = GX_REPEAT;
		GXTexWrapMode wrapT = GX_REPEAT;
		if (!core::isPowerOfTwo(width))
		{
			wrapS = GX_CLAMP;
			setWrapS(ETC_CLAMP);
		}
		if (!core::isPowerOfTwo(height))
		{
			wrapT = GX_CLAMP;
			setWrapT(ETC_CLAMP);
		}

		GXInitTexObj(&TexObj, TexData, (u16)width, (u16)height, format, wrapS, wrapT, GX_FALSE );
	}
}

#ifdef _IRR_ENABLE_NATIVE_TEXTURE_FORMAT_
CWiiTexture::CWiiTexture(io::IReadFile* file, 
						 const char* name, 
						 CWiiDriver* driver, 
						 bool refData)
	: ITexture(name)
	, TexData(0)
	, TexSize(0)
	, File(0)
{
	#ifdef _DEBUG
	setDebugName("CWiiTexture");
	#endif

	if (file)
	{
		struct
		{
			char magick[8];
			u16 width, height;
			u32 format;
			f32 minLOD, maxLOD;
			u32 size;
			u16 paletteSize;
			u16 paletteFormat;
		} header;

		// setup/read header
		memset(&header.magick[0], 0, 8);
		s32 sizeRead = file->read(&header.magick[0], sizeof(header));

		// all palettes have 16 bits pixel formats
		TexSize = header.size + header.paletteSize * sizeof(u16);

		if (header.format >= GX_TF_C4 && header.format <= GX_TF_C14X2)
		{
			setFlag(ETFE_IS_PALETTIZED);
		}

		// ensure data validation
		if (strncmp(header.magick, "BTEXwii", 8) == 0
			&& sizeRead == sizeof(header)
			&& (s32)TexSize <= file->getSize() - sizeof(header)
			&& (!isPalettized() || header.paletteSize > 0))
		{
			if (refData && file->isAllInMemory())
			{
				File = file;
				File->grab();
				TexData = static_cast<u8*>(file->getBuffer(0)) + (int)file->getPos();
				sizeRead = file->getSize() - file->getPos();
			}
			else
			{
				TexData = irrnew u8 [TexSize]; // is this allocation 32B aligned?	
				setFlag(ETFE_OWNS_TEX_DATA);
				sizeRead = file->read(TexData, TexSize);
			}
			_IRR_DEBUG_BREAK_IF((((long)TexData) % 32) != 0);
			_IRR_DEBUG_BREAK_IF((((long)TexData + header.size) % 32) != 0);

			if (sizeRead == TexSize)
			{
				Dim.Width = DimOriginal.Width = header.width;
				Dim.Height = DimOriginal.Height = header.height;
				setMinLOD(header.minLOD);
				setMaxLOD(header.maxLOD);

				if (header.maxLOD > header.minLOD)
				{
					setFlag(ETF_HAS_MIPMAPS);
					// default value as per GXInitTexObj
					setMinFilter(ETFT_LINEAR_MIPMAP_LINEAR);
				}
				else
				{
					setMinFilter(ETFT_LINEAR);
				}

				DCFlushRange(TexData, TexSize);

				GXTexWrapMode wrapS = GX_REPEAT;
				GXTexWrapMode wrapT = GX_REPEAT;
				if (!core::isPowerOfTwo(header.width))
				{
					wrapS = GX_CLAMP;
					setWrapS(ETC_CLAMP);
				}
				if (!core::isPowerOfTwo(header.height))
				{
					wrapT = GX_CLAMP;
					setWrapT(ETC_CLAMP);
				}
				
				if (isPalettized())
				{
					// We will have to do something better eventually if we want
					// to share palettes
					GXInitTexObjCI(&TexObj,
								   TexData,
								   header.width,
								   header.height,
								   (GXCITexFmt)header.format,
								   wrapS,
								   wrapT,
								   hasMipMaps(),
								   GX_TLUT0);
					GXInitTlutObj(&TlutObj,
								  TexData + header.size,
								  (GXTlutFmt)header.paletteFormat,
								  header.paletteSize);
				}
				else
				{
					GXInitTexObj(&TexObj,
								 TexData,
								 header.width,
								 header.height,
								 (GXTexFmt)header.format,
								 wrapS,
								 wrapT,
								 hasMipMaps());
				}
			}
			else
			{
				if (File)
				{
					File->drop();
				}
				else if(ownsTexData())
				{
					delete[] TexData;
				}
				TexData = 0;
			}
		}
	}
}
#endif

CWiiTexture::CWiiTexture(int w, 
						 int h, 
						 const char* name, 
						 CWiiDriver* driver)
	: ITexture(name)
	, TexData(0)
	, TexSize(0)
	, File(0)
{
	#ifdef _DEBUG
	setDebugName("CWiiTextureRT");
	#endif

	Dim.set(w, h);
	DimOriginal.set(w, h);

	setFlag(ETF_IS_RENDER_TARGET);
	setFlag(ETFE_OWNS_TEX_DATA);

	GXTexWrapMode wrapS = GX_REPEAT;
	GXTexWrapMode wrapT = GX_REPEAT;
	if (!core::isPowerOfTwo(w))
	{
		wrapS = GX_CLAMP;
		setWrapS(ETC_CLAMP);
	}
	if (!core::isPowerOfTwo(h))
	{
		wrapT = GX_CLAMP;
		setWrapT(ETC_CLAMP);
	}

	GXTexFmt format = GX_TF_RGBA8;
	TexSize = GXGetTexBufferSize(w, h, format, GX_FALSE, 0); //((w + 3) & ~3) * ((h + 3) & ~3) * 4;
	TexData = irrnew u8[TexSize];
	memset(TexData, 0, TexSize);
	DCFlushRange((void*)TexData, TexSize);
	GXInitTexObj(&TexObj, TexData, w, h, format, wrapS, wrapT, GX_FALSE);
}

//! destructor
CWiiTexture::~CWiiTexture()
{
	//if (Image)
	//	Image->drop();

	//if (Texture)
	//	Texture->drop();

	if (File)
	{
		File->drop();
	}
	else if (ownsTexData())
	{
		delete[] TexData;
	}
	TexData = 0;
}

//! lock function
void* CWiiTexture::lock(bool u)
{
	//return Image->lock();
	return TexData;
}

//! unlock function
void CWiiTexture::unlock()
{
	//if (Image != Texture)
	//{
	//	os::Printer::log("Performance warning, slow unlock of non power of 2 texture.", ELL_WARNING);
	//	Image->copyToScaling(Texture);
	//}

	//Image->unlock();
}

//! Returns original size of the texture.
const core::dimension2d<s32>& CWiiTexture::getOriginalSize() const
{
	return DimOriginal;
}

//! Returns (=size) of the texture.
const core::dimension2d<s32>& CWiiTexture::getSize() const
{
	return Dim;
}

//! returns driver type of texture (=the driver, who created the texture)
E_DRIVER_TYPE CWiiTexture::getDriverType() const
{
	return EDT_WII;
}

//! returns color format of texture
ECOLOR_FORMAT CWiiTexture::getColorFormat() const
{
	return ECF_UNKNOWN;
}

//! returns pitch of texture (in bytes)
u32 CWiiTexture::getPitch() const
{
	//return Image->getDimension().Width * 2;
	return 0;
}

//! Regenerates the mip map levels of the texture. Useful after locking and
//! modifying the texture
void CWiiTexture::regenerateMipMapLevels()
{
	// our software textures don't have mip maps
}

void CWiiTexture::setRenderTarget(bool val)
{
	if (val)
	{
		setFlag(ETF_IS_RENDER_TARGET);
	}
	else
	{
		unsetFlag(ETF_IS_RENDER_TARGET);
	}
}

//! Bind RenderTargetTexture
void CWiiTexture::bindRTT()
{
}

//! Unbind RenderTargetTexture
void CWiiTexture::unbindRTT()
{
	// Copy Our ViewPort To The Texture
	GXSetTexCopySrc(0, 0, getSize().Width, getSize().Height);
	GXSetTexCopyDst(getSize().Width, getSize().Height, GX_TF_RGBA8, GX_FALSE);
	GXCopyTex((void*)TexData, GX_FALSE);

	//const int length = GXGetTexBufferSize(getSize().Width, getSize().Height, m_format, m_mipmap, 0);
	//DCFlushRange((void *)m_pTexData, length);
}

//! Returns whether the texture's content is valid.
bool CWiiTexture::isValid() const
{
	return TexData != NULL;
}

size_t CWiiTexture::getTexSize() const
{
	return TexSize;
}

void CWiiTexture::setIsBiasClamped(bool value)
{
	if (value)
	{
		setFlag(ETFE_IS_BIAS_CLAMPED);
	}
	else
	{
		unsetFlag(ETFE_IS_BIAS_CLAMPED);
	}
	GXInitTexObjBiasClamp(&TexObj, value);
}

void CWiiTexture::updateParameters() const
{
	if (getFlag(ETF_MIN_FILTER_DIRTY
				| ETF_MAG_FILTER_DIRTY
				| ETF_ANISOTROPY_DIRTY
				| ETF_MIN_LOD_DIRTY
				| ETF_MAX_LOD_DIRTY
				| ETF_LOD_BIAS_DIRTY))
	{
		// sanitize filters
		if (getMinFilter() > ETFT_LINEAR && !hasMipMaps())
		{
			const_cast<CWiiTexture*>(this)->setMinFilter(ETFT_LINEAR);
		}

		GXInitTexObjLOD(&TexObj,
						FilterMap[getMinFilter()],
						FilterMap[getMagFilter()],
						getMinLOD(),
						getMaxLOD(),
						getLODBias(),
						getIsBiasClamped(),
						getIsBiasClamped() || getAnisotropy() >= 2.0f,
						getAnisotropy() < 2.0
						? GX_ANISO_1
						: (getAnisotropy() < 4.0
						   ? GX_ANISO_2
						   : GX_ANISO_4));
	}
	if (getFlag(ETF_WRAP_S_DIRTY | ETF_WRAP_T_DIRTY))
	{
		// validate power-of-two restriction on wrapping
		if (!core::isPowerOfTwo(Dim.Width))
		{
			const_cast<CWiiTexture*>(this)->setWrapS(ETC_CLAMP);
		}
		if (!core::isPowerOfTwo(Dim.Height))
		{
			const_cast<CWiiTexture*>(this)->setWrapT(ETC_CLAMP);
		}
		GXInitTexObjWrapMode(&TexObj,
							 WrapModeMap[getWrapS()],
							 WrapModeMap[getWrapT()]);
	}
	clearDirtyFlags();
}

} // end namespace video
} // end namespace irr


#endif // _IRR_COMPILE_WITH_WII_

