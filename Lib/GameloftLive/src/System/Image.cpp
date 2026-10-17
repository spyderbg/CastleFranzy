// Copyright (C) 2005 Gameloft. All rights reserved.
// GAMELOFT PROPRIETARY/CONFIDENTIAL.
//
// Author: Qiu Wei Min
#include "GLLiveStdafx.h"
#include "Image.h"
#include "GLLive_Stream.h"
#include "ResourceManager.h"
#include "MyGraphics.h"


#if PLATFORM == PLATFORM_MAC_OS_X
#include <OpenGLES/ES1/gl.h>
#elif IPHONE > 0
#include "gl.h"
#else
#include "gl.h"
#endif

#define RGB565_2_ARGB(clr)    ( ((clr & 0xf800) << 8) | ((clr & 0x7e0) << 5 )| ((clr & 0x1f) << 3 ))
#define ARGB_2_RGB565(clr)    ((clr >> 8 ) & 0xf800) | ((clr >> 5 ) & 0x7e0) | ((clr >> 3 ) & 0x1f)

#ifdef WIN32
int GenTextureFromPNGBuf(char* buf, int buf_size, int &width, int &height){return 0;}
#else
extern int GenTextureFromPNGBuf(char* buf, int buf_size, int &width, int &height);
#endif
namespace gllive
{
	CGLLiveImage* CGLLiveImage::CreateImage(CGLLiveStream &stream/*, MemoryType memType*/)
	{
		unsigned int pos = stream.GetPosition();
		unsigned char bpp = stream.ReadUByte();
		unsigned char flag = stream.ReadUByte();

		stream.SetPosition(pos);
		if(flag & BBM_PALETTE)
		{
			if(flag & BBM_COMPRESSION_RLE)
			{
				//not implemented
				ASSERT(false);
			}
			else
				return NEW/*GL_NEW(memType)*/ CGLLiveIndexedImage(stream);
		}
		else
		{
			ASSERT(!(flag & BBM_COMPRESSION_RLE)); //not allowed
			return NEW/*GL_NEW(memType)*/ CGLLiveRgbImage(stream);
		}
		ASSERT(false);
		return NULL;
	}

	CGLLiveImage::ImageType CGLLiveImage::GetImageType(CGLLiveStream& stream)
	{
		TextureHeader h;	
		stream.Read(&h, sizeof(TextureHeader));
		stream.SetPosition(0);

		if(h.format == GL_RGB)
		{
			if(h.pixel == GL_UNSIGNED_SHORT_5_6_5)
			{
				return TYPE_RGB;
			}
		}
		else if(h.format == GL_RGBA)
		{
			if(h.pixel == GL_UNSIGNED_BYTE || h.pixel == GL_BYTE || h.pixel == GL_UNSIGNED_SHORT_4_4_4_4)
			{
				return TYPE_RGB;
			}
		}
		else if(h.format == GL_PALETTE8_R5_G6_B5_OES)
		{
			return TYPE_INDEXED;
		}
		
		return TYPE_UNKNOW;	
	}

	CGLLiveImage* CGLLiveImage::CreateImage(const char *pResName/*, MemoryType memType*/)
	{
		CGLLiveStream* s = NEW CResourceStream(pResName);	
		CGLLiveImage *pImage = CreateImage(*s);
		SAFE_DELETE(s);
		return pImage;
	}

	void CGLLiveImage::SetSize(int width, int height, int pitch)
	{
		m_width = width;
		m_height = height;
		m_pitch = pitch;
		if(((pitch - 1) & pitch) == 0 && ((height - 1) & height) == 0)
		{
			//Todo: optimize the following in ARM with CLZ instruction
			int i, wshift;
			for(i = 0; i < sizeof(m_pitch) * 8; i++)
			{
				if(pitch & (1 << i))
				{
					wshift = i;
					break;
				}
			}
			int hshift;
			for(i = 0; i < sizeof(m_height) * 8; i++)
			{
				if(height & (1 << i))
				{
					hshift = i;
					break;
				}
			}
			//If wshift == 15 && hshift == 15, m_sizeShift = 255,
			//but 255 is used to mean that width&height is not power of 2.
			ASSERT(wshift < 15 && hshift < 15);
			m_sizeShift = (hshift << 4) | wshift;
		}
		else
			m_sizeShift = 255;
	}

