// Copyright (C) 2005 Gameloft. All rights reserved.
// GAMELOFT PROPRIETARY/CONFIDENTIAL.
//
// Author: Qiu Wei Min

#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "MyDisplay.h"

#if(PLATFORM==PLATFORM_MAC_OS_X)&&(!defined(WIN32))
#include <OpenGLES/ES1/gl.h>
#include <time.h>
#elif IPHONE > 0
#include "../tools/rasteroid3.1/include/egl/egl.h"
#else
#include "gl.h"
#endif

typedef struct
{
	unsigned short format;
	unsigned short pixel;
	unsigned short width;
	unsigned short height;
	unsigned short pal;
	unsigned short unused;
}TextureHeader;

enum {
	BBM_PALETTE = 0x01,		//Has palette data
	BBM_COLOR_KEY = 0x02,	//has color key (the transparent color)
	BBM_ALPHA = 0x04,		//has alpha data
	BBM_HAS_MODULE = 0x08,	//has module data
	
	BBM_COMPRESSION_RLE = 0x10	//the pixel data is compressed by RLE. If has module, it's compressed per module.
};


typedef struct _BBM_HEADER{
	unsigned char bpp;
	unsigned char flag;
	unsigned short width;
	unsigned short height;
	unsigned int colorKey;
	short nColors;
}BBM_HEADER;

/*
//BBM file format
typedef struct {
	unsigned char bpp;				//bit per pixel
	unsigned char flag;				//bit 0-2: palette location; bit 3-7: bit flag

	unsigned short width;
	unsigned short height;

#if (flag & BBM_COLOR_KEY)
	unsigned int colorKey;			//for indexed pixel, it's the entry in palette; for RGB pixel, it's the RGB number
#endif

#if (flag & BBM_PALETTE_MASK) == BBM_PALETTE_IN_SELF
	unsigned short paletteEntryNumber;	//number of palette entry
	unsigned char paletteEntryData[paletteEntryNumber][3];
#elif (flag & BBM_PALETTE_MASK) == BBM_PALETTE_IN_POOL
	unsigned short paletteIndexNumber;	//number of palette
	unsigned short paletteIndexData[paletteIndexNumber];	//palette index in palette pool
#endif

#if (flag & BBM_HAS_MODULE)
	unsigned short moduleNumber;	//number of module
	unsigned short moduleData[moduleNumber][4];	//module data
#endif

#if (flag & BBM_COMPRESSION_RLE)
	#if (flag & BBM_HAS_MODULE)
		unsigned short moduleRleDataSize[moduleNumber]; //size of each RLE module data
	#else
		unsigned int rleDataSize;
	#endif
#endif

	unsigned char pixelData[width * height * 8 / bpp];

}BBM;*/

class CGraphics;
class CStream;

/// @brief This class is used to hold graphical image data.
///
/// This class provide a set of functions similar to Image class in J2ME.
class CImage
{
public:
	/// @brief Image type
	enum ImageType {
		/// This is an indexed image and can be casted to CIndexedImage.
		TYPE_INDEXED,
		/// The is an image that hold RGB format pixel data and can be casted to CRgbImage. 
		TYPE_RGB,
		/// The is an image that can be changed.
		TYPE_MUTABLE,
		/// The is an image that hold pixel data compressed by RLE and can be casted to CRleImage. 
		TYPE_RLE,

		TYPE_UNKNOW
	};
	virtual ~CImage(){}

	/// @brief Creates an immutable image from the named resource.
	///	@param[in]	pResName	the name of the image resource.
	/// @param[in]	memType		the type of memory to hold created image object. Default value is POOL_BOTTOM.
	/// @return The created image. NULL if failed.
	static CImage* CreateImage(const char *pResName/*, MemoryType memType = POOL_BOTTOM*/);

	/// @brief Creates an immutable image from a stream.
	///	@param[in]	stream		the stream that containing the image data.
	/// @param[in]	memType		the type of memory to hold created image object. Default value is POOL_BOTTOM.
	/// @return The created image. NULL if failed.
	static CImage* CreateImage(CStream &stream/*, MemoryType memType = POOL_BOTTOM*/);

