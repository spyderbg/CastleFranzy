#ifndef __RawImage_H_INCLUDED__
#define __RawImage_H_INCLUDED__

#include "IImage.h"

namespace irr
{
namespace video
{


/*!
	Class for manipulating an IImage instanciated from raw image data.
	Heavily inspired on CImage class in irrlicht library 1.4.2
*/	
class RawImage : public IImage
{

public:

	//! constructor
	//! Constructs image from raw data
	RawImage(ECOLOR_FORMAT format, const core::dimension2d<s32>& size, void* data, bool copyImgData);

	//! destructor
	virtual ~RawImage();

	virtual void* lock() { return m_imgData; };
	virtual void unlock() {};

	virtual const irr::core::dimension2d<s32>& getDimension() const { return m_imgDimension; };
	
	//! Returns bits per pixel
	virtual u32 getBitsPerPixel() const { return m_BitsPerPixel; };
	//! Returns bytes per pixel
	virtual u32 getBytesPerPixel() const { return m_BytesPerPixel; };
	//! Returns image data size in bytes
	virtual u32 getImageDataSizeInBytes() const { return (m_imgDimension.getArea()*m_BytesPerPixel); };
	//! Returns image data size in pixels
	virtual u32 getImageDataSizeInPixels() const { return m_imgDimension.getArea(); };

	//! Returns a pixel
	virtual SColor getPixel(u32 x, u32 y) const;
	//! Sets a pixel
	virtual void setPixel(u32 x, u32 y, const SColor &color );

	//! Returns the color format
	virtual ECOLOR_FORMAT getColorFormat() const { return m_clrFormat; };

	//! Returns mask for red value of a pixel
	virtual u32 getRedMask() const { return m_RedMask; };
	//! Returns mask for green value of a pixel
	virtual u32 getGreenMask() const { return m_GreenMask; };
	//! Returns mask for blue value of a pixel
	virtual u32 getBlueMask() const { return m_BlueMask; };
	//! Returns mask for alpha value of a pixel
	virtual u32 getAlphaMask() const { return m_AlphaMask; };

	//! returns m_Pitch of image
	virtual u32 getPitch() const { return m_Pitch; }



	//! Copies the image into the target, scaling the image to fit -- NOT IMPLEMENTED FOR NOW
	virtual void copyToScaling(void* target, s32 width, s32 height, ECOLOR_FORMAT format=ECF_A8R8G8B8, u32 pitch=0) {};
	//! Copies the image into the target, scaling the image to fit -- NOT IMPLEMENTED FOR NOW
	virtual void copyToScaling(IImage* target) {};
	//! copies this surface into another -- NOT IMPLEMENTED FOR NOW
	virtual void copyTo(IImage* target, const core::position2d<s32>& pos=core::position2d<s32>(0,0)) {};
	//! copies this surface into another -- NOT IMPLEMENTED FOR NOW
	virtual void copyTo(IImage* target, const core::position2d<s32>& pos, const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect=0) {};
	//! copies this surface into another, using the alpha mask, an cliprect and a color to add with -- NOT IMPLEMENTED FOR NOW
	virtual void copyToWithAlpha(IImage* target, const core::position2d<s32>& pos,
			const core::rect<s32>& sourceRect, const SColor &color,
			const core::rect<s32>* clipRect = 0) {};
	//! fills the surface with black or white -- NOT IMPLEMENTED FOR NOW
	virtual void fill(const SColor &color) {};

private:

	void initialiseBitMasks();				// Initialise the color bit masks
	void initialiseBitsPerPixel();			// Initialise the bit per pixel data

	//! assumes format and size has been set and creates the rest
	void initialiseImgData(void* data, bool copy);		// Initialise the array containing image data

	void* m_imgData;						// The Image data

	core::dimension2d<s32> m_imgDimension;	// Image dimension

	u32 m_BitsPerPixel;						// Bits per pixel
	u32 m_BytesPerPixel;					// Bytes per pixel
	u32 m_Pitch;							// m_Pitch
	bool m_copyImgData;						// Should the image data be copied or referenced

	ECOLOR_FORMAT m_clrFormat;				// Color format used

	u32 m_RedMask;							// Mask for red channel
	u32 m_GreenMask;						// Mask for green channel
	u32 m_BlueMask;							// Mask for blue channel
	u32 m_AlphaMask;						// Mask for alpha channel

}; // end of RawImage class.


} // end namespace video
} // end namespace irr

#endif
