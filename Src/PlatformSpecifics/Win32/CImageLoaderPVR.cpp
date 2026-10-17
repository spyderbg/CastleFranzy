// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".

// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "stdafx.h"

#include "CImageLoaderPVR.h"

#include "PVRTTexture.h"
#include "PVRTDecompress.h"

#include "IReadFile.h"
#include "SColor.h"
#include "CImage.h"
#include "irrString.h"

namespace irr
{
namespace video
{

//! constructor
CImageLoaderPVR::CImageLoaderPVR()
{
#ifdef _DEBUG
	setDebugName("CImageLoaderPVR");
#endif
}


//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".tga")
bool CImageLoaderPVR::isALoadableFileExtension(const c8* fileName) const
{
	return strstr(fileName, ".pvr") != 0;
}


//! returns true if the file maybe is able to be loaded by this class
bool CImageLoaderPVR::isALoadableFileFormat(io::IReadFile* file) const
{
	return false;
}

int LeastSignificantBit (u32 value)
{
	for (int i = 0; i < 32; ++i)
	{
		if (value & (1 << i))
		{
			return i;
		}
	}
	return 0;
}

//! creates a surface from the file
IImage* CImageLoaderPVR::loadImage(io::IReadFile* file) const
{
	/// Read PVR File
	int size = file->getSize();
	u8 * buffer = new u8[size];
	file->read(buffer, size);

	/// PVR Header
	PVR_Texture_Header PVRHeader;
	PVR_Texture_Header* psPVRHeader;
	unsigned long* pData;
	if(!PVRTIsLittleEndian())
	{
		PVRHeader = *((PVR_Texture_Header*)buffer);
		psPVRHeader = (PVR_Texture_Header*) &PVRHeader;
		pData = (unsigned long*) psPVRHeader;
		unsigned int ui32HeaderSize = sizeof(PVRHeader) / sizeof(unsigned long);

		for(unsigned int i = 0; i < ui32HeaderSize; ++i)
			PVRTByteSwap((unsigned char*) &pData[i], sizeof(unsigned long));
	}
	else
	{
		psPVRHeader = (PVR_Texture_Header*)buffer;
	}

	// create surface
	const int nSizeX = psPVRHeader->dwWidth;
	const int nSizeY = psPVRHeader->dwHeight;
	ECOLOR_FORMAT format = ECF_A8R8G8B8;

	if ((psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE) == OGL_RGBA_5551)
		format = ECF_A1R5G5B5;

	IImage* image = new CImage(format, core::dimension2di(nSizeX, nSizeY));
	if (!image)
		return NULL;
	
	if ((psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE) == OGL_PVRTC4 ||
		(psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE) == OGL_PVRTC2)
	{
		u8* src = buffer + psPVRHeader->dwHeaderSize;
		u8* dst = (u8*)image->lock();
		PVRTDecompressPVRTC(src, (psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE) == OGL_PVRTC2, nSizeX, nSizeY, dst);

		for (int i = 0; i < nSizeX * nSizeY; i++)
		{
			PVRTswap(dst[i*4+0], dst[i*4+2]);
		}
		for (int y = 0; y < nSizeY / 2; y++)
		{
			for (int x = 0; x < nSizeX; x++)
			{
				PVRTswap(((u32*)dst)[x + y * nSizeX], ((u32*)dst)[x + (nSizeY - y - 1) * nSizeX]);
			}
		}
	}
	else if ((psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE) == OGL_RGBA_5551)
	{
		u16* src = (u16*)(buffer + psPVRHeader->dwHeaderSize);
		u16* dst = (u16*)image->lock();
		memcpy(dst, src, nSizeX * nSizeY * 2);

		for (int i = 0; i < nSizeX * nSizeY; i++)
		{
			dst[i] = (dst[i] << 15) | (dst[i] >> 1);
		}
		for (int y = 0; y < nSizeY / 2; y++)
		{
			for (int x = 0; x < nSizeX; x++)
			{
				PVRTswap(((u16*)dst)[x + y * nSizeX], ((u16*)dst)[x + (nSizeY - y - 1) * nSizeX]);
			}
		}
	}
	else if ((psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE) == OGL_RGBA_8888)
	{
		u16* src = (u16*)(buffer + psPVRHeader->dwHeaderSize);
		u8* dst = (u8*)image->lock();
		memcpy(dst, src, nSizeX * nSizeY * 4);

		for (int i = 0; i < nSizeX * nSizeY; i++)
		{
			PVRTswap(dst[i*4+0], dst[i*4+2]);
		}
		for (int y = 0; y < nSizeY / 2; y++)
		{
			for (int x = 0; x < nSizeX; x++)
			{
				PVRTswap(((u32*)dst)[x + y * nSizeX], ((u32*)dst)[x + (nSizeY - y - 1) * nSizeX]);
			}
		}
	}

	image->unlock();

	delete [] buffer;
	return image;
}


//! creates a loader which is able to load windows bitmaps
IImageLoader* createImageLoaderPVR()
{
	return new CImageLoaderPVR();
}


} // end namespace video
} // end namespace irr