	/// @brief Get the color key of the image.
	///
	/// Color key indicate the color to be used for transparent or translucent effects.
	/// It is RGB value for non-indexed image and palette entry index for indexed image and RLE image.
	/// @return The value of the current color key.
	/// @warning Color key effect is always disabled for image with alpha channel
	unsigned int GetColorKey() const { return m_colorKey; }

	/// @brief Set the color key of the image.
	///
	/// Color key indicate the color to be used for transparent or translucent effects.
	/// It is RGB value for non-indexed image and palette entry index for indexed image and RLE image.
	/// @param key The value of the color key to set.
	/// @warning Color key effect is always disabled for image with alpha channel
	void SetColorKey(unsigned int key) { m_colorKey = key; }

	/// @brief Enable or disable the color key effect of the image.
	///
	/// Color key indicate the color to be used for transparent or translucent effects.
	/// It is RGB value for non-indexed image and palette entry index for indexed image and RLE image.
	/// When color key is disabled, the pixel with the color value of color key is treated as normal one.
	/// @param isEnabled Use true to enable color key, false to disable.
	/// @warning Color key effect is always disabled for image with alpha channel
	void EnableColorKey(bool isEnabled) {
		if(isEnabled)
			m_flag |= BBM_COLOR_KEY;
		else
			m_flag &= ~BBM_COLOR_KEY;
	}
	
	/// @brief Get the number of bit per pixel.
	/// @return The number of bit per pixel.
	int GetBitsPerPixel() const { return m_bpp; }

	/// @brief Get the width of the image in pixels.
	/// @return Width of the image.
	int GetWidth() const { return m_width; }

	/// @brief Get the height of the image in pixels.
	/// @return Height of the image.
	int GetHeight() const {	return m_height; }

	/// @brief Get the distance in bytes to the start of next line
	/// @return Distance in bytes to the start of next line.
	/// @warning Pitch value has no meaning for RLE image
	int GetPitch() const { return m_pitch; }

	/// @brief Check whether the pitch&height is power of 2.
	///
	/// Useful if the image is used as texture in 3D graphics engine.
	/// @return ture if the pitch and height is power of 2, else false.
	bool IsSizePowerOf2() const {return m_sizeShift != 255;}
	
	/// @brief Get the pitch in power of 2.
	///
	/// Useful if the image is used as texture in 3D graphics engine.
	/// If IsSizePowerOf2() returns true, then (1 << GetPitchShift()) == GetPitch().
	/// @return Pitch in power of 2 if IsSizePowerOf2() returns true, else 15.
	unsigned int GetPitchShift() const {return m_sizeShift >> 4;}

	/// @brief Get the height in power of 2.
	///
	/// Useful if the image is used as texture in 3D graphics engine.
	/// If IsSizePowerOf2() returns true, then (1 << GetHeightShift()) == GetHeight().
	/// @return Pitch in power of 2 if IsSizePowerOf2() returns true, else 15.
	unsigned int GetHeightShift() const {return m_sizeShift & 0xf;}

	/// @brief Get the type of image
	/// @return Type of the image define in ImageType.
	int GetType() const { return m_type; }
	
	/// @brief Get the pointer to image pixel data
	/// @return Pointer to the image pixel data.
	unsigned char* GetPixelData() const { return m_pixel; }
	
	unsigned int GetFlag() { return m_flag; }
	
	void SetSize(int width, int height, int pitch);

	static ImageType GetImageType(CStream& stream);
	
	bool HasAlpha(){return m_alpha > 0;};

protected:
	CImage() {}
	unsigned int m_type;
	unsigned char m_alpha;
	unsigned char m_bpp;				//bit per pixel
	unsigned char m_flag;				//bit 0-2: palette location; bit 3-7: bit flag
	unsigned char m_sizeShift;

