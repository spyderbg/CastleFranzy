#include "stdafx.h"
// Copyright (C) 2002-2008 Nikolaus Gebhardt / Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CCompressedImage.h"
#include "irrString.h"
//#include "SoftwareDriver2_helper.h"
//#include "CColorConverter.h"

namespace irr
{
namespace video
{

//! constructor
CCompressedImage::CCompressedImage(ECOLOR_FORMAT format, const core::dimension2d<s32>& size, void* data,
			u32 imageDataSizeInBytes, u32 mipMapCount, bool ownForeignMemory, bool deleteForeignMemory)
: Data(0), Size(size), Format(format), DeleteMemory(deleteForeignMemory), 
  MipMapCount(mipMapCount), ImageDataSizeInBytes(imageDataSizeInBytes)
{
	if (ownForeignMemory)
	{
		Data = (void*)0xbadf00d;
		initData();
		Data = data;
	}
	else
	{
		Data = 0;
		initData();
		memcpy(Data, data, ImageDataSizeInBytes);
	}
}

//! constructor
CCompressedImage::CCompressedImage(CCompressedImage* imageToCopy)
: Data(0), DeleteMemory(true)
{
	if (!imageToCopy)
		return;

	Format = imageToCopy->getColorFormat();
	Size = imageToCopy->getDimension();
	MipMapCount = imageToCopy->getMipMapCount();
	ImageDataSizeInBytes = imageToCopy->getImageDataSizeInBytes();
	initData();

	// now copy data from other image
	memcpy(lock(), imageToCopy->lock(), ImageDataSizeInBytes);
	unlock();
	imageToCopy->unlock();
}


//! assumes format and size has been set and creates the rest
void CCompressedImage::initData()
{
	BytesPerBlock = getBytesPerBlockFromFormat(Format);

	// DXT compressed data
	Pitch = BytesPerBlock * (Size.Width / 4);

	if (!Data)
		Data = irrnew s8[ImageDataSizeInBytes];
}


//! destructor
CCompressedImage::~CCompressedImage()
{
	if ( DeleteMemory )
		delete [] (s8*)Data;
}


//! Returns width and height of image data.
const core::dimension2d<s32>& CCompressedImage::getDimension() const
{
	return Size;
}



//! Returns bits per pixel.
u32 CCompressedImage::getBitsPerPixel() const
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}


//! Returns bytes per pixel
u32 CCompressedImage::getBytesPerPixel() const
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}

//! Returns bytes per block
u32 CCompressedImage::getBytesPerBlock() const
{
	return BytesPerBlock;
}



//! Returns image data size in bytes
u32 CCompressedImage::getImageDataSizeInBytes() const
{
	return ImageDataSizeInBytes;
}



//! Returns image data size in pixels
u32 CCompressedImage::getImageDataSizeInPixels() const
{
	return Size.Width * Size.Height;
}



//! returns mask for red value of a pixel
u32 CCompressedImage::getRedMask() const
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}



//! returns mask for green value of a pixel
u32 CCompressedImage::getGreenMask() const
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}



//! returns mask for blue value of a pixel
u32 CCompressedImage::getBlueMask() const
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}



//! returns mask for alpha value of a pixel
u32 CCompressedImage::getAlphaMask() const
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}



u32 CCompressedImage::getBytesPerBlockFromFormat(ECOLOR_FORMAT format)
{
	switch(format)
	{
	case ECF_COMPRESSED_RGB_S3TC_DXT1:
		return 8;
	case ECF_COMPRESSED_RGBA_S3TC_DXT3:
	case ECF_COMPRESSED_RGBA_S3TC_DXT5:
		return 16;
	}

	return 0;
}

//! sets a pixel
void CCompressedImage::setPixel(u32 x, u32 y, const SColor &color )
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}


//! returns a pixel
SColor CCompressedImage::getPixel(u32 x, u32 y) const
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return SColor(0,0,0,0);
}


//! returns the color format
ECOLOR_FORMAT CCompressedImage::getColorFormat() const
{
	return Format;
}

//! copies this surface into another
void CCompressedImage::copyTo(IImage* target, const core::position2d<s32>& pos)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

//! copies this surface into another
void CCompressedImage::copyTo(IImage* target, const core::position2d<s32>& pos, const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

//! copies this surface into another, using the alpha mask, an cliprect and a color to add with
void CCompressedImage::copyToWithAlpha(IImage* target, const core::position2d<s32>& pos, const core::rect<s32>& sourceRect, const SColor &color, const core::rect<s32>* clipRect)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

//! copies this surface into another, scaling it to the target image size
// note: this is very very slow. (i didn't want to write a fast version.
// but hopefully, nobody wants to scale surfaces every frame.
void CCompressedImage::copyToScaling(void* target, s32 width, s32 height, ECOLOR_FORMAT format, u32 pitch)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

//! copies this surface into another, scaling it to the target image size
// note: this is very very slow. (i didn't want to write a fast version.
// but hopefully, nobody wants to scale surfaces every frame.
void CCompressedImage::copyToScaling(IImage* target)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}


//! fills the surface with given color
void CCompressedImage::fill(const SColor &color)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}



} // end namespace video
} // end namespace irr