	CGLLiveIndexedImage::CGLLiveIndexedImage(CGLLiveStream &stream)
	: m_alphaData(NULL)
	{
		m_flag = 0;
		m_type = TYPE_INDEXED;
		m_bpp = 8;

		TextureHeader h;
		stream.Read(&h, sizeof(TextureHeader));

		//ASSERT(h.format == GL_PALETTE8_R5_G6_B5_OES);
		
		int width = h.width & 0xFFFF;
		int height = h.height & 0xFFFF;

		SetSize(width, height, width * ((m_bpp + 7) / 8));

		m_paletteEntryNumber = h.pal;
		m_paletteData = NEW NativePixel[m_paletteEntryNumber * 256];

		stream.Read(m_paletteData, m_paletteEntryNumber * 256 * sizeof(NativePixel));

		unsigned int pixelDataSize = GetPitch() * GetHeight();
		m_pixel = NEW unsigned char[pixelDataSize];
		stream.Read(m_pixel, pixelDataSize);
	}

	CGLLiveIndexedImage::~CGLLiveIndexedImage()
	{
		SAFE_DELETE_ARRAY(m_pixel);
		SAFE_DELETE_ARRAY(m_paletteData);
		SAFE_DELETE_ARRAY(m_alphaData);
	}
	void CGLLiveIndexedImage::SetPaletteColor(int color, int index)
	{
	#if NATIVE_COLOR_DEPTH == 16
		m_paletteData[index] = ARGB_2_RGB565(color);
	#else
		ASSERT(false);
	#endif
	}
	void CGLLiveIndexedImage::MergePal(int dst, NativePixel* palette, int size)
	{
		memcpy(m_paletteData + dst, palette, size * sizeof(NativePixel));
	}

	void CGLLiveIndexedImage::LoadMaskBitmap(CGLLiveStream &stream)
	{
		TextureHeader h;	
		stream.Read(&h, sizeof(TextureHeader));

		ASSERT(h.format == GL_PALETTE8_R5_G6_B5_OES);

		int width = h.width & 0xFFFF;
		int height = h.height & 0xFFFF;

		ASSERT(width == GetWidth() && height == GetHeight());

		stream.Skip(h.pal * 256 * sizeof(NativePixel));

		m_alphaData = NEW unsigned char[width * height];
		
		stream.Read(m_alphaData, width * height);
	}


	CGLLiveRgbImage::CGLLiveRgbImage(CGLLiveStream &stream)
	{
		TextureHeader h;	
		stream.Read(&h, sizeof(TextureHeader));

		ASSERT((h.format == GL_RGB && h.pixel == GL_UNSIGNED_SHORT_5_6_5 )
			|| (h.format == GL_RGBA && (h.pixel == GL_UNSIGNED_BYTE || h.pixel == GL_BYTE || h.pixel == GL_UNSIGNED_SHORT_4_4_4_4)));

		m_flag = 0;
		m_type = TYPE_RGB;
		m_alpha = h.format == GL_RGBA;

		if(h.pixel == GL_UNSIGNED_SHORT_5_6_5 || h.pixel == GL_UNSIGNED_SHORT_4_4_4_4)
		{
			m_bpp = 16;
		}
		else if(h.pixel == GL_UNSIGNED_BYTE || h.pixel == GL_BYTE)
		{
			m_bpp = 32;
		}
		else
		{
			ASSERT(0);
		}

		int width = h.width & 0xFFFF;
		int height = h.height & 0xFFFF;

		SetSize(width, height, width * ((m_bpp + 7) / 8));

		unsigned int pixelDataSize = GetPitch() * GetHeight();

		m_pixel = NEW unsigned char[pixelDataSize];

		stream.Read(m_pixel, pixelDataSize);


		if(m_bpp == 32)
		{
			u32* pixel = (u32*)m_pixel;
			u32 idx, idx1, idx2, col1, col2;

			for(int i = 0; i < height / 2; i++)
			{
				for(int j = 0; j < width; j++)
				{
					idx1 = idx = j + i * width;
					col1 = (pixel[idx] & 0xFF00FF00) |  ((pixel[idx] & 0xFF) << 16) | ((pixel[idx] & 0xFF0000) >> 16);

					idx2 = idx = j + (height - 1 - i) * width;
					col2 = (pixel[idx] & 0xFF00FF00) |  ((pixel[idx] & 0xFF) << 16) | ((pixel[idx] & 0xFF0000) >> 16);

					pixel[idx1] = col2;
					pixel[idx2] = col1;
				}
			}
		}
		else if(m_bpp == 16 && h.pixel == GL_UNSIGNED_SHORT_4_4_4_4)
		{
			u16* pixel = (u16*)m_pixel;
			u32 idx, idx1, idx2, col1, col2;

			for(int i = 0; i < height / 2; i++)
			{
				for(int j = 0; j < width; j++)
				{
					idx1 = idx = j + i * width;
					col1 = pixel[idx];

					idx2 = idx = j + (height - 1 - i) * width;
					col2 = pixel[idx];

					pixel[idx1] = col2;
					pixel[idx2] = col1;
				}
			}
		}

		m_needFreePixel = 1;
	}