	//Transparent color. it's RGB for non-indexed image and palette entry index for indexed image
	unsigned int m_colorKey;

	unsigned char *m_pixel;

private:
	CImage& operator=(const CImage& rhs);
	unsigned short m_width;
	unsigned short m_height;
	unsigned short m_pitch;				//y pitch in byte
};

/// @brief Represent an indexed image.
class CIndexedImage: public CImage {
public:
	CIndexedImage() {}
	CIndexedImage(CStream &stream);
	virtual ~CIndexedImage();

	/// @brief Get the number of palette entry
	/// @return The number of palette entry
	unsigned int GetPaletteEntryNumber() {
		return m_paletteEntryNumber;
	}

	/// @brief Get the palette data in native pixel format.
	/// @return Pointer to palette data.
	NativePixel* GetPaletteData() {
		return m_paletteData;
	}

	void MergePal(int dst,	NativePixel* palette,  int size);
	void LoadMaskBitmap(CStream &stream);
	inline unsigned char* GetAlphaData(){return m_alphaData;};

	void SetPaletteColor(int color, int index = 0);
protected:
	unsigned short	m_paletteEntryNumber;
	unsigned char*  m_alphaData;	
	NativePixel*	m_paletteData;
};

/// @brief Represent a RGB format image.
class CRgbImage: public CImage {
public:
	CRgbImage(CStream &stream);
	virtual ~CRgbImage();
protected:
	// Should not call this default constructor directly because nothing is done here.
	// This default constructor is provided for derive class like CMutableImage
	CRgbImage(){}

	char m_needFreePixel;
};

/// @brief Represent a mutable image.
///
/// The CMutableImage subclass describes an CImage with an accessible buffer of image data.
/// A CMutableImage is comprised of a CGraphics and a CRgbImage of image data.
/// The location of pixel data can be a specified address passed by parameter, or can be allocated
/// in memory pool by constructor.
class CMutableImage: public CRgbImage {
public:
	/// @brief Create a CMutableImage and attach it to a buffer on given address.
	/// @param[in]	w	Width of the mutable image
	///	@param[in]	h	Height of the mutable image
	///	@param[in]	pitch	Distance in bytes to the start of next line.
	///	@param[in]	bpp	Bit per pixel of the pixel buffer
	///	@param[in]	pixel	Pointer to pixel data
	CMutableImage(unsigned int w, unsigned int h, unsigned int pitch, int bpp, void* pixel);

	/// @brief Create a CMutableImage on memory pool with given format.
	/// @param[in]	w	Width of the mutable image
	///	@param[in]	h	Height of the mutable image
	///	@param[in]	bpp	Bit per pixel of the pixel buffer
	///	@param[in]	memType	Memory type of the pixel data
	CMutableImage(unsigned int w, unsigned int h, int bpp/*, MemoryType memType = POOL_BOTTOM*/);
	virtual ~CMutableImage();

	/// @brief Get the Graphics object that renders to this image.
	///
	/// The newly created Graphics object has the following properties: 
	/// - the destination is this Image object; 
	/// - the clip region encompasses the entire Image; 
	/// - the current color is black; 
	/// - the origin of the coordinate system is located at the upper-left corner of the Image.
	///
	/// The lifetime of Graphics objects created using this method is the same as this image's.
	/// @warning Unlike the J2ME, this function get the attached Graphics object, instead of create a new one.
	/// @return The Graphics object attached to this image
	CGraphics& GetGraphics() { return *m_graphics; }

	inline void SetData(unsigned char* pData){m_pixel = pData;};

protected:
	CGraphics *m_graphics;
};

class CRleImage: public CImage {
public:
};

class CGLImage: public CImage {
public:
	CGLImage(CStream &stream, bool bFlipV = false);
	~CGLImage();
	
	static CGLImage* CreateImage(CStream &stream/*, MemoryType memType*/);
	static CGLImage* CreateImage(const char *pResName/*, MemoryType memType*/);

	inline u32 GLName(){return m_glName;};
	
protected:
	u32	m_glName;	
};

#endif
