#include "RawImage.h"



namespace irr
{
namespace video
{

// [PUBLIC METHODS]

	//! constructor
	RawImage::RawImage(ECOLOR_FORMAT format, const core::dimension2d<s32>& size, void* data, bool copyImgData)
		: m_clrFormat(format), m_imgDimension(size), m_imgData(0), m_copyImgData(copyImgData)
	{

		initialiseBitMasks();
		initialiseBitsPerPixel();
		m_Pitch = m_BytesPerPixel * size.Width;
		initialiseImgData(data, copyImgData);
	}

	//! destructor
	RawImage::~RawImage() {
		if (m_copyImgData) {
			delete m_imgData;
		}
	}


	//! returns a pixel color
	irr::video::SColor RawImage::getPixel(irr::u32 x, irr::u32 y) const {

		// Out of bound -- return black.
		if ((x >= (u32)m_imgDimension.Width )|| (y >= (u32)m_imgDimension.Height)) {
			return SColor(0);
		}

		// Depending on image format, fetch the pixel color
		switch(m_clrFormat)
		{
			case irr::video::ECF_A1R5G5B5:
				return SColor(A1R5G5B5toA8R8G8B8(((u16*)m_imgData)[(y * m_imgDimension.Width) + x]));
				break;

			case irr::video::ECF_R5G6B5:
				return SColor(R5G6B5toA8R8G8B8(((u16*)m_imgData)[(y * m_imgDimension.Width) + x]));
				break;

			case irr::video::ECF_R8G8B8:
				// Keep ptrPixel out of range
				{
					u8* ptrPixel = &((u8*)m_imgData)[((y * 3) * m_imgDimension.Width) + (x * 3)];
					return SColor(0xFF, ptrPixel[0], ptrPixel[1], ptrPixel[2]);
				}
				break;

			case irr::video::ECF_A8R8G8B8:
				return SColor(((u32*)m_imgData)[(y * m_imgDimension.Width) + x]);
				break;

			default:
				return SColor(0);
				break;
		};
	}


	//! sets the color of a given pixel
	void RawImage::setPixel(irr::u32 x, irr::u32 y, const irr::video::SColor &color) {

		// Out of bound -- return
		if ((x >= (u32)m_imgDimension.Width) || (y >= (u32)m_imgDimension.Height)) {
			return;
		}

		// Set pixel color depending on color format
		switch(m_clrFormat)
		{
			case ECF_A1R5G5B5:
				{
					u16* ptrPixel = (u16*)((u8*)m_imgData + ( y * m_Pitch ) + ( x * m_BytesPerPixel ));
					*ptrPixel = video::A8R8G8B8toA1R5G5B5( color.color );
				}	
				break;
			case ECF_R5G6B5:
				{
					u16* ptrPixel = (u16*)((u8*)m_imgData + ( y * m_Pitch ) + ( x * m_BytesPerPixel ));
					*ptrPixel = video::A8R8G8B8toR5G6B5( color.color );
				}
				break;

			case ECF_R8G8B8:
				{
					u8* ptrPixel = (u8*)m_imgData + ( y * m_Pitch ) + ( x * m_BytesPerPixel );
					ptrPixel[0] = (u8)color.getRed();
					ptrPixel[1] = (u8)color.getGreen();
					ptrPixel[2] = (u8)color.getBlue();
				}
				break;

			case ECF_A8R8G8B8:
				{
					u32* ptrPixel = (u32*)((u8*)m_imgData + ( y * m_Pitch ) + ( x  * m_BytesPerPixel ));
					*ptrPixel = color.color;
				}
				break;
		};
	}



// [PRIVATE METHODS]

	void RawImage::initialiseBitMasks(void) {

		// Initialise the color bit masks depending on the color format
		switch (m_clrFormat) {

			case ECF_A1R5G5B5:
				m_AlphaMask	= 0x1<<15;
				m_RedMask	= 0x1F<<10;
				m_GreenMask	= 0x1F<<5;
				m_BlueMask	= 0x1F;
				break;

			case ECF_R5G6B5:
				m_AlphaMask	= 0x0;
				m_RedMask	= 0x1F<<11;
				m_GreenMask	= 0x3F<<5;
				m_BlueMask	= 0x1F;
				break;

			case ECF_R8G8B8:
				m_AlphaMask	= 0x0;
				m_RedMask	= 0x00FF0000;
				m_GreenMask	= 0x0000FF00;
				m_BlueMask	= 0x000000FF;
				break;

			case ECF_A8R8G8B8:
				m_AlphaMask	= 0xFF000000;
				m_RedMask	=	0x00FF0000;
				m_GreenMask	= 0x0000FF00;
				m_BlueMask	= 0x000000FF;
				break;

			default:
				m_AlphaMask	= 0x00;
				m_RedMask	= 0x00;
				m_GreenMask	= 0x00;
				m_BlueMask	= 0x00;
				break;
		};
	}


	void RawImage::initialiseBitsPerPixel(void) {

		switch(m_clrFormat)
		{
			case ECF_A1R5G5B5:
				m_BitsPerPixel = 16;
				break;

			case ECF_R5G6B5:
				m_BitsPerPixel = 16;
				break;

			case ECF_R8G8B8:
				m_BitsPerPixel = 24;
				break;

			case ECF_A8R8G8B8:
				m_BitsPerPixel = 32;
				break;

			default:
				m_BitsPerPixel = 0;
				break;
		};

		// Bytes per pixel = Bits per pixel / 8
		m_BytesPerPixel = m_BitsPerPixel >> 3;
	}


	void RawImage::initialiseImgData(void* data, bool copy) {

		if (copy) {
			// Note that the pitch should be aligned.
			if (!m_imgData)
			{
				m_imgData = new s8[m_imgDimension.Height * m_Pitch];
			}
			memcpy((u8*)m_imgData, (u8*)data, m_imgDimension.Height * m_Pitch);
		} else {
			m_imgData = data;

		}
	}
}	// end of video namespace
}	// end of irr namespace