	CGLLiveRgbImage::~CGLLiveRgbImage()
	{
		if(m_needFreePixel)
			SAFE_DELETE_ARRAY(m_pixel);
	}

	CGLLiveMutableImage::CGLLiveMutableImage(unsigned int w, unsigned int h, unsigned int pitch, int bpp, void* pixel)
	{
		m_alpha = false;
		m_type = TYPE_MUTABLE;
		SetSize(w, h, pitch);
	//	ASSERT(bpp == COLOR_DEPTH);
		m_bpp = bpp;
		m_flag = 0;
		m_colorKey = ~0;//use non-exist color
		m_pixel = (unsigned char*)pixel;
		m_needFreePixel = 0;
		m_graphics = NULL;
	//#ifdef GRAPHICS_ENABLE_RGB8
	//	if(bpp == 8)
	//		m_graphics = GL_NEW(GetMemoryType(this)) CGLLiveGraphicsT<unsigned char, 8>();
	//#endif
	//#ifdef GRAPHICS_ENABLE_RGB12
	//	if(bpp == 12)
	//		m_graphics = GL_NEW(GetMemoryType(this)) CGLLiveGraphicsT<unsigned short, 12>();
	//#endif
	//#ifdef GRAPHICS_ENABLE_RGB16
	//	if(bpp == 16)
	//		m_graphics = GL_NEW(GetMemoryType(this)) CGLLiveGraphicsT<unsigned short, 16>();
	//#endif
	//#ifdef GRAPHICS_ENABLE_RGB24
	//	if(bpp == 24)
	//		m_graphics = GL_NEW(GetMemoryType(this)) CGLLiveGraphicsT<unsigned int, 24>();
	//#endif
	//	ASSERT(m_graphics != NULL);
	//
	//	m_graphics->SetImage(this);
	}

	CGLLiveMutableImage::~CGLLiveMutableImage()
	{
		SAFE_DELETE(m_graphics);
	}

	CGLLiveMutableImage::CGLLiveMutableImage(unsigned int w, unsigned int h, int bpp/*, MemoryType memType*/)
	{
		m_type = TYPE_MUTABLE;
		m_bpp = bpp;
		int pitch;
		if(bpp == 8)
			pitch = 1;
		else if(bpp <= 16)
			pitch = 2;
		else if(bpp <= 32)
			pitch = 4;
		else
		{
			ASSERT(false);
			pitch = 0;
		}
		pitch *= w;
		SetSize(w, h, pitch);
		m_flag = 0;
		m_colorKey = ~0;//use non-exist color
		m_pixel = /*GL_NEW(memType)*/NEW unsigned char[pitch * h];
		m_needFreePixel = 1;
		m_graphics = NULL;
	//#if (PLATFORM != PLATFORM_MAC_OS_X) || defined(WIN32)
	//#ifdef GRAPHICS_ENABLE_RGB8
	//	if(bpp == 8)
	//		m_graphics = GL_NEW(GetMemoryType(this)) CGLLiveGraphicsT<unsigned char, 8>();
	//#endif
	//#ifdef GRAPHICS_ENABLE_RGB12
	//	if(bpp == 12)
	//		m_graphics = GL_NEW(GetMemoryType(this)) CGLLiveGraphicsT<unsigned short, 12>();
	//#endif
	//#ifdef GRAPHICS_ENABLE_RGB16
	//	if(bpp == 16)
	//		m_graphics = GL_NEW(GetMemoryType(this)) CGLLiveGraphicsT<unsigned short, 16>();
	//#endif
	//#ifdef GRAPHICS_ENABLE_RGB24
	//	if(bpp == 24)
	//		m_graphics = GL_NEW(GetMemoryType(this)) CGLLiveGraphicsT<unsigned int, 24>();
	//#endif
	//	ASSERT(m_graphics != NULL);
	//	m_graphics->SetImage(this);
	//#endif
	}

	CGLImage::CGLImage(CGLLiveStream &stream, bool bFlipV)
	{
		m_isNull = false;
		unsigned int pos = stream.GetPosition();
		//unsigned char bpp = stream.ReadUByte();
		//unsigned char flag = stream.ReadUByte();
		stream.SetPosition(pos);
		
		TextureHeader h;
		stream.Read(&h, sizeof(h));
		
		m_flag = 0;
		m_type = h.format;
		int width = h.width & 0xFFFF;
		int height = h.height & 0xFFFF;
		
		u32 nSize;

		if(h.format == GL_PALETTE4_R5_G6_B5_OES)
		{
			m_bpp = 4;
			nSize = h.width * h.height / 2 + 16 * sizeof(short);
		}
		else if(h.format == GL_PALETTE8_R5_G6_B5_OES)
		{
			m_bpp = 8;
			nSize = h.width * h.height + 256 * sizeof(short);
		}
		else if(h.format == GL_RGBA || h.format == GL_RGB)
		{
			if(h.pixel == GL_UNSIGNED_BYTE || h.pixel == GL_BYTE)
			{
				m_bpp = 32;
				nSize = h.width * h.height * sizeof(int);
			}
			else
			{
				m_bpp = 16;
				nSize = h.width * h.height * sizeof(short);
			}
		}
		else
		{
			ASSERT(0);
		}


		
		SetSize(width, height, m_bpp == 4 ? width / 2 : width * ((m_bpp + 7) / 8));
		
		char *pData = NEW char[nSize];
		stream.Read(pData, nSize);
		stream.Close();
		
		if(h.format == GL_PALETTE8_R5_G6_B5_OES)
		{
			u16 *pPalette = (u16*)pData;
			
			for (int i = 0; i < 256; i++)
			{
				register u16 col = pPalette[i];

				if (col == 0xF81F) // alpha bit ?
					pPalette[i] = pPalette[i] = 0xFFFE;//(col & 0xF800) | (col & 0x7C0) | ((col & 0x1F) << 1);
				else
					pPalette[i] = (col & 0xF800) | (col & 0x7C0) | ((col & 0x1F) << 1) | 0x1;
			}

			h.format = GL_PALETTE8_RGB5_A1_OES;
		}
		else if(h.format == GL_PALETTE4_R5_G6_B5_OES)
		{
			u16 *pPalette = (u16*)pData;

			for (int i = 0; i < 16; i++)
			{
				register u16 col = pPalette[i];

				if (col == 0xF81F) // alpha bit ?
					pPalette[i] = 0xFFFE;//(col & 0xF800) | (col & 0x7C0) | ((col & 0x1F) << 1);
				else
					pPalette[i] = (col & 0xF800) | (col & 0x7C0) | ((col & 0x1F) << 1) | 0x1;
			}

			h.format = GL_PALETTE4_RGB5_A1_OES;
		}

		if(bFlipV)
		{
			u32		pitch = GetPitch();
			char*	pTemp = NEW char[pitch];
			char*	d =  pData;

			if(m_bpp == 4)
				d += 16 * sizeof(short);
			else if(m_bpp == 8)
				d += 256 * sizeof(short);

			for(int i = 0; i < h.height / 2; i++)
			{
				char* t = d + i * pitch;
				char* b = d + (h.height - 1 - i) * pitch;
				memcpy(pTemp, t, pitch);
				memcpy(t, b, pitch);
				memcpy(b, pTemp, pitch);
			}

			SAFE_DELETE(pTemp);
		}

		GLuint textures[1];
		glGenTextures(1, textures);
		glBindTexture(GL_TEXTURE_2D, textures[0]);

		if(h.format == GL_RGBA || h.format == GL_RGB)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, h.format, h.width, h.height, 0, h.format, h.pixel, pData);	
		}
		else
		{
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, h.format, h.width, h.height, 0, nSize, pData);
		}

	#if USE_LINEAR_TEXTURE
		//if(h.format == GL_PALETTE8_RGB5_A1_OES || h.format == GL_PALETTE4_RGB5_A1_OES)
		//{
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//}
		//else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

	#else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	#endif

		m_glName = textures[0];
		
		SAFE_DELETE(pData);
	}

	CGLImage::CGLImage(char* buf, int buf_size)
	{
		m_isNull = false;
	#ifdef WIN32
		m_glName = 0;
		return;
	#endif
		int width = 0;
		int height = 0;
		m_glName = GenTextureFromPNGBuf(buf, buf_size, width, height);
		SetSize(width, height, width * 4);
	}

	CGLImage::CGLImage()
	{
		m_isNull = true;
	}

	CGLImage::~CGLImage()
	{
		if(!m_isNull)
			glDeleteTextures(1, &m_glName);
	}

	CGLImage* CGLImage::CreateImage(CGLLiveStream &stream/*, MemoryType memType*/)
	{	
		return NEW CGLImage(stream);
	}

	CGLImage* CGLImage::CreateImage(const char *pResName/*, MemoryType memType*/)
	{
		CGLLiveStream* s = NEW CResourceStream(pResName);	
		CGLImage *pImage = CreateImage(*s);
		SAFE_DELETE(s);
		return pImage;
	}

	bool CGLImage::IsNull()
	{
		return m_isNull;
	}
}